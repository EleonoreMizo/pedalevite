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
,	_interp ()
,	_last_count (0)
,	_last_delay (1000)
,	_note_flag (false)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_VELO_CLIP    ,  0);
	_state_set.set_val_nat (desc_set, Param_REL_THR      ,  1e-2);

	_state_set.add_observer (Param_VELO_CLIP    , _param_change_flag);
	_state_set.add_observer (Param_REL_THR      , _param_change_flag);

	_dly_vol.set_interpolator (_interp);
	_dly_os.set_interpolator (_interp);
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

	const float    bs [3] = { 0, 0, 4 };
	const float    as [3] = { 1, 3, 1 };
	float          bz [3];
	float          az [3];
	mfx::dsp::iir::TransSZBilin::map_s_to_z (
		bz, az, bs, as, 1000, sample_freq
	);
	_prefilter.set_z_eq (bz, az);

	_env_os.set_times (0.003f, 0.030f);
	_dly_vol.set_max_delay_time (float (0.100 + max_buf_len / sample_freq));
	_dly_vol.set_sample_freq (sample_freq, 1);
	_dly_os.set_max_delay_time (float (0.100 + max_buf_len / sample_freq));
	_dly_os.set_sample_freq (sample_freq, 1);
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
		&_buf_env_vol [0], proc._src_arr [0], nbr_spl
	);
	_dly_vol.push_block (&_buf_env_vol [0], nbr_spl);
	_dly_vol.read_block (&_buf_old_vol [0], nbr_spl, 0.080f, 0.080f, -nbr_spl);

	// Emphasis high-mid frequencies and lowers the bass frequencies
	_prefilter.process_block (&_buf_tmp [0], proc._src_arr [0], nbr_spl);

	_env_os.process_block (
		&_buf_env_os [0], &_buf_tmp [0], nbr_spl
	);
	_dly_os.push_block (&_buf_env_os [0], nbr_spl);
	_dly_os.read_block (&_buf_old_os [0], nbr_spl, 0.015f, 0.015f, -nbr_spl);

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
			_last_count -= nbr_spl;
		}
		else
		{
			if (env_os_cur > env_os_old * 1.5f && vol2_cur > 1e-2f * 1e-2f)
			{
				ret_onset   = float (sqrt (vol2_cur));
				_note_flag  = true;
				_last_count = _last_delay - nbr_spl;
			}
		}

		if (_note_flag)
		{
			const float    thr = 1e-3f;
			if (vol2_cur * 2 < vol2_old || vol2_cur < thr * thr)
			{
				ret_offset = 1;
				_note_flag = false;
			}
		}
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
		/*** To do ***/
	}
}



}  // namespace osdet
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
