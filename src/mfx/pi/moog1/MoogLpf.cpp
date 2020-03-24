/*****************************************************************************

        MoogLpf.cpp
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
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/moog1/Param.h"
#include "mfx/pi/moog1/MoogLpf.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace moog1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MoogLpf::MoogLpf ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_type ()
,	_param_change_flag_param ()
,	_param_change_flag_mod ()
,	_chn_arr (_max_nbr_chn)
,	_flt_mode (FltMode_LP4)
,	_mod_sc_amp (0)
,	_mod_self_amp (0)
,	_mod_sc_flag (false)
,	_mod_self_flag (false)
,	_buf_arr ()
,	_stage_weight_arr ({{ 0, 0, 0, 0, 1 }})
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_MODE     , FltMode_LP4);
	_state_set.set_val_nat (desc_set, Param_CUTOFF   , 20480);
	_state_set.set_val_nat (desc_set, Param_RESO     , 0);
	_state_set.set_val_nat (desc_set, Param_GCOMP    , 0.5);
	_state_set.set_val_nat (desc_set, Param_SIDEC_ENA, 0);
	_state_set.set_val_nat (desc_set, Param_SIDEC_AMP, 0);
	_state_set.set_val_nat (desc_set, Param_SELF_ENA , 0);
	_state_set.set_val_nat (desc_set, Param_SELF_AMP , 0);
	_state_set.set_val_nat (desc_set, Param_SELF_LPF , 20480);

	_state_set.add_observer (Param_MODE     , _param_change_flag_type);
	_state_set.add_observer (Param_CUTOFF   , _param_change_flag_param);
	_state_set.add_observer (Param_RESO     , _param_change_flag_param);
	_state_set.add_observer (Param_GCOMP    , _param_change_flag_param);
	_state_set.add_observer (Param_SIDEC_ENA, _param_change_flag_mod);
	_state_set.add_observer (Param_SIDEC_AMP, _param_change_flag_mod);
	_state_set.add_observer (Param_SELF_ENA , _param_change_flag_mod);
	_state_set.add_observer (Param_SELF_AMP , _param_change_flag_mod);

	_param_change_flag_type.add_observer (_param_change_flag);
	_param_change_flag_param.add_observer (_param_change_flag);
	_param_change_flag_mod.add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_SIDEC_AMP, 0.010);
	_state_set.set_ramp_time (Param_SELF_AMP , 0.010);

	init_ovrspl_coef ();
	for (auto &chn : _chn_arr)
	{
		chn._flt.set_scale (_sig_scale);
		chn._upspl_m.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._upspl_s.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._dwspl.set_coefs (&_coef_42 [0], &_coef_21 [0]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	MoogLpf::do_get_state () const
{
	return _state;
}



double	MoogLpf::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	MoogLpf::do_reset (double sample_freq, int /* max_buf_len */, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	const float    fs_ovr  = float (sample_freq * _ovrspl);
	_inv_fs_ovr  = float (1 / (sample_freq * _ovrspl));

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &buf : _buf_arr)
	{
		buf.resize (_max_blk_size * _ovrspl);
		dsp::mix::Align::clear (buf.data (), int (buf.size ()));
	}

	for (auto &chn : _chn_arr)
	{
		chn._flt.set_sample_freq (fs_ovr);
		chn._flt.set_max_mod_freq (_sample_freq * 0.6f);
		chn._dckill.set_sample_freq (fs_ovr);
		chn._dckill.set_cutoff_freq (5);
	}

	_param_change_flag_type.set ();
	_param_change_flag_param.set ();
	_param_change_flag_mod.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	MoogLpf::do_process_block (piapi::ProcInfo &proc)
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

	const float    time_step = fstb::rcp_uint <float> (proc._nbr_spl);
	_mod_sc_amp.set_time (proc._nbr_spl, time_step);
	_mod_self_amp.set_time (proc._nbr_spl, time_step);

	// Signal processing
	int            pos_blk = 0;
	do
	{
		const int      len_blk =
			std::min (proc._nbr_spl - pos_blk, int (_max_blk_size));
		const int      len_blk_ovr = len_blk * _ovrspl;

		_mod_sc_amp.tick (len_blk);
		_mod_self_amp.tick (len_blk);

		for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
		{
			Channel &      chn = _chn_arr [chn_index];

			// Upsamples incoming signals
			chn._upspl_m.process_block (
				_buf_arr [0].data (),
				&proc._src_arr [chn_index] [pos_blk],
				len_blk
			);
			if (_mod_sc_flag)
			{
				chn._upspl_s.process_block (
					_buf_arr [1].data (),
					&proc._src_arr [nbr_chn_src + chn_index] [pos_blk],
					len_blk
				);
				mfx::dsp::mix::Align::scale_1_vlrauto (
					_buf_arr [1].data (),
					len_blk_ovr,
					_mod_sc_amp.get_beg (),
					_mod_sc_amp.get_end ()
				);
			}

			// Actual processing
			if (! _mod_self_flag && _flt_mode == FltMode_LP4)
			{
				// Optimised processing for this specific case
				process_subblock_lp4_no_self (chn, len_blk_ovr);
			}
			else
			{
				process_subblock_standard (chn, len_blk_ovr);
			}

			// Downsamples final signal
			chn._dwspl.process_block (
				&proc._dst_arr [chn_index] [pos_blk],
				_buf_arr [0].data (),
				len_blk
			);
		}

		pos_blk += len_blk;
	}
	while (pos_blk < proc._nbr_spl);

	// Duplicates the remaining output channels
	for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MoogLpf::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._self_mod_save = 0;
		chn._upspl_m.clear_buffers ();
		chn._upspl_s.clear_buffers ();
		chn._dwspl.clear_buffers ();
		chn._flt.clear_buffers ();
		chn._dckill.clear_buffers ();
		for (auto &biq : chn._lpf_selfmod_arr)
		{
			biq.clear_buffers ();
		}
	}

	_mod_sc_amp.clear_buffers ();
	_mod_self_amp.clear_buffers ();
}



