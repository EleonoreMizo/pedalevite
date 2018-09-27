/*****************************************************************************

        Voice.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/pi/hcomb/Voice.h"
#include "fstb/Approx.h"
#include "fstb/DataAlign.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace hcomb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Voice::Voice ()
:	_sample_freq (44100)
,	_note (60)
,	_decay_time (1)
,	_level (1)
,	_filt_freq (4000)
,	_filt_reso (1)
,	_filt_damp (0)
,	_max_block_size (256)
,	_neg_flag (false)
,	_tmp_buf_ptr (0)
,	_gain (1)
,	_feedback (0)
,	_comb_freq (262)
,	_gain_old (_gain)
,	_feedback_old (_feedback)
,	_comb_freq_old (_comb_freq)
,	_comb_dirty_flag (true)
,	_filt_dirty_flag (true)
,	_comb_arr ()
{
	dsp::mix::Generic::setup ();
}



void	Voice::reset (double sample_freq, int max_block_size, float *tmp_buf_ptr)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);
	assert (fstb::DataAlign <true>::check_ptr (tmp_buf_ptr));

	_sample_freq    = float (sample_freq);
	_inv_fs         = float (1 / sample_freq);
	_max_block_size = max_block_size;
	_tmp_buf_ptr    = tmp_buf_ptr;

	for (auto &comb : _comb_arr)
	{
		comb.set_sample_freq (_sample_freq);
		comb.set_min_freq (float (_note_freq_min));
		auto &      proc = comb.use_feedback_proc ();
		dsp::dyn::LimiterRms &  lim = proc.use_filter_3 ();
		lim.set_sample_freq (sample_freq);
		lim.set_level (0.125f);
		lim.set_time (0.030f);
	}

	update_comb ();
	update_filter ();
	for (auto &comb : _comb_arr)
	{
		comb.set_feedback (_feedback);
		comb.set_freq (_comb_freq);
	}

	clear_buffers ();
}



void	Voice::clear_buffers ()
{
	for (auto &comb : _comb_arr)
	{
		comb.clear_buffers ();
		auto &         fdbk_proc = comb.use_feedback_proc ();
		fdbk_proc.use_filter_1 ().clear_buffers ();
		fdbk_proc.use_filter_2 ().clear_buffers ();
		fdbk_proc.use_filter_3 ().clear_buffers ();
	}

	set_next_block ();
}



void	Voice::set_note (float note)
{
	// Arbitrary large thersholds for sanity checks, but usually we should stay
	// within the MIDI note range.
	assert (note >= -100);
	assert (note <=  200);

	_note            = note;
	_comb_dirty_flag = true;
}



void	Voice::set_decay_rt60 (float t)
{
	assert (t > 0);

	_decay_time      = t;
	_comb_dirty_flag = true;
}



void	Voice::set_polarity (bool neg_flag)
{
	_neg_flag        = neg_flag;
	_comb_dirty_flag = true;
}



void	Voice::set_level (float lvl)
{
	assert (lvl >= 0);
	assert (lvl < 1e4);  // Arbitrary large value

	_level = lvl;
}



void	Voice::set_filt_freq (float freq)
{
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);

	_filt_freq       = freq;
	_filt_dirty_flag = true;
}



void	Voice::set_filt_reso (float reso)
{
	assert (reso >= 0);
	assert (reso <= 1);

	_filt_reso       = reso;
	_filt_dirty_flag = true;
}



void	Voice::set_filt_damp (float damp)
{
	assert (damp >= 0);
	assert (damp <= 1);

	_filt_damp       = damp;
	_filt_dirty_flag = true;
}



void	Voice::compute_filt_param (float freq, float reso, float damp, float biq_b_z [3], float biq_a_z [3], float shf_b_z [2], float shf_a_z [2])
{
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);
	assert (reso >= 0);
	assert (reso <= 1);
	assert (damp >= 0);
	assert (damp <= 1);
	assert (biq_b_z != 0);
	assert (biq_a_z != 0);
	assert (shf_b_z != 0);
	assert (shf_a_z != 0);

	const float    r2      = reso * reso;
	const float    q_inv   = fstb::Approx::exp2 (r2 * -2);
	const float    damp2   = damp * damp;
	const float    shf_lvl = 1 - damp2 * damp2;
	const float    f_amp   = std::min (
		1.0f,
		float (sqrt (sqrt (2.f / (shf_lvl * shf_lvl + 1.f)) * q_inv))
	);

	const float    k =
		dsp::iir::TransSZBilin::compute_k_approx (freq * _inv_fs);

/*
We could simplify the filter by using b_1 = 2 :
    (s^2 + 2 * s + 1) / (s^2 + q_inv * 2 * s + 1)
  * (shf_lvl * s + 1) /                   (s + 1)
= (s^2 + (shf_lvl + 1) * s + 1) / (s^2 + q_inv * 2 * s + 1)
Thus we could remove the one-pole filter, but this would double the
peak width (leaving the levels the same).
*/
	const float    b_1         = 1;
	const float    biq_b_s [3] = { f_amp, b_1 * f_amp, f_amp };
	const float    biq_a_s [3] = {     1, b_1 * q_inv,     1 };
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		biq_b_z, biq_a_z, biq_b_s, biq_a_s, k
	);

	const float    shf_b_s [2] = { 1, shf_lvl };
	const float    shf_a_s [2] = { 1,       1 };
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		shf_b_z, shf_a_z, shf_b_s, shf_a_s, k
	);
}



