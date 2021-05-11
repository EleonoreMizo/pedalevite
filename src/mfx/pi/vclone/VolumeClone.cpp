/*****************************************************************************

        VolumeClone.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/dyn/SCPower.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/vclone/Param.h"
#include "mfx/pi/vclone/VolumeClone.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>
#include <utility>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace vclone
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



VolumeClone::VolumeClone ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_mode ()
,	_param_change_flag_gain ()
,	_param_change_flag_env ()
,	_chn_arr (_max_nbr_chn)
,	_chn_mode (ChnMode_LINK)
,	_mul_copy ()
,	_strength ()
,	_gain_min ()
,	_gain_max ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_MODE,  1);
	_state_set.set_val_nat (desc_set, Param_STRN,  1);
	_state_set.set_val_nat (desc_set, Param_CHN , ChnMode_LINK);
	_state_set.set_val_nat (desc_set, Param_GMIN,  0);
	_state_set.set_val_nat (desc_set, Param_GMAX, 10);
	_state_set.set_val_nat (desc_set, Param_ATK ,  1e-3);
	_state_set.set_val_nat (desc_set, Param_RLS , 50e-3);
	_state_set.set_val_nat (desc_set, Param_HOLD,  0);

	_state_set.add_observer (Param_MODE, _param_change_flag_mode);
	_state_set.add_observer (Param_STRN, _param_change_flag_mode);
	_state_set.add_observer (Param_CHN , _param_change_flag_mode);
	_state_set.add_observer (Param_GMIN, _param_change_flag_gain);
	_state_set.add_observer (Param_GMAX, _param_change_flag_gain);
	_state_set.add_observer (Param_ATK , _param_change_flag_env);
	_state_set.add_observer (Param_RLS , _param_change_flag_env);
	_state_set.add_observer (Param_HOLD, _param_change_flag_env);

	_param_change_flag_mode.add_observer (_param_change_flag);
	_param_change_flag_gain.add_observer (_param_change_flag);
	_param_change_flag_env .add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_MODE, 0.010f);
	_state_set.set_ramp_time (Param_STRN, 0.010f);
	_state_set.set_ramp_time (Param_GMIN, 0.010f);
	_state_set.set_ramp_time (Param_GMAX, 0.010f);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	VolumeClone::do_get_state () const
{
	return _state;
}



double	VolumeClone::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	VolumeClone::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_align = (max_buf_len + 3) & ~3;
	for (auto &chn : _chn_arr)
	{
		chn._buf_car.resize (mbs_align);
		chn._buf_mod.resize (mbs_align);
	}

	_param_change_flag_mode.set ();
	_param_change_flag_gain.set ();
	_param_change_flag_env.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	VolumeClone::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// Signal processing
	const int      mod_ofs     = nbr_chn_src;
	const int      nbr_spl     = proc._nbr_spl;
	const float    nbr_spl_inv = fstb::rcp_uint <float> (nbr_spl);

	_mul_copy.tick (nbr_spl);
	_strength.tick (nbr_spl);
	_gain_min.tick (nbr_spl);
	_gain_max.tick (nbr_spl);

	const float    m_c_beg = _mul_copy.get_beg ();
	const float    str_beg = _strength.get_beg ();
	const float    gmi_beg = _gain_min.get_beg ();
	const float    gma_beg = _gain_max.get_beg ();
	
	const float    m_c_end = _mul_copy.get_end ();
	const float    str_end = _strength.get_end ();
	const float    gmi_end = _gain_min.get_end ();
	const float    gma_end = _gain_max.get_end ();

	const float    m_c_stp = (m_c_end - m_c_beg) * nbr_spl_inv;
	const float    str_stp = (str_end - str_beg) * nbr_spl_inv;
	const float    gmi_stp = (gmi_end - gmi_beg) * nbr_spl_inv;
	const float    gma_stp = (gma_end - gma_beg) * nbr_spl_inv;

	// Envelope detection
	dsp::dyn::SCPower <> p;
	for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
	{
		Channel &      chn = _chn_arr [chn_idx];

		// Finds the number of channel to process or to group, depending on
		// the channel mode.
		static constexpr std::array <
			std::array <int, _max_nbr_chn>,
			ChnMode_NBR_ELT
		> nbr_chn_arr =
		{{
			{ 2, 0 }, // Link
			{ 1, 1 }, // Independant
			{ 1, 0 }, // Left
			{ 0, 1 }  // Right
		}};
		int            nbr_chn_env = nbr_chn_arr [_chn_mode] [chn_idx];
		nbr_chn_env = std::min (nbr_chn_env, nbr_chn_src);

		if (nbr_chn_env > 0)
		{
			p.prepare_env_input (
				chn._buf_car.data (),
				&proc._src_arr [chn_idx          ],
				nbr_chn_env, 0, nbr_spl
			);
			p.prepare_env_input (
				chn._buf_mod.data (),
				&proc._src_arr [chn_idx + mod_ofs],
				nbr_chn_env, 0, nbr_spl
			);

			chn._ef_car.process_block (
				chn._buf_car.data (), chn._buf_car.data (), nbr_spl
			);
			chn._ef_mod.process_block (
				chn._buf_mod.data (), chn._buf_mod.data (), nbr_spl
			);
		}
	}

	// Computes the gains
	static constexpr std::array <
		std::pair <bool, int>,
		ChnMode_NBR_ELT
	> chn_inf_arr =
	{{
		// false = one gain, true = several gains
		// Following: channel index for the envelope processing
		{ false, 0 }, // Link
		{ true , 0 }, // Independant
		{ false, 0 }, // Left
		{ false, 1 }  // Right
	}};
	const auto &   chn_inf     = chn_inf_arr [_chn_mode];
	int            chn_env_idx = chn_inf.second;
	const int      nbr_chn_g   = (chn_inf.first) ? nbr_chn_proc : 1;
	for (int chn_cnt = 0; chn_cnt < nbr_chn_g; ++chn_cnt)
	{
		Channel &      chn     = _chn_arr [chn_env_idx];

		float * fstb_RESTRICT		   car_ptr = chn._buf_car.data ();
		const float * fstb_RESTRICT   mod_ptr = chn._buf_mod.data ();

		float          m_c = m_c_beg;
		float          str = str_beg;
		float          gmi = gmi_beg;
		float          gma = gma_beg;

		/*** To do: optimize the following loop with SIMD ***/

		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const float    vol_c2 = car_ptr [pos];
			const float    vol_m2 = mod_ptr [pos];

			// We multiply the naive fraction vol_m / vol_c with
			// vol_c^2 / (vol_c^2 + thr^2) to reduce the gain down to 0 when the
			// carrier signal becomes really small (below thr). This is not
			// really mandatory but probably safer to avoid amplifying background
			// noise.
			const float    vol_c  = sqrtf (vol_c2);
			const float    vol_m  = sqrtf (vol_m2);
			const float    num_1  = vol_m;
			const float    den_1  = fstb::lerp (_vol_ref, vol_c, m_c);
			const float    num_2  = vol_c2;
			const float    den_2  = vol_c2 + fstb::sq (_thr_silence);
			const float    g_raw  = (num_1 * num_2) / (den_1 * den_2 + _eps);
			const float    g_lim  = fstb::limit (g_raw, gmi, gma);
			const float    g_out  = fstb::lerp (1.f, g_lim, str);

			car_ptr [pos] = g_out;

			m_c += m_c_stp;
			str += str_stp;
			gmi += gmi_stp;
			gma += gma_stp;
		}

		++ chn_env_idx;
	}

	// Applies the gain(s)
	chn_env_idx = chn_inf.second;
	const int      chn_env_idx_inc = (chn_inf.first) ? 1 : 0;
	for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
	{
		Channel &      chn_env = _chn_arr [chn_env_idx];
		dsp::mix::Align::mult_1_1 (
			proc._dst_arr [chn_idx],
			proc._src_arr [chn_idx],
			chn_env._buf_car.data (),
			nbr_spl
		);
		chn_env_idx += chn_env_idx_inc;
	}

	// Duplicates the remaining output channels
	for (int chn_idx = nbr_chn_proc; chn_idx < nbr_chn_dst; ++chn_idx)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_idx],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Makes the compiler happy
