/*****************************************************************************

        SkoolMood.cpp
        Author: Laurent de Soras, 2020

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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/StereoLevel.h"
#include "mfx/pi/smood/LfoType.h"
#include "mfx/pi/smood/Param.h"
#include "mfx/pi/smood/SkoolMood.h"
#include "mfx/piapi/Dir.h"
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
namespace smood
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SkoolMood::SkoolMood ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_misc ()
,	_param_change_flag_stdif ()
,	_param_change_flag_lfo ()
,	_param_change_flag_lfo_base ()
,	_param_change_flag_lfo_phase ()
,	_param_change_flag_lfo_phset ()
,	_param_change_flag_lfo_shape ()
,	_param_change_flag_lfo_wf ()
,	_chn_arr ()
,	_st_dif (0)
,	_depth (0.5f)
,	_width (1)
,	_mix ()
,	_chncross ()
,	_feedback ()
,	_lamptc (0)
,	_k (0)
,	_kc2 (0)
,	_kgain (0)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SPEED      , 1);
	_state_set.set_val_nat (desc_set, Param_MIX        , 0.5);
	_state_set.set_val_nat (desc_set, Param_WIDTH      , 0.5);
	_state_set.set_val_nat (desc_set, Param_STDIF      , 0.25);
	_state_set.set_val_nat (desc_set, Param_FEEDBACK   , 0);
	_state_set.set_val_nat (desc_set, Param_DEPTH      , 0.15);
	_state_set.set_val_nat (desc_set, Param_CHNCROSS   , 0);
	_state_set.set_val_nat (desc_set, Param_PAN        , 0);
	_state_set.set_val_nat (desc_set, Param_WAVEFORM   , LfoType_TRIANGLE);
	_state_set.set_val_nat (desc_set, Param_SNH        , 0);
	_state_set.set_val_nat (desc_set, Param_SMOOTH     , 0);
	_state_set.set_val_nat (desc_set, Param_CHAOS      , 0);
	_state_set.set_val_nat (desc_set, Param_PH_DIST_AMT, 0.5);
	_state_set.set_val_nat (desc_set, Param_PH_DIST_OFS, 0);
	_state_set.set_val_nat (desc_set, Param_SIGN       , 0);
	_state_set.set_val_nat (desc_set, Param_POLARITY   , 0);
	_state_set.set_val_nat (desc_set, Param_VAR1       , 0);
	_state_set.set_val_nat (desc_set, Param_VAR2       , 0);
	_state_set.set_val_nat (desc_set, Param_PHASE_SET  , 0);
	
	_state_set.add_observer (Param_MIX        , _param_change_flag_misc);
	_state_set.add_observer (Param_WIDTH      , _param_change_flag_misc);
	_state_set.add_observer (Param_STDIF      , _param_change_flag_stdif);
	_state_set.add_observer (Param_FEEDBACK   , _param_change_flag_misc);
	_state_set.add_observer (Param_DEPTH      , _param_change_flag_misc);
	_state_set.add_observer (Param_CHNCROSS   , _param_change_flag_misc);
	_state_set.add_observer (Param_PAN        , _param_change_flag_misc);

	_state_set.add_observer (Param_SPEED      , _param_change_flag_lfo_base);
	_state_set.add_observer (Param_WAVEFORM   , _param_change_flag_lfo_wf);
	_state_set.add_observer (Param_SNH        , _param_change_flag_lfo_shape);
	_state_set.add_observer (Param_SMOOTH     , _param_change_flag_lfo_shape);
	_state_set.add_observer (Param_CHAOS      , _param_change_flag_lfo_phase);
	_state_set.add_observer (Param_PH_DIST_AMT, _param_change_flag_lfo_phase);
	_state_set.add_observer (Param_PH_DIST_OFS, _param_change_flag_lfo_phase);
	_state_set.add_observer (Param_SIGN       , _param_change_flag_lfo_shape);
	_state_set.add_observer (Param_POLARITY   , _param_change_flag_lfo_shape);
	_state_set.add_observer (Param_VAR1       , _param_change_flag_lfo_wf);
	_state_set.add_observer (Param_VAR2       , _param_change_flag_lfo_wf);
	_state_set.add_observer (Param_PHASE_SET  , _param_change_flag_lfo_phset);

	_param_change_flag_lfo_base .add_observer (_param_change_flag_lfo);
	_param_change_flag_lfo_phase.add_observer (_param_change_flag_lfo);
	_param_change_flag_lfo_shape.add_observer (_param_change_flag_lfo);
	_param_change_flag_lfo_wf   .add_observer (_param_change_flag_lfo);
	_param_change_flag_lfo_phset.add_observer (_param_change_flag_lfo);

	_param_change_flag_stdif.add_observer (_param_change_flag);
	_param_change_flag_misc .add_observer (_param_change_flag);
	_param_change_flag_lfo  .add_observer (_param_change_flag);
	
	_state_set.set_ramp_time (Param_MIX        , 0.010);
	_state_set.set_ramp_time (Param_FEEDBACK   , 0.010);
	_state_set.set_ramp_time (Param_CHNCROSS   , 0.010);
	_state_set.set_ramp_time (Param_PAN        , 0.010);

	for (auto &chn : _chn_arr)
	{
		// Univibe cap values: 0.015 uF, 0.22 uF, 470 pF and 0.0047 uF
		chn._stage_arr [0]._c1 =  15e-9f;
		chn._stage_arr [1]._c1 = 220e-9f;
		chn._stage_arr [2]._c1 = 470e-12f;
		chn._stage_arr [3]._c1 = 4.7e-9f;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	SkoolMood::do_get_state () const
{
	return _state;
}



double	SkoolMood::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	SkoolMood::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_lamptc = _inv_fs / (0.012f + _inv_fs); // Guessing twiddle factor
	_k      =  2 * _sample_freq;
	_kc2    = _k * _c2;
	_kgain  = _k * _gain;

	for (auto &chn : _chn_arr)
	{
		chn._lfo.set_sample_freq (sample_freq);

		chn._drc    = _dtc;
		chn._alpha  = 1.0f - _inv_fs / (chn._drc + _inv_fs);
		chn._dalpha = chn._alpha;

		for (auto &stage : chn._stage_arr)
		{
			stage._kc2oc2pc1 = _kc2 / (_c2 + stage._c1);

			// Vo / Ve driven from emitter
			stage._en1 = _k *  _r1        * stage._c1;
			stage._en0 = 1;
			stage._ed1 = _k * (_r1 + _rv) * stage._c1;
			stage._ed0 = 1 + stage._c1 / _c2;

			// Vc ~= Ve / (Ic * Re * alpha^2) collector voltage from current input
			// Output here represents voltage at the collector
			stage._cn1 = _kgain * _rv * stage._c1;
			stage._cn0 = _gain * (1.0f + stage._c1 / _c2);
			stage._cd1 = _k * (_r1 + _rv) * stage._c1;
			stage._cd0 = 1 + stage._c1 / _c2;

			// Contribution from emitter load through passive filter network
			stage._ecd1 = (_r1 + _rv) * stage._c1 * stage._kc2oc2pc1;
			stage._ecd0 = 1;
			stage._ecn1 = _gain * _r1 * stage._ecd1 / _rv;
			stage._ecn0 = 0;

			// Represents Vo / Vc. Output over collector voltage
			stage._on1 = _kc2 * _rv;
			stage._on0 = 1;
			stage._od1 = stage._on1;
			stage._od0 = 1 + _c2 / stage._c1;

			// Bilinear xform stuff
			stage.setup_all_z_eq ();
		}
	}

	_param_change_flag_misc     .set ();
	_param_change_flag_stdif    .set ();
	_param_change_flag_lfo_base .set ();
	_param_change_flag_lfo_phase.set ();
	_param_change_flag_lfo_phset.set ();
	_param_change_flag_lfo_shape.set ();
	_param_change_flag_lfo_wf   .set ();

	update_param (true);
	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



/*
Optimisations done in this function:

1. drc = _dtc * exp (smtc)

	smtc is in [-1.5 ; 0]
	FindFit[
		Table[{x, Exp[x]}, {x, -1.5, 0, 0.1}],
		a*x^3 + b*x^2 + c*x + d,
		{a, b, c, d}, x
	]
	exp (x) ~ 0.0814891 * x^3 + 0.430334 * x^2 + 0.979987 * x + 0.998993


2. fx = exp (_ra_log / float (log (exp (1) + x0)))

	Assumes Ra = 500000
	x0 is in [0 ; 5.1] for Rb = 600
	Upper bound for x0 depends on Rb. Don't go below 100 for this formula
	which is invalid above 15.
	FindFit[
		Table[{x, Exp[Log[500000]/Log[x + Exp[1]]]}, {x, 0, 7, 0.05}],
		(a*x^3 + b*x^2 + c*x + d)/(e*x^3 + f*x^2 + g*x + h),
		{a, b, c, d, e, f, g, h}, x
	]
	exp (_ra_log / log (exp (1) + x)) ~
		  (-483.794 * x^3 + 7939.76 * x^2 - 17615.6 * x + 128782)
		/ ( 1.82899 * x^3 + 2.12768 * x^2 + 1.21012 * x + 0.257557)
*/

