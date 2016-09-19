/*****************************************************************************

        Flancho.cpp
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

#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/flancho/Cst.h"
#include "mfx/pi/flancho/Flancho.h"
#include "mfx/pi/flancho/Param.h"
#include "mfx/pi/flancho/WfType.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace flancho
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Flancho::Flancho ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_flag_depth_fdbk ()
,	_param_change_flag_wf ()
,	_param_change_flag_speed ()
,	_param_change_flag_delay ()
,	_param_change_flag_voices ()
,	_param_change_flag_phase_set ()
,	_param_change_flag_dry ()
,	_chn_arr ()
,	_interp ()
,	_buf_tmp (1024)
,	_buf_render (1024)
,	_nbr_chn_in (0)
,	_nbr_chn_out (0)
,	_dry_flag (false)
,	_neg_flag (false)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SPEED     , 0.5);
	_state_set.set_val_nat (desc_set, Param_DEPTH     , 0.5);
	_state_set.set_val_nat (desc_set, Param_DELAY     , 0.004);
	_state_set.set_val_nat (desc_set, Param_FDBK      , 0);
	_state_set.set_val_nat (desc_set, Param_WF_TYPE   , double (WfType_SINE));
	_state_set.set_val_nat (desc_set, Param_WF_SHAPE  , 0);
	_state_set.set_val_nat (desc_set, Param_NBR_VOICES, 1);
	_state_set.set_val_nat (desc_set, Param_DRY       , 1);
	_state_set.set_val_nat (desc_set, Param_NEGATIVE  , 0);
	_state_set.set_val_nat (desc_set, Param_PHASE_SET , 0);

	_state_set.add_observer (Param_SPEED     , _param_change_flag_speed);
	_state_set.add_observer (Param_DEPTH     , _param_change_flag_depth_fdbk);
	_state_set.add_observer (Param_DELAY     , _param_change_flag_delay);
	_state_set.add_observer (Param_FDBK      , _param_change_flag_depth_fdbk);
	_state_set.add_observer (Param_WF_TYPE   , _param_change_flag_wf);
	_state_set.add_observer (Param_WF_SHAPE  , _param_change_flag_wf);
	_state_set.add_observer (Param_NBR_VOICES, _param_change_flag_voices);
	_state_set.add_observer (Param_DRY       , _param_change_flag_dry);
	_state_set.add_observer (Param_NEGATIVE  , _param_change_flag_dry);
	_state_set.add_observer (Param_PHASE_SET , _param_change_flag_phase_set);

	_param_change_flag_depth_fdbk.add_observer (_param_change_flag);
	_param_change_flag_wf        .add_observer (_param_change_flag);
	_param_change_flag_speed     .add_observer (_param_change_flag);
	_param_change_flag_delay     .add_observer (_param_change_flag);
	_param_change_flag_voices    .add_observer (_param_change_flag);
	_param_change_flag_phase_set .add_observer (_param_change_flag);
	_param_change_flag_dry       .add_observer (_param_change_flag);

//	_state_set.set_ramp_time (Param_, 0.010);

	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		_chn_arr [chn_cnt] = ChnSPtr (new FlanchoChn (
			_interp,
			&_buf_tmp [0],
			long (_buf_tmp.size ()),
			&_buf_render [0],
			long (_buf_render.size ())
		));
		_chn_arr [chn_cnt]->set_rel_phase (0);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Flancho::do_get_state () const
{
	return _state;
}



double	Flancho::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Flancho::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_depth_fdbk.set ();
	_param_change_flag_wf.set ();
	_state_set.use_state (Param_SPEED     ).use_notif_flag ().set ();
	_state_set.use_state (Param_DELAY     ).use_notif_flag ().set ();
	_state_set.use_state (Param_NBR_VOICES).use_notif_flag ().set ();
	_state_set.use_state (Param_PHASE_SET ).use_notif_flag ().set ();

	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
		chn.set_sample_freq (_sample_freq);
	}

	_nbr_chn_in  = 0;
	_nbr_chn_out = 0;

	update_param (true);

	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
		chn.clear_buffers ();
	}

	_state = State_ACTIVE;

	return Err_OK;
}



void	Flancho::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_in =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_out =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	if (nbr_chn_out != _nbr_chn_out)
	{
		double         phase_mult = 0;
		if (nbr_chn_out > 1)
		{
			phase_mult = 0.25 / (nbr_chn_out - 1);
		}

		for (int chn_cnt = 0; chn_cnt < nbr_chn_out; ++chn_cnt)
		{
			_chn_arr [chn_cnt]->set_rel_phase (chn_cnt * phase_mult);
		}
	}

	_nbr_chn_in  = nbr_chn_in;
	_nbr_chn_out = nbr_chn_out;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
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

	int            pos = 0;
	do
	{
		// We need this intermediate varaible because for some reason GCC
		// fails to link when _update_resol is directly used in std::min.
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		_state_set.process_block (work_len);
		update_param ();

		// Signal processing
		int            chn_in     = 0;
		int            chn_in_inc = (_nbr_chn_in > _nbr_chn_out) ? 1 : 0;
		for (int chn_cnt = 0; chn_cnt < _nbr_chn_out; ++chn_cnt)
		{
			FlanchoChn &   chn = *(_chn_arr [chn_cnt]);
			chn.process_block (
				proc._dst_arr [chn_cnt] + pos,
				proc._src_arr [chn_in ] + pos,
				work_len
			);
			if (_dry_flag)
			{
				dsp::mix::Align::mix_1_1 (
					proc._dst_arr [chn_cnt] + pos,
					proc._src_arr [chn_in ] + pos,
					work_len
				);
			}
			chn_in += chn_in_inc;
		}

		pos += work_len;
	}
	while (pos < proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Flancho::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_speed (true) || force_flag)
		{
			update_lfo_period ();
		}
		if (_param_change_flag_delay (true) || force_flag)
		{
			const double   delay = _state_set.get_val_end_nat (Param_DELAY);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
				chn.set_delay (delay);
			}
		}
		if (_param_change_flag_depth_fdbk (true) || force_flag)
		{
			const double   depth = _state_set.get_val_end_nat (Param_DEPTH);
			const double   fdbk  = _state_set.get_val_end_nat (Param_FDBK);
			const double   dmul  = 0.5 / _max_nbr_chn;
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
				const double      chn_depth = depth * (1 - chn_cnt * dmul);
				chn.set_depth (chn_depth);
				chn.set_feedback (fdbk);
			}
		}
		if (_param_change_flag_wf (true) || force_flag)
		{
			const WfType   type  = static_cast <WfType> (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_WF_TYPE)
			));
			const double   shape = _state_set.get_val_end_nat (Param_WF_SHAPE);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
				chn.set_wf_type (type);
				chn.set_wf_shape (shape);
			}
		}
		if (_param_change_flag_voices (true) || force_flag)
		{
			const int      nbr_voices =
				fstb::round_int (_state_set.get_val_tgt_nat (Param_NBR_VOICES));
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
				chn.set_nbr_voices (nbr_voices);
			}
		}
		if (_param_change_flag_phase_set (true) || force_flag)
		{
			double         phase = _state_set.get_val_tgt_nat (Param_PHASE_SET);
			phase = std::min (phase, 0.999);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
				chn.resync (phase);
			}
		}
		if (_param_change_flag_dry (true) || force_flag)
		{
			_dry_flag = (_state_set.get_val_tgt_nat (Param_DRY) >= 0.5f);
			_neg_flag = (_state_set.get_val_tgt_nat (Param_NEGATIVE) >= 0.5f);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
				chn.set_polarity (_neg_flag);
			}
		}
	}
}



void	Flancho::update_lfo_period ()
{
	const double   freq = _state_set.get_val_end_nat (Param_SPEED);
	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		FlanchoChn &		chn = *(_chn_arr [chn_cnt]);
		chn.set_speed (freq);
	}
}



}  // namespace flancho
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
