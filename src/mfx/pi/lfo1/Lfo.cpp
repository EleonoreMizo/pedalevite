/*****************************************************************************

        Lfo.cpp
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

#include "fstb/fnc.h"
#include "mfx/pi/lfo1/Lfo.h"
#include "mfx/pi/lfo1/LfoType.h"
#include "mfx/pi/lfo1/Param.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"


#include <cassert>



namespace mfx
{
namespace pi
{
namespace lfo1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Lfo::Lfo ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_flag_base ()
,	_param_change_flag_phase ()
,	_param_change_flag_phset ()
,	_param_change_flag_shape ()
,	_param_change_flag_wf ()
,	_lfo ()
,	_amp (1)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SPEED      , 1);
	_state_set.set_val_nat (desc_set, Param_AMP        , 1);
	_state_set.set_val_nat (desc_set, Param_WAVEFORM   , LfoType_SINE);
	_state_set.set_val_nat (desc_set, Param_SNH        , 0);
	_state_set.set_val_nat (desc_set, Param_SMOOTH     , 0);
	_state_set.set_val_nat (desc_set, Param_CHAOS      , 0);
	_state_set.set_val_nat (desc_set, Param_PH_DIST_AMT, 0.5);
	_state_set.set_val_nat (desc_set, Param_PH_DIST_OFS, 0);
	_state_set.set_val_nat (desc_set, Param_SIGN       , 0);
	_state_set.set_val_nat (desc_set, Param_POLARITY   , 0);
	_state_set.set_val_nat (desc_set, Param_VAR1       , 0);
	_state_set.set_val_nat (desc_set, Param_VAR2       , 0);
	_state_set.set_val_nat (desc_set, Param_PHASE_SET  , 0);

	_state_set.add_observer (Param_SPEED      , _param_change_flag_base);
	_state_set.add_observer (Param_AMP        , _param_change_flag_base);
	_state_set.add_observer (Param_WAVEFORM   , _param_change_flag_wf);
	_state_set.add_observer (Param_SNH        , _param_change_flag_shape);
	_state_set.add_observer (Param_SMOOTH     , _param_change_flag_shape);
	_state_set.add_observer (Param_CHAOS      , _param_change_flag_phase);
	_state_set.add_observer (Param_PH_DIST_AMT, _param_change_flag_phase);
	_state_set.add_observer (Param_PH_DIST_OFS, _param_change_flag_phase);
	_state_set.add_observer (Param_SIGN       , _param_change_flag_shape);
	_state_set.add_observer (Param_POLARITY   , _param_change_flag_shape);
	_state_set.add_observer (Param_VAR1       , _param_change_flag_wf);
	_state_set.add_observer (Param_VAR2       , _param_change_flag_wf);
	_state_set.add_observer (Param_PHASE_SET  , _param_change_flag_phset);

	_param_change_flag_base .add_observer (_param_change_flag);
	_param_change_flag_phase.add_observer (_param_change_flag);
	_param_change_flag_shape.add_observer (_param_change_flag);
	_param_change_flag_wf   .add_observer (_param_change_flag);
	_param_change_flag_phset.add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Lfo::do_get_state () const
{
	return _state;
}



double	Lfo::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Lfo::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_lfo.set_sample_freq (sample_freq);

	_param_change_flag_base .set ();
	_param_change_flag_phase.set ();
	_param_change_flag_phset.set ();
	_param_change_flag_shape.set ();
	_param_change_flag_wf   .set ();

	update_param (true);

	_lfo.clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	Lfo::do_process_block (ProcInfo &proc)
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

	// Signal
	const float    val = float (_lfo.get_val ());
	proc._sig_arr [0] [0] = val * _amp;

	// Parameters
	update_param (false);

	_lfo.tick (proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Lfo::update_param (bool force_flag)
{
	if (_param_change_flag_base (true) || force_flag)
	{
		const double  spd = _state_set.get_val_tgt_nat (Param_SPEED);
		_lfo.set_period (1.0 / spd);
		_amp = float (_state_set.get_val_tgt_nat (Param_AMP));
	}
	if (_param_change_flag_phase (true) || force_flag)
	{
		const float    pch = float (_state_set.get_val_tgt_nat (Param_CHAOS));
		const float    pda = float (_state_set.get_val_tgt_nat (Param_PH_DIST_AMT));
		const float    pdo = float (_state_set.get_val_tgt_nat (Param_PH_DIST_OFS));

		_lfo.set_chaos (pch);
		_lfo.set_phase_dist (pda);
		_lfo.set_phase_dist_offset (pdo);
	}
	if (_param_change_flag_phset (true) || force_flag)
	{
		float          p = float (_state_set.get_val_tgt_nat (Param_PHASE_SET));
		if (p <= 0.9999f)
		{
			_lfo.set_phase (p);
		}
	}
	if (_param_change_flag_shape (true) || force_flag)
	{
		const float    snh = float (_state_set.get_val_tgt_nat (Param_SNH));
		const float    smt = float (_state_set.get_val_tgt_nat (Param_SMOOTH));
		const bool     inv_flag =
			(_state_set.get_val_tgt_nat (Param_SIGN) >= 0.5f);
		const bool     uni_flag =
			(_state_set.get_val_tgt_nat (Param_POLARITY) >= 0.5f);
		_lfo.set_snh (snh);
		_lfo.set_smooth (smt);
		_lfo.set_sign (inv_flag);
		_lfo.set_polarity (uni_flag);
	}
	if (_param_change_flag_wf (true) || force_flag)
	{
		const LfoType  wf = static_cast <LfoType> (fstb::round_int (
			_state_set.get_val_tgt_nat (Param_WAVEFORM)
		));
		dsp::ctrl::lfo::LfoModule::Type  wf2 =
			dsp::ctrl::lfo::LfoModule::Type_SINE;
		switch (wf)
		{
		case LfoType_SINE:      wf2 = dsp::ctrl::lfo::LfoModule::Type_SINE      ; break;
		case LfoType_TRIANGLE:  wf2 = dsp::ctrl::lfo::LfoModule::Type_TRIANGLE  ; break;
		case LfoType_SQUARE:    wf2 = dsp::ctrl::lfo::LfoModule::Type_SQUARE    ; break;
		case LfoType_SAW:       wf2 = dsp::ctrl::lfo::LfoModule::Type_SAW       ; break;
		case LfoType_PARABOLA:  wf2 = dsp::ctrl::lfo::LfoModule::Type_PARABOLA  ; break;
		case LfoType_BIPHASE:   wf2 = dsp::ctrl::lfo::LfoModule::Type_BIPHASE   ; break;
		case LfoType_N_PHASE:   wf2 = dsp::ctrl::lfo::LfoModule::Type_N_PHASE   ; break;
		case LfoType_VARISLOPE: wf2 = dsp::ctrl::lfo::LfoModule::Type_VARISLOPE ; break;
		case LfoType_NOISE_FLT: wf2 = dsp::ctrl::lfo::LfoModule::Type_NOISE_FLT2; break;
		default:
			assert (false);
			break;
		}
		_lfo.set_type (wf2);
		const float    v1 = float (_state_set.get_val_tgt_nat (Param_VAR1));
		const float    v2 = float (_state_set.get_val_tgt_nat (Param_VAR2));
		_lfo.set_variation (0, v1);
		_lfo.set_variation (1, v2);
	}
}



}  // namespace lfo1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