void	SkoolMood::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      chn_src_lst = nbr_chn_src - 1;

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	const float    time_step = fstb::rcp_uint <float> (proc._nbr_spl);
	for (auto &chn : _chn_arr)
	{
		chn._pan_lvl.set_time (proc._nbr_spl, time_step);
		chn._pan_lvl.tick (proc._nbr_spl);
	}
	_feedback.set_time (proc._nbr_spl, time_step);
	_chncross.set_time (proc._nbr_spl, time_step);
	_chncross.tick (proc._nbr_spl);
	_mix.set_time (proc._nbr_spl, time_step);
	_mix.tick (proc._nbr_spl);

	// Signal processing
	int            pos_blk = 0;
	do
	{
		const int      len_blk =
			std::min (proc._nbr_spl - pos_blk, int (_max_blk_size));
		const float    time_step_blk = fstb::rcp_uint <float> (len_blk);

		_feedback.tick (len_blk);
		const float    fdbk_step = _feedback.get_step ();

		for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
		{
			const int      chn_i_idx = std::min (chn_index, chn_src_lst);
			Channel &      chn = _chn_arr [chn_index];

			const float *  src_ptr = &proc._src_arr [chn_i_idx] [pos_blk];
			float *        dst_ptr = &proc._dst_arr [chn_index] [pos_blk];

			float          fdbk_cur = _feedback.get_beg ();

			chn._lfo.tick (len_blk);
			const float    lfo_tgt = get_lfo (chn._lfo);

			chn._lfo_val.set_time (len_blk, time_step_blk);
			chn._lfo_val.set_val (lfo_tgt);
			chn._lfo_val.tick (len_blk);
			float          lfo_cur = chn._lfo_val.get_beg ();
			const float    lfo_stp = chn._lfo_val.get_step ();

			for (int pos = 0; pos < len_blk; ++pos)
			{
				// Lamp
				chn._g  += _lamptc * (lfo_cur - chn._g);
				lfo_cur += lfo_stp;

				// Cds
				chn._step   = chn._g * chn._alpha + chn._step * chn._dalpha;
				const float    smtc = chn._step * _mintc;
#if 0 // Original formula
				chn._drc    = _dtc * exp (smtc);
#else // Optimisation by approximation
				chn._drc    =
					((  (_dtc * 0.0814891f) * smtc
					  + (_dtc * 0.430334f)) * smtc
					  + (_dtc * 0.979987f)) * smtc
					  + (_dtc * 0.998993f);
				assert (chn._drc > 0);
#endif // Org/opt
				chn._alpha  = _inv_fs / (chn._drc + _inv_fs);
				// Different attack & release character
#if 0 // Original formula
				chn._dalpha = 1.f - _inv_fs / (0.5f * chn._drc + _inv_fs);
#else // Optimisation by approximation
				// chn._drc >> _inv_fs
				chn._dalpha = 1.f - 2 * chn._alpha;
#endif // Org/opt
				const float    x0   = chn._step * _b;
#if 0 // Original formula
				const float    x    = float (fstb::EXP1) + x0;
				const float    ralx = _ra_log / float (log (x));
				const float    fx   = float (exp (ralx));
#else // Optimisation by approximation
				assert (_ra == 500000); // Approx valid only for Ra = 500k
				assert (x0 < 15);       // Lowering Rb increases the x0 range
				const float     fx_n =
					((-483.794f * x0 + 7939.76f) * x0 - 17615.6f) * x0 + 128782.f;
				const float     fx_d =
					(( 1.82899f * x0 + 2.12768f) * x0 + 1.21012f) * x0 + 0.257557f;
				const float     fx   = fx_n / fx_d;
#endif // Org/opt

				if ((pos & (_mod_rate - 1)) == 0)
				{
					modulate (chn, fx);
				}

				float          input = shape_bjt (src_ptr [pos] + chn._fb);
				const float    emitterfb = 25.f / fx;

				// 4 stages phasing
				float          ocvolt;
				for (int stage_cnt = 0; stage_cnt < _nbr_stages; ++stage_cnt)
				{
					Stage &        stage = chn._stage_arr [stage_cnt];

					const float    cvolt_fb = emitterfb * stage._cvolt_old;
					const float    cvolt =
						  stage._ecvc.process_sample (input)
						+ stage._vc.process_sample (input + cvolt_fb);

					ocvolt = stage._vcvo.process_sample (cvolt);
					stage._cvolt_old = ocvolt;

					const float    evolt = stage._vevo.process_sample (input);

					input = shape_bjt (ocvolt + evolt);
				}

				chn._fb   = fdbk_cur * ocvolt;
				fdbk_cur += fdbk_step;

				dst_ptr [pos] = input;
			}
		}

		pos_blk += len_blk;
	}
	while (pos_blk < proc._nbr_spl);

	const float       wet_b = _mix.get_beg ();
	const float       wet_e = _mix.get_end ();

	// Stereo: cross + pan + wet mix at once
	if (nbr_chn_dst == 2)
	{
		const float    send_b  = _chncross.get_beg ();
		const float    send_e  = _chncross.get_end ();
		const float    self_b  = 1 - float (fabs (send_b));
		const float    self_e  = 1 - float (fabs (send_e));
		const float    wet_l_b = _chn_arr [0]._pan_lvl.get_beg () * wet_b;
		const float    wet_l_e = _chn_arr [0]._pan_lvl.get_end () * wet_e;
		const float    wet_r_b = _chn_arr [1]._pan_lvl.get_beg () * wet_b;
		const float    wet_r_e = _chn_arr [1]._pan_lvl.get_end () * wet_e;
		const dsp::StereoLevel sl_b (
			self_b * wet_l_b,
			send_b * wet_r_b,
			send_b * wet_l_b,
			self_b * wet_r_b
		);
		const dsp::StereoLevel sl_e (
			self_e * wet_l_e,
			send_e * wet_r_e,
			send_e * wet_l_e,
			self_e * wet_r_e
		);
		dsp::mix::Align::copy_mat_2_2_vlrauto (
			proc._dst_arr [0], proc._dst_arr [1],
			proc._dst_arr [0], proc._dst_arr [1],
			proc._nbr_spl, sl_b, sl_e
		);
	}

	// Other channel configurations
	else
	{
		for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
		{
			// Mix, wet part
			float          lvl_b = wet_b;
			float          lvl_e = wet_e;

			// Panning, if applicable
			if (nbr_chn_dst > 1)
			{
				Channel &      chn = _chn_arr [chn_index];
				lvl_b *= chn._pan_lvl.get_beg ();
				lvl_e *= chn._pan_lvl.get_end ();
			}

			dsp::mix::Align::scale_1_vlrauto (
				proc._dst_arr [chn_index], proc._nbr_spl, lvl_b, lvl_e
			);
		}
	}

	// Mix, dry part
	const float       dry_b = 1 - wet_b;
	const float       dry_e = 1 - wet_e;
	for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
	{
		const int      chn_i_idx = std::min (chn_index, chn_src_lst);
		const float *  src_ptr = proc._src_arr [chn_i_idx];
		float *        dst_ptr = proc._dst_arr [chn_index];

		dsp::mix::Align::mix_1_1_vlrauto (
			dst_ptr, src_ptr, proc._nbr_spl, dry_b, dry_e
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SkoolMood::Stage::setup_all_z_eq ()
{
	setup_z_eq (_vc  , _cn0 , _cn1 , _cd0 , _cd1 );
	setup_z_eq (_ecvc, _ecn0, _ecn1, _ecd0, _ecd1);
	setup_z_eq (_vcvo, _on0 , _on1 , _od0 , _od1 );
	setup_z_eq (_vevo, _en0 , _en1 , _ed0 , _ed1 );
}



void	SkoolMood::Stage::setup_z_eq (dsp::iir::OnePole &flt, float n0, float n1, float d0, float d1)
{
	const float    a0_inv  = 1.f / (d0 + d1);
	float          b_z [2] = { a0_inv * (n0 + n1), a0_inv * (n0 - n1) };
	float          a_z [2] = {                  1, a0_inv * (d0 - d1) };
	flt.set_z_eq (b_z, a_z);
}



void	SkoolMood::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._lfo.clear_buffers ();
		chn._lfo_val.set_val (get_lfo (chn._lfo));
		chn._lfo_val.clear_buffers ();
		chn._pan_lvl.clear_buffers ();
		chn._step = 0;
		chn._fb   = 0;
		chn._g    = 0;

		for (auto &stage : chn._stage_arr)
		{
			stage._vc.clear_buffers ();
			stage._vcvo.clear_buffers ();
			stage._ecvc.clear_buffers ();
			stage._vevo.clear_buffers ();
			stage._cvolt_old = 0;
		}
	}
}