void	Voice::set_filt_param (float freq, float reso, float damp, const float biq_b_z [3], const float biq_a_z [3], const float shf_b_z [2], const float shf_a_z [2])
{
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);
	assert (reso >= 0);
	assert (reso <= 1);
	assert (damp >= 0);
	assert (damp <= 1);
	assert (biq_b_z != 0);
	assert (biq_a_z != 0);
	assert (shf_b_z != 0);
	assert (shf_a_z != 0);

	_filt_freq       = freq;
	_filt_reso       = reso;
	_filt_damp       = damp;
	set_filter_eq (biq_b_z, biq_a_z, shf_b_z, shf_a_z);
	_filt_dirty_flag = false;
}



void	Voice::process_block (float * const dst_ptr_arr [_max_nbr_chn], const float * const src_ptr_arr [_max_nbr_chn], int nbr_spl, int nbr_chn)
{
	assert (_tmp_buf_ptr != 0);
	assert (dst_ptr_arr != 0);
	assert (src_ptr_arr != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);
	assert (nbr_chn > 0);
	assert (nbr_chn <= _max_nbr_chn);

	if (_comb_dirty_flag)
	{
		update_comb ();
	}
	if (_filt_dirty_flag)
	{
		update_filter ();
	}

	for (int chn = 0; chn < nbr_chn; ++chn)
	{
		assert (dst_ptr_arr [chn] != 0);
		assert (src_ptr_arr [chn] != 0);

		dsp::mix::Generic::copy_1_1_vlrauto (
			_tmp_buf_ptr, src_ptr_arr [chn], nbr_spl, _gain_old, _gain
		);

		CombType &     comb = _comb_arr [chn];

		if (    _feedback !=  _feedback_old
		    || _comb_freq != _comb_freq_old)
		{
			comb.process_block_vff (
				dst_ptr_arr [0],
				_tmp_buf_ptr,
				nbr_spl,
				_comb_freq,
				_feedback
			);
		}
		else
		{
			comb.process_block (
				dst_ptr_arr [0],
				_tmp_buf_ptr,
				nbr_spl
			);
		}
	}

	set_next_block ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Voice::update_comb ()
{
	// Comb frequency
	float          freq = 440 * fstb::Approx::exp2 ((_note - 69) * (1.f / 12));
	if (_neg_flag)
	{
		freq *= 2;
	}
	_comb_freq = fstb::limit (
		freq,
		float (_note_freq_min),
		float (_note_freq_max)
	);

	// Feedback coefficient
/*
t: time, seconds
amp (t): output amplitude after t seconds (unit spike at t = 0)
f: feedback coefficient (absolute value)
d: comb delay duration (1 / freq)

amp (t) = f ^ (t / d)
amp (t) = exp (ln (f) * t / d)
ln (amp (t)) = ln (f) * t / d
ln (amp (t)) * d / t = ln (f)
f = exp (ln (amp (t)) * d / t)
*/
	const float    lvl_l2   = -10;   // float (log2 (1e-3));
	const float    fdbk_abs =
		fstb::Approx::exp2 (lvl_l2 / (_comb_freq * _decay_time));
	_feedback = (_neg_flag) ? -fdbk_abs : fdbk_abs;

	// Input gain
	_gain = compute_gain (fdbk_abs);

	_comb_dirty_flag = false;
}



void	Voice::update_filter ()
{
	float          biq_b_z [3];
	float          biq_a_z [3];
	float          shf_b_z [2];
	float          shf_a_z [2];
	compute_filt_param (
		_filt_freq, _filt_reso, _filt_damp,
		biq_b_z, biq_a_z,
		shf_b_z, shf_a_z
	);
	set_filter_eq (biq_b_z, biq_a_z, shf_b_z, shf_a_z);
	_filt_dirty_flag = false;
}



void	Voice::set_filter_eq (const float biq_b_z [3], const float biq_a_z [3], const float shf_b_z [2], const float shf_a_z [2])
{
	for (auto &comb : _comb_arr)
	{
		auto &         proc = comb.use_feedback_proc ();

		dsp::iir::Biquad &   filter = proc.use_filter_1 ();
		filter.set_z_eq (biq_b_z, biq_a_z);

		dsp::iir::OnePole &  shelf  = proc.use_filter_2 ();
		shelf.set_z_eq (shf_b_z, shf_a_z);
	}
}



void	Voice::set_next_block ()
{
	_gain_old      = _gain;
	_feedback_old  = _feedback;
	_comb_freq_old = _comb_freq;
}



float	Voice::compute_gain (float fdbk)
{
	assert (fdbk >= 0);
	assert (fdbk <= 1);

	const float    fatness = 0.09375; // Minimum gain at unity feedback
	const float    fdbk_sq = fdbk * fdbk;
	float          gain    = float (sqrt (1 - fdbk_sq));
	gain += fatness * (1 - gain);

	return gain;
}



}  // namespace hcomb
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
