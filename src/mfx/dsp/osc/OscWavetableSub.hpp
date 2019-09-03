/*****************************************************************************

        OscWavetableSub.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscWavetableSub_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscWavetableSub_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::set_wavetables (const WavetableDataType &wavetable_pos, const WavetableDataType &wavetable_neg)
{
	assert (&wavetable_pos != 0);
	assert (&wavetable_neg != 0);

	_wavetable_pos_ptr = &wavetable_pos;
	_wavetable_neg_ptr = &wavetable_neg;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
const typename OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::WavetableDataType &	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::use_wavetable (int number) const
{
	assert (number >= 0);
	assert (number < 2);
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	WavetableDataType *  wt_ptr =
		  (number == 1)
		? _wavetable_neg_ptr
		: _wavetable_pos_ptr;
	assert (wt_ptr != 0);

	return wt_ptr;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::set_base_pitch (int32_t pitch)
{
	_base_pitch = pitch;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::get_base_pitch () const
{
	return _base_pitch;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::set_pitch (int32_t pitch)
{
	assert (pitch < _base_pitch);

	const int      rel_pitch = pitch - _base_pitch;
	int            table     =
		(((1 << PITCH_FRAC_BITS) - 1) - rel_pitch) >> PITCH_FRAC_BITS;

	// 0x0000 => step = 0.5, 0xFFFF => step ~ 1
	// [1;2] -> [1;2].
	// We need 16 bits of fractionnal pitch
	assert (PITCH_FRAC_BITS == 16);
	const uint32_t pre_step (
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch)
	);

	set_pitch (pitch, pre_step, table);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::set_pitch (int32_t pitch, uint32_t pre_step, int table)
{
	assert (pitch < _base_pitch);
	assert (pre_step >= 0x80000000UL);
	assert (table > 0);	// Table 0 should never be used

	_pitch = pitch;

	int64_t        step_64 (pre_step);
	const int      old_table_len_log2 = _cur_table_len_log2;

	// Pitch is lower than table 0
	if (table > WavetableDataType::MAX_SIZE_LOG2)
	{
		step_64 >>= table - WavetableDataType::MAX_SIZE_LOG2;
		table     = WavetableDataType::MAX_SIZE_LOG2;
	}

	_cur_table_len      = WavetableDataType::get_table_len (table);
	_cur_table_len_log2 = WavetableDataType::get_table_len_log2 (table);
	_cur_table          = table;
	step_64 <<= _cur_table_len_log2 - table;
	_cur_table_mask = WavetableDataType::get_table_mask (table);
	_position_pos.shift (_cur_table_len_log2 - old_table_len_log2);
	sync_sub_phase ();
	_step.set_val_int64 (step_64);

	assert (_position_pos.get_int_val () < _cur_table_len);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::get_pitch () const
{
	return _pitch;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::reset_phase ()
{
	_position_pos.clear ();
	sync_sub_phase ();
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::set_phase_int (uint32_t phase)
{
	_position_pos.set_val_int64 (int64_t (phase) << _cur_table_len_log2);
	sync_sub_phase ();
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
uint32_t	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::get_phase_int () const
{
	assert (_cur_table_len > 0);

	return uint32_t (_position_pos.get_val_int64 () >> _cur_table_len_log2);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::set_rel_phase_int (uint32_t rel_phase, DataType dc_fixer)
{
	_rel_phase = rel_phase;
	_dc_fixer  = dc_fixer;
	sync_sub_phase ();
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
uint32_t	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::get_rel_phase_int () const
{
	return _rel_phase;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::get_sample_at_phase (DataType &ref_data, DataType &sub_data, uint32_t phase) const
{
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	fstb::FixedPoint  position_pos;
	fstb::FixedPoint  position_neg;
	position_pos.set_val_int64 (int64_t (phase) << _cur_table_len_log2);
	sync_sub_phase (position_pos, position_neg);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);
	generate_sample (
		ref_data, sub_data,
		position_pos, position_neg, 
		src_pos_ptr, src_neg_ptr
	);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::process_sample (DataType &ref_data, DataType &sub_data)
{
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);
	generate_sample (
		ref_data, sub_data,
		_position_pos, _position_neg, 
		src_pos_ptr, src_neg_ptr
	);

	step_one_sample ();
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::process_sample_fm (DataType &ref_data, DataType &sub_data, int32_t new_pitch)
{
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	set_pitch (new_pitch);
	process_sample (ref_data, sub_data);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::process_block (DataType ref_data_ptr [], DataType sub_data_ptr [], int nbr_spl)
{
	assert (ref_data_ptr != 0);
	assert (sub_data_ptr != 0);
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);
	assert (nbr_spl > 0);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		generate_sample (
			ref_data_ptr [pos], sub_data_ptr [pos],
			_position_pos, _position_neg, 
			src_pos_ptr, src_neg_ptr
		);

		step_one_sample ();
	}
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::process_block_mix (DataType ref_data_ptr [], DataType sub_data_ptr [], int nbr_spl)
{
	assert (ref_data_ptr != 0);
	assert (sub_data_ptr != 0);
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);
	assert (nbr_spl > 0);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);

	DataType			temp_ref;
	DataType			temp_sub;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		generate_sample (
			temp_ref, temp_sub,
			_position_pos, _position_neg, 
			src_pos_ptr, src_neg_ptr
		);
		ref_data_ptr [pos] += temp_ref;
		sub_data_ptr [pos] += temp_sub;

		step_one_sample ();
	}
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::conv_freq_to_pitch (float freq, float fs) const
{
	assert (freq > 0);
	assert (fs > 0);
	assert (freq < fs * 0.5f);

	// log2 is 0 at Fs and not Fnyquist
	const float    p_l2  = fstb::Approx::log2 (freq / fs) + 1;
	const int32_t  p_rel = fstb::round_int (p_l2 * (1 << PITCH_FRAC_BITS));
	const int32_t  pitch = p_rel + _base_pitch;

	return pitch;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::sync_sub_phase ()
{
	sync_sub_phase (_position_pos, _position_neg);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::sync_sub_phase (const fstb::FixedPoint &position_pos, fstb::FixedPoint &position_neg) const
{
	assert (position_pos.get_int_val () < _cur_table_len);

	int64_t        phase = position_pos.get_val_int64 ();
	phase -= int64_t (_rel_phase) << _cur_table_len_log2;
	position_neg.set_val_int64 (phase);
	position_neg.bound_and (_cur_table_mask);

	assert (position_neg.get_int_val () < _cur_table_len);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::generate_sample (DataType &ref_data, DataType &sub_data, const fstb::FixedPoint &position_pos, const fstb::FixedPoint &position_neg, const DataType * const src_pos_ptr, const DataType * const src_neg_ptr) const
{
	const uint32_t position_pos_frac = position_pos.get_frac_val ();
	const int      position_pos_int  = position_pos.get_int_val ();
	ref_data = _interpolator (
		position_pos_frac,
		&src_pos_ptr [position_pos_int]
	);

	const uint32_t position_neg_frac = position_neg.get_frac_val ();
	const int      position_neg_int  = position_neg.get_int_val ();
	sub_data = ref_data + _dc_fixer - _interpolator (
		position_neg_frac,
		&src_neg_ptr [position_neg_int]
	);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST>::step_one_sample ()
{
	_position_pos.add (_step, _cur_table_mask);
	_position_neg.add (_step, _cur_table_mask);
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscWavetableSub_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