void	SkoolMood::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		bool        resync_flag = false;

		// Effect parameters
		if (_param_change_flag_misc (true) || force_flag)
		{
			const float    mix      = float (_state_set.get_val_end_nat (Param_MIX));
			const float    width    = float (_state_set.get_val_tgt_nat (Param_WIDTH));
			const float    feedback = float (_state_set.get_val_end_nat (Param_FEEDBACK));
			const float    depth    = float (_state_set.get_val_tgt_nat (Param_DEPTH));
			const float    chncross = float (_state_set.get_val_end_nat (Param_CHNCROSS));
			const float    pan      = float (_state_set.get_val_end_nat (Param_PAN));

			_width = width * 0.5f;
			_depth = depth;

			_mix.set_val (mix);
			_chncross.set_val (chncross);
			_feedback.set_val (feedback);

			const std::array <float, 2> pan_lvl {{
				compute_pan_lvl (pan),
				compute_pan_lvl (-pan)
			}};
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				Channel &      chn = _chn_arr [chn_cnt];
				chn._pan_lvl.set_val (pan_lvl [chn_cnt & 1]);
			}

		}

		if (_param_change_flag_stdif (true) || force_flag)
		{
			_st_dif     = float (_state_set.get_val_tgt_nat (Param_STDIF));
			resync_flag = true;
		}

		// LFO
		if (_param_change_flag_lfo (true) || force_flag)
		{
			if (_param_change_flag_lfo_base (true) || force_flag)
			{
				const double  spd = _state_set.get_val_tgt_nat (Param_SPEED);
				const double  per = 1.0 / spd;
				for (auto &chn : _chn_arr)
				{
					chn._lfo.set_period (per);
				}
			}
			if (_param_change_flag_lfo_phase (true) || force_flag)
			{
				const float    pch = float (_state_set.get_val_tgt_nat (Param_CHAOS));
				const float    pda = float (_state_set.get_val_tgt_nat (Param_PH_DIST_AMT));
				const float    pdo = float (_state_set.get_val_tgt_nat (Param_PH_DIST_OFS));

				for (auto &chn : _chn_arr)
				{
					chn._lfo.set_chaos (pch);
					chn._lfo.set_phase_dist (pda);
					chn._lfo.set_phase_dist_offset (pdo);
				}
			}
			if (_param_change_flag_lfo_phset (true) || force_flag)
			{
				float          p = float (_state_set.get_val_tgt_nat (Param_PHASE_SET));
				if (p >= 1)
				{
					p = 0;
				}
				_chn_arr [0]._lfo.set_phase (p);
				resync_flag = true;
			}
			if (_param_change_flag_lfo_shape (true) || force_flag)
			{
				const float    snh = float (_state_set.get_val_tgt_nat (Param_SNH));
				const float    smt = float (_state_set.get_val_tgt_nat (Param_SMOOTH));
				const bool     inv_flag =
					(_state_set.get_val_tgt_nat (Param_SIGN) >= 0.5f);
				const bool     uni_flag =
					(_state_set.get_val_tgt_nat (Param_POLARITY) >= 0.5f);
				for (auto &chn : _chn_arr)
				{
					chn._lfo.set_snh (snh);
					chn._lfo.set_smooth (smt);
					chn._lfo.set_sign (inv_flag);
					chn._lfo.set_polarity (uni_flag);
				}
			}
			if (_param_change_flag_lfo_wf (true) || force_flag)
			{
				const LfoType  wf = static_cast <LfoType> (fstb::round_int (
					_state_set.get_val_tgt_nat (Param_WAVEFORM)
				));
				dsp::ctrl::lfo::LfoModule::Type  wf2 =
					dsp::ctrl::lfo::LfoModule::Type_SINE;
				switch (wf)
				{
				case LfoType_SINE:      wf2 = dsp::ctrl::lfo::LfoModule::Type_SINE      ; break;
				case LfoType_TRIANGLE:  wf2 = dsp::ctrl::lfo::LfoModule::Type_TRIANGLE  ; break;
				case LfoType_SQUARE:    wf2 = dsp::ctrl::lfo::LfoModule::Type_SQUARE    ; break;
				case LfoType_SAW:       wf2 = dsp::ctrl::lfo::LfoModule::Type_SAW       ; break;
				case LfoType_PARABOLA:  wf2 = dsp::ctrl::lfo::LfoModule::Type_PARABOLA  ; break;
				case LfoType_BIPHASE:   wf2 = dsp::ctrl::lfo::LfoModule::Type_BIPHASE   ; break;
				case LfoType_N_PHASE:   wf2 = dsp::ctrl::lfo::LfoModule::Type_N_PHASE   ; break;
				case LfoType_VARISLOPE: wf2 = dsp::ctrl::lfo::LfoModule::Type_VARISLOPE ; break;
				case LfoType_NOISE_FLT: wf2 = dsp::ctrl::lfo::LfoModule::Type_NOISE_FLT2; break;
				default:
					assert (false);
					break;
				}
				const float    v1 = float (_state_set.get_val_tgt_nat (Param_VAR1));
				const float    v2 = float (_state_set.get_val_tgt_nat (Param_VAR2));
				for (auto &chn : _chn_arr)
				{
					chn._lfo.set_type (wf2);
					chn._lfo.set_variation (0, v1);
					chn._lfo.set_variation (1, v2);
				}
			}
		}

		if (resync_flag)
		{
			resync_lfo_phases ();
		}
	}
}



