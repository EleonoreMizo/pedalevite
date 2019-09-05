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



/*
==============================================================================
Name: set_wavetable
Description:
	Links the oscillator to a wavetable. The wavetable should be filled by
	the caller. This can be done before or after the call to this function, but
	before any call to sample-generating functions.
	With this function, both positive and negative sub-oscillator use the same
	wavetable.
	This is a mandatory call before using the oscillator.
Input parameters:
	- wavetable: the wavetable.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_wavetable (const WavetableDataType &wavetable)
{
	_wavetable_pos_ptr = &wavetable;
	_wavetable_neg_ptr = &wavetable;
}



/*
==============================================================================
Name: use_wavetable
Description:
	Returns the first attached wavetable. A wavetable must have been set
	previously.
Returns: A reference on wavetable #1.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
const typename OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::WavetableDataType &	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::use_wavetable () const
{
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	return *_wavetable_pos_ptr;
}



/*
==============================================================================
Name: set_wavetables
Description:
	Links the oscillator to a set of two wavetables. The wavetables should be
	filled by the caller. Thie can be done before or after the call to this
	function, but 	before any call to sample-generating functions.
	This is a mandatory call before using the oscillator.
Input parameters:
	- wavetable_pos: wavetable for the positive sub-oscillator
	- wavetable_neg: wavetable for the negative sub-oscillator
hrows: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_wavetables (const WavetableDataType &wavetable_pos, const WavetableDataType &wavetable_neg)
{
	_wavetable_pos_ptr = &wavetable_pos;
	_wavetable_neg_ptr = &wavetable_neg;
}



/*
==============================================================================
Name: use_wavetable
Description:
	Returns one of the attached wavetable. Wavetables must have been set
	previously.
Input parameters:
	- number: Wavetable to retrieve. 0 (positive wave) or 1 (negative wave)
Returns: A reference on the selected wavetable.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
const typename OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::WavetableDataType &	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::use_wavetable (int number) const
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



/*
==============================================================================
Name: set_base_pitch
Description:
	Sets the reference pitch value for the highest possible output frequency
	(= Nyquist). Pitch is logarithmic: each 1 << PITCH_FRAC_BITS more is one
	octave up.
	The current pitch nor generation parameter are not updated, so the
	oscillator may be in an invalid state after a call to this function.
	The caller must ensure that the current pitch is set to a legal value after
	having set the reference and before calling any	other function.
	Default pitch is 0.
Input parameters:
	- pitch: reference pitch. There is almost no constraint on its value, as
		it is just a reference.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_base_pitch (int32_t pitch)
{
	_base_pitch = pitch;
}



/*
==============================================================================
Name: get_base_pitch
Description:
	Returns the reference pitch value. It is defaulted to 0 if it has never
	been set previously.
Returns: The reference pitch value
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::get_base_pitch () const
{
	return _base_pitch;
}



/*
==============================================================================
Name: set_pitch
Description:
	Sets the current oscillator pitch. The reference pitch value is the Nyquist
	frequency. When 1 << PITCH_FRAC_BITS is subtracted to the pitch, the
	corresponding frequency is decreased by one octave.
Input parameters:
	- pitch: pitch to set. Should be less than the reference pitch.
Throws: Nothing
Returns:
	The fractional part of the frequency (2 ^ pitch_frac), on a 32-bit scale
	(0x80000000 to 0xFFFFFFFF).
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
uint32_t	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_pitch (int32_t pitch)
{
	assert (pitch < _base_pitch);

	const int      rel_pitch = pitch - _base_pitch;
	int            table     =
		(((1 << PITCH_FRAC_BITS) - 1) - rel_pitch) >> PITCH_FRAC_BITS;

	// 0x0000 => step = 0.5, 0xFFFF => step ~ 1
	// [1;2] -> [1;2].
	// We need 16 bits of fractionnal pitch
	static_assert (PITCH_FRAC_BITS == 16, "");
	const uint32_t pre_step (
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch)
	);

	set_pitch (pitch, pre_step, table);

	return pre_step;
}



/*
==============================================================================
Name: set_pitch
Description:
	Like the main set_pitch() function, but allows external computation for the
	step and the choice of the table.
Input parameters:
	- pitch: pitch to set. Should be less than the reference pitch.
	- pre_step:
		Fractional part of the frequency. Here the frequency is defined as a
		number of cycles (step) within the wavetable per final playback sample.
		The value is freq / floor (log2 (freq)) * (1 << 32).
		It should be in [1 << 31 ; (1 << 32) - 1].
	- table: Index of the wavetable to use. [0 ; MAX_SIZE_LOG2].
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_pitch (int32_t pitch, uint32_t pre_step, int table)
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



/*
==============================================================================
Name: get_pitch
Description:
	Returns the current pitch.
Returns: The pitch
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::get_pitch () const
{
	return _pitch;
}



/*
==============================================================================
Name: reset_phase
Description:
	Sets the current phase to 0. Nothing is done to prevent aliasing during
	this operation.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::reset_phase ()
{
	_position_pos.clear ();
	sync_sub_phase ();
}



/*
==============================================================================
Name: set_phase
Description:
	Sets the phase. There is no trick to prevent the oscillator from aliasing.
	You may use OscWavetableSyncHard if you need an alias-free sync oscillator.
Input parameters:
	- phase: New phase, in range [0 ; 0xFFFFFFFF[
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_phase (uint32_t phase)
{
	_position_pos.set_val_int64 (int64_t (phase) << _cur_table_len_log2);
	sync_sub_phase ();
}



/*
==============================================================================
Name: get_phase
Description:
	Gets the current phase.
Returns: Current phase, ranging in [0 ; 0xFFFFFFFF].
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
uint32_t	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::get_phase () const
{
	assert (_cur_table_len > 0);

	return uint32_t (_position_pos.get_val_int64 () >> _cur_table_len_log2);
}



/*
==============================================================================
Name: set_phase_rel
Description:
	Set the phase difference between both oscillators. If you need to modulate
	this parameter, try to achieve the highest rate as possible (ideally sample
	per sample) to avoid zipper noise.
Input parameters:
	- rel_phase:
		Phase difference, in [0 ; (1 << 32) - 1]. The value wraps around
		1 << 32, which corresponds to a full cycle.
	- dc_fixer:
		A value that may be used to cancel the DC offset that may come with the
		phase difference, depending on the waveforms used. The value is directly
		related to the scale of the wavetable data.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::set_phase_rel (uint32_t rel_phase, DataType dc_fixer)
{
	_rel_phase = rel_phase;
	_dc_fixer  = dc_fixer;
	sync_sub_phase ();
}



/*
==============================================================================
Name: get_phase_rel
Description:
	Gets the current phase difference.
Returns: the phase difference, in cycle scaled to 1 << 32.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
uint32_t	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::get_phase_rel () const
{
	return _rel_phase;
}



/*
==============================================================================
Name: get_sample_at_phase
Description:
	Computes the output sample for a given phase. The interpolator needs to be
	stateless to make the function work correctly.
Input parameters:
	- phase: the phase to pick, in fraction of cycle scaled to 1 << 32.
Returns: the combined oscillator output value
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
typename OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::DataType	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::get_sample_at_phase (uint32_t phase) const
{
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	fstb::FixedPoint  position_pos;
	fstb::FixedPoint  position_neg;
	position_pos.set_val_int64 (int64_t (phase) << _cur_table_len_log2);
	sync_sub_phase (position_pos, position_neg);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);
	
	return generate_sample (
		position_pos, position_neg, 
		src_pos_ptr, src_neg_ptr
	);
}



/*
==============================================================================
Name: get_sample_at_phase
Description:
	Same as the get_sample_at_phase(phase), but returns the main sub-oscillator
	value in addition to the subtracted value.
Input parameters:
	- phase: the phase to pick, in fraction of cycle scaled to 1 << 32.
Output parameters:
	- ref_data: main oscillator output
	- sub_data: the combined oscillator output
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::get_sample_at_phase (DataType &ref_data, DataType &sub_data, uint32_t phase) const
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



/*
==============================================================================
Name: process_sample
Description:
	Outputs a sample and updates the oscillator state.
Returns: the combined oscillator output
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
typename OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::DataType	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::process_sample ()
{
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);
	const DataType sub_data = generate_sample (
		_position_pos, _position_neg, 
		src_pos_ptr, src_neg_ptr
	);

	step_one_sample ();

	return sub_data;
}



/*
==============================================================================
Name: process_sample
Description:
	Same as the other process_sample(), but obtains the positive output in
	addition to the combined output.
Output parameters:
	- ref_data: The positive sub-oscillator output sample.
	- sub_data: The combined output sample.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::process_sample (DataType &ref_data, DataType &sub_data)
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



/*
==============================================================================
Name: process_block
Description:
	Outputs a block of samples and updates the oscillator state.
Input parameters:
	- nbr_spl: Number of samples to output. > 0.
Output parameters:
	- dest_ptr: Pointer on the output buffer receiving the combined output.
		Not null.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::process_block (DataType sub_data_ptr [], int nbr_spl)
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
		sub_data_ptr [pos] = generate_sample (
			_position_pos, _position_neg, 
			src_pos_ptr, src_neg_ptr
		);

		step_one_sample ();
	}
}



/*
==============================================================================
Name: process_block
Description:
	Outputs a block of samples and updates the oscillator state. Both positive
	and combined outputs are retrieved.
Input parameters:
	- nbr_spl: Number of samples to output. > 0.
Output parameters:
	- ref_data_ptr: Pointer on the output buffer receiving the positive output.
		Not null.
	- sub_data_ptr: Pointer on the output buffer receiving the combined output.
		Not null.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::process_block (DataType ref_data_ptr [], DataType sub_data_ptr [], int nbr_spl)
{
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



/*
==============================================================================
Name: process_block_mix
Description:
	Same as process_block(), but the output is added to the provided buffer.
Input parameters:
	- nbr_spl: Number of samples to output. > 0.
Input/output parameters:
	- sub_data_ptr: Pointer on the buffer receiving the combined output,
		containing valid samples on input. Not null.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::process_block_mix (DataType sub_data_ptr [], int nbr_spl)
{
	assert (sub_data_ptr != 0);
	assert (_wavetable_pos_ptr != 0);
	assert (_wavetable_neg_ptr != 0);
	assert (nbr_spl > 0);

	const DataType *  src_pos_ptr = _wavetable_pos_ptr->use_table (_cur_table);
	const DataType *  src_neg_ptr = _wavetable_neg_ptr->use_table (_cur_table);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		sub_data_ptr [pos] += generate_sample (
			_position_pos, _position_neg, 
			src_pos_ptr, src_neg_ptr
		);

		step_one_sample ();
	}
}



/*
==============================================================================
Name: process_block_mix
Description:
Input parameters:
	- nbr_spl: 
Input/output parameters:
	- ref_data_ptr: Pointer on the output buffer to which the positive output
		is added. Not null.
	- sub_data_ptr: Pointer on the output buffer to which the combined output
		is added. Not null.
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::process_block_mix (DataType ref_data_ptr [], DataType sub_data_ptr [], int nbr_spl)
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



/*
==============================================================================
Name: conv_freq_to_pitch
Description:
	Utility function converting a frequency and a sampling rate into a pitch
	value usable in set_pitch().
Input parameters:
	- freq: Oscillator frequency, Hz, ]0 ; Fs / 2[
	- fs: Sampling rate, Hz, > 0.
Returns: The computed pitch
Throws: Nothing
==============================================================================
*/

