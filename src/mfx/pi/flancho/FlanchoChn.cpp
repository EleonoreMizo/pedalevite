/*****************************************************************************

        FlanchoChn.cpp
        Author: Laurent de Soras, 2016

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
#include "mfx/dsp/rspl/InterpolatorInterface.h"
#include "mfx/pi/flancho/FlanchoChn.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace flancho
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FlanchoChn::FlanchoChn (dsp::rspl::InterpolatorInterface &interp, float dly_buf_ptr [], int dly_buf_len, float render_buf_ptr [], int render_buf_len)
:	_dly_line ()
,	_voice_arr ()
,	_sample_freq (44100)
,	_rel_phase (0)
,	_tmp_buf_arr ()
,	_nbr_voices (1)
,	_voice_vol (1)
,	_period (1)
,	_delay (Cst::_delay_max * 0.5e-6)
,	_depth (0.5)
,	_feedback (0)
,	_wf_shape (0)
,	_wf_type (WfType_SINE)
,	_neg_flag (false)
,	_max_proc_len (0)
,	_mpl_lfo (0)
,	_feedback_old (0)
,	_sat_in_a (1)
,	_sat_in_b (0)
,	_sat_out_a (1)
,	_sat_out_b (0)
{
	assert (&interp != 0);
	assert (dly_buf_ptr != 0);
	assert (dly_buf_len > 0);
	assert (render_buf_ptr != 0);
	assert (render_buf_len > 0);

	dsp::mix::Generic::setup ();

	_tmp_buf_arr [TmpBufType_DLY_READ]._ptr = dly_buf_ptr;
	_tmp_buf_arr [TmpBufType_DLY_READ]._len = dly_buf_len;
	_tmp_buf_arr [TmpBufType_RENDER  ]._ptr = render_buf_ptr;
	_tmp_buf_arr [TmpBufType_RENDER  ]._len = render_buf_len;

	_dly_line.set_sample_freq (_sample_freq, 0);
	_dly_line.set_interpolator (interp);
	_dly_line.set_max_delay_time (Cst::_delay_max * 2e-6);

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &			voice = _voice_arr [v_cnt];

		voice._dly_reader.set_delay_line (_dly_line);
		voice._dly_reader.set_tmp_buf (
			_tmp_buf_arr [TmpBufType_DLY_READ]._ptr,
			_tmp_buf_arr [TmpBufType_DLY_READ]._len
		);
		voice._dly_reader.set_crossfade (256, 0);
		voice._dly_reader.set_resampling_range (-4.0, +4.0);

		voice._rel_phase = 2 * pow (5.0 / 7.0, v_cnt);

		voice._lfo.set_type (dsp::ctrl::lfo::LfoModule::Type_SINE);
	}

	update_shape ();
}



void	FlanchoChn::set_sample_freq (double sample_freq, bool fast_flag, dsp::rspl::InterpolatorInterface &interp)
{
	_sample_freq = sample_freq;

	_dly_line.set_sample_freq (_sample_freq, 0);
	_dly_line.set_interpolator (interp);

	const float          hpf_freq = 80; // HPF cutoff
	static const float   b_s [2]  = { 0, 1 };
	static const float   a_s [2]  = { 1, 1 };
	float                b_z [2];
	float                a_z [2];
	if (fast_flag)
	{
		const float    k = dsp::iir::TransSZBilin::compute_k_approx (
			hpf_freq / float (_sample_freq)
		);
		dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
			b_z, a_z, b_s, a_s, k
		);
	}
	else
	{
		dsp::iir::TransSZBilin::map_s_to_z_one_pole (
			b_z, a_z,
			b_s, a_s,
			hpf_freq,
			_sample_freq
		);
	}

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &			voice = _voice_arr [v_cnt];
		voice._lfo.set_sample_freq (_sample_freq);
		voice._hpf.set_z_eq (b_z, a_z);
		voice._hpf.clear_buffers ();
	}

	update_max_proc_len ();
	update_mpl_lfo ();
}



void	FlanchoChn::set_rel_phase (double rel_phase)
{
	assert (rel_phase >= 0);
	assert (rel_phase < 1);

	const double	dif = rel_phase - _rel_phase;

	_rel_phase = rel_phase;

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &			voice = _voice_arr [v_cnt];
		double			phase = voice._lfo.get_phase ();
		phase += dif;
		phase -= floor (phase);
		voice._lfo.set_phase (phase);
	}
}



void	FlanchoChn::set_nbr_voices (int nbr_voices)
{
	assert (nbr_voices > 0);
	assert (nbr_voices <= Cst::_max_nbr_voices);

	if (_nbr_voices < nbr_voices)
	{
		const double	base_phase = estimate_base_phase ();

		for (int v_cnt = _nbr_voices; v_cnt < nbr_voices; ++v_cnt)
		{
			Voice &			voice = _voice_arr [v_cnt];

			set_wf_type (voice._lfo, _wf_type);
			voice._lfo.set_period (_period);

			update_phase (voice, base_phase);
		}
	}

	_nbr_voices = nbr_voices;
	_voice_vol  = sqrt (2.0f / float (_nbr_voices + 1));
}



void	FlanchoChn::set_period (double period)
{
	_period = period;

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &			voice = _voice_arr [v_cnt];
		voice._lfo.set_period (_period);
	}

	update_mpl_lfo ();
}



void	FlanchoChn::set_speed (double freq)
{
	assert (freq > 0);
	set_period (1 / freq);
}



void	FlanchoChn::set_delay (double delay)
{
	assert (delay * 1e6 >= Cst::_delay_min);
	assert (delay * 1e6 <= Cst::_delay_max);

	_delay = delay;

	update_max_proc_len ();
}



void	FlanchoChn::set_depth (double depth)
{
	assert (depth >= 0);
	assert (depth <= 1);

	_depth = depth;

	update_max_proc_len ();
}



void	FlanchoChn::set_wf_type (WfType wf_type)
{
	assert (wf_type >= 0);
	assert (wf_type < WfType_NBR_ELT);

	_wf_type = wf_type;

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &			voice = _voice_arr [v_cnt];
		set_wf_type (voice._lfo, _wf_type);
	}
}



void	FlanchoChn::set_wf_shape (double shape)
{
	assert (shape >= -1);
	assert (shape <= +1);

	_wf_shape = shape;

	update_shape ();
}



void	FlanchoChn::set_feedback (double fdbk)
{
	assert (fdbk > -1);
	assert (fdbk < +1);

	_feedback = fdbk;
}



void	FlanchoChn::set_polarity (bool neg_flag)
{
	_neg_flag = neg_flag;
}



void	FlanchoChn::resync (double base_phase)
{
	assert (base_phase >= 0);
	assert (base_phase < 1);

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &			voice = _voice_arr [v_cnt];

		update_phase (voice, base_phase);
	}
}



void	FlanchoChn::process_block (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (_max_proc_len > 0);
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const double   fdbk_step  = (_feedback - _feedback_old) / nbr_spl;
	double         fdbk_cur   = _feedback_old;

	float *        render_ptr = _tmp_buf_arr [TmpBufType_RENDER]._ptr;

	int            block_pos  = 0;
	do
	{
		int            work_len = nbr_spl - block_pos;
		work_len = std::min (work_len, _max_proc_len);
		work_len = std::min (work_len, _mpl_lfo);

		const double   fdbk_new = fdbk_cur + fdbk_step * work_len;

		// We scan the voices in reverse order because:
		// 1. We want to process feedback last, when it already has been read on
		// the delay line.
		// 2. We want to generate the feedback from the first voice, to ensure
		// signal continuity when number of voices varies.
		const int      last_voice = _nbr_voices - 1;
		for (int v_cnt = last_voice; v_cnt >= 0; --v_cnt)
		{
			Voice &        voice = _voice_arr [v_cnt];

			voice._lfo.tick (work_len);
			const double   delay_time_new = compute_delay_time (voice._lfo);
			voice._dly_reader.set_delay_time (delay_time_new, work_len);

			// render_ptr contains the delay line output.
			voice._dly_reader.read_data (render_ptr, work_len, 0);

			float          vol = float (_voice_vol);
			if (_neg_flag)
			{
				vol = -vol;
			}

			if (work_len == 1)
			{
				float          val     = render_ptr [0];
				const float    val_vol = val * vol;
				if (v_cnt == last_voice)
				{
					out_ptr [block_pos] = val_vol;
				}
				else
				{
					out_ptr [block_pos] += val_vol;
				}

				if (v_cnt == 0)
				{
					// Feedback processing
					val *= float (fdbk_cur);
					val = voice._hpf.process_sample (val);
					val += in_ptr [block_pos];
					_dly_line.push_sample (val);
				}
			}

			else
			{
				if (v_cnt == last_voice)
				{
					// Copies to the output
					if (vol != 1)
					{
						dsp::mix::Generic::copy_1_1_v (
							out_ptr + block_pos,
							render_ptr,
							work_len,
							vol
						);
					}
					else
					{
						dsp::mix::Generic::copy_1_1 (
							out_ptr + block_pos,
							render_ptr,
							work_len
						);
					}
				}

				// Other voices: just mix the output
				else
				{
					if (vol != 1)
					{
						dsp::mix::Generic::mix_1_1_v (
							out_ptr + block_pos,
							render_ptr,
							work_len,
							vol
						);
					}
					else
					{
						dsp::mix::Generic::mix_1_1 (
							out_ptr + block_pos,
							render_ptr,
							work_len
						);
					}
				}

				if (v_cnt == 0)
				{
					// Feedback processing
					dsp::mix::Generic::scale_1_vlrauto (
						render_ptr,
						work_len,
						float (fdbk_cur),
						float (fdbk_new)
					);
					dsp::mix::Generic::mix_1_1 (
						render_ptr,
						in_ptr + block_pos,
						work_len
					);
					_dly_line.push_block (render_ptr, work_len);
				}
			}
		}

		fdbk_cur = fdbk_new;
		block_pos += work_len;
	}
	while (block_pos < nbr_spl);

	_feedback_old = _feedback;
}



void	FlanchoChn::clear_buffers ()
{
	_dly_line.clear_buffers ();

	for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
	{
		Voice &        voice = _voice_arr [v_cnt];

		const double   phase = voice._lfo.get_phase ();
		voice._lfo.clear_buffers ();
		voice._lfo.set_phase (phase);

		const double   delay_time_new = compute_delay_time (voice._lfo);
		voice._dly_reader.set_delay_time (delay_time_new, 0);
		voice._dly_reader.clear_buffers ();

		voice._hpf.clear_buffers ();
	}


	_feedback_old = _feedback;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	FlanchoChn::estimate_base_phase () const
{
	const Voice &  voice = _voice_arr [0];
	double         phase = voice._lfo.get_phase ();
	phase -= voice._rel_phase;
	phase -= _rel_phase;

	phase -= floor (phase);

	return (phase);
}



void	FlanchoChn::set_wf_type (dsp::ctrl::lfo::LfoModule &lfo, WfType wf_type)
{
	assert (&lfo != 0);
	assert (wf_type >= 0);
	assert (wf_type < WfType_NBR_ELT);

	dsp::ctrl::lfo::LfoModule::Type  lfo_type =
		dsp::ctrl::lfo::LfoModule::Type_SINE;
	bool           inv_flag = false;
	double         chaos    = 0;

	switch (wf_type)
	{
	case	WfType_SINE:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_SINE;
		break;

	case	WfType_TRI:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_VARISLOPE;
		break;

	case	WfType_PARABOLA:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_PARABOLA;
		break;

	case	WfType_PARABOLA_INV:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_PARABOLA;
		inv_flag = true;
		break;

	case	WfType_RAMP_UP:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_SAW;
		break;

	case	WfType_RAMP_DOWN:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_SAW;
		inv_flag = true;
		break;

	case	WfType_RND:
		lfo_type = dsp::ctrl::lfo::LfoModule::Type_SINE;
		chaos = 80.0 / 256;
		break;

	default:
		assert (false);
		break;
	}

	lfo.set_type (lfo_type);
	lfo.set_sign (inv_flag);
	lfo.set_chaos (chaos);
	update_lfo_param ();
}



double	FlanchoChn::compute_delay_time (dsp::ctrl::lfo::LfoModule &lfo)
{
	assert (&lfo != 0);

	double         lfo_val = lfo.get_val ();

	if (_wf_type != WfType_TRI)
	{
		// Scales from [-1 ; 1] to the shaper input range
		lfo_val *= _sat_in_a;
		lfo_val += _sat_in_b;

		// Shapes the signal
		lfo_val = SatFnc::saturate (lfo_val);

		// Back to [-1 ; 1]
		lfo_val *= _sat_out_a;
		lfo_val += _sat_out_b;
	}

	double         delay_time  = _delay * (1 + lfo_val * _depth);
	const double   delay_limit = _dly_line.get_min_delay_time ();
	delay_time = std::max (delay_time, delay_limit);

	return (delay_time);
}



void	FlanchoChn::update_phase (Voice &voice, double base_phase)
{
	double         phase = base_phase + _rel_phase + voice._rel_phase;
	phase -= floor (phase);
	voice._lfo.set_phase (phase);
}



void	FlanchoChn::update_shape ()
{
	update_lfo_param ();
	update_shaper_data ();
}



// Principle:
// When _wf_shape is close to 0, we work in the nearly-linear zone of
// the waveshaper (MapSaturate). The input signal is scaled to a
// small range around 0.
// When _wf_shape is close to 1 or -1, we put the waveshaper knee of
// the correspounding polarity at the center of the input range, so the
// symmetric shape of the input signal turns into an asymmetric shape.
// Therefore the minimum input level keeps staying close to 0, and the
// maximum input level becomes close to the waveshaper maximum input
// level.
// Inbetween both cases, one of the range bound is moved, depending on
// the _wf_shape polarity.
void	FlanchoChn::update_shaper_data ()
{
	const double   wp = std::max (_wf_shape, 0.0);
	const double   wn = std::max (-_wf_shape, 0.0);
	const double   lin_limit = 1.0 / 64;
	const double   scale = SatFnc::get_xs () - lin_limit;
	const double   a = 0.875;
	const double   b = 1 - a;
	const double   wp_m = (a * wp * wp + b) * wp;
	const double   wn_m = (a * wn * wn + b) * wn;

	const double   x_min = -lin_limit - wn_m * scale;
	const double   x_max = +lin_limit + wp_m * scale;
	_sat_in_a = (x_max - x_min) * 0.5;
	_sat_in_b = x_min - _sat_in_a * -1;

	const double   y_min = SatFnc::saturate (x_min);
	const double   y_max = SatFnc::saturate (x_max);
	_sat_out_a = 2 / (y_max - y_min);
	_sat_out_b = -1 - _sat_out_a * y_min;
}



void	FlanchoChn::update_lfo_param ()
{
	if (_wf_type == WfType_TRI)
	{
		const double   var_time = (_wf_shape + 1) * 0.5;

		for (int v_cnt = 0; v_cnt < Cst::_max_nbr_voices; ++v_cnt)
		{
			Voice &        voice = _voice_arr [v_cnt];

			voice._lfo.set_variation (
				dsp::ctrl::lfo::OscInterface::Variation_TIME,
				var_time
			);
			voice._lfo.set_variation (
				dsp::ctrl::lfo::OscInterface::Variation_SHAPE,
				0
			);
		}
	}
}



void	FlanchoChn::update_max_proc_len ()
{
	double         dly_min     = _delay * (1 - _depth);
	const double   delay_limit = _dly_line.get_min_delay_time ();
	dly_min = std::max (dly_min, delay_limit);
	_max_proc_len = _dly_line.estimate_max_one_shot_proc_w_feedback (dly_min);
	assert (_max_proc_len > 0);
	_max_proc_len =
		std::min (_max_proc_len, _tmp_buf_arr [TmpBufType_RENDER]._len);
}



void	FlanchoChn::update_mpl_lfo ()
{
	float          p_spl = float (_period * _sample_freq);
	const int      seg   = fstb::round_int (p_spl * (1.0f / 64));
	_mpl_lfo = fstb::limit (seg, 2, 256);
}



}  // namespace flancho
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