void	SkoolMood::resync_lfo_phases ()
{
	double         phase = _chn_arr [0]._lfo.get_phase ();
	for (int chn_cnt = 1; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		phase += _st_dif;
		if (phase >= 1)
		{
			phase -= 1;
		}
		else if (phase < 0)
		{
			phase += 1;
		}
		_chn_arr [chn_cnt]._lfo.set_phase (phase);
	}
}



float	SkoolMood::get_lfo (const dsp::ctrl::lfo::LfoModule &lfo) const
{
	const float    r = float (lfo.get_val ());
	float          x = r * _width + _depth;
	x = fstb::limit (x, 0.f, 1.f);

	// Emulates lamp turn on/off characteristic by typical curves
	// Approximates 2 - 2 / (x + 1) on [0 ; 1]
	const float    val_out = (((29.f/64) * x - 87.f/64) * x + 61.f/32) * x;

	return val_out;
}



void	SkoolMood::modulate (Channel &chn, float ldr)
{
	const float    rv        = _rv + ldr;
	const float    r1prv     = _r1 + rv;

	const float    kr1prv    = _k * r1prv;
	const float    kgainrv   = _kgain * rv;
	const float    gainr1orv = (_gain * _r1) / rv;

	for (auto &stage : chn._stage_arr)
	{
		// Vo / Ve driven from emitter
		stage._ed1 = kr1prv * stage._c1;

		// Vc ~= Ve / (Ic * Re * alpha^2) collector voltage from current input
		// Output here represents voltage at the collector
		stage._cn1 = kgainrv * stage._c1;
		stage._cd1 = stage._ed1;

		// Contribution from emitter load through passive filter network
		stage._ecd1 = stage._cd1  * stage._kc2oc2pc1;
		stage._ecn1 = stage._ecd1 * gainr1orv;

		// Represents Vo/Vc. Output over collector voltage
		stage._on1 = rv * _kc2;
		stage._od1 = stage._on1;

		// Bilinear xform stuff
		stage.setup_all_z_eq ();
	}
}



