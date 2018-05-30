/*****************************************************************************

        Ramp.cpp
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
#include "mfx/pi/ramp/CurveType.h"
#include "mfx/pi/ramp/Param.h"
#include "mfx/pi/ramp/Ramp.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"


#include <cassert>



namespace mfx
{
namespace pi
{
namespace ramp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Ramp::Ramp ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_flag_base ()
,	_param_change_flag_shape ()
,	_param_change_flag_pos ()
,	_ramp ()
,	_amp (1)
,	_pause_flag (false)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_TIME  , 1);
	_state_set.set_val_nat (desc_set, Param_AMP   , 1);
	_state_set.set_val_nat (desc_set, Param_CURVE , CurveType_LINEAR);
	_state_set.set_val_nat (desc_set, Param_SNH   , 0);
	_state_set.set_val_nat (desc_set, Param_SMOOTH, 0);
	_state_set.set_val_nat (desc_set, Param_DIR   , 0);
	_state_set.set_val_nat (desc_set, Param_POS   , 0);
	_state_set.set_val_nat (desc_set, Param_DELAY , 0);
	_state_set.set_val_nat (desc_set, Param_STATE , 0);

	_state_set.add_observer (Param_TIME  , _param_change_flag_base);
	_state_set.add_observer (Param_AMP   , _param_change_flag_base);
	_state_set.add_observer (Param_CURVE , _param_change_flag_base);
	_state_set.add_observer (Param_SNH   , _param_change_flag_shape);
	_state_set.add_observer (Param_SMOOTH, _param_change_flag_shape);
	_state_set.add_observer (Param_DIR   , _param_change_flag_shape);
	_state_set.add_observer (Param_POS   , _param_change_flag_pos);
	_state_set.add_observer (Param_DELAY , _param_change_flag_base);
	_state_set.add_observer (Param_STATE , _param_change_flag_base);

	_param_change_flag_base .add_observer (_param_change_flag);
	_param_change_flag_shape.add_observer (_param_change_flag);
	_param_change_flag_pos  .add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Ramp::do_get_state () const
{
	return _state;
}



double	Ramp::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Ramp::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_ramp.set_sample_freq (sample_freq);

	_param_change_flag_base .set ();
	_param_change_flag_shape.set ();
	_param_change_flag_pos  .set ();

	update_param (true);
	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	Ramp::do_clean_quick ()
{
	clear_buffers ();
}



void	Ramp::do_process_block (ProcInfo &proc)
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
	const float    val = float (_ramp.get_val ());
	proc._sig_arr [0] [0] = val * _amp;

	// Parameters
	update_param (false);

	if (! _pause_flag)
	{
		_ramp.tick (proc._nbr_spl);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Ramp::clear_buffers ()
{
	_ramp.clear_buffers ();
}



void	Ramp::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_base (true) || force_flag)
		{
			const double   per = _state_set.get_val_tgt_nat (Param_TIME);
			_ramp.set_period (per);

			_amp = float (_state_set.get_val_tgt_nat (Param_AMP));

			const CurveType   curve = static_cast <CurveType> (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_CURVE)
			));
			_ramp.set_type (curve);

			const double   dly = _state_set.get_val_tgt_nat (Param_DELAY);
			_ramp.set_initial_delay (dly);

			_pause_flag = (_state_set.get_val_tgt_nat (Param_STATE) >= 0.5f);
		}
		if (_param_change_flag_pos (true) || force_flag)
		{
			const double   p = _state_set.get_val_tgt_nat (Param_POS);
			if (p < 1)
			{
				_ramp.set_phase (p);
			}
		}
		if (_param_change_flag_shape (true) || force_flag)
		{
			const double   snh = _state_set.get_val_tgt_nat (Param_SNH);
			_ramp.set_snh (snh);

			const double   smt = _state_set.get_val_tgt_nat (Param_SMOOTH);
			_ramp.set_smooth (smt);

			const bool     inv_flag = (_state_set.get_val_tgt_nat (Param_DIR) >= 0.5f);
			_ramp.set_sign (inv_flag);
		}
	}
}



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
