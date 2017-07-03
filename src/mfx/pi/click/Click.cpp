/*****************************************************************************

        Click.cpp
        Author: Laurent de Soras, 2017

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
#include "mfx/dsp/mix/Generic.h"
#include "mfx/pi/click/Click.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTransport.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace click
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Click::Click ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_pos_beat (0)
,	_tempo (120)
,	_beat_per_spl (0)
,	_beat_per_bar (4)
,	_vol (0.5f)
,	_gain_bar (1.25f)
,	_cur_beat (-1)
,	_osc_pos (0)
,	_osc_inc (0)
,	_gain_cur (_vol)
,	_rnd_gen ()
,	_rnd_pos (0)
,	_rnd_inc (0)
,	_rnd_val (0)
,	_rnd_mix (0.5f)
,	_chn_mask (~uint64_t (0))
,	_env ()
,	_buf_env ()
{
	dsp::mix::Align::setup ();
	dsp::mix::Generic::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Click::do_get_state () const
{
	return _state;
}



double	Click::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Click::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_env.set_sample_freq (_sample_freq);
	_env.set_atk_time (0.0005f);
	_env.set_atk_virt_lvl (1.5f);
	_env.set_dcy_time (0.010f);
	_env.set_sus_time (0.040f);
	_env.set_sus_lvl (1.f);
	_env.set_rls_time (0.010f);

	const int      msb_align = (max_buf_len + 3) & -4;
	_buf_env.resize (msb_align);

	update_rates ();
	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	Click::do_clean_quick ()
{
	clear_buffers ();
}



void	Click::do_process_block (ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._nbr_chn_arr [Dir_IN ];
	const int      nbr_chn_dst = proc._nbr_chn_arr [Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);

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
		else if (evt._type == piapi::EventType_TRANSPORT)
		{
			const piapi::EventTransport & evtt = evt._evt._transport;
			if ((evtt._flags & piapi::EventTransport::Flag_TEMPO) != 0)
			{
				_tempo = evtt._tempo;
				update_rates ();
			}
		}
	}

	const int      nbr_spl   = proc._nbr_spl;
	int            pos_block = 0;
	do
	{
		int            work_len    = nbr_spl - pos_block;
		int            len_nxt_evt =
			fstb::ceil_int ((_cur_beat + 1 - _pos_beat) / _beat_per_spl);
		work_len = std::min (work_len, len_nxt_evt);

		if (work_len > 0)
		{
			// Generates a tone
			_env.process_block (&_buf_env [0], work_len);
			for (int pos = 0; pos < work_len; ++pos)
			{
				float          val = (_osc_pos < 0.5f) ? -1.f : 1.f;
				val += _rnd_val * _rnd_mix;
				_buf_env [pos] *= val * _gain_cur;
				_osc_pos += _osc_inc;
				_rnd_pos += _rnd_inc;
				if (_osc_pos >= 1)
				{
					_osc_pos -= 1;
				}
				if (_rnd_pos >= 1)
				{
					_rnd_val = ((_rnd_gen () >> 16) & 2) - 1.f;
					_rnd_pos -= 1;
				}
			}

			for (int chn = 0; chn < nbr_chn_src; ++chn)
			{
				dsp::mix::Generic::copy_2_1 (
					&proc._dst_arr [chn] [pos_block],
					&proc._src_arr [chn] [pos_block],
					&_buf_env [0],
					work_len
				);
			}

			pos_block += work_len;
			_pos_beat += work_len * _beat_per_spl;
		}

		if (work_len >= len_nxt_evt)
		{
			// New event
			_env.note_on ();
			_gain_cur = _vol;
			if (_cur_beat == 0)
			{
				_gain_cur *= _gain_bar;
			}

			++ _cur_beat;
			if (_cur_beat >= _beat_per_bar)
			{
				_cur_beat -= _beat_per_bar;
				_pos_beat -= _beat_per_bar;
			}

			float          freq = 500;
			float          frq2 = 16000;
			if (_cur_beat == 0)
			{
				freq = 1000;
				frq2 = 1500;
			}
			_osc_inc = freq * _inv_fs;
			_rnd_inc = frq2 * _inv_fs;
		}
	}
	while (pos_block < nbr_spl);

	// Duplicates the remaining output channels
	for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Click::clear_buffers ()
{
	_env.clear_buffers ();
	_pos_beat = 0;
	_cur_beat = -1;
	_osc_pos  = 0;
}



void	Click::update_rates ()
{
	_beat_per_spl = _tempo / (60 * _sample_freq);
}



}  // namespace click
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
