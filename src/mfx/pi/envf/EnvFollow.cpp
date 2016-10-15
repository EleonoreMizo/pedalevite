/*****************************************************************************

        EnvFollow.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
#include "mfx/pi/envf/EnvFollow.h"
#include "mfx/pi/envf/Param.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace envf
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EnvFollow::EnvFollow ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_envf ()
,	_buf_src ()
,	_buf_env ()
,	_mode (Mode_LINEAR)
,	_gain (10)
,	_thresh (0)
,	_log_mul (0.5f)
,	_log_add (0)
,	_clip_flag (false)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_ATTACK ,  0.001);
	_state_set.set_val_nat (desc_set, Param_HOLD   ,  0    );
	_state_set.set_val_nat (desc_set, Param_RELEASE,  0.250);
	_state_set.set_val_nat (desc_set, Param_GAIN   , 10);
	_state_set.set_val_nat (desc_set, Param_THRESH ,  0);
	_state_set.set_val_nat (desc_set, Param_MODE   , Mode_LINEAR);
	_state_set.set_val_nat (desc_set, Param_CLIP   , 0);

	_state_set.add_observer (Param_ATTACK , _param_change_flag_time);
	_state_set.add_observer (Param_HOLD   , _param_change_flag_time);
	_state_set.add_observer (Param_RELEASE, _param_change_flag_time);
	_state_set.add_observer (Param_GAIN   , _param_change_flag_misc);
	_state_set.add_observer (Param_THRESH , _param_change_flag_misc);
	_state_set.add_observer (Param_MODE   , _param_change_flag_misc);
	_state_set.add_observer (Param_CLIP   , _param_change_flag_misc);

	_param_change_flag_time.add_observer (_param_change_flag);
	_param_change_flag_misc.add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	EnvFollow::do_get_state () const
{
	return _state;
}



double	EnvFollow::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	EnvFollow::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);
	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      len_align = (max_buf_len + 3) & -4;
	_buf_src.resize (len_align);
	_buf_env.resize (len_align);

	update_param (true);

	_state = State_ACTIVE;

	return Err_OK;
}



void	EnvFollow::do_process_block (ProcInfo &proc)
{
	// Events
	const int      nbr_evt = proc._nbr_evt;
	for (int index = 0; index < nbr_evt; ++index)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [index]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	// Parameters
	update_param ();

	// Signal processing
	square_block (proc);
	_envf->process_block_1_chn (&_buf_env [0], &_buf_src [0], proc._nbr_spl);
	const float    val2 = _buf_env [proc._nbr_spl >> 1];
	float          val  = 0;
	if (_mode == Mode_LOG)
	{
		val = fstb::Approx::log2 (val2 + 1e-20f);
		val = val * _log_mul + _log_add;
	}
	else
	{
		val = sqrt (val2) * _gain;
	}
	val -= _thresh;
	val  = std::max (val, 0.0f);
	if (_clip_flag)
	{
		val = std::min (val, 1.0f);
	}
	proc._sig_arr [0] [0] = val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EnvFollow::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_time (true) || force_flag)
		{
			const double   atk = _state_set.get_val_tgt_nat (Param_ATTACK);
			const double   hld = _state_set.get_val_tgt_nat (Param_HOLD);
			const double   rls = _state_set.get_val_tgt_nat (Param_RELEASE);

			const int      hld_spl = fstb::round_int (hld * _sample_freq);
			const float    c_atk   = conv_time_to_coef (float (atk));
			const float    c_rls   = conv_time_to_coef (float (rls));

			_envf->set_atk_coef (0, c_atk);
			_envf->set_rls_coef (0, c_rls);
			_envf->set_hold_time (0, hld_spl);
		}

		if (_param_change_flag_misc (true) || force_flag)
		{
			_gain   = float (_state_set.get_val_tgt_nat (Param_GAIN));
			_thresh = float (_state_set.get_val_tgt_nat (Param_THRESH));
			_mode   = static_cast <Mode> (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_MODE)
			));
			if (_mode == Mode_LOG)
			{
				// val = 1 / gain -> 1
				// val = thr / gain -> 0

				// val *= 0.5;
				// val += l2g; (1 / gain -> 0, thr / gain -> l2t)
				// val /= l2t;
				// val = 1 - val;

				// val = 1 - ((val * 0.5 + l2g)) / l2t
				// val = val * -0.5 / l2t + 1 - l2g / l2t

				const float  l2g = fstb::Approx::log2 (_gain);
				const float  l2t = fstb::Approx::log2 (std::max (_thresh, 1e-3f));
				_log_add = 1 - l2g / l2t;
				_log_mul = -0.5f   / l2t;
			}
			_clip_flag = (_state_set.get_val_tgt_nat (Param_CLIP) >= 0.5f);
		}
	}
}



float	EnvFollow::conv_time_to_coef (float t)
{
	float          coef = 1;
	const float    ts   = t * _sample_freq;
	if (ts >= 1)
	{
		coef = 1.f - fstb::Approx::exp2 (float (-1 / fstb::LN2) / ts);
	}

	return coef;
}



void	EnvFollow::square_block (const ProcInfo &proc)
{
	const int      nbr_chn_src = proc._nbr_chn_arr [Dir_IN ];
	const float *  s0_ptr      = proc._src_arr [0];
	if (nbr_chn_src == 1)
	{
		for (int pos = 0; pos < proc._nbr_spl; pos += 4)
		{
			const auto  x  = fstb::ToolsSimd::load_f32 (s0_ptr + pos);
			const auto  x2 = x * x;
			fstb::ToolsSimd::store_f32 (&_buf_src [pos], x2);
		}
	}

	else
	{
		assert (nbr_chn_src == 2);
		const float *  s1_ptr = proc._src_arr [1];
		const auto     mult   = fstb::ToolsSimd::set1_f32 (0.5f);
		for (int pos = 0; pos < proc._nbr_spl; pos += 4)
		{
			const auto     x   = fstb::ToolsSimd::load_f32 (s0_ptr + pos);
			const auto     y   = fstb::ToolsSimd::load_f32 (s1_ptr + pos);
			const auto     sum = (x * x + y * y) * mult;
			fstb::ToolsSimd::store_f32 (&_buf_src [pos], sum);
		}
	}
}



}  // namespace envf
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
