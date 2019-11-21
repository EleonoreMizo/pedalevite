/*****************************************************************************

        TestGen.cpp
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
#include "mfx/pi/testgen/Param.h"
#include "mfx/pi/testgen/TestGen.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace testgen
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TestGen::TestGen ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_chn_arr ()
,	_work_flag (false)
,	_multichn_flag (false)
,	_type (Type_N_PNK)
,	_level (0.1f)
,	_tone_phase (0)
,	_tone_phase_step (0)
,	_sweep_phase (0)
,	_sweep_phase_step (0)
,	_sweep_len (0)
,	_sweep_pos (0)
,	_sweep_fade_in (0)
,	_sweep_fade_out (0)
,	_pause_len (0)
,	_pause_pos (0)
,	_pause_flag (false)
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_STATE     , 0);
	_state_set.set_val_nat (desc_set, Param_TYPE      , Type_N_PNK);
	_state_set.set_val_nat (desc_set, Param_LVL       , 0.1);
	_state_set.set_val_nat (desc_set, Param_NZ_COR    , 0);
	_state_set.set_val_nat (desc_set, Param_TONE_FRQ  , ToneFreq_1K);
	_state_set.set_val_nat (desc_set, Param_SWEEP_DUR , SweepDur_30);
	_state_set.set_val_nat (desc_set, Param_LOOP_PAUSE, LoopPause_1);

	_state_set.add_observer (Param_STATE     , _param_change_flag);
	_state_set.add_observer (Param_TYPE      , _param_change_flag);
	_state_set.add_observer (Param_LVL       , _param_change_flag);
	_state_set.add_observer (Param_NZ_COR    , _param_change_flag);
	_state_set.add_observer (Param_TONE_FRQ  , _param_change_flag);
	_state_set.add_observer (Param_SWEEP_DUR , _param_change_flag);
	_state_set.add_observer (Param_LOOP_PAUSE, _param_change_flag);

	for (auto &chn : _chn_arr)
	{
		const int      seed = reinterpret_cast <int> (&chn);
		chn._nz_pink.set_seed (seed);
		chn._nz_white.set_seed (seed);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	TestGen::do_get_state () const
{
	return _state;
}



double	TestGen::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	TestGen::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag.set ();

	_sweep_fade_in  = fstb::round_int (sample_freq * 0.050);
	_sweep_fade_out = fstb::round_int (sample_freq * 0.001);

	update_param (true);
	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	TestGen::do_clean_quick ()
{
	clear_buffers ();
}



void	TestGen::do_process_block (piapi::ProcInfo &proc)
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

	const int      nbr_spl = proc._nbr_spl;

	// Parameters
	_state_set.process_block (nbr_spl);
	update_param (false);

	// Signal
	if (_work_flag)
	{
		switch (_type)
		{
		case Type_N_PNK:
		case Type_N_WHT:
			gen_noise (proc);
			break;
		case Type_TONE:
			gen_tone (proc);
			break;
		case Type_SWEEP:
			gen_sweep (proc);
			break;
		case Type_PULSE:
			gen_pulse (proc);
			break;
		default:
			assert (false);
			break;
		}
	}

	else
	{
		const int      nbr_chn = proc._nbr_chn_arr [piapi::Dir_OUT];
		for (int chn_index = 0; chn_index < nbr_chn; ++chn_index)
		{
			dsp::mix::Align::clear (proc._dst_arr [chn_index], proc._nbr_spl);
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestGen::clear_buffers ()
{
	restart_gen ();
}



void	TestGen::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const bool     work_new_flag =
			(_state_set.get_val_tgt_nat (Param_STATE) >= 0.5f);

		// State
		if (work_new_flag && ! _work_flag)
		{
			// Start
			restart_gen ();
		}
		_work_flag = work_new_flag;

		// Type
		const Type     type = Type (fstb::round_int (
			_state_set.get_val_tgt_nat (Param_TYPE)
		));
		if (_type != type && _work_flag)
		{
			restart_gen ();
		}
		_type = type;

		// Level
		_level = float (_state_set.get_val_tgt_nat (Param_LVL));

		// Correlation
		_multichn_flag = (_state_set.get_val_tgt_nat (Param_NZ_COR) >= 0.5f);

		// Tone frequency
		const int      freq_index = fstb::round_int (
			_state_set.get_val_tgt_nat (Param_TONE_FRQ)
		);
		const float    freq = _tone_freq_table [freq_index];
		_tone_phase_step = uint32_t (freq * _inv_fs * (1<<16) * (1<<16));

		// Sweep duration
		const int      dur_index = fstb::round_int (
			_state_set.get_val_tgt_nat (Param_SWEEP_DUR)
		);
		const float    dur  = _duration_table [dur_index];
		_sweep_len = fstb::round_int (dur * _sample_freq);
		_sweep_pos = std::min (_sweep_pos, _sweep_len - 1);

		// Loop/pause
		const int      pause_index = fstb::round_int (
			_state_set.get_val_tgt_nat (Param_LOOP_PAUSE)
		);
		const float    pause = _pause_table [pause_index];
		_pause_len = fstb::round_int (pause * _sample_freq);
		if (_pause_len > 0)
		{
			_pause_pos = std::min (_pause_pos, _pause_len - 1);
		}
	}
}



void	TestGen::restart_gen ()
{
	// Because we use a cosine function, we start at 3pi/4 (zero crossing)
	_tone_phase       = 0xC0000000U;
	_sweep_pos        = 0;
	_sweep_phase      = 0xC0000000U;
	_sweep_phase_step = 0;
	_pause_pos        = 0;
	_pause_flag       = false;
}



void	TestGen::gen_noise (piapi::ProcInfo &proc)
{
	const int      nbr_chn =
		  (_multichn_flag)
		? proc._nbr_chn_arr [piapi::Dir_OUT]
		: 1;

	for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_cnt];

		switch (_type)
		{
		case Type_N_WHT:
			chn._nz_white.process_block (proc._dst_arr [chn_cnt], proc._nbr_spl);
			break;
		case Type_N_PNK:
		default:
			chn._nz_pink.process_block (proc._dst_arr [chn_cnt], proc._nbr_spl);
			break;
		}

		dsp::mix::Align::scale_1_v (
			proc._dst_arr [chn_cnt], proc._nbr_spl, _level
		);
	}

	if (! _multichn_flag)
	{
		dup_mono_out (proc);
	}
}



void	TestGen::gen_tone (piapi::ProcInfo &proc)
{
	const int      nbr_spl = proc._nbr_spl;
	float *        dst_ptr = proc._dst_arr [0];
	int32_t        phase   = _tone_phase;
	int32_t        step    = _tone_phase_step;
	const float    level   = _level;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		float          x = compute_cos (phase);
		x *= level;
		dst_ptr [pos] = x;
		phase += step;
	}

	_tone_phase = phase;

	dup_mono_out (proc);
}



void	TestGen::gen_sweep (piapi::ProcInfo &proc)
{
	handle_pause (proc);

	if (! _pause_flag)
	{
		gen_sweep_running (proc);
		if (_sweep_pos >= _sweep_len)
		{
			_pause_flag = true;
			_pause_pos  = 0;
		}
	}
}



void	TestGen::gen_sweep_running (piapi::ProcInfo &proc)
{
	assert (_sweep_pos >= 0);
	assert (_sweep_pos < _sweep_len);

	const int      nbr_spl = proc._nbr_spl;
	float *        dst_ptr = proc._dst_arr [0];

	const int      rem_len  = _sweep_len - _sweep_pos;
	const int      work_len = std::min (nbr_spl, rem_len);

	if (_sweep_pos == 0 || _sweep_phase_step == 0)
	{
		_sweep_phase      = 0xC0000000U;
		_sweep_phase_step = compute_phase_step (_sweep_pos);
	}

	uint32_t       step    = _sweep_phase_step;
	const int      old_pos = _sweep_pos;
	_sweep_pos += work_len;
	_sweep_phase_step = compute_phase_step (_sweep_pos);

	const uint32_t step2 = (_sweep_phase_step - step) / work_len;

	uint32_t       phase = _sweep_phase;
	const float    level = _level;
	for (int pos = 0; pos < work_len; ++pos)
	{
		float          x = compute_cos (phase);
		x *= level;
		dst_ptr [pos] = x;
		phase += step;
		step  += step2;
	}
	_sweep_phase = phase;

	if (old_pos < _sweep_fade_in)
	{
		const float    mul = 1.0f / float (_sweep_fade_in);
		dsp::mix::Align::scale_1_vlr (
			dst_ptr,
			work_len,
			float (old_pos) * mul,
			std::min (float (_sweep_pos) * mul, 1.0f)
		);
	}
	else if (_sweep_pos >= _sweep_len - _sweep_fade_out)
	{
		const float    mul = 1.0f / float (_sweep_fade_out);
		dsp::mix::Align::scale_1_vlr (
			dst_ptr,
			work_len,
			std::min (float (_sweep_len - old_pos) * mul, 1.0f),
			float (_sweep_len - _sweep_pos) * mul
		);
	}

	if (work_len < nbr_spl)
	{
		dsp::mix::Align::clear (dst_ptr + work_len, nbr_spl - work_len);
	}

	dup_mono_out (proc);
}



void	TestGen::gen_pulse (piapi::ProcInfo &proc)
{
	handle_pause (proc);

	if (! _pause_flag)
	{
		dsp::mix::Align::clear (proc._dst_arr [0], proc._nbr_spl);
		proc._dst_arr [0] [0] = _level;

		dup_mono_out (proc);

		_pause_flag = true;
		_pause_pos  = 0;
	}
}



void	TestGen::handle_pause (piapi::ProcInfo &proc)
{
	if (_pause_flag)
	{
		if (_pause_len > 0)
		{
			_pause_pos += proc._nbr_spl;
			if (_pause_pos >= _pause_len)
			{
				_pause_pos        = 0;
				_pause_flag       = false;
				_sweep_pos        = 0;
				_sweep_phase      = 0xC0000000U;
				_sweep_phase_step = 0;
			}
		}
	}
	if (_pause_flag)
	{
		const int      nbr_chn = proc._nbr_chn_arr [piapi::Dir_OUT];
		for (int chn_index = 0; chn_index < nbr_chn; ++chn_index)
		{
			dsp::mix::Align::clear (proc._dst_arr [chn_index], proc._nbr_spl);
		}
	}
}



void	TestGen::dup_mono_out (piapi::ProcInfo &proc)
{
	const int      nbr_chn = proc._nbr_chn_arr [piapi::Dir_OUT];
	for (int chn_index = 1; chn_index < nbr_chn; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



uint32_t	TestGen::compute_phase_step (int pos)
{
	assert (_sweep_len > 0);
	assert (pos >= 0);
	assert (pos <= _sweep_len);

	const float    pos_flt = float (pos) / float (_sweep_len);

	// 20 Hz to 20 kHz
	const float    freq = float (20 * exp (pos_flt * (3 * fstb::LN10)));
	const uint32_t step = uint32_t (freq * _inv_fs * (1<<16) * (1<<16));

	return step;
}



float	TestGen::compute_cos (uint32_t phase)
{
	// The RBJ sine approximation gives about -150 dB of THD, enough for
	// this job.
	const float    mult      = (4.0f / ((1 << 16)) / (1 << 16));
	float          phase_flt = (phase & 0x7FFFFFFF) * mult;
	if ((phase & (1U << 31)) != 0)
	{
		phase_flt -= 1;
	}
	else
	{
		phase_flt = 1 - phase_flt;
	}
	const float    x = fstb::Approx::sin_rbj_halfpi (phase_flt);

	return x;
}



const std::array <float, ToneFreq_NBR_ELT>	TestGen::_tone_freq_table =
{{
	30, 40, 50, 100, 500, 1000, 3000, 10000, 15000
}};
const std::array <float, SweepDur_NBR_ELT>	TestGen::_duration_table =
{{
	6, 15, 30, 60, 180
}};
const std::array <float, LoopPause_NBR_ELT>	TestGen::_pause_table =
{{
	0, 1, 3, 10, 30
}};



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
