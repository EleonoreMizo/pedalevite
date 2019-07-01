/*****************************************************************************

        OnsetDetect2.cpp
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

#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/osdet2/Param.h"
#include "mfx/pi/osdet2/OnsetDetect2.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace osdet2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OnsetDetect2::OnsetDetect2 ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_buf_env_vol ()
,	_buf_old_vol ()
,	_buf_tmp ()
,	_buf_os_dly ()
,	_buf_sum ()
,	_prefilter ()
,	_dly_vol ()
,	_env_pk ()
,	_deriv_smth ()
,	_flt_sum ()
,	_flt_med ()
,	_dly_pk ()
,	_velo_clip_flag (true)
,	_hpf_freq (1000)
,	_lpf1_freq (25)
,	_lpf2_freq (25)
,	_a (62)
,	_b (2)
,	_beta (0.06e-4f)
,	_alpha (0.8f)
,	_lambda (0.8f)
,	_sum_mul (1.f / 64)
,	_rls_thr (1e-3f)
,	_rls_ratio (1.414f)
,	_last_count (0)
,	_last_delay (1000)
,	_note_flag (false)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_VELO_CLIP, 1);
	_state_set.set_val_nat (desc_set, Param_HP       , 1000);
	_state_set.set_val_nat (desc_set, Param_LP1      , 25);
	_state_set.set_val_nat (desc_set, Param_LP2      , 25);
	_state_set.set_val_nat (desc_set, Param_A        , 62);
	_state_set.set_val_nat (desc_set, Param_B        , 2);
	_state_set.set_val_nat (desc_set, Param_BETA     , 0.06);
	_state_set.set_val_nat (desc_set, Param_LAMBDA   , 0.8);
	_state_set.set_val_nat (desc_set, Param_ALPHA    , 0.8);
	_state_set.set_val_nat (desc_set, Param_RLS_THR  , 1e-3);
	_state_set.set_val_nat (desc_set, Param_RLS_RATIO, 1.414);

	_state_set.add_observer (Param_VELO_CLIP, _param_change_flag);
	_state_set.add_observer (Param_HP       , _param_change_flag);
	_state_set.add_observer (Param_LP1      , _param_change_flag);
	_state_set.add_observer (Param_LP2      , _param_change_flag);
	_state_set.add_observer (Param_A        , _param_change_flag);
	_state_set.add_observer (Param_B        , _param_change_flag);
	_state_set.add_observer (Param_BETA     , _param_change_flag);
	_state_set.add_observer (Param_LAMBDA   , _param_change_flag);
	_state_set.add_observer (Param_ALPHA    , _param_change_flag);
	_state_set.add_observer (Param_RLS_THR  , _param_change_flag);
	_state_set.add_observer (Param_RLS_RATIO, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	OnsetDetect2::do_get_state () const
{
	return _state;
}



double	OnsetDetect2::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	OnsetDetect2::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_env_vol.resize (mbs_align);
	_buf_old_vol.resize (mbs_align);
	_buf_tmp.resize (mbs_align);
	_buf_os_dly.resize (mbs_align);
	_buf_sum.resize (mbs_align);

	const float    dly_max_s   = 0.100f;
	const float    dly_vol_s   = 0.080f;
	const int      dly_max_spl = fstb::round_int (sample_freq * dly_max_s);
	const int      dly_vol_spl = fstb::round_int (sample_freq * dly_vol_s);
	_dly_vol.setup (dly_max_spl, max_buf_len);
	_dly_vol.set_delay (dly_vol_spl);
	_dly_pk.setup (fstb::round_int (sample_freq * 0.005f), max_buf_len); // max (A + B)
	_last_delay = fstb::round_int (0.020f * sample_freq);

	_env_pk.set_sample_freq (sample_freq);

	_param_change_flag.set ();

	update_param (true);
	update_os_stuff ();

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	OnsetDetect2::do_clean_quick ()
{
	clear_buffers ();
}



void	OnsetDetect2::do_process_block (ProcInfo &proc)
{
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

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Signal processing

	const int      nbr_spl = proc._nbr_spl;

	_env_vol.process_block_no_sqrt (
		_buf_env_vol.data (), proc._src_arr [0], nbr_spl
	);
	_dly_vol.process_block (
		_buf_old_vol.data (), _buf_env_vol.data (), nbr_spl
	);

	// Boosts high-mid frequencies and lowers the bass frequencies
	_prefilter.process_block (_buf_tmp.data (), proc._src_arr [0], nbr_spl);

	// Squaring + smoothing
	_env_pk.process_block (_buf_tmp.data (), _buf_tmp.data (), nbr_spl);

	// Derivative + smoothing
	_deriv_smth.process_block (_buf_tmp.data (), _buf_tmp.data (), nbr_spl);

	// Delay of the pre-thresholded signal
	_dly_pk.process_block (_buf_os_dly.data (), _buf_tmp.data (), nbr_spl);

	// Dynamic threshold computation
	_flt_sum.process_block (_buf_sum.data (), _buf_tmp.data (), nbr_spl);
	_flt_med.process_block (_buf_tmp.data (), _buf_tmp.data (), nbr_spl);
	dsp::mix::Align::scale_1_v (_buf_tmp.data (), nbr_spl, _lambda);
	dsp::mix::Align::mix_1_1_v (
		_buf_tmp.data (), _buf_sum.data (), nbr_spl, _alpha * _sum_mul
	);

	// Thresholding & Note off detection
	float          ret_on  = 0;
	float          ret_off = 0;

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    vol2_cur = _buf_env_vol [pos];

		// Note On
		if (_last_count > 0)
		{
			-- _last_count;
		}
		else
		{
			const float    val   = _buf_os_dly [pos];
			const float    delta = _buf_tmp [pos];
			if (val > delta && val > _beta)
			{
				const float    sig_on = float (sqrt (vol2_cur));
				ret_on      = std::max (ret_on, sig_on);
				_note_flag  = true;
				_last_count = _last_delay - nbr_spl;
			}
		}

		// Note Off
		if (_note_flag)
		{
			const float    vol2_old = _buf_old_vol [pos];
			if (   vol2_cur * (_rls_ratio * _rls_ratio) < vol2_old
			    || vol2_cur < _rls_thr * _rls_thr)
			{
				ret_off = 1;
				_note_flag = false;
			}
		}
	}

	if (_velo_clip_flag)
	{
		ret_on = std::min (ret_on, 1.0f);
	}

	proc._sig_arr [0] [0] = ret_on;
	proc._sig_arr [1] [0] = ret_off;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OnsetDetect2::clear_buffers ()
{
	_prefilter.clear_buffers ();
	_dly_vol.clear_buffers ();
	_last_count = 0;
	_note_flag  = false;
	_env_pk.clear_buffers ();
	_deriv_smth.clear_buffers ();
	_dly_pk.clear_buffers ();
}



void	OnsetDetect2::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		_velo_clip_flag =
			(_state_set.get_val_tgt_nat (Param_VELO_CLIP) >= 0.5f);

		_hpf_freq  = float (_state_set.get_val_end_nat (Param_HP    ));
		_lpf1_freq = float (_state_set.get_val_end_nat (Param_LP1   ));
		_lpf2_freq = float (_state_set.get_val_end_nat (Param_LP2   ));
		_a         = fstb::round_int (_state_set.get_val_tgt_nat (Param_A));
		_b         = fstb::round_int (_state_set.get_val_tgt_nat (Param_B));
		_beta      = float (_state_set.get_val_end_nat (Param_BETA  )) * 1e-4f;
		_alpha     = float (_state_set.get_val_end_nat (Param_ALPHA ));
		_lambda    = float (_state_set.get_val_end_nat (Param_LAMBDA));
		update_os_stuff ();

		_rls_thr   = float (_state_set.get_val_tgt_nat (Param_RLS_THR  ));
		_rls_ratio = float (_state_set.get_val_tgt_nat (Param_RLS_RATIO));
	}
}



void	OnsetDetect2::update_os_stuff ()
{
	// High-pass filter
	{
		const float    bs [3] = { 0, 0, 4 };
		const float    as [3] = { 1, 3, 1 };
		float          bz [3];
		float          az [3];
		const float    k =
			mfx::dsp::iir::TransSZBilin::compute_k_approx (_hpf_freq * _inv_fs);
		mfx::dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
		_prefilter.set_z_eq (bz, az);
	}

	// RMS detector
	const float    t_env = 1.f / (float (2 * fstb::PI) * _lpf1_freq);
	_env_pk.set_times (t_env, t_env);

	// Derivative and smoothing
	{
		// Low-pass filter
		const float    lbs [2] = { 1, 0 };
		const float    las [2] = { 1, 1 };
		float          lbz [2];
		float          laz [2];
		const float    k =
			mfx::dsp::iir::TransSZBilin::compute_k_approx (_lpf2_freq * _inv_fs);
		mfx::dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (lbz, laz, lbs, las, k);

		// Convolution with derivative g * (1 - z^-1)
		// { g, -g } / { 1, 0 }
		const float    g      = _sample_freq * (1.f / _fs_ref);
		const float    bz [3] =
		{
			g *  lbz [0],
			g * (lbz [1] - lbz [0]),
			g *           -lbz [1]
		};
		const float    az [3] = { laz [0], laz [1], 0 };
		_deriv_smth.set_z_eq (bz, az);
	}

	// Scales a and b to the current sampling rate
	int            a = _a;
	int            b = _b;
	if (_sample_freq != float (_fs_ref))
	{
		const float    r = _sample_freq / float (_fs_ref);
		a = fstb::round_int (a * r);
		b = fstb::round_int (b * r);
	}

	_flt_sum.set_win_len (a + b);
	_sum_mul = 1.f / (a + b);

	_flt_med.set_len (a + b);
	_flt_med.set_rank ((a + b) >> 1);

	_dly_pk.set_delay (b);
}



}  // namespace osdet2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