void	MoogLpf::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_type (true) || force_flag)
		{
			_flt_mode = static_cast <FltMode> (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_MODE)
			));
			set_stage_weights (_flt_mode);
		}

		if (_param_change_flag_param (true) || force_flag)
		{
			const float    cutoff_freq =
				float (_state_set.get_val_end_nat (Param_CUTOFF));
			const float    reso =
				float (_state_set.get_val_end_nat (Param_RESO));
			const float    gcomp =
				float (_state_set.get_val_end_nat (Param_GCOMP));
			for (auto &chn : _chn_arr)
			{
				chn._flt.set_freq_compensated (cutoff_freq);
				chn._flt.set_reso_norm (reso);
				chn._flt.set_gain_comp (gcomp);
			}
		}

		if (_param_change_flag_mod (true) || force_flag)
		{
			_mod_sc_flag =
				(float (_state_set.get_val_end_nat (Param_SIDEC_ENA)) >= 0.5f);
			const float    sc_amp =
				float (_state_set.get_val_end_nat (Param_SIDEC_AMP));
			_mod_sc_amp.set_val (sc_amp * (1.0f / _sig_scale));

			_mod_self_flag =
				(float (_state_set.get_val_end_nat (Param_SELF_ENA)) >= 0.5f);
			if (! _mod_self_flag)
			{
				for (auto &chn : _chn_arr)
				{
					chn._self_mod_save = 0;
				}
			}
			const float    self_amp =
				float (_state_set.get_val_end_nat (Param_SELF_AMP));
			_mod_self_amp.set_val (self_amp * (1.0f / _sig_scale));

			const float    self_lpf_freq =
				float (_state_set.get_val_end_nat (Param_SELF_LPF));
			update_lpf_selfmod (self_lpf_freq);
		}
	}
}



void	MoogLpf::update_lpf_selfmod (float f)
{
	static const float   b_s [3]  = { 0,                   0, 1 };
	static const float   a_s [3]  = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float    k =
		dsp::iir::TransSZBilin::compute_k_approx (f * _inv_fs_ovr);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z, b_s, a_s, k
	);
	for (auto &chn : _chn_arr)
	{
		for (auto &biq : chn._lpf_selfmod_arr)
		{
			biq.set_z_eq (b_z, a_z);
		}
	}
}



