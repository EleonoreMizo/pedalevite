/*****************************************************************************

        SplitThiele8.cpp
        Author: Laurent de Soras, 2021

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

#include "mfx/dsp/iir/SplitThiele8.h"
#include "mfx/dsp/iir/Svf2p.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SplitThiele8::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	_dirty_flag  = true;
}



void	SplitThiele8::set_split_freq (float f)
{
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_split_freq = f;
	_dirty_flag = true;
}



void	SplitThiele8::set_thiele_coef (float k)
{
	assert (k >= 0);
	assert (k < 1);

	_thiele_k   = k;
	_dirty_flag = true;
}



// Mandatory call between setting parameters and processing
void	SplitThiele8::update_coef ()
{
	if (_dirty_flag)
	{
		update_filters ();
		_dirty_flag = false;
	}
}



void	SplitThiele8::clear_buffers ()
{
	_split_1.clear_buffers ();
	_split_2.clear_buffers ();
	_split_3.clear_buffers ();
	_split_4.clear_buffers ();

	_comp_1.clear_buffers ();
	_comp_2.clear_buffers ();
}



void	SplitThiele8::process_block_split (float lo_ptr [], float hi_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (! _dirty_flag);
	assert (lo_ptr != nullptr);
	assert (hi_ptr != nullptr);
	assert (lo_ptr != hi_ptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		auto        both = process_sample_split (src_ptr [pos]);
		lo_ptr [pos] = both [0];
		hi_ptr [pos] = both [1];
	}
}



void	SplitThiele8::process_block_compensate (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (! _dirty_flag);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample_compensate (src_ptr [pos]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SplitThiele8::update_filters ()
{
	// _r1 and _r2 need to be calculated as accurately as possible,
	// to make sure (_r1 * _r2) ^ 2 == 2
	const double   k2 = double (_thiele_k) * double (_thiele_k);
	const double   k4 = k2 * k2;
	const double   d0 = 0.5 *      (4 - k2);
	const double   d1 = 0.5 * sqrt (8 + k4);
	_r1 = float (sqrt (d0 + d1));
	_r2 = float (sqrt (d0 - d1));

	_n1 = -2  * (_r1 + _r2);
	_n4 = _r2 * _n1;
	_n3 = _r2 * _n4;

	const float    f0_fs = _split_freq * _inv_fs;
	float          g0_1  = 0;
	float          g1_1  = 0;
	float          g2_1  = 0;
	float          g0_2  = 0;
	float          g1_2  = 0;
	float          g2_2  = 0;
	Svf2p::conv_poles (g0_1, g1_1, g2_1, f0_fs, _r1);
	Svf2p::conv_poles (g0_2, g1_2, g2_2, f0_fs, _r2);

	_split_1.set_coefs (g0_1, g1_1, g2_1);
	_split_2.set_coefs (g0_2, g1_2, g2_2);
	_split_3.set_coefs (g0_1, g1_1, g2_1);
	_split_4.set_coefs (g0_2, g1_2, g2_2);

	// Numerator of the s equation: 1 + k^2 * s^2
	const float    k2f = float (k2);
	_split_3.set_mix (k2f, k2f * -_r1, 1 - k2f);

	_comp_1.set_coefs (g0_1, g1_1, g2_1);
	_comp_2.set_coefs (g0_2, g1_2, g2_2);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
