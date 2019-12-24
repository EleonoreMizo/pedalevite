/*****************************************************************************

        EnvAdsr.cpp
        Author: Laurent de Soras, 2019

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
#include "mfx/pi/adsr/EnvAdsr.h"
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
namespace adsr
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EnvAdsr::EnvAdsr ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_trig ()
,	_param_change_flag_adsr ()
,	_env ()
,	_velo (1)
,	_velo_sens (0)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_TRIG_ON  ,   0);
	_state_set.set_val_nat (desc_set, Param_TRIG_OFF ,   0);
	_state_set.set_val_nat (desc_set, Param_VELO_SENS,   0);
	_state_set.set_val_nat (desc_set, Param_ATK_T    ,   0.01);
	_state_set.set_val_nat (desc_set, Param_DCY_T    ,   0.1);
	_state_set.set_val_nat (desc_set, Param_SUS_T    , 256);
	_state_set.set_val_nat (desc_set, Param_SUS_L    ,   0.5);
	_state_set.set_val_nat (desc_set, Param_RLS_T    ,   0.25);

	_state_set.add_observer (Param_TRIG_ON  , _param_change_flag_trig);
	_state_set.add_observer (Param_TRIG_OFF , _param_change_flag_trig);
	_state_set.add_observer (Param_VELO_SENS, _param_change_flag_adsr);
	_state_set.add_observer (Param_ATK_T    , _param_change_flag_adsr);
	_state_set.add_observer (Param_DCY_T    , _param_change_flag_adsr);
	_state_set.add_observer (Param_SUS_T    , _param_change_flag_adsr);
	_state_set.add_observer (Param_SUS_L    , _param_change_flag_adsr);
	_state_set.add_observer (Param_RLS_T    , _param_change_flag_adsr);

	_param_change_flag_trig.add_observer (_param_change_flag);
	_param_change_flag_adsr.add_observer (_param_change_flag);

	_env.set_atk_virt_lvl (1.5f);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	EnvAdsr::do_get_state () const
{
	return _state;
}



double	EnvAdsr::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	EnvAdsr::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_env.set_sample_freq (_sample_freq);

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	EnvAdsr::do_clean_quick ()
{
	clear_buffers ();
}



void	EnvAdsr::do_process_block (piapi::ProcInfo &proc)
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

	// Signal processing
	const float    amp     = 1 + (_velo - 1) * _velo_sens;
	const float    val_env = _env.get_val ();
	proc._sig_arr [0] [0]  = val_env * amp;

	_env.skip_block (proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EnvAdsr::clear_buffers ()
{
	_env.clear_buffers ();
}



void	EnvAdsr::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_adsr (true) || force_flag)
		{
			_velo_sens = float (_state_set.get_val_end_nat (Param_VELO_SENS));
			const float    at =
				float (_state_set.get_val_end_nat (Param_ATK_T));
			const float    dt =
				float (_state_set.get_val_end_nat (Param_DCY_T));
			float          st =
				float (_state_set.get_val_end_nat (Param_SUS_T));
			const float    sl =
				float (_state_set.get_val_end_nat (Param_SUS_L));
			const float    rt =
				float (_state_set.get_val_end_nat (Param_RLS_T));
			if (st >= float (_sus_time_inf))
			{
				st = dsp::ctrl::env::AdsrRc::_infinite_sus;
			}

			_env.set_atk_time (at);
			_env.set_dcy_time (dt);
			_env.set_sus_time (st);
			_env.set_sus_lvl (sl);
			_env.set_rls_time (rt);
		}

		if (_param_change_flag_trig (true) || force_flag)
		{
			_velo = float (_state_set.get_val_tgt_nat (Param_TRIG_ON));
			const bool     note_on_flag = (_velo > 0);
			const bool     note_off_flag =
				(_state_set.get_val_tgt_nat (Param_TRIG_OFF) > 0);

			if (note_on_flag)
			{
				_env.note_on ();
			}

			if (note_off_flag)
			{
				_env.note_off ();
			}
		}
	}
}



}  // namespace adsr
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
