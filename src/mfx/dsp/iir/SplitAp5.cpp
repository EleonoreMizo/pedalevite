/*****************************************************************************

        SplitAp5.cpp
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

#include "mfx/dsp/iir/SplitAp5.h"
#include "mfx/dsp/iir/TransSZBilin.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SplitAp5::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	_dirty_flag  = true;
}



void	SplitAp5::set_split_freq (float f)
{
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_split_freq = f;
	_dirty_flag = true;
}



void	SplitAp5::set_param (float ka, float kb)
{
	assert (ka > 0);
	assert (kb > 0);

	_ka         = ka;
	_kb         = kb;
	_dirty_flag = true;
}



// Mandatory call between setting parameters and processing
void	SplitAp5::update_coef ()
{
	if (_dirty_flag)
	{
		update_filters ();
		_dirty_flag = false;
	}
}



void	SplitAp5::copy_param_from (const SplitAp5 &other)
{
	assert (_sample_freq > 0);
	assert (_sample_freq == other._sample_freq);

	_split_freq = other._split_freq;
	_ka         = other._ka;
	_kb         = other._kb;
	_dirty_flag = other._dirty_flag;

	if (! _dirty_flag)
	{
		const auto &   f0     = other._band_split.use_filter_0 ();
		const auto &   src_f1 = other._band_split.use_filter_1 ();
		const auto &   f1e    = src_f1.use_filter ();
		const auto &   f1o    = src_f1.use_filter (0);

		Filter0 &	filter_0 = _band_split.use_filter_0 ();
		Filter1 &	filter_1 = _band_split.use_filter_1 ();
		Filter0 &   filter_f = _band_split.use_fix_filter ();
		filter_0.copy_z_eq (f0);
		filter_1.use_filter ().copy_z_eq (f1e);
		filter_1.use_filter (0).copy_z_eq (f1o);
		filter_f.copy_z_eq (f0);
	}
}



void	SplitAp5::clear_buffers ()
{
	_band_split.clear_buffers ();
}



void	SplitAp5::process_block_split (float lo_ptr [], float hi_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (! _dirty_flag);
	assert (lo_ptr != nullptr);
	assert (hi_ptr != nullptr);
	assert (lo_ptr != hi_ptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	_band_split.split_block (lo_ptr, hi_ptr, src_ptr, nbr_spl);
}



void	SplitAp5::process_block_compensate (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (! _dirty_flag);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	_band_split.compensate_block (dst_ptr, src_ptr, nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SplitAp5::update_filters ()
{
	assert (_sample_freq > 0);
	assert (_split_freq  > 0);

	const float		x1 = _ka + _kb;
	const float		x2 = _ka;

	// Bilinear transform
	const float    k =
		dsp::iir::TransSZBilin::compute_k_approx (_split_freq * _inv_fs);

	float          zb_1o [3];
	float          zb_1e [2];
	float          zb_0 [3];
	dsp::iir::TransSZBilin::map_s_to_z_ap1_approx (zb_1e,     k);
	dsp::iir::TransSZBilin::map_s_to_z_ap2_approx (zb_1o, x2, k);
	dsp::iir::TransSZBilin::map_s_to_z_ap2_approx (zb_0 , x1, k);

	Filter0 &	filter_0 = _band_split.use_filter_0 ();
	Filter1 &	filter_1 = _band_split.use_filter_1 ();
	Filter0 &	filter_f = _band_split.use_fix_filter ();
	filter_1.use_filter ().set_coef (zb_1e [0]);
	filter_1.use_filter (0).set_z_eq (zb_1o [0], zb_1o [1]);
	filter_0.set_z_eq (zb_0 [0], zb_0 [1]);
	filter_f.set_z_eq (zb_0 [0], zb_0 [1]);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
