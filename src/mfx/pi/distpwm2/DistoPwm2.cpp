/*****************************************************************************

        DistoPwm2.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/distpwm2/DetectionMethod.h"
#include "mfx/pi/distpwm2/DistoPwm2.h"
#include "mfx/pi/distpwm2/Param.h"
#include "mfx/pi/distpwm2/PreFilterType.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace distpwm2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoPwm2::DistoPwm2 (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_misc ()
,	_param_change_flag_vol ()
,	_param_change_flag_osc_arr ()
,	_filter_in ()
,	_chn_arr ()
,	_voice_arr ()
,	_prefilter (PreFilterType_WEAK)
,	_threshold (1e-4f)
,	_buf_tmp ()
,	_buf_mix_arr ()
,	_peak_det_flag (true)
,	_density (1)
,	_sust_lvl (0)
,	_gate_lvl (0)
,	_env_pre ()
,	_env_post ()
,	_fixgain_cur (1)
,	_fixgain_old (1)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_LPF , PreFilterType_MILD);
	_state_set.set_val_nat (desc_set, Param_DET , DetectionMethod_ZX);
	_state_set.set_val_nat (desc_set, Param_THR , 1e-4);
	_state_set.set_val_nat (desc_set, Param_DENS,    1);
	_state_set.set_val_nat (desc_set, Param_SUST,    0);
	_state_set.set_val_nat (desc_set, Param_GATE, 1e-4);

	_state_set.add_observer (Param_LPF , _param_change_flag_misc);
	_state_set.add_observer (Param_DET , _param_change_flag_misc);
	_state_set.add_observer (Param_THR , _param_change_flag_misc);
	_state_set.add_observer (Param_DENS, _param_change_flag_vol);
	_state_set.add_observer (Param_SUST, _param_change_flag_vol);
	_state_set.add_observer (Param_GATE, _param_change_flag_vol);

	_state_set.set_ramp_time (Param_DENS, 0.010);
	_state_set.set_ramp_time (Param_SUST, 0.010);
	_state_set.set_ramp_time (Param_GATE, 0.010);

	_param_change_flag_misc.add_observer (_param_change_flag);
	_param_change_flag_vol.add_observer (_param_change_flag);

	for (int osc = 0; osc < OscType_NBR_ELT; ++osc)
	{
		const int      base = Param_OSC_BASE + osc * ParamOsc_NBR_ELT;
		const double   lvl  = (osc == OscType_STD) ? 1 : 0;

		_state_set.set_val_nat (desc_set, base + ParamOsc_PULSE, PulseType_RECT);
		_state_set.set_val_nat (desc_set, base + ParamOsc_LVL  , lvl);
		_state_set.set_val_nat (desc_set, base + ParamOsc_PF   , 2000);

		auto &         pcf_voice = _param_change_flag_osc_arr [osc];
		_state_set.add_observer (base + ParamOsc_PULSE, pcf_voice);
		_state_set.add_observer (base + ParamOsc_LVL  , pcf_voice);
		_state_set.add_observer (base + ParamOsc_PF   , pcf_voice);

		pcf_voice.add_observer (_param_change_flag);
	}

	_env_pre.set_times (0.001f, 0.050f);
	_env_post.set_times (0.001f, 0.050f);

	dsp::mix::Align::setup ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DistoPwm2::do_get_state () const
{
	return _state;
}



double	DistoPwm2::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistoPwm2::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const float    b2_s [2] = { 0, 1 };
	const float    a2_s [2] = { 1, 1 };
	float          a2_z [2];
	float          b2_z [2];
	dsp::iir::TransSZBilin::map_s_to_z_one_pole (
		b2_z, a2_z, b2_s, a2_s, 30, _sample_freq
	);

	// Base: order 1, HPF 30 Hz, order 1, cutoff 1500 Hz
	const float    b3_s [3] = { 1,                   0, 0 };
	const float    a3_s [3] = { 1, float (fstb::SQRT2), 1 };
	float          a3_z [3];
	float          b3_z [3];
	dsp::iir::TransSZBilin::map_s_to_z (
		b3_z, a3_z, b3_s, a3_s, 1500, _sample_freq
	);
	_filter_in->set_z_eq_one (0, b3_z, a3_z);
	_filter_in->set_z_eq_one (1, b3_z, a3_z);

	update_prefilter ();

	const int      mbl_align = max_buf_len & -4;
	_buf_tmp.resize (mbl_align * _max_nbr_chn);
	for (auto &vcinf : _voice_arr)
	{
		vcinf._buf_gen.resize (mbl_align);
	}
	for (auto &buf : _buf_mix_arr)
	{
		buf.resize (mbl_align);
	}
	for (auto &chn : _chn_arr)
	{
		for (auto &unip : chn._peak_analyser._env_bip)
		{
			unip._env.set_sample_freq (sample_freq);
			unip._env.set_times (0.05e-3f, 5e-3f);
		}
		chn._zx_history.reserve (max_buf_len);
	}

	_env_pre.set_sample_freq (sample_freq);
	_env_post.set_sample_freq (sample_freq);
	_fixgain_cur = 1;

	update_param (true);

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	DistoPwm2::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// Signal processing
	const int      nbr_spl = proc._nbr_spl;

	// Envelope detection
	float          lvl_pre_sq = 1;
	if (has_vol_proc ())
	{
		square_block (
			_buf_tmp.data (), proc._src_arr, nbr_spl, nbr_chn_src
		);
		lvl_pre_sq = _env_pre.analyse_block_raw (_buf_tmp.data (), nbr_spl);
	}

	// Interleave samples for low-pass filtering
	const int      chn_aux = (nbr_chn_proc == 2) ? 1 : 0;
	dsp::mix::Align::copy_2_2i (
		_buf_tmp.data (),
		proc._src_arr [0],
		proc._src_arr [chn_aux],
		nbr_spl
	);

	// Low-pass pre-filtering
	_filter_in->process_block_2x2_latency (
		_buf_tmp.data (),
		_buf_tmp.data (),
		nbr_spl
	);

	for (auto &vcinf : _voice_arr)
	{
		vcinf._vol_beg = float (vcinf._vol.get_val ());
		vcinf._vol.tick (nbr_spl);
		vcinf._vol_end = float (vcinf._vol.get_val ());
		vcinf._active_flag =
			(vcinf._vol_beg != 0 || vcinf._vol_beg != vcinf._vol_end);
	}

	// Zero-crossing detection and main voice generation
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn      = _chn_arr [chn_index];
		float *        dst_ptr  = _buf_mix_arr [chn_index].data ();
		bool           mix_flag = false;

		// Detects zero-crossings and evaluates their exact position
		detect_zero_cross_block (chn_index, nbr_spl);

		// Generates data for all active channels
		const int      nbr_zx = int (chn._zx_history.size ());
		static const int  per_arr [OscType_NBR_ELT] = { 2, 1, 4, 8 };
		for (int vc_index = 0; vc_index < OscType_NBR_ELT; ++ vc_index)
		{
			VoiceInfo &    vcinf  = _voice_arr [vc_index];
			if (vcinf._active_flag)
			{
				// Audio generation
				Voice &        voice  = chn._voice_arr [vc_index];
				const int      per    = per_arr [vc_index];
				const int      per_m  = per - 1;
				const int      per_v  = per_m >> 1;
				int            zx_idx = 0;
				int            pos    = 0;
				do
				{
					int            stop_pos    = nbr_spl;
					int            zx_idx_sync = -1;
					while (zx_idx < nbr_zx && stop_pos == nbr_spl)
					{
						const ZeroCross & zx = chn._zx_history [zx_idx];
						if ((zx._zx_idx & per_m) == per_v)
						{
							stop_pos    = zx._pos;
							zx_idx_sync = zx_idx;
						}
						++ zx_idx;
					}

					const int      step_size = stop_pos - pos;
					if (step_size > 0)
					{
						voice.process_block (&vcinf._buf_gen [pos], step_size);
						pos += step_size;
					}

					if (zx_idx_sync >= 0)
					{
						voice.sync (chn._zx_history [zx_idx_sync]._frac);
					}
				}
				while (pos < nbr_spl);

				// Mixing
				const float *  src_ptr = vcinf._buf_gen.data ();
				if (mix_flag)
				{
					dsp::mix::Align::mix_1_1_vlrauto (
						dst_ptr,
						src_ptr,
						nbr_spl,
						vcinf._vol_beg,
						vcinf._vol_end
					);
				}
				else
				{
					dsp::mix::Align::copy_1_1_vlrauto (
						dst_ptr,
						src_ptr,
						nbr_spl,
						vcinf._vol_beg,
						vcinf._vol_end
					);
					mix_flag = true;
				}
			}
		}

		if (! mix_flag)
		{
			dsp::mix::Align::clear (dst_ptr, nbr_spl);
		}
	}

	// Removes the DC component
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];
		const float *  src_ptr = _buf_mix_arr [chn_index].data ();
		float *        dst_ptr = proc._dst_arr [chn_index];
		chn._hpf_out.process_block (dst_ptr, src_ptr, proc._nbr_spl);
	}

	// Volume processing
	_fixgain_cur = 1;
	if (has_vol_proc ())
	{
		// Envelope detection
		square_block (
			_buf_tmp.data (), proc._dst_arr, nbr_spl, nbr_chn_src
		);
		float          lvl_post_sq =
			_env_post.analyse_block_raw (_buf_tmp.data (), nbr_spl);

		// Fix gain calculation
		const auto     lvl_sq   = fstb::Vf32::set_pair (lvl_pre_sq, lvl_post_sq);
		const auto     lvl      = fstb::sqrt (lvl_sq);
		const float    lvl_pre  = lvl.template extract <0> ();
		const float    lvl_post = lvl.template extract <1> ();
		const float    lvl_lim  = 2; // Limiter

		assert (lvl_post > 0);
		const float    lvl_post_inv = 1.0f / lvl_post;
		const float    lvl_t = std::max (lvl_pre, _sust_lvl);      // Modified input level doesn't go below the threshold
		const float    r  = std::min (lvl_t * lvl_post_inv, 1.0f); // Amplification factor to reach the modified input level
		float          gd = powf (r, 1 - _density);                // Moderated by the density
		if (_gate_lvl > 0 && lvl_pre < _gate_lvl)
		{
			float          gain_red = lvl_pre / _gate_lvl;
			gain_red *= gain_red; // Gate ratio: 8
			gain_red *= gain_red;
			gain_red *= gain_red;
			gd       *= gain_red;
		}
		_fixgain_cur  = std::min (gd, lvl_lim * lvl_post_inv);
	}

	// Fix gain
	if (_param_proc.is_req_steady_state ())
	{
		_fixgain_old = _fixgain_cur;
	}
	if (_fixgain_cur != 1 || _fixgain_old != 1)
	{
		for (int chn = 0; chn < nbr_chn_src; ++chn)
		{
			dsp::mix::Align::scale_1_vlrauto (
				proc._dst_arr [chn], nbr_spl, _fixgain_old, _fixgain_cur
			);
		}
	}

	// Duplicates the remaining output channels
	for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}

	_fixgain_old = _fixgain_cur;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoPwm2::clear_buffers ()
{
	_filter_in->clear_buffers ();
	for (auto &chn : _chn_arr)
	{
		chn._hpf_out.clear_buffers ();
		for (auto &voice : chn._voice_arr)
		{
			voice.clear_buffers ();
		}
		for (auto &pu : chn._peak_analyser._env_bip)
		{
			pu._env.clear_buffers ();
			for (auto &val : pu._mem)
			{
				val = 0;
			}
		}
		chn._zx_idx   = 0;
		chn._spl_prev = 0;
	}
	for (auto &vcinf : _voice_arr)
	{
		vcinf._active_flag = false;
		vcinf._vol.clear_buffers ();
	}
	_fixgain_old = _fixgain_cur;
}



void	DistoPwm2::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_misc (true) || force_flag)
		{
			_prefilter = PreFilterType (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_LPF)
			));
			_peak_det_flag = (DetectionMethod (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_DET)
			)) == DetectionMethod_PEAK);
			_threshold = float (_state_set.get_val_tgt_nat (Param_THR));

			update_prefilter ();
		}

		if (_param_change_flag_vol (true) || force_flag)
		{
			_density  =
				float (_state_set.get_val_end_nat (Param_DENS));
			_sust_lvl =
				float (_state_set.get_val_end_nat (Param_SUST));
			_gate_lvl =
				float (_state_set.get_val_end_nat (Param_GATE));
		}

		for (int vc_index = 0; vc_index < OscType_NBR_ELT; ++vc_index)
		{
			if (_param_change_flag_osc_arr [vc_index] (true) || force_flag)
			{
				const int     base  =
					Param_OSC_BASE + vc_index * ParamOsc_NBR_ELT;
				const PulseType pt  = PulseType (fstb::round_int (
					_state_set.get_val_tgt_nat (base + ParamOsc_PULSE)
				));
				const float   pf_hz =
					float (_state_set.get_val_tgt_nat (base + ParamOsc_PF));
				const float   pw    = 0.5f * _sample_freq / pf_hz;
				const float   lvl   =
					float (_state_set.get_val_end_nat (base + ParamOsc_LVL));

				VoiceInfo &   vcinf = _voice_arr [vc_index];
				vcinf._vol.set_val (lvl);

				for (auto &chn : _chn_arr)
				{
					Voice &        voice = chn._voice_arr [vc_index];
					voice.set_pulse_type (pt);
					voice.set_pulse_width (pw);
				}
			}
		}
	}
}



void	DistoPwm2::update_prefilter ()
{
	const float    f_hpf    =   5;
	float          f_lpf    = 500;
	float          b3_s [3] = { 1, 2, 1 };
	float          a3_s [3] = { 1, 2, 1 };

	if (_prefilter >= PreFilterType_MILD)
	{
		if (_prefilter == PreFilterType_STRONG)
		{
			f_lpf = 50;
		}
		const float    f_rel_inv = f_lpf / f_hpf;
		b3_s [0] = 0;
		b3_s [1] = f_rel_inv;
		b3_s [2] = 0;
		a3_s [1] = f_rel_inv + 1;
		a3_s [2] = f_rel_inv;
	}
	
	float          a3_z [3];
	float          b3_z [3];
	dsp::iir::TransSZBilin::map_s_to_z (
		b3_z, a3_z, b3_s, a3_s, f_lpf, _sample_freq
	);

	_filter_in->set_z_eq_one (2, b3_z, a3_z);
	_filter_in->set_z_eq_one (3, b3_z, a3_z);
}



bool	DistoPwm2::detect_zero_cross (Channel &chn, float x, bool positive_flag)
{
	bool           trig_flag = false;

	if (   (  positive_flag && x <= -_threshold)
	    || (! positive_flag && x >= +_threshold))
	{
		trig_flag = true;
		++ chn._zx_idx;
	}

	return trig_flag;
}



bool	DistoPwm2::detect_peak (Channel &chn, float x, bool positive_flag)
{
	bool           trig_flag = false;

	std::array <std::array <float, 3>, 2> env_res_arr;
	std::array <float, 2>   env_inp = {{
		(x < 0) ?  0 : x,
		(x < 0) ? -x : 0
	}};
	for (int env_cnt = 0; env_cnt < 2; ++env_cnt)
	{
		auto &         pu = chn._peak_analyser._env_bip [env_cnt];
		env_res_arr [env_cnt] [0] = pu._mem [0];
		env_res_arr [env_cnt] [1] = pu._mem [1];
		env_res_arr [env_cnt] [2] =
			pu._env.process_sample (env_inp [env_cnt]);
	}

	if (   (   positive_flag
				&& env_res_arr [0] [0] < env_res_arr [0] [1]
				&& env_res_arr [0] [2] < env_res_arr [0] [1])
			|| (   ! positive_flag
				&& env_res_arr [1] [0] < env_res_arr [1] [1]
				&& env_res_arr [1] [2] < env_res_arr [1] [1])
	)
	{
		trig_flag = true;
		++ chn._zx_idx;
	}
	for (int env_cnt = 0; env_cnt < 2; ++env_cnt)
	{
		auto &         pu = chn._peak_analyser._env_bip [env_cnt];
		pu._mem [0] = env_res_arr [env_cnt] [1];
		pu._mem [1] = env_res_arr [env_cnt] [2];
	}

	return trig_flag;
}



void	DistoPwm2::detect_zero_cross_block (int chn_index, int nbr_spl)
{
	assert (nbr_spl > 0);

	Channel &      chn      = _chn_arr [chn_index];
	const float *  tmp_ptr  = &_buf_tmp [chn_index]; // 2 interleaved channels
	chn._zx_history.clear ();
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x = tmp_ptr [pos * 2];
		const bool     positive_flag = ((chn._zx_idx & 1) != 0);
		if (positive_flag && x <= -_threshold)
		{
			add_zc (chn, pos, -chn._spl_prev, -x);
		}
		else if (! positive_flag && x >= _threshold)
		{
			add_zc (chn, pos, chn._spl_prev, x);
		}

		chn._spl_prev = x;
	}
}



void	DistoPwm2::add_zc (Channel &chn, int pos, float x0, float x1)
{
	assert (pos >= 0);
	assert (x1  >= _threshold);

	const size_t   nbr_zx = chn._zx_history.size ();
	chn._zx_history.resize (nbr_zx + 1);
	ZeroCross &    zx = chn._zx_history.back ();
	zx._pos    = pos;
	zx._zx_idx = chn._zx_idx;

	// We need to limit the result because everything could go wrong when
	// _threshold is changing.
	zx._frac   = fstb::limit ((x1 - _threshold) / (x1 - x0), 0.f, 0.99999f);

	++ chn._zx_idx;
}



bool	DistoPwm2::has_vol_proc () const
{
	return (_density < 1 || _gate_lvl > 0);
}



void	DistoPwm2::square_block (float dst_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn)
{
	static const float   not_zero = 1e-30f;	// -600 dB
	if (nbr_chn == 1)
	{
		dsp::mix::Align::sum_square_n_1 (
			dst_ptr, src_ptr_arr, nbr_spl, nbr_chn, not_zero
		);
	}
	else if (nbr_chn == 2)
	{
		dsp::mix::Align::sum_square_n_1_v (
			dst_ptr, src_ptr_arr, nbr_spl, nbr_chn, not_zero, 0.5f
		);
	}
	else
	{
		const float    gain = 1.f / float (nbr_chn);
		dsp::mix::Align::sum_square_n_1_v (
			dst_ptr, src_ptr_arr, nbr_spl, nbr_chn, not_zero, gain
		);
	}
}



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
