/*****************************************************************************

        ToMono.cpp
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
#include "mfx/pi/tomo/Param.h"
#include "mfx/pi/tomo/Source.h"
#include "mfx/pi/tomo/ToMono.h"
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
namespace tomo
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ToMono::ToMono (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_chn_arr ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SRC, Source_MIX6);

	_state_set.add_observer (Param_SRC, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	ToMono::do_get_state () const
{
	return _state;
}



double	ToMono::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	ToMono::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	constexpr float   ramp_time_s   = 0.010f;
	const int         ramp_time_spl =
		fstb::round_int (sample_freq * ramp_time_s);
	const float       ramp_step     = 1.f / float (ramp_time_spl);
	bool              first_flag    = true;
	for (auto &chn : _chn_arr)
	{
		chn._vol.set_time (ramp_time_spl, ramp_step);
		chn._vol.set_val ((first_flag) ? 1.f : 0.f);
		first_flag = false;
	}

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	ToMono::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_dst == 1);
	fstb::unused (nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	for (auto &chn : _chn_arr)
	{
		chn._vol.tick (proc._nbr_spl);
	}

	// Signal processing
	if (nbr_chn_src == 1)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [0], proc._src_arr [0], proc._nbr_spl
		);
	}
	else
	{
		bool        w_flag = false;
		for (int chn_index = 0; chn_index < nbr_chn_src; ++chn_index)
		{
			Channel &      chn     = _chn_arr [chn_index];
			const float    vol_beg = chn._vol.get_beg ();
			const float    vol_end = chn._vol.get_end ();
			if (! (fstb::is_null (vol_beg) && fstb::is_null (vol_end)))
			{
				if (w_flag)
				{
					dsp::mix::Align::mix_1_1_vlrauto (
						proc._dst_arr [0], proc._src_arr [0], proc._nbr_spl,
						vol_beg, vol_end
					);
				}
				else
				{
					dsp::mix::Align::copy_1_1_vlrauto (
						proc._dst_arr [0], proc._src_arr [0], proc._nbr_spl,
						vol_beg, vol_end
					);
					w_flag = true;
				}
			}
		}

		if (! w_flag)
		{
			// This shouldn't happen
			assert (false);
			dsp::mix::Align::clear (proc._dst_arr [0], proc._nbr_spl);
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ToMono::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._vol.clear_buffers ();
	}
}



void	ToMono::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const Source   src = _state_set.get_val_enum <Source> (Param_SRC);
		switch (src)
		{
		case Source_MIX6:
			_chn_arr [0]._vol.set_val (0.5f);
			_chn_arr [1]._vol.set_val (0.5f);
			break;
		case Source_MIX3:
			_chn_arr [0]._vol.set_val (float (fstb::SQRT2 * 0.5));
			_chn_arr [1]._vol.set_val (float (fstb::SQRT2 * 0.5));
			break;
		case Source_L:
			_chn_arr [0]._vol.set_val (1.f);
			_chn_arr [1]._vol.set_val (0.f);
			break;
		case Source_R:
			_chn_arr [0]._vol.set_val (0.f);
			_chn_arr [1]._vol.set_val (1.f);
			break;
		default:
			assert (false);
			break;
		}
	}
}



}  // namespace tomo
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
