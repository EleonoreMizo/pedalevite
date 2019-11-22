/*****************************************************************************

        HarmTrem.cpp
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
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/tremh/HarmTrem.h"
#include "mfx/pi/tremh/LfoType.h"
#include "mfx/pi/tremh/Param.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace tremh
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



HarmTrem::HarmTrem ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_vol ()
,	_param_change_flag_tone ()
,	_param_change_flag_lfo ()
,	_param_change_flag_lfo_base ()
,	_param_change_flag_lfo_phase ()
,	_param_change_flag_lfo_phset ()
,	_param_change_flag_lfo_shape ()
,	_param_change_flag_lfo_wf ()
,	_lfo ()
,	_amt (0)
,	_bias (0)
,	_sat (0.5f)
,	_tone (0)
,	_stereo (0)
,	_lo (0)
,	_hi (2)
,	_freq (1500)
,	_chn_arr ()
,	_buf_arr ()
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SPEED      , 5);
	_state_set.set_val_nat (desc_set, Param_AMT        , 1);
	_state_set.set_val_nat (desc_set, Param_LO         , +1);
	_state_set.set_val_nat (desc_set, Param_HI         , -1);
	_state_set.set_val_nat (desc_set, Param_FREQ       , 1500);
	_state_set.set_val_nat (desc_set, Param_GSAT       , 0.75);
	_state_set.set_val_nat (desc_set, Param_BIAS       , 0);
	_state_set.set_val_nat (desc_set, Param_TONE       , 0);
	_state_set.set_val_nat (desc_set, Param_STEREO     , 0);
	_state_set.set_val_nat (desc_set, Param_WAVEFORM   , LfoType_SINE);
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

	_state_set.add_observer (Param_AMT        , _param_change_flag_vol);
	_state_set.add_observer (Param_GSAT       , _param_change_flag_vol);
	_state_set.add_observer (Param_BIAS       , _param_change_flag_vol);

	_state_set.add_observer (Param_LO         , _param_change_flag_tone);
	_state_set.add_observer (Param_HI         , _param_change_flag_tone);
	_state_set.add_observer (Param_FREQ       , _param_change_flag_tone);
	_state_set.add_observer (Param_TONE       , _param_change_flag_tone);
	_state_set.add_observer (Param_STEREO     , _param_change_flag_tone);

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

	_param_change_flag_vol .add_observer (_param_change_flag);
	_param_change_flag_tone.add_observer (_param_change_flag);
	_param_change_flag_lfo .add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_AMT   , 0.010);
	_state_set.set_ramp_time (Param_LO    , 0.010);
	_state_set.set_ramp_time (Param_HI    , 0.010);
	_state_set.set_ramp_time (Param_GSAT  , 0.010);
	_state_set.set_ramp_time (Param_BIAS  , 0.010);
	_state_set.set_ramp_time (Param_TONE  , 0.010);
	_state_set.set_ramp_time (Param_STEREO, 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	HarmTrem::do_get_state () const
{
	return _state;
}



double	HarmTrem::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	HarmTrem::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_lfo.set_sample_freq (sample_freq);

	update_filter_freq ();

	const int      mbs_align = (max_buf_len + 3) & -4;
	for (auto &buf : _buf_arr)
	{
		buf.resize (mbs_align);
	}

	_param_change_flag_vol      .set ();
	_param_change_flag_tone     .set ();
	_param_change_flag_lfo_base .set ();
	_param_change_flag_lfo_phase.set ();
	_param_change_flag_lfo_phset.set ();
	_param_change_flag_lfo_shape.set ();
	_param_change_flag_lfo_wf   .set ();

	update_param (true);
	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	HarmTrem::do_clean_quick ()
{
	clear_buffers ();
}



void	HarmTrem::clear_buffers ()
{
	_lfo.clear_buffers ();
	for (auto &chn : _chn_arr)
	{
		chn._lpf.clear_buffers ();
	}
}



void	HarmTrem::do_process_block (piapi::ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);

	// Events
	for (int evt_cnt = 0; evt_cnt < proc._nbr_evt; ++evt_cnt)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [evt_cnt]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	const int      nbr_spl  = proc._nbr_spl;

	// Parameters
	const float    lfo_beg  = float (_lfo.get_val ());
	const float    amt_beg  = _amt;
	const float    bias_beg = _bias;
	const float    sat_beg  = _sat;
	const float    tone_beg = _tone;
	const float    ster_beg = _stereo;
	const float    lo_beg   = _lo;
	const float    hi_beg   = _hi;
	_state_set.process_block (nbr_spl);
	update_param (false);
	_lfo.tick (nbr_spl);
	const float    lfo_end  = float (_lfo.get_val ());
	const float    amt_end  = _amt;
	const float    bias_end = _bias;
	const float    sat_end  = _sat;
	const float    tone_end = _tone;
	const float    ster_end = _stereo;
	const float    lo_end   = _lo;
	const float    hi_end   = _hi;

	// Computes the resulting volume
	// Vector content: Beg+ End+ Beg- End-
	//             or: Beg  End   x    x
	const auto     v_amt  = fstb::ToolsSimd::set_2f32_fill (amt_beg , amt_end );
	const auto     v_bias = fstb::ToolsSimd::set_2f32_fill (bias_beg, bias_end);
	const auto     v_sat  = fstb::ToolsSimd::set_2f32_fill (sat_beg , sat_end );
	const auto     v_tone = fstb::ToolsSimd::set_2f32_fill (tone_beg, tone_end);
	const auto     v_ster = fstb::ToolsSimd::set_2f32_fill (ster_beg, ster_end);
	const auto     v_t_lo = fstb::ToolsSimd::set_2f32_fill (lo_beg, lo_end);
	const auto     v_t_hi = fstb::ToolsSimd::set_2f32_fill (hi_beg, hi_end);
	const auto     v_lfo  = fstb::ToolsSimd::set_f32 (lfo_beg , lfo_end, -lfo_beg , -lfo_end);
	const auto     one    = fstb::ToolsSimd::set1_f32 (1);
	const auto     half   = fstb::ToolsSimd::set1_f32 (0.5f);
	const auto     zero   = fstb::ToolsSimd::set_f32_zero ();
	const auto     v_satm = fstb::ToolsSimd::set1_f32 (1e-4f);
	const auto     v_p2n2 = fstb::ToolsSimd::set_2f32_dbl (1, -1);

	auto           vol    = one + v_amt * (v_lfo + v_bias);
	vol = fstb::ToolsSimd::max_f32 (vol, zero);

	// Saturation:
	// x = (1 - (1 - min (0.5 * s * x, 1)) ^ 2) / s
	const auto     v_satl = fstb::ToolsSimd::max_f32 (v_sat, v_satm);
	const auto     inv_s  = fstb::ToolsSimd::rcp_approx2 (v_satl);
	vol = one - fstb::ToolsSimd::min_f32 (vol * v_satl * half, one);
	vol = (one - vol * vol) * inv_s;

	// From the modulation (4 elements):
	const auto     v_lo_m = fstb::ToolsSimd::max_f32 (v_t_lo * v_p2n2, zero);
	const auto     v_hi_m = fstb::ToolsSimd::max_f32 (v_t_hi * v_p2n2, zero);

	// From the original signal (2 elements, duplicated):
	const auto     v_lo_m2 = fstb::ToolsSimd::Shift <2>::rotate (v_lo_m);
	const auto     v_hi_m2 = fstb::ToolsSimd::Shift <2>::rotate (v_hi_m);
	const auto     v_lo_o = one - v_lo_m - v_lo_m2;
	const auto     v_hi_o = one - v_hi_m - v_hi_m2;

	// Resulting levels + stereo
	// vl = m+ * lfo+ + m- * lfo-
	// vr = m+ * lerp (lfo+, lfo-) + m- * lerp (lfo-, lfo+)
	// v = m+ * lfo+ + m- * lfo-   |   m+ * lerp (lfo+, lfo-) + m- * lerp (lfo-, lfo+)
	auto           vol_r  =
		vol + v_ster * (fstb::ToolsSimd::swap_2f32 (vol) - vol);

	auto           v_lo_l = vol   * v_lo_m;
	auto           v_lo_r = vol_r * v_lo_m;
	v_lo_l += fstb::ToolsSimd::swap_2f32 (v_lo_l);
	v_lo_r += fstb::ToolsSimd::swap_2f32 (v_lo_r);
	auto           v_lo   = fstb::ToolsSimd::interleave_2f32_lo (v_lo_l, v_lo_r);
	v_lo += v_lo_o;

	auto           v_hi_l = vol   * v_hi_m;
	auto           v_hi_r = vol_r * v_hi_m;
	v_hi_l += fstb::ToolsSimd::swap_2f32 (v_hi_l);
	v_hi_r += fstb::ToolsSimd::swap_2f32 (v_hi_r);
	auto           v_hi   = fstb::ToolsSimd::interleave_2f32_lo (v_hi_l, v_hi_r);
	v_hi += v_hi_o;

	// Bass/treble level from the tone setting
	auto           v_to_b = v_tone * v_p2n2;
	const auto     two    = fstb::ToolsSimd::set1_f32 (2);
	const auto     three  = fstb::ToolsSimd::set1_f32 (3);
	v_to_b = fstb::ToolsSimd::max_f32 (v_to_b, zero);
	v_to_b = one + v_to_b * v_to_b * (v_to_b * two - three);
	fstb::ToolsSimd::VectF32   v_to_t;
	fstb::ToolsSimd::spread_2f32 (v_to_b, v_to_t, v_to_b);

	v_lo *= v_to_b;
	v_hi *= v_to_t;

	// Signal
	int            chn_src_idx = 0;
	const int      chn_src_inc = (nbr_chn_dst <= nbr_chn_src) ? 1 : 0;
	for (int chn_cnt = 0; chn_cnt < nbr_chn_dst; ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_src_idx];

		// New input channel?
		if (chn_cnt == 0 || chn_src_inc > 0)
		{
			const float *  src_ptr = proc._src_arr [chn_src_idx];

			// Bass part
			chn._lpf.process_block (&_buf_arr [0] [0], src_ptr, nbr_spl);

			// Treble part
			for (int pos = 0; pos < nbr_spl; pos += 4)
			{
				const auto     s = fstb::ToolsSimd::load_f32 (src_ptr + pos);
				const auto     l = fstb::ToolsSimd::load_f32 (&_buf_arr [0] [pos]);
				const auto     h = s - l;
				fstb::ToolsSimd::store_f32 (&_buf_arr [1] [pos], h);
			}
		}

		// Mixing
		const bool     r_flag  = (chn_cnt > 0); // Uses the right channel settings
		float * const  dst_ptr = proc._dst_arr [chn_cnt];
		mix_buf (dst_ptr, 0, nbr_spl, v_lo, r_flag, true);
		mix_buf (dst_ptr, 1, nbr_spl, v_hi, r_flag, false);

		chn_src_idx += chn_src_inc;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	HarmTrem::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_vol (true) || force_flag)
		{
			_amt    = float (_state_set.get_val_end_nat (Param_AMT   ));
			_sat    = float (_state_set.get_val_end_nat (Param_GSAT  ));
			_bias   = float (_state_set.get_val_end_nat (Param_BIAS  ));
		}

		if (_param_change_flag_tone (true) || force_flag)
		{
			_lo     = float (_state_set.get_val_end_nat (Param_LO    ));
			_hi     = float (_state_set.get_val_end_nat (Param_HI    ));
			_freq   = float (_state_set.get_val_end_nat (Param_FREQ  ));
			_tone   = float (_state_set.get_val_end_nat (Param_TONE  ));
			_stereo = float (_state_set.get_val_end_nat (Param_STEREO));
			update_filter_freq ();
		}

		if (_param_change_flag_lfo (true) || force_flag)
		{
			if (_param_change_flag_lfo_base (true) || force_flag)
			{
				const double  spd = _state_set.get_val_tgt_nat (Param_SPEED);
				_lfo.set_period (1.0 / spd);
			}
			if (_param_change_flag_lfo_phase (true) || force_flag)
			{
				const float    pch = float (_state_set.get_val_tgt_nat (Param_CHAOS));
				const float    pda = float (_state_set.get_val_tgt_nat (Param_PH_DIST_AMT));
				const float    pdo = float (_state_set.get_val_tgt_nat (Param_PH_DIST_OFS));

				_lfo.set_chaos (pch);
				_lfo.set_phase_dist (pda);
				_lfo.set_phase_dist_offset (pdo);
			}
			if (_param_change_flag_lfo_phset (true) || force_flag)
			{
				float          p = float (_state_set.get_val_tgt_nat (Param_PHASE_SET));
				if (p >= 1)
				{
					p = 0;
				}
				_lfo.set_phase (p);
			}
			if (_param_change_flag_lfo_shape (true) || force_flag)
			{
				const float    snh = float (_state_set.get_val_tgt_nat (Param_SNH));
				const float    smt = float (_state_set.get_val_tgt_nat (Param_SMOOTH));
				const bool     inv_flag =
					(_state_set.get_val_tgt_nat (Param_SIGN) >= 0.5f);
				const bool     uni_flag =
					(_state_set.get_val_tgt_nat (Param_POLARITY) >= 0.5f);
				_lfo.set_snh (snh);
				_lfo.set_smooth (smt);
				_lfo.set_sign (inv_flag);
				_lfo.set_polarity (uni_flag);
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
				_lfo.set_type (wf2);
				const float    v1 = float (_state_set.get_val_tgt_nat (Param_VAR1));
				const float    v2 = float (_state_set.get_val_tgt_nat (Param_VAR2));
				_lfo.set_variation (0, v1);
				_lfo.set_variation (1, v2);
			}
		}
	}
}



void	HarmTrem::update_filter_freq ()
{
	assert (_inv_fs > 0);

	static const float   b_s [2] = { 1, 0 };
	static const float   a_s [2] = { 1, 1 };
	float                b_z [2];
	float                a_z [2];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		b_z, a_z, b_s, a_s, k
	);
	for (auto &chn : _chn_arr)
	{
		chn._lpf.set_z_eq (b_z, a_z);
	}
}



void	HarmTrem::mix_buf (float dst_ptr [], int buf, int nbr_spl, fstb::ToolsSimd::VectF32 v_gain, bool r_flag, bool copy_flag) const
{
	float          gain_beg;
	float          gain_end;

	if (r_flag)
	{
		gain_beg = fstb::ToolsSimd::Shift <2>::extract (v_gain);
		gain_end = fstb::ToolsSimd::Shift <3>::extract (v_gain);
	}
	else
	{
		gain_beg = fstb::ToolsSimd::Shift <0>::extract (v_gain);
		gain_end = fstb::ToolsSimd::Shift <1>::extract (v_gain);
	}
	if (copy_flag)
	{
		dsp::mix::Align::copy_1_1_vlrauto (
			dst_ptr, &_buf_arr [buf] [0], nbr_spl, gain_beg, gain_end
		);
	}
	else
	{
		dsp::mix::Align::mix_1_1_vlrauto (
			dst_ptr, &_buf_arr [buf] [0], nbr_spl, gain_beg, gain_end
		);
	}
}



}  // namespace tremh
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