float	SkoolMood::shape_bjt (float x)
{
	// PSU voltage
	const float    a    = 15.f;

	// b was 0.8f originally, but 1 makes the curve C1 on the negative corner
	// Resulting difference is tiny anyway
	const float    b    = 1.f;

	float          vin  = (a * 0.5f) * (1 + x);
	vin = fstb::limit (vin, 0.f, a);

	// Really rough, simplistic bjt turn-on emulator
	const float    vbe  = b - b / (vin + 1);
	float          vout = vin - vbe;

	// Returns gain to unity & zero the DC
	vout = vout * (2.f / a) - (1 - b / (a * 0.5f + 1)); 

	return vout;
}



// x in [-1 ; 1]
// Returns the value for the left channel. Right channel = fnc (-x)
float	SkoolMood::compute_pan_lvl (float x)
{
	assert (x >= -1);
	assert (x <= 1);

	const float    xp1  = x + 1;
	const float    xp13 = xp1 * xp1 * xp1;
	const float    p    = 16.f/11 + (-5.f/11 + x * (3.f/11)) * xp13;

	return p;
}



// Swing was measured on operating device of: 10k to 250k.
// 400k is reported to sound better for the "low end" (high resistance)
// Because of time response, Rb needs to be driven further.
// End resistance will max out to around 10k for most LFO freqs.
// pushing low end a little lower for kicks and giggles
const float	SkoolMood::_ra    = 500000;
const float	SkoolMood::_ra_log = float (log (_ra));
const float	SkoolMood::_rb    = 600;
const float	SkoolMood::_b     = float (exp (_ra_log / log (_rb)) - fstb::EXP1);
const float	SkoolMood::_r1    = 4700.f;
const float	SkoolMood::_rv    = 4700.f;
const float	SkoolMood::_c2    = 1e-6f;
const float	SkoolMood::_beta  = 150.f;
const float	SkoolMood::_gain  = -_beta / (_beta + 1);
const float	SkoolMood::_dtc   = 0.045f;
const float	SkoolMood::_mintc = float (log (0.0025f / _dtc)); // ~ -2.8



}  // namespace smood
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
