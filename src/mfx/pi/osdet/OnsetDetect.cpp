/*****************************************************************************

        OnsetDetect.cpp
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
#include "mfx/pi/osdet/Param.h"
#include "mfx/pi/osdet/OnsetDetect.h"
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
namespace osdet
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OnsetDetect::OnsetDetect ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_buf_env_vol ()
,	_buf_env_os ()
,	_buf_old_vol ()
,	_buf_old_os ()
,	_buf_tmp ()
,	_prefilter ()
,	_env_os ()
,	_dly_vol ()
,	_dly_os ()
,	_velo_clip_flag (true)
,	_atk_thr (2e-3f)
,	_atk_ratio (1.35f)
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
	_state_set.set_val_nat (desc_set, Param_ATK_THR  , 2e-3);
	_state_set.set_val_nat (desc_set, Param_ATK_RATIO, 1.35);
	_state_set.set_val_nat (desc_set, Param_RLS_THR  , 1e-3);
	_state_set.set_val_nat (desc_set, Param_RLS_RATIO, 1.414);

	_state_set.add_observer (Param_VELO_CLIP, _param_change_flag);
	_state_set.add_observer (Param_ATK_THR  , _param_change_flag);
	_state_set.add_observer (Param_ATK_RATIO, _param_change_flag);
	_state_set.add_observer (Param_RLS_THR  , _param_change_flag);
	_state_set.add_observer (Param_RLS_RATIO, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	OnsetDetect::do_get_state () const
{
	return _state;
}



double	OnsetDetect::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	OnsetDetect::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_env_vol.resize (mbs_align);
	_buf_env_os.resize (mbs_align);
	_buf_old_vol.resize (mbs_align);
	_buf_old_os.resize (mbs_align);
	_buf_tmp.resize (mbs_align);

#if 1
	const float    f      = 3500;
	const float    q      = 2;
	const float    r      = 2;
	const float    g      = 2.875f;
	const float    v      = 0.028f;
	const float    bs [3] = { g * v, g * (r + 1) / q, g };
	const float    as [3] = {     1,           1 / q, 1 };
#else
	// Old version
	const float    f      = 1000;
	const float    bs [3] = { 0, 0, 4 };
	const float    as [3] = { 1, 3, 1 };
#endif
	float          bz [3];
	float          az [3];
	mfx::dsp::iir::TransSZBilin::map_s_to_z (
		bz, az, bs, as, f, sample_freq
	);
	_prefilter.set_z_eq (bz, az);

	_env_os.set_times (0.003f, 0.030f);
	const float    dly_max_s   = 0.100f;
	const float    dly_vol_s   = 0.080f;
	const float    dly_os_s    = 0.015f;
	const int      dly_max_spl = fstb::round_int (sample_freq * dly_max_s);
	const int      dly_vol_spl = fstb::round_int (sample_freq * dly_vol_s);
	const int      dly_os_spl  = fstb::round_int (sample_freq * dly_os_s);
	_dly_vol.setup (dly_max_spl, max_buf_len);
	_dly_vol.set_delay (dly_vol_spl);
	_dly_os.setup (dly_max_spl, max_buf_len);
	_dly_os.set_delay (dly_os_spl);
	_last_delay = fstb::round_int (0.020f * sample_freq);

	_param_change_flag.set ();

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	OnsetDetect::do_clean_quick ()
{
	clear_buffers ();
}



void	OnsetDetect::do_process_block (ProcInfo &proc)
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

	// Emphasis high-mid frequencies and lowers the bass frequencies
	_prefilter.process_block (_buf_tmp.data (), proc._src_arr [0], nbr_spl);

	_env_os.process_block (
		_buf_env_os.data (), _buf_tmp.data (), nbr_spl
	);
	_dly_os.process_block (
		_buf_old_os.data (), _buf_env_os.data (), nbr_spl
	);

	float          ret_onset  = 0;
	float          ret_offset = 0;

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    vol2_cur   = _buf_env_vol [pos];
		const float    vol2_old   = _buf_old_vol [pos];
		const float    env_os_cur = _buf_env_os [pos];
		const float    env_os_old = _buf_old_os [pos];
		if (_last_count > 0)
		{
			-- _last_count;
		}
		else
		{
			if (   env_os_cur > env_os_old * _atk_ratio
			    && vol2_cur > _atk_thr * _atk_thr)
			{
				ret_onset   = float (sqrt (vol2_cur));
				_note_flag  = true;
				_last_count = _last_delay;
			}
		}

		if (_note_flag)
		{
			if (   vol2_cur * (_rls_ratio * _rls_ratio) < vol2_old
			    || vol2_cur < _rls_thr * _rls_thr)
			{
				ret_offset = 1;
				_note_flag = false;
			}
		}
	}

	if (_velo_clip_flag)
	{
		ret_onset = std::min (ret_onset, 1.0f);
	}

	proc._sig_arr [0] [0] = ret_onset;
	proc._sig_arr [1] [0] = ret_offset;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OnsetDetect::clear_buffers ()
{
	_prefilter.clear_buffers ();
	_dly_vol.clear_buffers ();
	_dly_os.clear_buffers ();
	_last_count = 0;
	_note_flag  = false;
}



void	OnsetDetect::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		_velo_clip_flag =
			(_state_set.get_val_tgt_nat (Param_VELO_CLIP) >= 0.5f);
		_atk_thr   = float (_state_set.get_val_tgt_nat (Param_ATK_THR  ));
		_atk_ratio = float (_state_set.get_val_tgt_nat (Param_ATK_RATIO));
		_rls_thr   = float (_state_set.get_val_tgt_nat (Param_RLS_THR  ));
		_rls_ratio = float (_state_set.get_val_tgt_nat (Param_RLS_RATIO));
	}
}



}  // namespace osdet
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
