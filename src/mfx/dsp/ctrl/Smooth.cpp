/*****************************************************************************

        Smooth.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/Smooth.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ctrl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_sample_freq
Description:
	Sets the main sampling rate.
Input parameters:
	- sample_freq: sampling rate, in Hz. > 0.
Throws: Nothing
==============================================================================
*/

void	Smooth::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (       sample_freq);
	_inv_fs      = float (1.0f / sample_freq);
	update_g0_cond ();
}



/*
==============================================================================
Name: set_base_freq
Description:
	Sets the filter base frequency. This the low-pass filter cutoff frequency
	used when the variations are extremely low (or null actually).
Input parameters:
	- freq: base frequency, in Hz.
Throws: Nothing
==============================================================================
*/

void	Smooth::set_base_freq (float freq) noexcept
{
	assert (freq > 0);

	_f0 = freq;
	update_g0_cond ();
}



/*
==============================================================================
Name: set_sensitivity
Description:
	Sets the sensibility to signal variation. When the signal changes, the low-
	pass filter frequency is increased, depending on the amplitude of the
	variations. This parameter controls the intensity of the filter reaction
	in presence of signal variations.
Input parameters:
	- s: the sensitivity, a positive value. 0 = no variation (static low-pass
		filter)
Throws: Nothing
==============================================================================
*/

void	Smooth::set_sensitivity (float s) noexcept
{
	assert (s >= 0);

	_sense = s;
}



/*
==============================================================================
Name: force_val
Description:
	Sets a value directly, without filtering it.
Input parameters:
	- x: input value.
Throws: Nothing
==============================================================================
*/

void	Smooth::force_val (float x) noexcept
{
	_val = x;
	clear_buffers ();
}



/*
==============================================================================
Name: process_sample
Description:
	Filters an input value, at the defined sampling rate.
Input parameters:
	- x: Input value
Returns: Filtered value
Throws: Nothing
==============================================================================
*/

float	Smooth::process_sample (float x) noexcept
{
	_val = x;

	return process_sample (x, _g0, _sense);
}



/*
==============================================================================
Name: skip_block
Description:
	Skips a N-sample block, assuming the input value remains constant during
	this block.
	Complexity in O(1).
Input parameters:
	- nbr_spl: block length, in samples. > 0.
Returns: filtered value at the end of the block.
Throws: Nothing
==============================================================================
*/

float	Smooth::skip_block (int nbr_spl) noexcept
{
	return process_block (_val, nbr_spl);
}



/*
==============================================================================
Name: process_block
Description:
	Sets the input value at the beginning of a sample block, and assumes its
	value remains unchanged for its duration.
	Complexity in O(1).
Input parameters:
	- x: input value
	- nbr_spl: block length, in samples. > 0.
Returns:
Throws: Nothing
==============================================================================
*/

float	Smooth::process_block (float x, int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	_val = x;
	const float    f0_blk = _f0 * float (nbr_spl);
	const float    g0_blk = compute_g0 (f0_blk);
	const float    se_blk = _sense * float (nbr_spl);

	return process_sample (x, g0_blk, se_blk);
}



/*
==============================================================================
Name: process_block
Description:
	Filters a block of samples.
Input parameters:
	- src_ptr: pointer on a nbr_spl-long input sample block.
	- nbr_spl: block length, in samples. > 0.
Output parameters:
	- dst_ptr: pointer on the filtered samples, with room for nbr_spl samples.
Throws: Nothing
==============================================================================
*/

void	Smooth::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos], _g0, _sense);
	}
	_val = src_ptr [nbr_spl - 1];
}



/*
==============================================================================
Name: get_target_val
Description:
	Gets the the latest input value.
Returns: the input value.
Throws: Nothing
==============================================================================
*/

float	Smooth::get_target_val () const noexcept
{
	return _val;
}



/*
==============================================================================
Name: get_smooth_val
Description:
	Gets the latest output value.
Returns: the filtered value.
Throws: Nothing
==============================================================================
*/

float	Smooth::get_smooth_val () const noexcept
{
	return _low2;
}



/*
==============================================================================
Name: clear_buffers
Description:
	Assumes the filter as a steady input for an infinite amount of time.
Throws: Nothing
==============================================================================
*/

void	Smooth::clear_buffers () noexcept
{
	_low1 = _val;
	_low2 = _val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Smooth::update_g0_cond () noexcept
{
	if (_sample_freq > 0)
	{
		_g0 = compute_g0 (_f0);
	}
}



float	Smooth::compute_g0 (float f0) noexcept
{
	assert (_inv_fs > 0);
	assert (f0 > 0);
	assert (f0 < _sample_freq * 0.5f);

	const float    wc = f0 * _inv_fs;

	// Third-order polynomial approximation of:
	// gc = tan (pi * wc)
	// g0 = 2 * gc / (1 + gc)
	return ((15.952062f * wc - 11.969296f) * wc + 5.9948827f) * wc;
}



float	Smooth::process_sample (float x, float g0, float sense) noexcept
{
	const float    low1z = _low1;
	const float    low2z = _low2;
	const float    bandz = low1z - low2z;
	const float    g     = std::min (g0 + sense * fabsf (bandz), 1.0f);
	_low1 = low1z + g * (x     - low1z);
	_low2 = low2z + g * (_low1 - low2z);

	return _low2;
}



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
