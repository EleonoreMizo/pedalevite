/*****************************************************************************

        IIFix.cpp
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

#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/iifix/IIFix.h"
#include "mfx/pi/iifix/Param.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace iifix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



IIFix::IIFix ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_inv_fs (0)
,	_chn_arr ()
,	_level (2)
,	_freq (2000)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FREQ , 2000);
	_state_set.set_val_nat (desc_set, Param_LEVEL,    2);

	_state_set.add_observer (Param_FREQ , _param_change_flag);
	_state_set.add_observer (Param_LEVEL, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	IIFix::do_get_state () const
{
	return _state;
}



double	IIFix::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	IIFix::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	update_filter ();

	for (auto &op : _chn_arr)
	{
		op.clear_buffers ();
	}

	_state = State_ACTIVE;

	return Err_OK;
}



void	IIFix::do_process_block (ProcInfo &proc)
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

	// Parameters
	_state_set.process_block (proc._nbr_spl);

	if (_param_change_flag (true))
	{
		_freq  = float (_state_set.get_val_tgt_nat (Param_FREQ ));
		_level = float (_state_set.get_val_tgt_nat (Param_LEVEL));
		update_filter ();
	}

	// Signal processing
	const int      nbr_chn =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN];
	assert (nbr_chn == proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT]);
	for (int chn = 0; chn < nbr_chn; ++chn)
	{
		_chn_arr [chn].process_block (
			proc._dst_arr [chn],
			proc._src_arr [chn],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	IIFix::update_filter ()
{
	const float    b_s [2] = { 1, _level };
	const float    a_s [2] = { 1,      1 };
	float          bz [2];
	float          az [2];
	const float    k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (bz, az, b_s, a_s, k);
	for (auto &op : _chn_arr)
	{
		op.set_z_eq (bz, az);
	}
}



}  // namespace iifix
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
