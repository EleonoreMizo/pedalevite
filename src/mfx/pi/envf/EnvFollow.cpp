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
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/envf/Cst.h"
#include "mfx/pi/envf/EnvFollow.h"
#include "mfx/pi/envf/Param.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

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
,	_inv_fs (0)
,	_param_change_flag ()
,	_chn_arr ()
,	_envf ()
,	_buf_src ()
,	_buf_env ()
,	_hpf ()
,	_hpf_freq (Cst::_lc_freq_min)
,	_mode (Mode_LINEAR)
,	_gain (10)
,	_thresh (0)
,	_log_mul (0.5f)
,	_log_add (0)
,	_clip_src_lvl (Cst::_clip_max)
,	_clip_env_flag (false)
,	_hpf_flag (false)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_ATTACK ,  0.001);
	_state_set.set_val_nat (desc_set, Param_HOLD   ,  0    );
	_state_set.set_val_nat (desc_set, Param_RELEASE,  0.250);
	_state_set.set_val_nat (desc_set, Param_GAIN   , 10);
	_state_set.set_val_nat (desc_set, Param_THRESH ,  0);
	_state_set.set_val_nat (desc_set, Param_MODE   , Mode_LINEAR);
	_state_set.set_val_nat (desc_set, Param_CLIP_E , 0);
	_state_set.set_val (Param_CLIP_S, 1);
	_state_set.set_val_nat (desc_set, Param_LC_FREQ, Cst::_lc_freq_min);

	_state_set.add_observer (Param_ATTACK , _param_change_flag_time);
	_state_set.add_observer (Param_HOLD   , _param_change_flag_time);
	_state_set.add_observer (Param_RELEASE, _param_change_flag_time);
	_state_set.add_observer (Param_GAIN   , _param_change_flag_misc);
	_state_set.add_observer (Param_THRESH , _param_change_flag_misc);
	_state_set.add_observer (Param_MODE   , _param_change_flag_misc);
	_state_set.add_observer (Param_CLIP_E , _param_change_flag_misc);
	_state_set.add_observer (Param_CLIP_S , _param_change_flag_misc);

	_param_change_flag_time.add_observer (_param_change_flag);
	_param_change_flag_misc.add_observer (_param_change_flag);

	for (auto &chn : _chn_arr)
	{
		chn._hpf.neutralise ();
	}
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

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	_state_set.set_sample_freq (sample_freq);

	const int      len_align = (max_buf_len + 3) & -4;
	_buf_src.resize (len_align);
	_buf_env.resize (len_align);

	update_param (true);
	_state_set.clear_buffers ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	EnvFollow::do_clean_quick ()
{
	clear_buffers ();
}



void	EnvFollow::do_process_block (piapi::ProcInfo &proc)
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

	if (_clip_src_lvl < Cst::_clip_max)
	{
		clip_block (proc._nbr_spl);
	}

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
	if (_clip_env_flag)
	{
		val = std::min (val, 1.0f);
	}
	proc._sig_arr [0] [0] = val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EnvFollow::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._hpf.clear_buffers ();
	}
}



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
			_clip_env_flag = (_state_set.get_val_tgt_nat (Param_CLIP_E) >= 0.5f);
			_clip_src_lvl  = float (_state_set.get_val_end_nat (Param_CLIP_S));
			_hpf_freq      = float (_state_set.get_val_end_nat (Param_LC_FREQ));
			_hpf_flag = (_hpf_freq > Cst::_lc_freq_min);
			if (_hpf_flag)
			{
				// We need some gain compensation to keep the same average
				// energy when the cutoff frequency increases.
				const float    fz     = 100; // Hz
				const float    fz_inv = 1.0f / fz;
				float          gain   =
					std::max (float (sqrt (_hpf_freq * fz_inv)), 1.0f);
				const float    bs [2] = { 0, gain };
				const float    as [2] = { 1,    1 };
				const float    f = _hpf_freq * _inv_fs;
				const float    k = dsp::iir::TransSZBilin::compute_k_approx (f);
				float          bz [2];
				float          az [2];
				dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
					bz, az, bs, as, k
				);
				for (auto &chn : _chn_arr)
				{
					chn._hpf.set_z_eq (bz, az);
				}
			}
			else
			{
				_hpf.neutralise ();
			}
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



void	EnvFollow::square_block (const piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_spl     = proc._nbr_spl;
	float *        dst_ptr = &_buf_src [0];

	if (! _hpf_flag)
	{
		if (nbr_chn_src == 1)
		{
			dsp::mix::Align::sum_square_n_1 (
				dst_ptr, proc._src_arr, nbr_spl, nbr_chn_src, 0
			);
		}
		else if (nbr_chn_src == 2)
		{
			dsp::mix::Align::sum_square_n_1_v (
				dst_ptr, proc._src_arr, nbr_spl, nbr_chn_src, 0, 0.5f
			);
		}
		else
		{
			dsp::mix::Align::sum_square_n_1_v (
				dst_ptr, proc._src_arr, nbr_spl, nbr_chn_src, 0, 1.0f / nbr_chn_src
			);
		}
	}

	else
	{
		float *        tmp_ptr = &_buf_env [0];
		const auto     v       = fstb::ToolsSimd::set1_f32 (1.0f / nbr_chn_src);
		for (int chn_index = 0; chn_index < nbr_chn_src; ++chn_index)
		{
			const float *  src_ptr = proc._src_arr [chn_index];
			Channel &      chn = _chn_arr [chn_index];
			chn._hpf.process_block (tmp_ptr, src_ptr, nbr_spl);
			if (chn_index == 0)
			{
				for (int pos = 0; pos < nbr_spl; pos += 4)
				{
					auto           x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
					x *= x;
					fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
				}
			}
			if (chn_index > 0)
			{
				if (chn_index == nbr_chn_src - 1)
				{
					for (int pos = 0; pos < nbr_spl; pos += 4)
					{
						auto           x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
						auto           y = fstb::ToolsSimd::load_f32 (dst_ptr + pos);
						fstb::ToolsSimd::mac (y, x, x);
						y *= v;
						fstb::ToolsSimd::store_f32 (dst_ptr + pos, y);
					}
				}
				else
				{
					for (int pos = 0; pos < nbr_spl; pos += 4)
					{
						auto           x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
						auto           y = fstb::ToolsSimd::load_f32 (dst_ptr + pos);
						fstb::ToolsSimd::mac (y, x, x);
						fstb::ToolsSimd::store_f32 (dst_ptr + pos, y);
					}
				}
			}
		}
	}
}



void	EnvFollow::clip_block (int nbr_spl)
{
	const auto     lvl_sq  = _clip_src_lvl * _clip_src_lvl;
	const auto     c       = fstb::ToolsSimd::set1_f32 (lvl_sq);
	float *        spl_ptr = &_buf_src [0];
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = fstb::ToolsSimd::load_f32 (spl_ptr + pos);
		x = fstb::ToolsSimd::min_f32 (x, c);
		fstb::ToolsSimd::store_f32 (spl_ptr + pos, x);
	}
}



}  // namespace envf
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
