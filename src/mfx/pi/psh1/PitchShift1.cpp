/*****************************************************************************

        PitchShift1.cpp
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
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/psh1/Cst.h"
#include "mfx/pi/psh1/Param.h"
#include "mfx/pi/psh1/PitchShift1.h"
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
namespace psh1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PitchShift1::PitchShift1 ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_min_dly_time (0)
,	_param_change_flag ()
,	_chn_arr ()
,  _interp ()
,  _xfade_shape ()
,  _tmp_buf ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_PITCH  , 0);
	_state_set.set_val_nat (desc_set, Param_WIN_DUR, 0.100);

	_state_set.add_observer (Param_PITCH  , _param_change_flag);
	_state_set.add_observer (Param_WIN_DUR, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	PitchShift1::do_get_state () const
{
	return _state;
}



double	PitchShift1::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	PitchShift1::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	const int      mbl_align = (max_buf_len + 3) & ~3;
	_tmp_buf.resize (mbl_align);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();
	_xfade_shape.set_sample_freq (sample_freq);

	// When the delay is set to the maximum, we need room to push first
	// the new data, then read the delayed data.
	const double   add_dly = max_buf_len / sample_freq;
	const double   max_dly = Cst::_max_win_size / 1000.0 + add_dly;
	for (auto &chn : _chn_arr)
	{
		chn._reader.set_tmp_buf (&_tmp_buf [0], int (_tmp_buf.size ()));
		chn._reader.set_delay_line (chn._delay);
		chn._reader.set_resampling_range (-2.0f, 2.0f);
		chn._reader.set_crossfade_normal (
			_xfade_shape.get_len (),
			_xfade_shape.use_shape ()
		);
		chn._reader.set_crossfade_pitchshift (
			_xfade_shape.get_len (),
			_xfade_shape.use_shape ()
		);
		assert (chn._reader.is_ready ());
		chn._delay.set_interpolator (_interp);
		chn._delay.set_sample_freq (sample_freq, 0);
		chn._delay.set_max_delay_time (max_dly);
	}

	// We add a microsecond because of possible rounding errors
	_min_dly_time = float (_chn_arr [0]._delay.get_min_delay_time () + 1e-6);

	_param_change_flag.set ();

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	PitchShift1::do_clean_quick ()
{
	clear_buffers ();
}



void	PitchShift1::do_process_block (piapi::ProcInfo &proc)
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
	update_param ();

	// Signal processing
	const int      nbr_spl = proc._nbr_spl;
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		// Inserts incoming data into the delay lines
		Channel &      chn = _chn_arr [chn_index];
		chn._delay.push_block (proc._src_arr [chn_index], nbr_spl);

		// Reads the line
		chn._reader.read_data (proc._dst_arr [chn_index], nbr_spl, -nbr_spl);
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



void	PitchShift1::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._reader.clear_buffers ();
	}
}



void	PitchShift1::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const float    pitch = float (_state_set.get_val_end_nat (Param_PITCH));
		const float    win   = float (_state_set.get_val_tgt_nat (Param_WIN_DUR));

		_xfade_shape.set_duration (win);
		const float    rate  = fstb::Approx::exp2 (pitch);
		const float    dly_s = std::max (2 * win * (rate - 1), _min_dly_time);
		for (auto &chn : _chn_arr)
		{
			chn._reader.set_delay_time (dly_s, 0);
			chn._reader.set_grain_pitch (rate);
			chn._reader.set_crossfade_normal (
				_xfade_shape.get_len (),
				_xfade_shape.use_shape ()
			);
			chn._reader.set_crossfade_pitchshift (
				_xfade_shape.get_len (),
				_xfade_shape.use_shape ()
			);
		}
	}
}



}  // namespace psh1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
