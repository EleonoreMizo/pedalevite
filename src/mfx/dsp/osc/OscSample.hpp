/*****************************************************************************

        OscSample.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscSample_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscSample_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"

#include <cassert>
#include <cstdlib>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_sample_data
Description:
	Set the oscillator sample data. Data should be valid to be set.
Input parameters:
	- sample_data: reference on the data. It is copied from the original,
		therefore the original can be discarded after the call.
Throws: Nothing
==============================================================================
*/

template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::set_sample_data (const SampleTable &sample_data)
{
	assert (&sample_data != 0);
	assert (sample_data.is_valid ());

	_sample_data = sample_data;
}



/*
==============================================================================
Name: use_sample_data
Description:
	Use the current sample data, for direct access. There is no guaranty that
	it is valid. However client should ensure it is valid before using the
	"working" member functions.
Returns: Reference on the data.
Throws: Nothing
==============================================================================
*/

template <typename SD, typename IF, typename CDT>
const typename OscSample <SD, IF, CDT>::SampleTable &	OscSample <SD, IF, CDT>::use_sample_data () const
{
	return _sample_data;
}



template <typename SD, typename IF, typename CDT>
typename OscSample <SD, IF, CDT>::SampleTable &	OscSample <SD, IF, CDT>::use_sample_data ()
{
	return _sample_data;
}



/*
==============================================================================
Name: set_bandlimit
Description:
	Set the frequency range for the upper harmonic. For example:
	-0 * 0x10000 -> [Nyquist   ; Nyquist*2]. Gives 1-fold aliasing
	-1 * 0x10000 -> [Nyquist/2 ; Nyquist  ]. No aliasing, but halved bandwidth.
	The latter value is well-suited for x2 oversampling, but something a bit
	higher could be better to avoid table-switching glitches.
Input parameters:
	- limit: Limit in octaves, relative to Nyquist, 32:16. <= 0.
Throws: Nothing
==============================================================================
*/

template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::set_bandlimit (int32_t limit)
{
	assert (limit <= 0);

	_bandlimit = limit;
}



template <typename SD, typename IF, typename CDT>
int32_t	OscSample <SD, IF, CDT>::get_bandlimit () const
{
	return _bandlimit;
}



/*
==============================================================================
Name: set_pitch
Description:
	Set the pitch. 0 is the natural playback rate (1 output sample = 1 input
	sample). It is measured in octaves * 0x10000.
Input parameters:
	- pitch: The pitch.
Throws: Nothing
==============================================================================
*/

template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::set_pitch (int32_t pitch)
{
	_pitch = pitch;

	const int      old_table = _table;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Find the right fractionnal step / table / shift for the given pitch

	int            pitch_frac = pitch & PITCH_FRAC_MASK;
	int            step_p2    = pitch >> PITCH_FRAC_BITS;
	int            table      = (pitch - _bandlimit) >> PITCH_FRAC_BITS;
	assert (std::abs (fstb::sgn (step_p2) - fstb::sgn (pitch             )) < 2);
	assert (std::abs (fstb::sgn (table  ) - fstb::sgn (pitch - _bandlimit)) < 2);
	const int      last_table = _sample_data.get_nbr_tables () - 1;
	table = fstb::limit (table, 0, last_table);

	// Default to 1 to compensate the scale introduced by the exponential
	// mapping of the fractionnal part.
	const int      shift = step_p2 - table + 1;

	_table     = table;
	_table_ptr = _sample_data.use_table (_table);

	// 0x0000 => step = 0.5, 0xFFFF => step ~ 1
	// [1;2] -> [1;2]: f(x) = (x^2)/3 + 2/3. Max error: ~4 cents
	// We need 16 bits of fractionnal pitch
	static_assert (PITCH_FRAC_BITS == 16, "");
	const uint32_t pre_step (
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (pitch_frac)
	);

	_step.set_val (0, pre_step);
	_step.shift (shift);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Adapt the current position

	const int      table_dif = old_table - _table;
	if (table_dif != 0)
	{
		_pos.shift (table_dif);
	}
}



template <typename SD, typename IF, typename CDT>
int32_t	OscSample <SD, IF, CDT>::get_pitch () const
{
	return _pitch;
}



/*
==============================================================================
Name: get_table_rate
Description:
	Returns the playing rate in the current table. It is the playing frequency
	relative to the table frequency. The range depends on the current table
	and the _bandlimit parameter.
Output parameters:
	- rate: playing rate
Throws: Nothing
==============================================================================
*/

