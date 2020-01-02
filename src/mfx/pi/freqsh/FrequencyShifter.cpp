/*****************************************************************************

        FrequencyShifter.cpp
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
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/freqsh/FrequencyShifter.h"
#include "mfx/pi/freqsh/Param.h"
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
namespace freqsh
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FrequencyShifter::FrequencyShifter ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_param_change_flag ()
,	_freq_shift ()
{
	dsp::mix::Align::setup ();

	_state_set.init (piapi::ParamCateg_GLOBAL, _desc.use_desc_set ());

	_state_set.set_val (Param_FREQ, 0.5f);

	_state_set.add_observer (Param_FREQ, _param_change_flag);

	_state_set.set_ramp_time (Param_FREQ, 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	FrequencyShifter::do_get_state () const
{
	return _state;
}



double	FrequencyShifter::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	FrequencyShifter::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	double         latency_f = 0;
	_freq_shift.reset (sample_freq, max_buf_len, latency_f);
	latency = fstb::round_int (latency_f);

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	FrequencyShifter::do_process_block (piapi::ProcInfo &proc)
{
	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);

	if (_param_change_flag (true))
	{
		const float    freq = float (_state_set.get_val_end_nat (Param_FREQ));
		_freq_shift.set_freq (freq);
	}

	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// Signal processing
	const int      nbr_chn_i = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_o = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	const int      nbr_chn_p = std::min (nbr_chn_i, nbr_chn_o);
	_freq_shift.process_block (
		proc._dst_arr,
		proc._src_arr,
		proc._nbr_spl,
		nbr_chn_p
	);

	for (int c = nbr_chn_p; c < nbr_chn_o; ++c)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [c],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FrequencyShifter::clear_buffers ()
{
	_freq_shift.clear_buffers ();
}



}  // namespace freqsh
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
