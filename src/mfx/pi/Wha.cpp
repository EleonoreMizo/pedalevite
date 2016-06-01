/*****************************************************************************

        Wha.cpp
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
#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/Wha.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Wha::Wha ()
:	_state (State_CONSTRUCTED)
,	_desc_set (Param_NBR_ELT, 0)
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_filter_arr ()
,	_inv_fs (0)
,	_freq (1000)
,	_q (1)
{
	// Frequency
	param::TplLog *   log_ptr = new param::TplLog (
		120, 120 * 32,
		"Frequency\nFreq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FREQ, log_ptr);

	// Base Q
	log_ptr = new param::TplLog (
		2, 32,
		"Selectivity\nQ",
		"",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%4.1f"
	);
	_desc_set.add_glob (Param_Q, log_ptr);

	_state_set.init (piapi::ParamCateg_GLOBAL, _desc_set);

	_state_set.set_val (Param_FREQ, 0.50   );
	_state_set.set_val (Param_Q   , 1.0 / 3); // -> q = 5

	_state_set.add_observer (Param_FREQ, _param_change_flag);
	_state_set.add_observer (Param_Q   , _param_change_flag);

	_state_set.set_ramp_time (Param_FREQ, 0.010);
	_state_set.set_ramp_time (Param_Q   , 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Wha::do_get_state () const
{
	return _state;
}



int	Wha::do_init ()
{
	_state = State_INITIALISED;

	return Err_OK;
}



int	Wha::do_restore ()
{
	_state = State_CONSTRUCTED;

	return Err_OK;
}



void	Wha::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	Wha::do_prefer_stereo () const
{
	return false;
}



int	Wha::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Wha::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



double	Wha::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Wha::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;
	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();
	_param_change_flag.set ();

	_inv_fs = float (1.0 / _sample_freq);
	for (int c = 0; c < int (_filter_arr.size ()); ++ c)
	{
		_filter_arr [c].clear_buffers ();
	}

	_state = State_ACTIVE;

	return Err_OK;
}



void	Wha::do_process_block (ProcInfo &proc)
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

//	float          freq_beg = _freq;
	float          freq_end = _freq;
//	float          q_beg    = _q;
	float          q_end    = _q;

	if (_param_change_flag (true))
	{
		freq_end = float (_state_set.get_val_end_nat (Param_FREQ));
		q_end    = float (_state_set.get_val_end_nat (Param_Q   ));

		_freq = freq_end;
		_q    = q_end;

		// Computes the new filter coefficients
		const float    inv_f      = 1.0f / freq_end;
		const float    q          = q_end * 1000 * inv_f;
		const float    inv_q      = 1.0f / q;
		float          g          = (freq_end - 70) * inv_f;
		g *= g;
		const float    s_eq_b [3] =
		{
			0.0f,
			q_end * inv_q * 1.5f,
			0.5f  * inv_q
		};
		const float    s_eq_a [3] =
		{
			1,
			g * inv_q,
			1
		};
		float          z_eq_b [3];
		float          z_eq_a [3];
		const float		k =
			dsp::iir::TransSZBilin::compute_k_approx (freq_end * _inv_fs);
		dsp::iir::TransSZBilin::map_s_to_z_approx (
			z_eq_b, z_eq_a,
			s_eq_b, s_eq_a,
			k
		);

		for (int c = 0; c < int (_filter_arr.size ()); ++ c)
		{
			_filter_arr [c].set_z_eq (z_eq_b, z_eq_a);
		}
	}

	// Signal processing
	const int      nbr_chn_i =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_o =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	int            chn_i_index = 0;
	const int      chn_i_inc   = (nbr_chn_i > 0) ? 1 : 0;
	for (int c = 0; c < nbr_chn_o; ++c)
	{
		_filter_arr [c].process_block (
			proc._dst_arr [c],
			proc._src_arr [chn_i_index],
			proc._nbr_spl
		);
		chn_i_index += chn_i_inc;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
