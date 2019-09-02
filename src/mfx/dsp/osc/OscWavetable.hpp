/*****************************************************************************

        OscWavetable.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscWavetable_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscWavetable_CODEHEADER_INCLUDED



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



/*
==============================================================================
Name: reset_phase
Description:
	Sets the phase to 0.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::reset_phase ()
{
	_pos.clear ();
}



/*
==============================================================================
Name: set_phase_flt
Description:
	Sets the phase. There is no trick to prevent the oscillator from aliasing.
	You may use OscWavetableSync instead.
Input parameters:
	- phase: New phase, in range [0 ; 1[.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::set_phase_flt (float phase)
{
	assert (phase >= 0);
	assert (phase < 1);
	assert (_cur_table_len > 0);

	_pos.set_val (phase * _cur_table_len);
}



/*
==============================================================================
Name: get_phase_flt
Description:
	Gets the current phase.
Returns:
	Current phase, ranging in [0 ; 1[. May be 1.0 because of the rounding.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
float	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_phase_flt () const
{
	assert (_cur_table_len > 0);

	return _pos.get_val_flt () / _cur_table_len;
}



/*
==============================================================================
Name: set_phase_int
Description:
	Sets the phase. There is no trick to prevent the oscillator from aliasing.
	You may use OscWavetableSync instead.
Input parameters:
	- phase: New phase, in range [0 ; 0xFFFFFFFF[
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::set_phase_int (uint32_t phase)
{
	assert (_cur_table_len > 0);

	_pos.set_val (0, phase);
	_pos <<= _cur_table_len_log2;
}



/*
==============================================================================
Name: get_phase_int
Description:
	Gets the current phase.
Returns: Current phase, ranging in [0 ; 0xFFFFFFFF[.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
uint32_t	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_phase_int () const
{
	assert (_cur_table_len > 0);

	return uint32_t (_pos.get_val_int64 () >> _cur_table_len_log2);
}



/*
==============================================================================
Name: set_base_pitch
Description:
	Sets the reference pitch value for the highest possible output frequency
	(= Nyquist). Pitch is logarithmic: each 1 << PITCH_FRAC_BITS more is one
	octave up.
Input parameters:
	- pitch: reference pitch. There is almost no constraint on its value, as
		it is just a reference.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::set_base_pitch (int32_t pitch)
{
	_base_pitch = pitch;
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_base_pitch () const
{
	return _base_pitch;
}



/*
==============================================================================
Name: set_pitch
Description:
	Sets the current oscillator pitch.
Input parameters:
	- pitch: pitch to set. Should be less than the reference pitch.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::set_pitch (int32_t pitch)
{
	assert (pitch < _base_pitch);

	const int      rel_pitch = pitch - _base_pitch;
	int            table     =
		(((1 << PITCH_FRAC_BITS) - 1) - rel_pitch) >> PITCH_FRAC_BITS;
	assert (table > 0);	// Table 0 should never be used

	// 0x0000 => step = 0.5, 0xFFFF => step ~ 1
	// [1;2] -> [1;2].
	// We need 16 bits of fractionnal pitch
	assert (PITCH_FRAC_BITS == 16);
	const uint32_t pre_step (
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch)
	);
	assert (pre_step >= 0x80000000UL);

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
	_pos.shift (_cur_table_len_log2 - old_table_len_log2);
	_step.set_val_int64 (step_64);

	assert (_pos.get_int_val () < _cur_table_len);
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_pitch () const
{
	return _pitch;
}



/*
==============================================================================
Name: set_pitch_no_table_update
Description:
	Sets the current oscillator pitch without checking for table change.
	This could be useful to avoid tonal glitches when small pitch variations
	(like a vibrato) make the table change often.
Input parameters:
	- pitch: pitch to set. Because we don't change the table, we can allow
		pitch to be greater than _base_pitch (the signal will alias for sure).
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::set_pitch_no_table_update (int32_t pitch)
{
	const int      rel_pitch = pitch - _base_pitch;
	int            table     =
		(((1 << PITCH_FRAC_BITS) - 1) - rel_pitch) >> PITCH_FRAC_BITS;

	// 0x0000 => step = 0.5, 0xFFFF => step ~ 1
	// [1;2] -> [1;2]. Max error : ~0.08 cents
	// We need 16 bits of fractionnal pitch
	assert (PITCH_FRAC_BITS == 16);
	const uint32_t pre_step (
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch)
	);
	assert (pre_step >= 0x80000000UL);

	_pitch = pitch;

	int64_t        step_64 (pre_step);
	_step.set_val_int64 (step_64);
	_step.shift (_cur_table_len_log2 - table);
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::set_wavetable (const WavetableDataType &wavetable)
{
	assert (&wavetable != 0);

	_wavetable_ptr = &wavetable;
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
const typename OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::WavetableDataType &	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::use_wavetable () const
{
	assert (_wavetable_ptr != 0);

	return *_wavetable_ptr;
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_sample_at_phase_flt (float phase)
{
	assert (_wavetable_ptr != 0);
	assert (_cur_table_len > 0);
	assert (phase >= 0);
	assert (phase < 1);

	const float    phase_wt   = phase * _cur_table_len;
	const int      int_pos    = fstb::floor_int (phase_wt);
	const float    frac_pos   = phase_wt - int_pos;
	const DataType *  src_ptr = _wavetable_ptr->use_table (_cur_table);

	return _interpolator (frac_pos, &src_ptr [int_pos]);
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_sample_at_phase_int (uint32_t phase)
{
	assert (_wavetable_ptr != 0);
	assert (_cur_table_len > 0);

	const int      shift      = _cur_table_len_log2;
	const float    mult       = float (fstb::TWOPM32);
	const int      int_pos    = phase >> (32 - shift);
	const float    frac_pos   = (phase << shift) * mult;
	const DataType *  src_ptr = _wavetable_ptr->use_table (_cur_table);

	return _interpolator (frac_pos, &src_ptr [int_pos]);
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::get_cur_sample () const
{
	const DataType *  src_ptr = _wavetable_ptr->use_table (_cur_table);
	const float    frac_pos   = _pos.get_frac_val_flt ();
	const int      int_pos    = _pos.get_int_val ();
	const DataType val        = _interpolator (frac_pos, &src_ptr [int_pos]);

	return val;
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::process_sample ()
{
	const DataType val = get_cur_sample ();
	_pos.add (_step, _cur_table_mask);

	return val;
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::process_block (typename OscWavetable::DataType dest_ptr [], int nbr_spl)
{
	assert (_wavetable_ptr != 0);
	assert (nbr_spl > 0);

	const DataType *  src_ptr = _wavetable_ptr->use_table (_cur_table);
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    frac_pos = _pos.get_frac_val_flt ();
		const int      int_pos  = _pos.get_int_val ();
		dest_ptr [pos] = _interpolator (frac_pos, &src_ptr [int_pos]);
		_pos.add (_step, _cur_table_mask);
	}
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::process_block_mix (typename OscWavetable::DataType dest_ptr [], int nbr_spl)
{
	assert (_wavetable_ptr != 0);
	assert (nbr_spl > 0);

	const DataType *  src_ptr = _wavetable_ptr->use_table (_cur_table);
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    frac_pos = _pos.get_frac_val_flt ();
		const int      int_pos  = _pos.get_int_val ();
		dest_ptr [pos] += _interpolator (frac_pos, &src_ptr [int_pos]);
		_pos.add (_step, _cur_table_mask);
	}
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::skip_block (int nbr_spl)
{
	assert (_wavetable_ptr != 0);
	assert (nbr_spl > 0);

	int64_t        step_64 (_step.get_val_int64 ());
	step_64 *= nbr_spl;
	fstb::FixedPoint	step_block;
	step_block.set_val_int64 (step_64);
	_pos.add (step_block, _cur_table_mask);
}



template <typename IF, int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetable <IF, MAXSL2, MINSL2, DT, UPRE, UPOST>::conv_freq_to_pitch (float freq, float fs) const
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



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscWavetable_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