constexpr float	VolumeClone::_vol_ref;
constexpr float	VolumeClone::_thr_silence;
constexpr float	VolumeClone::_eps;



void	VolumeClone::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._ef_car.clear_buffers ();
		chn._ef_mod.clear_buffers ();
	}

	_mul_copy.clear_buffers ();
	_strength.clear_buffers ();
	_gain_min.clear_buffers ();
	_gain_max.clear_buffers ();
}



void	VolumeClone::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_mode (true) || force_flag)
		{
			const float    mul_copy =
				float (_state_set.get_val_end_nat (Param_MODE));
			const float    strength =
				float (_state_set.get_val_end_nat (Param_STRN));
			_chn_mode = _state_set.get_val_enum <ChnMode> (Param_CHN);
			_mul_copy.set_val (mul_copy);
			_strength.set_val (strength);
		}

		if (_param_change_flag_gain (true) || force_flag)
		{
			const float    gmi = float (_state_set.get_val_end_nat (Param_GMIN));
			const float    gma = float (_state_set.get_val_end_nat (Param_GMAX));
			_gain_min.set_val (gmi);
			_gain_max.set_val (gma);
		}

		if (_param_change_flag_env (true) || force_flag)
		{
			float          t_a = float (_state_set.get_val_end_nat (Param_ATK));
			float          t_r = float (_state_set.get_val_end_nat (Param_RLS));
			float          t_h = float (_state_set.get_val_end_nat (Param_HOLD));

			t_a = dsp::dyn::EnvHelper::compensate_order (t_a, _order_env);
			t_r = dsp::dyn::EnvHelper::compensate_order (t_r, _order_env);

			const float    c_a     =
				dsp::dyn::EnvHelper::compute_env_coef_simple (t_a, _sample_freq);
			const float    c_r     =
				dsp::dyn::EnvHelper::compute_env_coef_simple (t_r, _sample_freq);
			const int      t_h_spl = fstb::round_int (t_h * _sample_freq);

			for (auto &chn : _chn_arr)
			{
				chn._ef_car.set_atk_coef (c_a);
				chn._ef_car.set_rls_coef (c_r);
				chn._ef_car.set_hold_time (t_h_spl);
				chn._ef_mod.set_atk_coef (c_a);
				chn._ef_mod.set_rls_coef (c_r);
				chn._ef_mod.set_hold_time (t_h_spl);
			}
		}
	}
}



}  // namespace vclone
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
