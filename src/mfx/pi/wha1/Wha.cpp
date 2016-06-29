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
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/wha1/Param.h"
#include "mfx/pi/wha1/Wha.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace wha1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Wha::Wha ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_filter_arr ()
,	_inv_fs (0)
,	_freq (1000)
,	_q (1)
{
	_state_set.init (piapi::ParamCateg_GLOBAL, _desc.use_desc_set ());

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

		for (auto &filter : _filter_arr)
		{
			filter.set_z_eq (z_eq_b, z_eq_a);
		}
	}

	// Signal processing
	const int      nbr_chn_i =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_o =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	const int      nbr_chn_p = std::min (nbr_chn_i, nbr_chn_o);
	for (int c = 0; c < nbr_chn_p; ++c)
	{
		_filter_arr [c].process_block (
			proc._dst_arr [c],
			proc._src_arr [c],
			proc._nbr_spl
		);
	}
	for (int c = nbr_chn_p; c < nbr_chn_o; ++c)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [c],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wha1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
