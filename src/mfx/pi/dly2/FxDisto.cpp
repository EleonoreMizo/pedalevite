/*****************************************************************************

        FxDisto.cpp
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

#include "fstb/def.h"
#include "fstb/Approx.h"
#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/FxDisto.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxDisto::set_sample_freq (double sample_freq)
{
	_env_pre.set_sample_freq (sample_freq);
	_env_post.set_sample_freq (sample_freq);
	_env_pre.set_time (0.008f);
	_env_post.set_time (0.008f);

	dsp::mix::Align::setup ();
}



void	FxDisto::set_amount (float amt)
{
	assert (amt >= 0);
	assert (amt <= 1);

	_amount_cur = amt;
}



void	FxDisto::set_foldback (float foldback)
{
	assert (foldback >= 0);
	assert (foldback <= 1);

	_fold_cur = foldback;
}



void	FxDisto::clear_buffers ()
{
	_env_pre.clear_buffers ();
	_env_post.clear_buffers ();
	_amount_old = _amount_cur;
	_fold_old   = _fold_cur;
	update_gains ();
}



void	FxDisto::process_block (float data_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	int            block_pos = 0;
	do
	{
		const int      max_len  = _resol;
		const int      work_len = std::min (nbr_spl - block_pos, max_len);
		process_block_sub (data_ptr + block_pos, work_len);
		block_pos += work_len;
	}
	while (block_pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxDisto::update_gains ()
{
	const float    k = 1 - _fold_cur * 0.25f;
	_gain_pre  = fstb::Approx::exp2 (_amount_cur * k * 8 - 2);

	_gain_post = 1.0f / _gain_pre;
	_gain_post = std::max (_gain_post, 1.0f);

	const int      nbr_steps = fstb::round_int (12 * _fold_cur);
	_clip_val = 1.5f * std::min (float (1 + nbr_steps), 8.0f);
}



void	FxDisto::process_block_sub (float data_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	// Volume detection
	float          vol_pre_beg_sq = _env_pre.get_state_no_sqrt ();
	float          vol_pre_end_sq =
		_env_pre.analyse_block_no_sqrt (data_ptr, nbr_spl);

	float          gain_pre_beg   = _gain_pre;
	float          gain_post_beg  = _gain_post;

	float          gain_pre_end   = gain_pre_beg;
	float          gain_post_end  = gain_post_beg;
	const bool     amt_chg_flag   = (
		   _amount_cur != _amount_old
		|| _fold_cur   != _fold_old
	);
	if (amt_chg_flag)
	{
		update_gains ();
		gain_pre_end  = _gain_pre;
		gain_post_end = _gain_post;
	}

	float          vol_post_beg_sq = vol_pre_beg_sq;
	float          vol_post_end_sq = vol_pre_end_sq;
	const bool     dist_flag =
		   (amt_chg_flag || ! fstb::is_null (_amount_cur, 1e-4f));
	if (! dist_flag)
	{
		// Try to get the _env_post up to date, in case we switch on the
		// distortion. Therefore there won't be a low or null post-volume which
		// could produce an extremely high gain, resulting in a loud click.
		_env_post.set_state_raw (vol_post_end_sq);
	}
	else
	{
		// Pre-scaling
		dsp::mix::Align::scale_1_vlrauto (
			data_ptr, nbr_spl, gain_pre_beg, gain_pre_end
		);

		// Waveshaping
		if (_fold_cur + _fold_old < 1e-3f)
		{
			process_softclip (data_ptr, nbr_spl);
		}
		else
		{
			process_foldback (data_ptr, nbr_spl);
		}

		// New volume detection with virtual post-scaling
		vol_post_beg_sq = _env_post.get_state_no_sqrt ();
		vol_post_end_sq =
			_env_post.analyse_block_no_sqrt (data_ptr, nbr_spl);
		vol_post_beg_sq *= gain_post_beg * gain_post_beg;
		vol_post_end_sq *= gain_post_end * gain_post_end;
	}

	// Volume compensation and limiting
	float          comp_gain_beg = 0;
	float          comp_gain_end = 0;
	if (! fstb::is_null (vol_post_beg_sq + vol_post_end_sq))
	{
		const float    nodiv0     = 1e-20f;
		const float    lim_lvl_sq = _limiter_level * _limiter_level;

#if defined (fstb_HAS_SIMD)

		const auto     lls_v = fstb::Vf32 (lim_lvl_sq);
		const auto     vol_pre_sq  =
			fstb::Vf32::set_pair (vol_pre_beg_sq, vol_pre_end_sq);
		auto           vol_post_sq =
			fstb::Vf32::set_pair (vol_post_beg_sq, vol_post_end_sq);
		const auto     nodiv0_v    = fstb::Vf32 (nodiv0);
		vol_post_sq = fstb::max (vol_post_sq, nodiv0_v);
		auto           comp_gain   = fstb::sqrt (
			fstb::min (vol_pre_sq, lls_v) / vol_post_sq
		);

#	if 0 // Not needed anymore because of the _env_post update
		// We need to limit the gain because sometimes (when the distortion
		// is turned on and the post-envelope is empty) both post- and pre-
		// volumes are very low, causing a sharp transient if the post-
		// volume is significantly lower.
		const auto     max_gain    = fstb::Vf32 (_gain_max_comp);
		comp_gain = fstb::min (comp_gain, max_gain);
#	endif

		comp_gain_beg = fstb::ToolsSimd::Shift <0>::extract (comp_gain);
		comp_gain_end = fstb::ToolsSimd::Shift <1>::extract (comp_gain);

#else // Reference implementation

		comp_gain_beg = sqrtf (
			  std::min (vol_pre_beg_sq, lim_lvl_sq)
			/ std::max (vol_post_beg_sq, nodiv0)
		);
		comp_gain_end = sqrtf (
			  std::min (vol_pre_end_sq, lim_lvl_sq)
			/ std::max (vol_post_end_sq, nodiv0)
		);

#endif

		if (dist_flag)
		{
			// We must apply the post-scaling here
			comp_gain_beg *= gain_post_beg;
			comp_gain_end *= gain_post_end;
		}
	}

	if (comp_gain_beg == 0 && comp_gain_end == 0)
	{
		// Beg == 0 and end is not: does not ramp to avoid smoothing a fast
		// attack
		dsp::mix::Align::clear (data_ptr, nbr_spl);
	}
	else if (comp_gain_beg == comp_gain_end)
	{
		if (comp_gain_beg != 1)
		{
			dsp::mix::Align::scale_1_v (data_ptr, nbr_spl, comp_gain_end);
		}
	}
	else
	{
		dsp::mix::Align::scale_1_vlr (
			data_ptr, nbr_spl, comp_gain_beg, comp_gain_end
		);
	}

	_amount_old = _amount_cur;
	_fold_old   = _fold_cur;
}



// y = x - (4/27) * x^3
void	FxDisto::process_softclip (float data_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	const float    a  = -4.0f / 27;
	const float    mi = -1.5f;
	const float    ma = +1.5f;

#if 1
	const auto     v_a  = fstb::Vf32 (a);
	const auto     v_mi = fstb::Vf32 (mi);
	const auto     v_ma = fstb::Vf32 (ma);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x  = fstb::Vf32::load (data_ptr + pos);
		x = fstb::limit (x, v_mi, v_ma);

		x.mac (v_a * x, x * x);
		x.store (data_ptr + pos);
	}

#else // Reference implementation

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		float          x = data_ptr [pos];
		x  = std::min (x, ma);
		x  = std::max (x, mi);
		x += (a * x) * (x * x);
		data_ptr [pos] = x;
	}

#endif
}



// v = abs (abs (x - 6 * round (x/6) - 3/2) - 3) - 3/2
// y = v - (4/27) * v^3
void	FxDisto::process_foldback (float data_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	const float    a    = -4.0f / 27;
	const float    mi   = -_clip_val;
	const float    ma   =  _clip_val;
	const float    c1_6 =  1.0f / 6 ;
	const float    c6_1 = -6.0f;
	const float    ofs1 =  1.5f;
	const float    ofs2 =  3.0f;

#if 1

	const auto     v_a    = fstb::Vf32 (a   );
	const auto     v_mi   = fstb::Vf32 (mi  );
	const auto     v_ma   = fstb::Vf32 (ma  );
	const auto     v_c1_6 = fstb::Vf32 (c1_6);
	const auto     v_c6_1 = fstb::Vf32 (c6_1);
	const auto     v_ofs1 = fstb::Vf32 (ofs1);
	const auto     v_ofs2 = fstb::Vf32 (ofs2);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x  = fstb::Vf32::load (data_ptr + pos);
		x = fstb::limit (x, v_mi, v_ma);

		const auto     u  = fstb::ToolsSimd::conv_s32_to_f32 (
			fstb::ToolsSimd::round_f32_to_s32 (x * v_c1_6)
		);
		x.mac (u, v_c6_1);
		x -= v_ofs1;
		x  = fstb::abs (x);
		x -= v_ofs2;
		x  = fstb::abs (x);
		x -= v_ofs1;

		x.mac (v_a * x, x * x);
		x.store (data_ptr + pos);
	}

#else // Reference implementation

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		float          x = data_ptr [pos];

		x = std::min (x, ma);
		x = std::max (x, mi);
		const float    u = fstb::round (x * c1_6);
		x = fabsf (fabsf (x + u * c6_1 - ofs1) - ofs2) - ofs1;
		x += (a * x) * (x * x);

		data_ptr [pos] = x;
	}

#endif
}



const float	FxDisto::_gain_max_comp = 16;



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
