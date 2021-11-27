/*****************************************************************************

        Eq.cpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Eq.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Eq::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	_dirty_flag  = true;
}



void	Eq::clear_buffers ()
{
	if (_dirty_flag)
	{
		update_filter ();
	}
	_filter.clear_buffers ();
}



void	Eq::set_freq_lo (float f)
{
	assert (f > 0);

	_freq_lo    = f;
	_dirty_flag = true;
}



void	Eq::set_freq_hi (float f)
{
	assert (f > 0);

	_freq_hi    = f;
	_dirty_flag = true;
}



void	Eq::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	if (_dirty_flag)
	{
		update_filter ();
	}

	_filter.process_block (dst_ptr, src_ptr, nbr_spl);
}



bool	Eq::is_active () const
{
	return (
		   _freq_lo >= float (Cst::_eq_freq_min) +   0.1f
		|| _freq_hi <= float (Cst::_eq_freq_max) - 100.0f
	);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Eq::update_filter ()
{
#if 1 /*** To do: check which method is the fastest ***/

	const float    mult = float (fstb::PI) * _inv_fs;
	const float    fh   = fstb::Approx::tan_taylor5 (_freq_hi * mult);
	const float    fli  =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_lo * _inv_fs);
	const float    mh   = fh * fli;

	const float    bs [3] = {  0, mh    , 0 };
	const float    as [3] = { mh, mh + 1, 1 };
	float          bz [3];
	float          az [3];
	const float    k = fli;
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);

#else

	const auto     fv   = fstb::Vf32::set_pair (_freq_lo, _freq_hi);
	const auto     ifs  = fstb::Vf32 (_inv_fs);
	const auto     kv   = dsp::iir::TransSZBilin::compute_k_approx (fv * ifs);

	const auto     zero = fstb::Vf32::zero ();
	const auto     one  = fstb::Vf32 (1);
	const fstb::Vf32     s_eq_b [2] =
	{
		fstb::Vf32::set_pair (1, 0),
		fstb::Vf32::set_pair (0, 1)
	};
	const fstb::Vf32     s_eq_a [2] = { one, one };

	fstb::Vf32     z_eq_b [2];
	fstb::Vf32     z_eq_a [2];
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		z_eq_b, z_eq_a, s_eq_b, s_eq_a, kv
	);

	const auto     b0z   = fstb::Vf32::load (&z_eq_b [0]);
	const auto     b1z   = fstb::Vf32::load (&z_eq_b [1]);
	const auto     a0z   = fstb::Vf32::load (&z_eq_a [0]);
	const auto     a1z   = fstb::Vf32::load (&z_eq_a [1]);

#if 0

	const float    b0z_l = fstb::ToolsSimd::Shift <0>::extract (b0z);
	const float    b0z_h = fstb::ToolsSimd::Shift <1>::extract (b0z);
	const float    b1z_l = fstb::ToolsSimd::Shift <0>::extract (b1z);
	const float    b1z_h = fstb::ToolsSimd::Shift <1>::extract (b1z);
	const float    a0z_l = fstb::ToolsSimd::Shift <0>::extract (a0z);
	const float    a0z_h = fstb::ToolsSimd::Shift <1>::extract (a0z);
	const float    a1z_l = fstb::ToolsSimd::Shift <0>::extract (a1z);
	const float    a1z_h = fstb::ToolsSimd::Shift <1>::extract (a1z);

	const float    bz [3] =
	{
		b0z_l * b0z_h,
		b0z_l * b1z_h + b1z_l * b0z_h,
		b1z_l * b1z_h
	};
	const float    az [3] =
	{
		a0z_l * a0z_h,
		a0z_l * a1z_h + a1z_l * a0z_h,
		a1z_l * a1z_h
	};

#else

	fstb::Vf32     x0z_l;
	fstb::Vf32     x0z_h;
	fstb::ToolsSimd::deinterleave_f32 (x0z_l, x0z_h, b0z, a0z);
	fstb::Vf32     x1z_l;
	fstb::Vf32     x1z_h;
	fstb::ToolsSimd::deinterleave_f32 (x1z_l, x1z_h, b1z, a1z);
	const auto     x0z = x0z_l * x0z_h;
	const auto     x1z = x0z_l * x1z_h + x1z_l * x0z_h;
	const auto     x2z = x1z_l * x1z_h;
	const float    bz [3] =
	{
		fstb::ToolsSimd::Shift <0>::extract (x0z),
		fstb::ToolsSimd::Shift <0>::extract (x1z),
		fstb::ToolsSimd::Shift <0>::extract (x2z)
	};
	const float    az [3] =
	{
		fstb::ToolsSimd::Shift <1>::extract (x0z),
		fstb::ToolsSimd::Shift <1>::extract (x1z),
		fstb::ToolsSimd::Shift <1>::extract (x2z)
	};

#endif

#endif

	_filter.set_z_eq (bz, az);

	_dirty_flag = false;
}



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