template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::get_table_rate (fstb::FixedPoint &rate) const
{
	rate = _step;
}



template <typename SD, typename IF, typename CDT>
int	OscSample <SD, IF, CDT>::get_cur_table () const
{
	return _table;
}



template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::set_playback_pos (const fstb::FixedPoint &pos)
{
	_pos   = pos;
	_pos >>= _table;
}



template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::get_playback_pos (fstb::FixedPoint &pos) const
{
	pos   = _pos;
	pos <<= _table;
}



template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::set_playback_pos_in_cur_table (const fstb::FixedPoint &pos)
{
	_pos = pos;
}



template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::get_playback_pos_in_cur_table (fstb::FixedPoint &pos) const
{
	pos = _pos;
}




template <typename SD, typename IF, typename CDT>
typename OscSample <SD, IF, CDT>::CalcDataType	OscSample <SD, IF, CDT>::get_sample_in_cur_table (const fstb::FixedPoint &pos) const
{
	assert (pos.get_int_val () >= -SampleTable::UNROLL_PRE + InterpFtor::DATA_PRE);
	assert (pos.get_int_val () < _sample_data.get_table_len (_table) + SampleTable::UNROLL_POST - InterpFtor::DATA_POST);

	const uint32_t frac_pos = pos.get_frac_val ();
	const int32_t  data_pos = pos.get_int_val ();

	const StorageDataType * src_ptr = _table_ptr + data_pos;

	return _interpolator (frac_pos, src_ptr);
}



/*
==============================================================================
Name: process_sample
Description:
	Generate a sample and move sample index according to the pitch.
	Bound checks are loosy, so client must ensure that the position is really
	correct.
Returns: Generated data
Throws: Nothing
==============================================================================
*/

template <typename SD, typename IF, typename CDT>
typename OscSample <SD, IF, CDT>::CalcDataType	OscSample <SD, IF, CDT>::process_sample ()
{
	assert (_pos.get_int_val () >= -SampleTable::UNROLL_PRE + InterpFtor::DATA_PRE);
	assert (_pos.get_int_val () < _sample_data.get_table_len (_table) + SampleTable::UNROLL_POST - InterpFtor::DATA_POST);

	// Read data
	const CalcDataType   result = get_sample_in_cur_table (_pos);

	// Next sample
	_pos += _step;

	return result;
}



template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::process_block (CalcDataType data_ptr [], int nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	// Prepare
	fstb::FixedPoint        spl_pos   = _pos;
	const fstb::FixedPoint  spl_step  = _step;
	const StorageDataType * table_ptr = _table_ptr;

	// Loop
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		assert (spl_pos.get_int_val () >= -SampleTable::UNROLL_PRE + InterpFtor::DATA_PRE);
		assert (spl_pos.get_int_val () < _sample_data.get_table_len (_table) + SampleTable::UNROLL_POST - InterpFtor::DATA_POST);

		const uint32_t frac_pos = spl_pos.get_frac_val ();
		const int32_t  data_pos = spl_pos.get_int_val ();
		
		const CalcDataType   result = _interpolator (
			frac_pos,
			table_ptr + data_pos
		);
		
		data_ptr [pos] = result;

		spl_pos += spl_step;
	}

	_pos = spl_pos;
}


template <typename SD, typename IF, typename CDT>
void	OscSample <SD, IF, CDT>::process_block_mix (CalcDataType data_ptr [], int nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	// Prepare
	fstb::FixedPoint        spl_pos   = _pos;
	const fstb::FixedPoint  spl_step  = _step;
	const StorageDataType * table_ptr = _table_ptr;

	// Loop
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		assert (spl_pos.get_int_val () >= -SampleTable::UNROLL_PRE + InterpFtor::DATA_PRE);
		assert (spl_pos.get_int_val () < _sample_data.get_table_len (_table) + SampleTable::UNROLL_POST - InterpFtor::DATA_POST);
		
		const uint32_t frac_pos = spl_pos.get_frac_val ();
		const int32_t  data_pos = spl_pos.get_int_val ();
		
		const CalcDataType   result = _interpolator (
			frac_pos,
			table_ptr + data_pos
		);
		
		data_ptr [pos] += result;

		spl_pos += spl_step;
	}

	_pos = spl_pos;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSample_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
