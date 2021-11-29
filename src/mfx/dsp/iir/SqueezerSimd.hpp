/*****************************************************************************

        SqueezerSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SqueezerSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SqueezerSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/shape/Poly3L1Bias.h"

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



template <bool BR, class LFOP>
SqueezerSimd <BR, LFOP>::SqueezerSimd () noexcept
{
	update_eq ();
	clear_buffers ();
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::set_sample_freq (float fs) noexcept
{
	assert (fs > 0);

	_fs = fs;

	// Base: 1/256 at 44100 Hz and 8x-oversampling
	_gain_out_lerp_step = ((1.0f / 256) * (44100.0f * 8)) / _fs;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::set_freq (float f0) noexcept
{
	assert (f0 > 0);

	_freq = f0;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::set_freq_and_update_eq_fast (float f0) noexcept
{
	set_freq (f0);
	update_internal_variables_fast (
		_r, _g, _p, _gain_out, _fs, _freq, _reso
	);
}



template <bool BR, class LFOP>
float	SqueezerSimd <BR, LFOP>::get_freq () const noexcept
{
	return _freq;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::set_reso (float reso) noexcept
{
	assert (reso >= 0);

	_reso = reso;
}



template <bool BR, class LFOP>
float	SqueezerSimd <BR, LFOP>::get_reso () const noexcept
{
	return _reso;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::set_p1 (float p1) noexcept
{
	assert (p1 >= 0);
	assert (p1 <= 1);

	_p1 = p1;

	const float    max_scale = 1.0f;
	const float    max_val   = 1.0f / 1024;
	_br_scale     = std::max (_p1 * max_scale, max_val);
	_br_scale_inv = 1.0f / _br_scale;

	const float    p1_rev = 1 - _p1;
	_br_amt = 1 - p1_rev * p1_rev * p1_rev;
	_br_amt *= 1.5f;

	_lofi_op.config (_reso, _p1);
}



template <bool BR, class LFOP>
float	SqueezerSimd <BR, LFOP>::get_p1 () const noexcept
{
	return _p1;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::copy_z_eq (const SqueezerSimd <BR, LFOP> &other) noexcept
{
	assert (_fs == other._fs);

	_r                  = other._r;          
	_p                  = other._p;          
	_g                  = other._g;          
	_lofi_op            = other._lofi_op;    
   _gain_out           = other._gain_out;   
	_gain_out_cur       = other._gain_out_cur;
   _gain_out_lerp_step = other._gain_out_lerp_step;
	_br_scale           = other._br_scale;    
	_br_scale_inv       = other._br_scale_inv;
	_br_amt             = other._br_amt;      
	_freq               = other._freq;        
	_reso               = other._reso;        
	_p1                 = other._p1;          

}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::update_eq () noexcept
{
	update_internal_variables (
		_r, _g, _p, _gain_out, _fs, _freq, _reso
	);

	_lofi_op.config (_reso, _p1);
}



template <bool BR, class LFOP>
float	SqueezerSimd <BR, LFOP>::process_sample (float x) noexcept
{
	_gain_out_cur += (_gain_out - _gain_out_cur) * _gain_out_lerp_step;

	typedef dsp::shape::Poly3L1Bias <std::ratio <1, 8> > ClipperFdbk;

	// Feedback
	float          fdbk = _r * ClipperFdbk::process_sample (_y [4 - 1]);
	if (BR)
	{
		const float    fdbk_i =
			float (fstb::trunc_int (fdbk * _br_scale_inv)) * _br_scale;
		fdbk += (fdbk_i - fdbk) * _br_amt;
	}
	const float    xp = x - fdbk;

	// Fucks things up
	_y [3 - 1] = _lofi_op.process_sample (_y [3 - 1]);

	// Non-linear stuffs (parallelisable)
	const auto     ytmp   = fstb::Vf32::loadu (&_y [0]);
	const auto     shaped = shape_feedback (ytmp);

	// 4 poles
	const float    sv_0 = shaped.template extract <0> ();
	const float    sv_1 = shaped.template extract <1> ();
	const float    sv_2 = shaped.template extract <2> ();
	const float    sv_3 = shaped.template extract <3> ();
	_y [1 - 1] = _g * (xp         + _x  ) - _p * sv_0;
	_y [2 - 1] = _g * (_y [1 - 1] + sv_0) - _p * sv_1;
	_y [3 - 1] = _g * (_y [2 - 1] + sv_1) - _p * sv_2;
	_y [4 - 1] = _g * (_y [3 - 1] + sv_2) - _p * sv_3;

	_x = xp;

	const float    ret_val = _y [4 - 1] * _gain_out_cur;

	return ret_val;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int index = 0; index < nbr_spl; ++index)
	{
		dst_ptr [index] = process_sample (src_ptr [index]);
	}
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::process_block (float spl_ptr [], int nbr_spl) noexcept
{
	assert (spl_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int index = 0; index < nbr_spl; ++index)
	{
		spl_ptr [index] = process_sample (spl_ptr [index]);
	}
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::process_block_fm (float dst_ptr [], const float src_ptr [], const float freq_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (freq_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int index = 0; index < nbr_spl; ++index)
	{
		const float		freq = freq_ptr [index];
		set_freq_and_update_eq_fast (freq);
		dst_ptr [index] = process_sample (src_ptr [index]);
	}
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::clear_buffers () noexcept
{
	_x = 0;
	for (int stage = 0; stage < _nbr_stages; ++stage)
	{
		_y [stage] = 0;
	}

	_gain_out_cur = _gain_out;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::update_internal_variables (float &r, float &g, float &p, float &out_gain, float fs, float freq, float reso) noexcept
{
	assert (fs > 0);
	assert (freq > 0);
	assert (freq < fs * 0.5f);
	assert (reso >= 0);

	const float		f = freq / fs;

	// Resonance
	r = reso * (5.5f * 2) / (2 + f * (6 + f * (5 + f * 8)));

	// Cutoff
	// Mapped frequency for bilinear tranform
	const float    k = 1 / tanf (f * float (fstb::PI));
	g = float (1 / (1 + k));
	p = float ((1 - k) * g);

	// Compensation gain
	// The right forumla would be approximately a log fade from 1 for f = 20 Hz
	// to 1/3 for f = 20 kHz.
	// This formula is an approximation for x4 oversampling min
	assert (f <= 0.25f);
	const float    f_025 = f - 0.25f;
	const float    gain_curve = 0.5f + 24 * f_025 * f_025;
	out_gain = 1 - reso + reso * gain_curve;
}



template <bool BR, class LFOP>
void	SqueezerSimd <BR, LFOP>::update_internal_variables_fast (float &r, float &g, float &p, float &out_gain, float fs, float freq, float reso) noexcept
{
	assert (fs > 0);
	assert (freq > 0);
	assert (freq < fs * 0.5f);
	assert (reso >= 0);

	const float    f = freq / fs;

	r = reso * (5.5f * 2) / (2 + f * (6 + f * (5 + f * 8)));

	const float    k = float (1 / fstb::Approx::tan_taylor5 (f * fstb::PI));
	g = 1 / (1 + k);  /*** To do: direct polynomial approx from f. See pi::distapf::DistApf ***/
	p = (1 - k) * g;  /*** To do: direct polynomial approx from f. Note: p = 2g - 1 ***/

	assert (f <= 0.25);
	const float    f_025 = f - 0.25f;
	const float    gain_curve = 0.5f + 24 * f_025 * f_025;
	out_gain = 1 - reso + reso * gain_curve;
}



template <bool BR, class LFOP>
fstb::Vf32	SqueezerSimd <BR, LFOP>::shape_feedback (fstb::Vf32 x) noexcept
{
	const auto     c1   = fstb::Vf32 ( 1);
	const auto     cm1  = fstb::Vf32 (-1);
	const auto     fold = fstb::Vf32 ( 0.5f);
	const auto     xc   = fstb::limit (x, cm1, c1);
	const auto     xn  = xc - x;
	const auto     xna = fstb::abs (xn);
	x.mac (xna, xn * fold);

	return x;
}



}  // namesHeaderpace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SqueezerSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