template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
int32_t	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::conv_freq_to_pitch (float freq, float fs) const
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
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::sync_sub_phase ()
{
	sync_sub_phase (_position_pos, _position_neg);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::sync_sub_phase (const fstb::FixedPoint &position_pos, fstb::FixedPoint &position_neg) const
{
	assert (position_pos.get_int_val () < _cur_table_len);

	int64_t        phase = position_pos.get_val_int64 ();
	phase -= int64_t (_rel_phase) << _cur_table_len_log2;
	position_neg.set_val_int64 (phase);
	position_neg.bound_and (_cur_table_mask);

	assert (position_neg.get_int_val () < _cur_table_len);
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
typename OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::DataType	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::generate_sample (const fstb::FixedPoint &position_pos, const fstb::FixedPoint &position_neg, const DataType * const src_pos_ptr, const DataType * const src_neg_ptr) const
{
	DataType       ref_data;
	DataType       sub_data;
	generate_sample (
		ref_data, sub_data,
		position_pos, position_neg,
		src_pos_ptr, src_neg_ptr
	);

	return sub_data;
}



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT, int UPRE, int UPOST>
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::generate_sample (DataType &ref_data, DataType &sub_data, const fstb::FixedPoint &position_pos, const fstb::FixedPoint &position_neg, const DataType * const src_pos_ptr, const DataType * const src_neg_ptr) const
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
void	OscWavetableSub <
	IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST
>::step_one_sample ()
{
	_position_pos.add (_step, _cur_table_mask);
	_position_neg.add (_step, _cur_table_mask);
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscWavetableSub_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
