/*****************************************************************************

        DelayLineBbd.cpp
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
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dly2/DelayLineBbd.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DelayLineBbd::DelayLineBbd ()
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
,	_delay_time_lb (0)
,	_xfade_len (0)
,	_xfade_pos (0)
,	_xfade_dly_old (0)
,	_xfade_shape ()
{
	// Nothing
}



void	DelayLineBbd::init (int line_index, const ParamDescSet &desc_set, ParamStateSet &state_set)
{
	assert (line_index >= 0);
	assert (line_index < Cst::_nbr_lines);

	_fx.init (line_index, desc_set, state_set);
}



void	DelayLineBbd::reset (double sample_freq, int max_block_size, float buf_zone_ptr [], int buf_len)
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
	_fx.reset (sample_freq, max_block_size);
	_eq.set_sample_freq (sample_freq);
	_xfade_shape.set_sample_freq (sample_freq);
	_xfade_len = _xfade_shape.get_len ();

	set_ramp_time (max_block_size);

	clear_buffers ();

	_delay_time_lb = _bbd.compute_min_delay ();
}



void	DelayLineBbd::clear_buffers ()
{
	_bbd.clear_buffers ();
	_fx.clear_buffers ();
	_eq.clear_buffers ();
	_xfade_pos = 0;
	_block_len = 0;

	_input_gain.clear_buffers ();
	_fdbk      .clear_buffers ();
	_pan       .clear_buffers ();
	_vol       .clear_buffers ();
	_delay_time.clear_buffers ();
	_duck_amt  .clear_buffers ();
}



// Call this first each frame, before changing the parameters
void	DelayLineBbd::set_ramp_time (int ramp_time)
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



void	DelayLineBbd::set_input_gain (float g)
{
	assert (_ramp_time > 0);

	_input_gain.set_val (g);
}



// t in seconds
void	DelayLineBbd::set_delay_time (float t)
{
	assert (_ramp_time > 0);

	_delay_time.set_val (t * _sample_freq);
}



void	DelayLineBbd::set_bbd_speed (float spd)
{
	assert (_ramp_time > 0);

	_bbd_speed = spd;
	_bbd.set_speed (spd);
	_delay_time_lb = _bbd.compute_min_delay ();
}



void	DelayLineBbd::set_feedback (float fdbk)
{
	assert (_ramp_time > 0);

	_fdbk.set_val (fdbk);
}



void	DelayLineBbd::set_freq_lo (float f)
{
	assert (_ramp_time > 0);

	_eq.set_freq_lo (f);
}



void	DelayLineBbd::set_freq_hi (float f)
{
	assert (_ramp_time > 0);

	_eq.set_freq_hi (f);
}



void	DelayLineBbd::set_vol (float v)
{
	assert (_ramp_time > 0);

	_vol.set_val (v);
}



void	DelayLineBbd::set_pan (float p)
{
	assert (_ramp_time > 0);

	_pan.set_val (p);
}



void	DelayLineBbd::set_duck_sensitivity (float s)
{
	assert (s > 0);

	_duck_sens     = s;
	_duck_sens_inv = 1.0f / _duck_sens;
}



void	DelayLineBbd::set_duck_amount (float amt)
{
	assert (amt >= 0);
	assert (amt <= 1);

	_duck_amt.set_val (amt);
}



// Before calling: set the parameters for the frame
int	DelayLineBbd::start_and_compute_max_proc_len ()
{
	assert (_ramp_time > 0);

	// Checks the minimum delay time (in samples) reached during this frame
	const float    dly_min = find_min_delay_time ();

	// Now computes the maximum processing length
	const int      max_len =
		_bbd.estimate_max_one_shot_proc_w_feedback (dly_min);

	return max_len;
}



void	DelayLineBbd::read_line (float dst_ptr [], int nbr_spl, float &f_beg, float &f_end)
{
	_block_len = nbr_spl;

	_input_gain.tick (nbr_spl);
	_fdbk      .tick (nbr_spl);
	_pan       .tick (nbr_spl);
	_vol       .tick (nbr_spl);
	_delay_time.tick (nbr_spl);
	_duck_amt  .tick (nbr_spl);

	// Read data from line
	float          dly_beg = _delay_time.get_beg ();
	float          dly_end = _delay_time.get_end ();
	dly_beg = std::max (dly_beg, _delay_time_lb);
	dly_end = std::max (dly_end, _delay_time_lb);

	// Handles new delay times only when not crossfading
	if (_xfade_pos <= 0 && dly_beg != dly_end)
	{
		// The new delay time requires crossfading?
		const float    read_speed =
			1 - (dly_end - dly_beg) * fstb::rcp_uint <float> (nbr_spl);
		if (fabsf (read_speed) > _speed_limit)
		{
			_xfade_pos     = _xfade_len;
			_xfade_dly_old = _delay_time.get_beg ();
			dly_beg        = dly_end;
		}
	}

	// Resampling or static
	if (_xfade_pos <= 0)
	{
		_bbd.read_block (dst_ptr, nbr_spl, dly_beg, dly_end, 0);
	}

	// Crossfade
	else
	{
		float *        tmp_old_ptr = use_buf (Buf_TMP_0);
		float *        tmp_cur_ptr = use_buf (Buf_TMP_1);
		const float    dly_old     = std::max (_xfade_dly_old, _delay_time_lb);
		const int      overlap_len = std::min (nbr_spl, _xfade_pos);
		_bbd.read_block (tmp_old_ptr, overlap_len, dly_old, dly_old, 0);
		_bbd.read_block (tmp_cur_ptr, nbr_spl    , dly_beg, dly_end, 0);
		MixUnalign::copy_xfade_3_1 (
			dst_ptr,
			tmp_old_ptr,
			tmp_cur_ptr,
			_xfade_shape.use_shape () + _xfade_len - _xfade_pos,
			overlap_len
		);
		_xfade_pos -= overlap_len;

		// Resampling or static part after the crossfade
		const int      rem_len = nbr_spl - overlap_len;
		if (rem_len > 0)
		{
			MixUnalign::copy_1_1 (
				dst_ptr     + overlap_len,
				tmp_cur_ptr + overlap_len,
				rem_len
			);
		}
	}

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
void	DelayLineBbd::finish_processing (float * const out_ptr_arr [2], const float fx_ptr [], const float src_ptr [], const float fdbk_ptr [], const float duck_ptr [], int pos_out, bool stereo_flag, bool mix_flag)
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
		auto           env      = fstb::Vf32::set_pair (env_beg, env_end);
		env  = env.sqrt_approx ();
		env *= fstb::Vf32 (_duck_sens_inv);
		const auto     amt      =
			fstb::Vf32::set_pair (duck_amt_beg, duck_amt_end);
		const auto     dvol     = fstb::max (
			fstb::Vf32 (1) - amt * env,
			fstb::Vf32::zero ()
		);
		auto           vol      = fstb::Vf32::set_pair (vol_beg, vol_end);
		vol *= dvol;
		std::tie (vol_beg, vol_end) = vol.extract_pair ();

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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float *	DelayLineBbd::use_buf (Buf buf) const
{
	assert (_buf_zone_ptr != nullptr);
	assert (buf >= 0);
	assert (buf < Buf_NBR_ELT);

	return _buf_zone_ptr + _buf_len * buf;
}



// All delay times are in samples
float	DelayLineBbd::find_min_delay_time () const
{
	float          delay_min = std::min (
		_delay_time.get_beg (),
		_delay_time.get_end ()
	);
	if (_xfade_pos > 0)
	{
		delay_min = std::min (delay_min, _xfade_dly_old);
	}
	delay_min = std::max (delay_min, _delay_time_lb);

	return delay_min;
}



const float	DelayLineBbd::_speed_limit = 4.0f;



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
