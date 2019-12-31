/*****************************************************************************

        Delay.cpp
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
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dly0/Param.h"
#include "mfx/pi/dly0/Delay.h"
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
namespace dly0
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Delay::Delay ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_pin_arr ()
,	_nbr_pins (1)
,	_dly_spl (0)
,	_req_clear_flag (true)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Delay::do_get_state () const
{
	return _state;
}



double	Delay::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Delay::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &pin : _pin_arr)
	{
		for (auto &chn : pin)
		{
			chn._delay.setup (_max_dly_spl, max_buf_len);
		}
	}

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Delay::do_clean_quick ()
{
	_req_clear_flag = true;
}



void	Delay::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

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

	// Parameters (none actually)
	_state_set.process_block (proc._nbr_spl);

	if (_dly_spl > 0 && _req_clear_flag)
	{
		clear_dly_buf_quick ();
	}

	// Signal processing
	for (int pin_index = 0; pin_index < _nbr_pins; ++pin_index)
	{
		ChannelArray & chn_arr = _pin_arr [pin_index];
		const int   pin_ofs_dst = pin_index * nbr_chn_dst;
		const int   pin_ofs_src = pin_index * nbr_chn_src;

		for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
		{
			Channel &      chn = chn_arr [chn_index];

			chn._delay.process_block (
				proc._dst_arr [pin_ofs_dst + chn_index],
				proc._src_arr [pin_ofs_src + chn_index],
				proc._nbr_spl
			);
		}

		// Duplicates the remaining output channels
		for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
		{
			dsp::mix::Align::copy_1_1 (
				proc._dst_arr [pin_ofs_dst + chn_index],
				proc._dst_arr [pin_ofs_src            ],
				proc._nbr_spl
			);
		}
	}
}



void	Delay::do_set_aux_param (int dly_spl, int pin_mult)
{
	const int      dly_old = _dly_spl;

	_dly_spl  = dly_spl;
	_nbr_pins = pin_mult;

	for (int pin_cnt = 0; pin_cnt < _nbr_pins; ++pin_cnt)
	{
		for (auto &chn : _pin_arr [pin_cnt])
		{
			chn._delay.set_delay (dly_spl);
		}
	}

	// Delay time has changed: we most likely switched to another effect, or
	// changed the settings in a way we cannot ensure signal continuity.
	// Therefore the best thing to do is to clean the delay buffers.
	if (dly_old != _dly_spl)
	{
		_req_clear_flag = true;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Delay::clear_buffers ()
{
	for (auto &pin : _pin_arr)
	{
		for (auto &chn : pin)
		{
			chn._delay.clear_buffers ();
		}
	}
}



void	Delay::clear_dly_buf_quick ()
{
	for (auto &pin : _pin_arr)
	{
		for (auto &chn : pin)
		{
			chn._delay.clear_buffers_quick ();
		}
	}
	_req_clear_flag = false;
}



}  // namespace dly0
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
