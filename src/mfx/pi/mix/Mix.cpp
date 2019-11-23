/*****************************************************************************

        Mix.cpp
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
#include "mfx/pi/mix/Param.h"
#include "mfx/pi/mix/Mix.h"
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
namespace mix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Mix::Mix ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Mix::do_get_state () const
{
	return _state;
}



double	Mix::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Mix::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Mix::do_clean_quick ()
{
	// Nothing
}



void	Mix::do_process_block (piapi::ProcInfo &proc)
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

	// Parameters
	_state_set.process_block (proc._nbr_spl);

	// Signal processing

	// We assume here that the number of pins has been explicitely set
	// (we don't use the negative value meaning all the pins are connected).
	const int      nbr_pins = proc._dir_arr [piapi::Dir_IN ]._nbr_pins;
	if (nbr_chn_src == 2 && nbr_chn_dst == 2)
	{
		dsp::mix::Align::copy_2_2 (
			proc._dst_arr [0],
			proc._dst_arr [1],
			proc._src_arr [0],
			proc._src_arr [1],
			proc._nbr_spl
		);
		for (int pin_cnt = 1; pin_cnt < nbr_pins; ++pin_cnt)
		{
			dsp::mix::Align::mix_2_2 (
				proc._dst_arr [0],
				proc._dst_arr [1],
				proc._src_arr [pin_cnt * 2    ],
				proc._src_arr [pin_cnt * 2 + 1],
				proc._nbr_spl
			);
		}
	}
	else
	{
		for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
		{
			dsp::mix::Align::copy_1_1 (
				proc._dst_arr [chn_index],
				proc._src_arr [chn_index],
				proc._nbr_spl
			);
			for (int pin_cnt = 1; pin_cnt < nbr_pins; ++pin_cnt)
			{
				dsp::mix::Align::mix_1_1 (
					proc._dst_arr [chn_index],
					proc._src_arr [chn_index + pin_cnt * nbr_chn_src],
					proc._nbr_spl
				);
			}
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
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mix
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
