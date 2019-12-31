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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/flancho/Cst.h"
#include "mfx/pi/flancho/Flancho.h"
#include "mfx/pi/flancho/Param.h"
#include "mfx/pi/flancho/WfType.h"
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
,	_param_change_flag_ovrspl ()
,	_chn_arr (_max_nbr_chn)
,	_interp_cubic ()
,	_interp_linear ()
,	_buf_tmp (1024)
,	_buf_render (1024)
,	_buf_ovrspl_src ()
,	_buf_ovrspl_dst ()
,	_nbr_chn_in (0)
,	_nbr_chn_out (0)
,	_mix (0.5f)
,	_neg_flag (false)
,	_ovrspl_flag (false)
,	_ovrspl_cur (1)
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
	_state_set.set_val_nat (desc_set, Param_MIX       , 0.5);
	_state_set.set_val_nat (desc_set, Param_NEGATIVE  , 0);
	_state_set.set_val_nat (desc_set, Param_PHASE_SET , 0);
	_state_set.set_val_nat (desc_set, Param_OVRSPL    , 0);

	_state_set.add_observer (Param_SPEED     , _param_change_flag_speed);
	_state_set.add_observer (Param_DEPTH     , _param_change_flag_depth_fdbk);
	_state_set.add_observer (Param_DELAY     , _param_change_flag_delay);
	_state_set.add_observer (Param_FDBK      , _param_change_flag_depth_fdbk);
	_state_set.add_observer (Param_WF_TYPE   , _param_change_flag_wf);
	_state_set.add_observer (Param_WF_SHAPE  , _param_change_flag_wf);
	_state_set.add_observer (Param_NBR_VOICES, _param_change_flag_voices);
	_state_set.add_observer (Param_MIX       , _param_change_flag_dry);
	_state_set.add_observer (Param_NEGATIVE  , _param_change_flag_dry);
	_state_set.add_observer (Param_PHASE_SET , _param_change_flag_phase_set);
	_state_set.add_observer (Param_OVRSPL    , _param_change_flag_ovrspl);

	_param_change_flag_depth_fdbk.add_observer (_param_change_flag);
	_param_change_flag_wf        .add_observer (_param_change_flag);
	_param_change_flag_speed     .add_observer (_param_change_flag);
	_param_change_flag_delay     .add_observer (_param_change_flag);
	_param_change_flag_voices    .add_observer (_param_change_flag);
	_param_change_flag_phase_set .add_observer (_param_change_flag);
	_param_change_flag_dry       .add_observer (_param_change_flag);
	_param_change_flag_ovrspl    .add_observer (_param_change_flag);

//	_state_set.set_ramp_time (Param_, 0.010);

	init_coef ();
	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_cnt];
		chn._fchn_sptr = ChnSPtr (new FlanchoChn (
			_interp_cubic,
			&_buf_tmp [0],
			int (_buf_tmp.size ()),
			&_buf_render [0],
			int (_buf_render.size ())
		));
		chn._fchn_sptr->set_rel_phase (0);

		chn._us.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._ds.set_coefs (&_coef_42 [0], &_coef_21 [0]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Flancho::do_get_state () const
{
	return _state;
}



double	Flancho::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Flancho::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbl_ovrspl = max_buf_len * _ovrspl;
	_buf_ovrspl_src.resize (mbl_ovrspl);
	_buf_ovrspl_dst.resize (mbl_ovrspl);

	_param_change_flag_depth_fdbk.set ();
	_param_change_flag_wf.set ();
	_state_set.use_state (Param_SPEED     ).use_notif_flag ().set ();
	_state_set.use_state (Param_DELAY     ).use_notif_flag ().set ();
	_state_set.use_state (Param_NBR_VOICES).use_notif_flag ().set ();
	_state_set.use_state (Param_PHASE_SET ).use_notif_flag ().set ();

	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		Channel &		chn = _chn_arr [chn_cnt];

		// Sets the oversampled rate first to make sure enough buffer is
		// reserved.
		chn._fchn_sptr->set_sample_freq (
			_sample_freq * _ovrspl, true, _interp_linear
		);
		chn._fchn_sptr->set_sample_freq (
			_sample_freq, false, _interp_cubic
		);
	}

	_nbr_chn_in  = 0;
	_nbr_chn_out = 0;

	update_param (true);
	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Flancho::do_clean_quick ()
{
	clear_buffers ();
}



