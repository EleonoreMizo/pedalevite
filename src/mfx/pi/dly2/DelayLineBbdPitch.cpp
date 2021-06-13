/*****************************************************************************

        DelayLineBbdPitch.cpp
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
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dly2/DelayLineBbdPitch.h"

#include <cassert>
#include <cmath>


namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DelayLineBbdPitch::DelayLineBbdPitch ()
:	_sample_freq (0)
,	_inv_fs (0)
,	_buf_zone_ptr (nullptr)
,	_buf_len (0)
,	_interp ()
,	_bbd ()
,	_fx ()
,	_eq ()
,	_ramp_time (0)
,	_input_gain (1)
,	_bbd_speed (1)
,	_fdbk (0.5f)
,	_pan (0)
,	_vol (0)
,	_duck_amt (0)
,	_duck_sens (1)
,	_duck_sens_inv (1)
,	_block_len (0)
,	_delay_time (0)
,	_dly_min (-1)
,	_dly_max (-1)
,	_dly_min_assumed (-1)
,	_grain_arr ()
,	_rate_grain (1)
,	_rate_inf (-4)
,	_rate_sup (+4)
,	_time_cur (0)
,	_prog_time (0)
,	_prog_trans(-1)
,	_xfade_dn (64)
,	_xfade_dp (4096)
,	_xfade_pos (-1)
,	_grain_cur (0)
,	_ps_flag (false)
{
	dsp::mix::Generic::setup ();

	_xfade_shape_sn.set_duration (0.005f);
	_xfade_shape_sp.set_duration (0.125f, 0.5f);
}



void	DelayLineBbdPitch::init (int line_index, const ParamDescSet &desc_set, ParamStateSet &state_set)
{
	assert (line_index >= 0);
	assert (line_index < Cst::_nbr_lines);

	_fx.init (line_index, desc_set, state_set);
}



void	DelayLineBbdPitch::reset (double sample_freq, int max_block_size, float buf_zone_ptr [], int buf_len)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);
	assert (fstb::DataAlign <true>::check_ptr (buf_zone_ptr));
	assert ((buf_len & 3) == 0);

	_sample_freq  = float (    sample_freq);
	_inv_fs       = float (1 / sample_freq);

	_buf_zone_ptr = buf_zone_ptr;
	_buf_len      = buf_len;

	const int      max_dly_spl = fstb::ceil_int (
		sample_freq * Cst::_max_delay * (1 / 1000.0)
	);
	_bbd.init (max_dly_spl, _interp, 0);
	_dly_max = float (max_dly_spl);
	_fx.reset (sample_freq, max_block_size);
	_eq.set_sample_freq (sample_freq);
	_xfade_shape_sn.set_sample_freq (sample_freq);
	_xfade_shape_sp.set_sample_freq (sample_freq);
	_xfade_dn = _xfade_shape_sn.get_len ();
	_xfade_dp = _xfade_shape_sp.get_len ();

	set_ramp_time (max_block_size);

	clear_buffers ();

	_dly_min = _bbd.compute_min_delay ();
}



void	DelayLineBbdPitch::clear_buffers ()
{
	_bbd.clear_buffers ();
	_fx.clear_buffers ();
	_eq.clear_buffers ();
	_block_len = 0;

	_input_gain.clear_buffers ();
	_fdbk      .clear_buffers ();
	_pan       .clear_buffers ();
	_vol       .clear_buffers ();
	_delay_time.clear_buffers ();
	_duck_amt  .clear_buffers ();

	_prog_time   = _delay_time.get_tgt ();
	_time_cur    = _prog_time;
	_prog_trans  = -1;
	_xfade_pos   = -1;
	_grain_cur   = 0;

	Grain &        g = _grain_arr [_grain_cur];
	g._dly_cur   = _time_cur;
	g._dly_tgt   = _time_cur;
	g._dly_stp   = 0;
	g._trans_pos = -1;
}



// Call this first each frame, before changing the parameters
void	DelayLineBbdPitch::set_ramp_time (int ramp_time)
{
	assert (ramp_time > 0);

	if (ramp_time != _ramp_time)
	{
		_ramp_time = ramp_time;
		const float    time_step = fstb::rcp_uint <float> (ramp_time);
		_input_gain.set_time (ramp_time, time_step);
		_fdbk      .set_time (ramp_time, time_step);
		_pan       .set_time (ramp_time, time_step);
		_vol       .set_time (ramp_time, time_step);
		_duck_amt  .set_time (ramp_time, time_step);
		_delay_time.set_time (ramp_time, time_step);
	}
}



void	DelayLineBbdPitch::set_input_gain (float g)
{
	assert (_ramp_time > 0);

	_input_gain.set_val (g);
}



// t in seconds
void	DelayLineBbdPitch::set_delay_time (float t)
{
	assert (_ramp_time > 0);

	_delay_time.set_val (t * _sample_freq);
}



void	DelayLineBbdPitch::set_bbd_speed (float spd)
{
	assert (_ramp_time > 0);

	_bbd_speed = spd;
	_bbd.set_speed (spd);
	_dly_min = _bbd.compute_min_delay ();
}



void	DelayLineBbdPitch::set_grain_pitch (float ratio)
{
	const bool     ps_flag_old = _ps_flag;
	_rate_grain = ratio;
	_ps_flag    = (! fstb::is_eq (ratio, 1.0f, 1e-3f));

	if (_xfade_pos >= 0)
	{
		if (_ps_flag && ! ps_flag_old)
		{
			_xfade_pos = _xfade_pos * _xfade_dp / _xfade_dn;
		}
		else if (! _ps_flag && ps_flag_old)
		{
			_xfade_pos = _xfade_pos * _xfade_dn / _xfade_dp;
		}
		assert (_xfade_pos < get_xfade_len ());
	}

	if (_ps_flag && ! is_time_change_programmed ())
	{
		_prog_time  = _time_cur;
		_prog_trans = 0;
	}
}



void	DelayLineBbdPitch::set_feedback (float fdbk)
{
	assert (_ramp_time > 0);

	_fdbk.set_val (fdbk);
}



void	DelayLineBbdPitch::set_freq_lo (float f)
{
	assert (_ramp_time > 0);

	_eq.set_freq_lo (f);
}



void	DelayLineBbdPitch::set_freq_hi (float f)
{
	assert (_ramp_time > 0);

	_eq.set_freq_hi (f);
}



void	DelayLineBbdPitch::set_vol (float v)
{
	assert (_ramp_time > 0);

	_vol.set_val (v);
}



void	DelayLineBbdPitch::set_pan (float p)
{
	assert (_ramp_time > 0);

	_pan.set_val (p);
}



void	DelayLineBbdPitch::set_duck_sensitivity (float s)
{
	assert (s > 0);

	_duck_sens     = s;
	_duck_sens_inv = 1.0f / _duck_sens;
}



void	DelayLineBbdPitch::set_duck_amount (float amt)
{
	assert (amt >= 0);
	assert (amt <= 1);

	_duck_amt.set_val (amt);
}



// Before calling: set the parameters for the frame
int	DelayLineBbdPitch::start_and_compute_max_proc_len ()
{
	assert (_ramp_time > 0);

	// Checks the minimum delay time (in samples) reached during this frame
	_dly_min_assumed = find_min_delay_time ();

	// Now computes the maximum processing length
	const int      max_len =
		_bbd.estimate_max_one_shot_proc_w_feedback (_dly_min_assumed);

	return max_len;
}



void	DelayLineBbdPitch::read_line (float dst_ptr [], int nbr_spl, float &f_beg, float &f_end)
{
	int            src_pos = 0;
	int            transition_time = nbr_spl;
	_block_len = nbr_spl;

	_input_gain.tick (nbr_spl);
	_fdbk      .tick (nbr_spl);
	_pan       .tick (nbr_spl);
	_vol       .tick (nbr_spl);
	_delay_time.tick (nbr_spl);
	_duck_amt  .tick (nbr_spl);

	// Read data from line
	float          dly_end = _delay_time.get_end ();
	dly_end = std::max (dly_end, _dly_min);
	_prog_time  = dly_end;
	_prog_trans = transition_time;

	check_and_start_transition ();

	float *        tmp_ptr = use_buf (Buf_TMP_0);

	int            dst_pos = 0;
	do
	{
		int            work_len = nbr_spl - dst_pos;
		Grain &        g_cur    = _grain_arr [    _grain_cur];
		Grain &        g_old    = _grain_arr [1 - _grain_cur];

		work_len = g_cur.clip_block_len (work_len);
		const bool     xfade_flag = (_xfade_pos >= 0);
		int            xfade_dur  = 0;
		const float *  xfade_shape_ptr = nullptr;
		if (xfade_flag)
		{
			xfade_dur       = get_xfade_len ();
			xfade_shape_ptr = use_xfade_shape ();
			work_len        = std::min (work_len, xfade_dur - _xfade_pos);
			work_len        = std::min (work_len, _buf_len);
			work_len        = g_old.clip_block_len (work_len);
		}

		process_grain (
			g_cur,
			dst_ptr + dst_pos,
			src_pos + dst_pos,
			work_len
		);

		if (xfade_flag)
		{
			process_grain (
				g_old,
				tmp_ptr,
				src_pos + dst_pos,
				work_len
			);

			const float    inv_dur  = 1.0f / float (xfade_dur);
			const float    lerp_beg = float (_xfade_pos           ) * inv_dur;
			const float    lerp_end = float (_xfade_pos + work_len) * inv_dur;

			if (xfade_shape_ptr == nullptr)
			{
				// Default: linear
				dsp::mix::Generic::scale_1_vlr (
					dst_ptr + dst_pos,
					work_len,
					lerp_beg,
					lerp_end
				);
				dsp::mix::Generic::mix_1_1_vlr (
					dst_ptr + dst_pos,
					tmp_ptr,
					work_len,
					1 - lerp_beg,
					1 - lerp_end
				);
			}
			else
			{
				// Custom shape
				dsp::mix::Generic::copy_xfade_3_1 (
					dst_ptr + dst_pos,
					tmp_ptr,
					dst_ptr + dst_pos,
					xfade_shape_ptr + _xfade_pos,
					work_len
				);
			}

			_xfade_pos += work_len;
			if (_xfade_pos >= xfade_dur)
			{
				_xfade_pos = -1;
			}
		}

		// Updates the time position of the programmed delay change
		if (is_time_change_programmed ())
		{
			_prog_trans -= work_len;
			_prog_trans  = std::max (_prog_trans, 0);
		}

		check_and_start_transition ();

		dst_pos += work_len;
	}
	while (dst_pos < nbr_spl);

	// Processes effects
	_fx.process_block (dst_ptr, nbr_spl);

	// Feedback data
	f_beg = _fdbk.get_beg ();
	f_end = _fdbk.get_end ();
}



// Inbetween: feedback matrix processing

// out_ptr_arr: stereo line output, after mixing data. Written position is
// given by pos_out.
// fx_ptr: output from the previous call to read_line(),
// src_ptr: line input
// fdbk_ptr: processed feedback, to be mixed
// duck_ptr: detected envelope for ducking, squared.
void	DelayLineBbdPitch::finish_processing (float * const out_ptr_arr [2], const float fx_ptr [], const float src_ptr [], const float fdbk_ptr [], const float duck_ptr [], int pos_out, bool stereo_flag, bool mix_flag)
{
	assert (_block_len > 0);

	// Output
	float *        tmp_ptr     = use_buf (Buf_TMP_0);
	const float *  out_src_ptr = tmp_ptr;
	if (! _eq.is_active ())
	{
		out_src_ptr = fx_ptr;
	}
	else
	{
		_eq.process_block (tmp_ptr, fx_ptr, _block_len);
	}

	float          vol_beg      = _vol.get_beg ();
	float          vol_end      = _vol.get_end ();
	
	const float    duck_amt_beg = _duck_amt.get_beg ();
	const float    duck_amt_end = _duck_amt.get_end ();
	if (duck_amt_beg + duck_amt_end >= 1e-3f)
	{
		float          env_beg  = duck_ptr [0             ];
		float          env_end  = duck_ptr [_block_len - 1];

#if defined (fstb_HAS_SIMD)
		auto           env      = fstb::ToolsSimd::set_2f32 (env_beg, env_end);
		env  = fstb::ToolsSimd::sqrt_approx (env);
		env *= fstb::ToolsSimd::set1_f32 (_duck_sens_inv);
		const auto     amt      =
			fstb::ToolsSimd::set_2f32 (duck_amt_beg, duck_amt_end);
		auto           dvol     = fstb::ToolsSimd::max_f32 (
			fstb::ToolsSimd::set1_f32 (1) - amt * env,
			fstb::ToolsSimd::set_f32_zero ()
		);
		auto           vol      = fstb::ToolsSimd::set_2f32 (vol_beg, vol_end);
		vol *= dvol;
		fstb::ToolsSimd::extract_2f32 (vol_beg, vol_end, vol);

#else // Reference implementation
		env_beg = sqrtf (env_beg) * _duck_sens_inv;
		env_end = sqrtf (env_end) * _duck_sens_inv;
		const float    dvol_beg = std::max (1.f - duck_amt_beg * env_beg, 0.f);
		const float    dvol_end = std::max (1.f - duck_amt_end * env_end, 0.f);
		vol_beg *= dvol_beg;
		vol_end *= dvol_end;

#endif
	}

	// Stereo output
	if (stereo_flag)
	{
		const float    lvl_l_beg = (1 - _pan.get_beg ()) * 0.5f;
		const float    lvl_l_end = (1 - _pan.get_end ()) * 0.5f;
		const float    lvl_r_beg = 1 - lvl_l_beg;
		const float    lvl_r_end = 1 - lvl_l_end;
		if (mix_flag)
		{
			MixUnalign::mix_spread_1_2_vlrauto (
				out_ptr_arr [0] + pos_out,
				out_ptr_arr [1] + pos_out,
				out_src_ptr,
				_block_len,
				lvl_l_beg * vol_beg,
				lvl_r_beg * vol_beg,
				lvl_l_end * vol_end,
				lvl_r_end * vol_end
			);
		}
		else
		{
			MixUnalign::copy_spread_1_2_vlrauto (
				out_ptr_arr [0] + pos_out,
				out_ptr_arr [1] + pos_out,
				out_src_ptr,
				_block_len,
				lvl_l_beg * vol_beg,
				lvl_r_beg * vol_beg,
				lvl_l_end * vol_end,
				lvl_r_end * vol_end
			);
		}
	}

	// Mono ouput
	else
	{
		if (mix_flag)
		{
			MixUnalign::mix_1_1_vlrauto (
				out_ptr_arr [0] + pos_out,
				out_src_ptr,
				_block_len,
				vol_beg,
				vol_end
			);
		}
		else
		{
			MixUnalign::copy_1_1_vlrauto (
				out_ptr_arr [0] + pos_out,
				out_src_ptr,
				_block_len,
				vol_beg,
				vol_end
			);
		}
	}

	// Feedback
	MixUnalign::copy_1_1 (tmp_ptr, fdbk_ptr, _block_len);
	MixUnalign::mix_1_1_vlrauto (
		tmp_ptr,
		src_ptr,
		_block_len,
		_input_gain.get_beg (),
		_input_gain.get_end ()
	);

	_bbd.push_block (tmp_ptr, _block_len);

	// Next
	_block_len = 0;
}



bool	DelayLineBbdPitch::is_time_change_programmed () const
{
	return (_prog_trans >= 0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	DelayLineBbdPitch::Grain::is_ramping () const
{
	return (_trans_len > 0);
}



int	DelayLineBbdPitch::Grain::clip_block_len (int len) const
{
	assert (len > 0);

	if (is_ramping ())
	{
		assert (_trans_len > _trans_pos);
		len = std::min (len, _trans_len - _trans_pos);
	}

	return len;
}



float *	DelayLineBbdPitch::use_buf (Buf buf) const
{
	assert (_buf_zone_ptr != nullptr);
	assert (buf >= 0);
	assert (buf < Buf_NBR_ELT);

	return _buf_zone_ptr + _buf_len * buf;
}



// All delay times are in samples
float	DelayLineBbdPitch::find_min_delay_time () const
{
	float          delay_min = std::min (
		_delay_time.get_beg (),
		_delay_time.get_tgt ()
	);
	if (_ps_flag)
	{
		const Grain &  g = _grain_arr [_grain_cur];
		delay_min = std::min (delay_min, g._dly_cur);
		delay_min = std::min (delay_min, g._dly_tgt);
	}
	if (_xfade_pos >= 0)
	{
		const Grain &  g = _grain_arr [1 - _grain_cur];
		delay_min = std::min (delay_min, g._dly_cur);
		delay_min = std::min (delay_min, g._dly_tgt);
	}
	delay_min = std::max (delay_min, _dly_min);

	return delay_min;
}



void	DelayLineBbdPitch::check_and_start_transition ()
{
	// Check if we need a transition
	bool           req_trans_flag = _ps_flag;
	bool           req_xfade_flag = _ps_flag;
	float          dly            = _time_cur;
	const int      xfade_dur      = get_xfade_len ();
	int            nbr_spl        = xfade_dur;
	if (is_time_change_programmed ())
	{
		req_trans_flag = true;
		dly            = _prog_time;
		nbr_spl        = _prog_trans;
	}

	if (req_trans_flag)
	{
		const bool     time_chg_flag       = (_time_cur != dly);
		const int      ovrspl_l2           = _bbd.get_ovrspl_l2 ();
		const int      nbr_spl_normal_rate = nbr_spl >> ovrspl_l2;
		if (nbr_spl_normal_rate == 0)
		{
			req_xfade_flag |= time_chg_flag;
		}
		else
		{
			const float    nbr_spl_div =
				fstb::rcp_uint <float> (nbr_spl_normal_rate);
			const float    rate = 1 + (_time_cur - dly) * nbr_spl_div;

			req_xfade_flag |= (rate < _rate_inf || rate > _rate_sup);
		}

		// With crossfading
		if (req_xfade_flag)
		{
			if (_xfade_pos < 0)
			{
				// Grain switch
				const int      pos_other = 1 - _grain_cur;
				Grain &        g_old     = _grain_arr [_grain_cur];
				Grain &        g_new     = _grain_arr [pos_other ];
				_grain_cur = pos_other;

				// New grain data
				g_new._dly_cur   = dly;
				g_new._dly_tgt   = dly;
				g_new._dly_stp   = 0;
				g_new._trans_pos = -1;

				// If pitch shifting is activated, applies it on both parts
				if (_ps_flag)
				{
					const float    add_step = 1 - _rate_grain;
					const float    ofs_tgt  = add_step * xfade_dur;

					if (! g_old.is_ramping ())
					{
						g_old._dly_stp   = add_step;
						g_old._dly_tgt   = g_old._dly_cur + ofs_tgt;
						g_old._trans_len = xfade_dur;
						g_old._trans_pos = 0;
					}

					g_new._dly_stp    = add_step;
					g_new._dly_tgt   += ofs_tgt;
					g_new._trans_len  = xfade_dur;
					g_new._trans_pos  = 0;
				}

				// No pitch shifting: if there was a transition, extends it
				// during the fadeout
				else if (g_old._dly_stp != 0)
				{
					g_old._trans_len = xfade_dur;
					g_old._trans_pos = std::min (g_old._trans_pos, g_old._trans_len - 1);
				}

				// Starts crossfading
				_xfade_pos = 0;

				_time_cur   = dly;
				_prog_trans = -1;
			}
		}

		// Simple time transition
		else if (time_chg_flag)
		{
			Grain &        g = _grain_arr [_grain_cur];
			if (! g.is_ramping ())
			{
				const float    dif = dly - g._dly_cur;
				assert (nbr_spl > 0);
				g._dly_stp   = dif / nbr_spl;
				g._dly_tgt   = dly;
				g._trans_pos = 0;
				g._trans_len = nbr_spl;
			}

			_time_cur   = dly;
			_prog_trans = -1;
		}
	}
}



// Returns true if the ramp is terminated
bool	DelayLineBbdPitch::process_grain (Grain &g, float dest_ptr [], int src_pos, int nbr_spl)
{
	assert (dest_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (g._trans_len <= 0 || nbr_spl <= g._trans_len);

	bool           term_flag = false;

	float          dly_beg   = g._dly_cur;
	float          dly_end   = dly_beg;
	if (g.is_ramping ())
	{
		g._dly_cur   += nbr_spl * g._dly_stp;
		g._trans_pos += nbr_spl;
		if (g._trans_pos >= g._trans_len)
		{
			g._trans_len = -1;
			g._dly_cur   = g._dly_tgt;
			term_flag    = true;
		}
		dly_end = g._dly_cur;
	}

	dly_beg = fstb::limit (dly_beg, _dly_min_assumed, _dly_max);
	dly_end = fstb::limit (dly_end, _dly_min_assumed, _dly_max);

	_bbd.read_block (
		dest_ptr,
		nbr_spl,
		dly_beg,
		dly_end,
		src_pos
	);

	return term_flag;
}



int	DelayLineBbdPitch::get_xfade_len () const
{
	return _ps_flag ? _xfade_dp : _xfade_dn;
}



const float *	DelayLineBbdPitch::use_xfade_shape () const
{
	return _ps_flag
		? _xfade_shape_sp.use_shape ()
		: _xfade_shape_sn.use_shape ();
}



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
