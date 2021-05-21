/*****************************************************************************

        Synth0.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/syn0/Param.h"
#include "mfx/pi/syn0/Synth0.h"
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
namespace syn0
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Synth0::Synth0 (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_pitch_oct (0)
,	_velo (1)
,	_env_amp ()
,	_osc_stp (0.002f)
,	_osc_pos (0)
,	_osc_stp_inv (1.f / _osc_stp)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_PITCH   , 0);
	_state_set.set_val_nat (desc_set, Param_TRIG_ON , 0);
	_state_set.set_val_nat (desc_set, Param_TRIG_OFF, 0);

	_state_set.add_observer (Param_PITCH   , _param_change_flag);
	_state_set.add_observer (Param_TRIG_ON , _param_change_flag);
	_state_set.add_observer (Param_TRIG_OFF, _param_change_flag);

	_env_amp.set_atk_time (0.001f);
	_env_amp.set_dcy_time (0.250f);
	_env_amp.set_sus_lvl (0.333f);
	_env_amp.set_rls_time (0.100f);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Synth0::do_get_state () const
{
	return _state;
}



double	Synth0::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Synth0::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_env_amp.set_sample_freq (float (sample_freq));

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_syn.resize (mbs_align);
	_buf_env_amp.resize (mbs_align);

	_param_change_flag.set ();

	update_param (true);

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Synth0::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Signal processing

	int            nbr_spl = proc._nbr_spl;

	// Waveform
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		constexpr float   duty = 0.5f; // Dutycycle

		// Dirty aliased square wave. I hope you like aliasing.
		float          x = (_osc_pos < duty) ? -1.f : 1.f;

		// Now, tries to fix naive pulsewave synthesis with PolyBLEPs
		// Rising edge
		float          pos_rise = _osc_pos - duty;
		if (pos_rise < 0)
		{
			pos_rise += 1;
		}
		auto           add = [] (float a, float b) { return a + b; };
		fix_edge_polyblep (x, pos_rise, add);

		// Falling edge
		auto           sub = [] (float a, float b) { return a - b; };
		fix_edge_polyblep (x, _osc_pos, sub);

		// Velocity
		x *= _velo;

		_buf_syn [pos] = x;

		_osc_pos += _osc_stp;
		while (_osc_pos >= 1)
		{
			_osc_pos -= 1;
		}
	}

	// Volume envelope
	_env_amp.process_block (&_buf_env_amp [0], nbr_spl);
	dsp::mix::Align::mult_1_1 (
		&_buf_syn [0],
		&_buf_syn [0],
		&_buf_env_amp [0],
		nbr_spl
	);

	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		dsp::mix::Align::copy_2_1 (
			proc._dst_arr [chn_index],
			proc._src_arr [chn_index],
			&_buf_syn [0],
			nbr_spl
		);
	}

	// Duplicates the remaining output channels
	for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Synth0::clear_buffers ()
{
	_env_amp.clear_buffers ();
	_osc_pos = 0;

	/*** To do ***/

}



void	Synth0::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		constexpr float   c2a   = (12 - 9) / 12.f; // From C to A, octaves
		constexpr float   f_a   = 220;             // A below Middle C, Hz

		_pitch_oct   = float (_state_set.get_val_tgt_nat (Param_PITCH));
		const float    freq = f_a * fstb::Approx::exp2_5th (_pitch_oct + c2a);
		_osc_stp     = std::min (freq * _inv_fs, 0.5f); // Max freq: Nyquist
		_osc_stp_inv = 1.f / _osc_stp;

		const float    velo = float (_state_set.get_val_tgt_nat (Param_TRIG_ON));
		const bool     note_on_flag = (velo > 0);
		const bool     note_off_flag =
			(_state_set.get_val_tgt_nat (Param_TRIG_OFF) > 0);

		if (note_on_flag)
		{
			_env_amp.note_on ();
		}

		if (note_off_flag)
		{
			_env_amp.note_off ();
		}
	}
}



// OP is addition or subtraction
template <typename OP>
void	Synth0::fix_edge_polyblep (float &x, float &pos, OP op)
{
	if (pos < _osc_stp)
	{
		const float    t = pos * _osc_stp_inv;
		x = op (x, t+t - t*t - 1);
	}
	else if (pos > 1 - _osc_stp)
	{
		const float    t = (pos - 1) * _osc_stp_inv;
		x = op (x, t+t + t*t + 1);
	}
}



}  // namespace syn0
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