void	Flancho::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	if (nbr_chn_out != _nbr_chn_out)
	{
		double         phase_mult = 0;
		if (nbr_chn_out > 1)
		{
			phase_mult = 0.25 * fstb::rcp_uint <double> (nbr_chn_out - 1);
		}

		for (int chn_cnt = 0; chn_cnt < nbr_chn_out; ++chn_cnt)
		{
			_chn_arr [chn_cnt]._fchn_sptr->set_rel_phase (chn_cnt * phase_mult);
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
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		const float       mix_old = _mix;
		_state_set.process_block (work_len);
		update_param ();

		// Signal processing
		int            chn_in     = 0;
		int            chn_in_inc = (_nbr_chn_in > _nbr_chn_out) ? 1 : 0;
		for (int chn_cnt = 0; chn_cnt < _nbr_chn_out; ++chn_cnt)
		{
			Channel &      chn = _chn_arr [chn_cnt];

			const float *  src_ptr      = proc._src_arr [chn_in ] + pos;
			float *        dst_ptr      = proc._dst_arr [chn_cnt] + pos;
			const float *  src_proc_ptr = src_ptr;
			float *        dst_proc_ptr = dst_ptr;
			int            len_mult     = 1;
			if (_ovrspl_flag)
			{
				if (chn_in_inc > 0 || chn_cnt == 0)
				{
					chn._us.process_block (
						&_buf_ovrspl_src [0], src_ptr, work_len
					);
				}
				src_proc_ptr = &_buf_ovrspl_src [0];
				dst_proc_ptr = &_buf_ovrspl_dst [0];
				len_mult     = _ovrspl;
			}

			chn._fchn_sptr->process_block (
				dst_proc_ptr, src_proc_ptr, work_len * len_mult
			);

			dsp::mix::Align::copy_xfade_2_1_vlrauto (
				dst_proc_ptr,
				src_proc_ptr,
				dst_proc_ptr,
				work_len * len_mult,
				mix_old,
				_mix
			);

			if (_ovrspl_flag)
			{
				chn._ds.process_block (dst_ptr, dst_proc_ptr, work_len);
			}

			chn_in += chn_in_inc;
		}

		pos += work_len;
	}
	while (pos < proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Flancho::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._fchn_sptr->clear_buffers ();
		chn._ds.clear_buffers ();
		chn._us.clear_buffers ();
	}
}



void	Flancho::init_coef ()
{
	if (! _coef_init_flag)
	{
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_42 [0], _nbr_coef_42, 1.0 / 5
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_21 [0], _nbr_coef_21, 1.0 / 100
		);

		_coef_init_flag = true;
	}
}



void	Flancho::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_ovrspl (true) || force_flag)
		{
			const bool     ovrspl_old_flag = _ovrspl_flag;
			_ovrspl_flag = (_state_set.get_val_tgt_nat (Param_OVRSPL) >= 0.5f);
			if (_ovrspl_flag != ovrspl_old_flag)
			{
				_ovrspl_cur  = (_ovrspl_flag) ? _ovrspl : 1;
				const float    fs = float (_sample_freq) * _ovrspl_cur;
				dsp::rspl::InterpolatorInterface * interp_ptr =
					(_ovrspl_flag)
					? static_cast <dsp::rspl::InterpolatorInterface *> (&_interp_linear)
					: static_cast <dsp::rspl::InterpolatorInterface *> (&_interp_cubic);
				for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
				{
					Channel &		chn = _chn_arr [chn_cnt];
					chn._fchn_sptr->set_sample_freq (fs, true, *interp_ptr);
				}
			}
		}
		if (_param_change_flag_speed (true) || force_flag)
		{
			update_lfo_period ();
		}
		if (_param_change_flag_delay (true) || force_flag)
		{
			const double   delay = _state_set.get_val_end_nat (Param_DELAY);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				Channel &      chn = _chn_arr [chn_cnt];
				chn._fchn_sptr->set_delay (delay);
			}
		}
		if (_param_change_flag_depth_fdbk (true) || force_flag)
		{
			const double   depth = _state_set.get_val_end_nat (Param_DEPTH);
			const double   fdbk  = _state_set.get_val_end_nat (Param_FDBK);
			const double   dmul  = 0.5 / _max_nbr_chn;
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				Channel &      chn = _chn_arr [chn_cnt];
				const double      chn_depth = depth * (1 - chn_cnt * dmul);
				chn._fchn_sptr->set_depth (chn_depth);
				chn._fchn_sptr->set_feedback (fdbk);
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
				Channel &      chn = _chn_arr [chn_cnt];
				chn._fchn_sptr->set_wf_type (type);
				chn._fchn_sptr->set_wf_shape (shape);
			}
		}
		if (_param_change_flag_voices (true) || force_flag)
		{
			const int      nbr_voices =
				fstb::round_int (_state_set.get_val_tgt_nat (Param_NBR_VOICES));
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				Channel &      chn = _chn_arr [chn_cnt];
				chn._fchn_sptr->set_nbr_voices (nbr_voices);
			}
		}
		if (_param_change_flag_phase_set (true) || force_flag)
		{
			double         phase = _state_set.get_val_tgt_nat (Param_PHASE_SET);
			phase = std::min (phase, 0.999);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				Channel &      chn = _chn_arr [chn_cnt];
				chn._fchn_sptr->resync (phase);
			}
		}
		if (_param_change_flag_dry (true) || force_flag)
		{
			_mix      = float (_state_set.get_val_tgt_nat (Param_MIX));
			_neg_flag = (_state_set.get_val_tgt_nat (Param_NEGATIVE) >= 0.5f);
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				Channel &      chn = _chn_arr [chn_cnt];
				chn._fchn_sptr->set_polarity (_neg_flag);
			}
		}
	}
}



void	Flancho::update_lfo_period ()
{
	const double   freq = _state_set.get_val_end_nat (Param_SPEED);
	for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_cnt];
		chn._fchn_sptr->set_speed (freq);
	}
}



bool	Flancho::_coef_init_flag = false;
std::array <double, Flancho::_nbr_coef_42>	Flancho::_coef_42;
std::array <double, Flancho::_nbr_coef_21>	Flancho::_coef_21;



}  // namespace flancho
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