// nbr_spl = oversampled data
void	MoogLpf::process_subblock_lp4_no_self (Channel &chn, int nbr_spl)
{
	float *        sig_ptr = _buf_arr [0].data ();

	// With sidechain frequency modulation
	if (_mod_sc_flag)
	{
		const float *  mod_ptr = _buf_arr [1].data ();
		chn._flt.process_block_pitch_mod (sig_ptr, sig_ptr, mod_ptr, nbr_spl);
	}

	// No sidechain
	else
	{
		chn._flt.process_block (sig_ptr, sig_ptr, nbr_spl);
	}

	chn._dckill.process_block (sig_ptr, sig_ptr, nbr_spl);
}



// nbr_spl = oversampled data
void	MoogLpf::process_subblock_standard (Channel &chn, int nbr_spl)
{
	float          self_vol      = 0;
	float          self_vol_step = 0;
	if (_mod_self_flag)
	{
		self_vol      = _mod_self_amp.get_beg ();
		self_vol_step = _mod_self_amp.get_step () * (1.f / _ovrspl);
	}

	WeightList     out_arr;
	float *        sig_ptr = _buf_arr [0].data ();
	const float *  mod_ptr = _buf_arr [1].data ();
	float          val_old = chn._self_mod_save;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		// Self-modulation
		float          sm_val  = val_old * self_vol;
		for (auto &biq : chn._lpf_selfmod_arr)
		{
			sm_val = biq.process_sample (sm_val);
		}
		sm_val   = fstb::limit (sm_val, -1.f, +1.f); // +/-1 octave max

		// Side-chain modulation
		const float    sc_val  = (_mod_sc_flag) ? mod_ptr [pos] : 0;

		// All modulations summed together
		const float    mod_val = sc_val + sm_val;

		// Main filter processing
		float          val = chn._flt.process_sample_pitch_mod (
			sig_ptr [pos], mod_val, out_arr.data ()
		);

		// Weighted sum of the stages to get the desired filter mode
		val *= _stage_weight_arr [4];
		for (int s_cnt = 0; s_cnt < 4; ++ s_cnt)
		{
			val += out_arr [s_cnt] * _stage_weight_arr [s_cnt];
		}

		// DC offset removal
		val = chn._dckill.process_sample (val);

		sig_ptr [pos] = val;

		self_vol += self_vol_step;
		val_old   = val;
	}

	chn._self_mod_save = val_old;
}



void	MoogLpf::set_stage_weights (FltMode flt_mode)
{
	assert (flt_mode >= 0);
	assert (flt_mode < FltMode_NBR_ELT);

	const WeightList &   src_list = _weight_table [flt_mode];
	for (int pos = 0; pos < _nbr_weights; ++pos)
	{
		_stage_weight_arr [pos] = src_list [pos];
	}
}



void	MoogLpf::init_ovrspl_coef ()
{
	if (! _coef_init_flag)
	{
		const double   tbw = 0.01;
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_42 [0], _nbr_coef_42, tbw * 0.5 + 0.25
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_21 [0], _nbr_coef_21, tbw
		);

		_coef_init_flag = true;
	}
}



bool	MoogLpf::_coef_init_flag = false;
std::array <double, MoogLpf::_nbr_coef_42>	MoogLpf::_coef_42;
std::array <double, MoogLpf::_nbr_coef_21>	MoogLpf::_coef_21;

// Signal nominal level
const float	MoogLpf::_sig_scale = 0.125f;

const std::array <MoogLpf::WeightList, FltMode_NBR_ELT>	MoogLpf::_weight_table =
{{
	{{ 0,  0,  0,  0,  1 }},
	{{ 0,  0,  1,  0,  0 }},
	{{ 0,  0,  4, -8,  4 }},
	{{ 0,  2, -2,  0,  0 }},
	{{ 1, -4,  6, -4,  1 }},
	{{ 1, -2,  1,  0,  0 }}
}};


}  // namespace moog1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
