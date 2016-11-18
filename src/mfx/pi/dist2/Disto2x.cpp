/*****************************************************************************

        Disto2x.cpp
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
#include "fstb/ToolsSimd.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dist2/Disto2x.h"
#include "mfx/pi/dist2/Param.h"
#include "mfx/piapi/EventTs.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace dist2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Disto2x::Disto2x ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_change_flag ()
,	_param_change_flag_stage_arr ()
,	_param_change_flag_other ()
,	_sample_freq (44100)
,	_inv_fs (1 / _sample_freq)
,	_proc ()
,	_gmod_atk (1)
,	_gmod_sus (1)
,	_gmod_atk_l2 (0)
,	_gmod_sus_l2 (0)
,	_gmod_atk_max_l2 (0)
,	_gmod_sus_max_l2 (0)
,	_mix_s12_cur (1)
,	_mix_s12_old (1)
,	_mix_lb_cur (0)
,	_mix_lb_old (0)
,	_freq_lpf_pre (30)
,	_buf_trans_atk ()
,	_buf_trans_sus ()
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_XOVER  ,    30);
	_state_set.set_val_nat (desc_set, Param_PRE_LPF, 20480);
	_state_set.set_val_nat (desc_set, Param_DYN_ATK,     1);
	_state_set.set_val_nat (desc_set, Param_DYN_RLS,     1);
	_state_set.set_val_nat (desc_set, Param_S12_MIX,     1);
	_state_set.set_val_nat (desc_set, Param_LB_MIX ,     0);

	_state_set.add_observer (Param_XOVER  , _param_change_flag_other);
	_state_set.add_observer (Param_PRE_LPF, _param_change_flag_other);
	_state_set.add_observer (Param_DYN_ATK, _param_change_flag_other);
	_state_set.add_observer (Param_DYN_RLS, _param_change_flag_other);
	_state_set.add_observer (Param_S12_MIX, _param_change_flag_other);
	_state_set.add_observer (Param_LB_MIX , _param_change_flag_other);

	_state_set.set_ramp_time (Param_DYN_ATK, 0.010);
	_state_set.set_ramp_time (Param_DYN_RLS, 0.010);
	_state_set.set_ramp_time (Param_S12_MIX, 0.010);
	_state_set.set_ramp_time (Param_LB_MIX , 0.010);

	for (int stage = 0; stage < _nbr_stages; ++stage)
	{
		const int      base = _param_stage_base_arr [stage];
		_state_set.set_val_nat (desc_set, base + ParamStage_HPF_PRE ,    30);
		_state_set.set_val_nat (desc_set, base + ParamStage_BIAS    ,     0);
		_state_set.set_val_nat (desc_set, base + ParamStage_TYPE    , DistoStage::Type_DIODE_CLIPPER);
		_state_set.set_val_nat (desc_set, base + ParamStage_GAIN    ,     8);
		_state_set.set_val_nat (desc_set, base + ParamStage_LPF_POST, 20480);

		for (int index = 0; index < ParamStage_NBR_ELT; ++index)
		{
			_state_set.add_observer (
				base + index,
				_param_change_flag_stage_arr [stage]
			);
		}

		_state_set.set_ramp_time (base + ParamStage_BIAS, 0.010);
		_state_set.set_ramp_time (base + ParamStage_GAIN, 0.010);

		_param_change_flag_stage_arr [stage].add_observer (_param_change_flag);
	}

	_param_change_flag_other.add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Disto2x::do_get_state () const
{
	return _state;
}



double	Disto2x::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Disto2x::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1 / sample_freq);
	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_align = (max_buf_len + 3) & -4;
	_buf_trans_atk.resize (mbs_align);
	_buf_trans_sus.resize (mbs_align);
	for (auto &chn : _proc->_chn_arr)
	{
		for (auto &buf : chn._buf_xover_arr)
		{
			buf.resize (mbs_align);
		}
		for (auto &buf : chn._buf_stage_arr)
		{
			buf.resize (mbs_align);
		}
	}

	_proc->_trans_ana.reset (sample_freq, max_buf_len);
	_proc->_freq_split.set_sample_freq (sample_freq);
	for (auto &stage : _proc->_stage_arr)
	{
		stage.reset (sample_freq, max_buf_len);
		stage.set_bias_freq (500);
	}

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	Disto2x::do_process_block (ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._nbr_chn_arr [Dir_IN ];
	const int      nbr_chn_dst = proc._nbr_chn_arr [Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);

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

	const int      nbr_spl = proc._nbr_spl;

	// Parameters
	_state_set.process_block (nbr_spl);
	update_param ();

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Audio processing

	// Transient analysis
	_proc->_trans_ana.process_block (
		&_buf_trans_atk [0],
		&_buf_trans_sus [0],
		proc._src_arr,
		nbr_chn_src,
		nbr_spl
	);

	// 0 = LPF input / stage mix,
	// 1 = stage 1 output,
	// 2 = stage 2 output
	std::array <std::array <float *, _max_nbr_chn>, 3> stage_io_arr;

	// Frequency splitting, input LPF and transcient processing
	for (int chn = 0; chn < nbr_chn_src; ++chn)
	{
		stage_io_arr [0] [chn] = &_proc->_chn_arr [chn]._buf_xover_arr [1] [0];
		stage_io_arr [1] [chn] = &_proc->_chn_arr [chn]._buf_stage_arr [0] [0];
		stage_io_arr [2] [chn] = &_proc->_chn_arr [chn]._buf_stage_arr [1] [0];

		_proc->_freq_split.process_block (
			chn,
			&_proc->_chn_arr [chn]._buf_xover_arr [0] [0],
			stage_io_arr [0] [chn],
			proc._src_arr [chn],
			nbr_spl
		);
		_proc->_chn_arr [chn]._lpf_pre.process_block (
			stage_io_arr [0] [chn],
			stage_io_arr [0] [chn],
			nbr_spl
		);

		// Transient processing
		// x *= exp2 (
		//         clip (log2 (tr_atk)) * log2 (atk)
		//       + clip (log2 (tr_sus)) * log2 (sus)
		//      )
		const auto     atk_l2     = fstb::ToolsSimd::set1_f32 (_gmod_atk_l2);
		const auto     sus_l2     = fstb::ToolsSimd::set1_f32 (_gmod_sus_l2);
		const auto     atk_max_l2 = fstb::ToolsSimd::set1_f32 (_gmod_atk_max_l2);
		const auto     sus_max_l2 = fstb::ToolsSimd::set1_f32 (_gmod_sus_max_l2);
		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			auto           x =
				fstb::ToolsSimd::load_f32 (&stage_io_arr [0] [chn] [pos]);
			auto           tra_l2 =
				fstb::ToolsSimd::load_f32 (&_buf_trans_atk [pos]);
			auto           trs_l2 =
				fstb::ToolsSimd::load_f32 (&_buf_trans_sus [pos]);
			tra_l2 = fstb::ToolsSimd::min_f32 (tra_l2, atk_max_l2);
			trs_l2 = fstb::ToolsSimd::min_f32 (trs_l2, sus_max_l2);
			const auto     mul_l2 = tra_l2 * atk_l2 + trs_l2 * sus_l2;
			const auto     mul    = fstb::ToolsSimd::exp2_approx (mul_l2);
			x *= mul;
			fstb::ToolsSimd::store_f32 (&stage_io_arr [0] [chn] [pos], x);
		}
	}

	// Distortion stages 1 and 2
	_proc->_stage_arr [0].process_block (
		&stage_io_arr [1] [0],
		&stage_io_arr [0] [0],
		nbr_spl,
		nbr_chn_src
	);

	_proc->_stage_arr [1].process_block (
		&stage_io_arr [2] [0],
		&stage_io_arr [1] [0],
		nbr_spl,
		nbr_chn_src
	);

	// Mixing
	for (int chn = 0; chn < nbr_chn_src; ++chn)
	{
		dsp::mix::Align::copy_xfade_2_1_vlrauto (
			stage_io_arr [0] [chn],
			stage_io_arr [1] [chn],
			stage_io_arr [2] [chn],
			nbr_spl,
			_mix_s12_old,
			_mix_s12_cur
		);

		dsp::mix::Align::copy_xfade_2_1_vlrauto (
			stage_io_arr [0] [chn],
			stage_io_arr [0] [chn],
			&_proc->_chn_arr [chn]._buf_xover_arr [0] [0],
			nbr_spl,
			_mix_lb_cur,
			_mix_lb_old
		);

		dsp::mix::Align::copy_1_1 (
			&proc._dst_arr [chn] [0],
			stage_io_arr [0] [chn],
			nbr_spl
		);
	}

	for (int chn = nbr_chn_src; chn < nbr_chn_dst; ++chn)
	{
		const float *  src_ptr = &proc._dst_arr [0  ] [0];
		float *        dst_ptr = &proc._dst_arr [chn] [0];
		memcpy (dst_ptr, src_ptr, sizeof (*dst_ptr) * nbr_spl);
	}

	set_next_buffer ();
}



void	Disto2x::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_other (true) || force_flag)
		{
			const float    freq_xover =
				float (_state_set.get_val_tgt_nat (Param_XOVER));
			_proc->_freq_split.set_split_freq (freq_xover);

			_freq_lpf_pre =
				float (_state_set.get_val_tgt_nat (Param_PRE_LPF));
			update_lpf_pre ();

			_gmod_atk        =
				float (_state_set.get_val_end_nat (Param_DYN_ATK));
			_gmod_atk_l2     = fstb::Approx::log2 (_gmod_atk);
			_gmod_atk_max_l2 = std::max (_gmod_atk_l2, 3.0f);

			_gmod_sus        =
				float (_state_set.get_val_end_nat (Param_DYN_RLS));
			_gmod_sus_l2     = fstb::Approx::log2 (_gmod_sus);
			_gmod_sus_max_l2 = std::max (_gmod_sus_l2, 3.0f);

			_mix_s12_cur =
				float (_state_set.get_val_end_nat (Param_S12_MIX));
			_mix_lb_cur  =
				float (_state_set.get_val_end_nat (Param_LB_MIX));
		}

		for (int stage_cnt = 0; stage_cnt < _nbr_stages; ++stage_cnt)
		{
			if (_param_change_flag_stage_arr [stage_cnt] (true) || force_flag)
			{
				const int      base     = _param_stage_base_arr [stage_cnt];
				DistoStage &   stage    = _proc->_stage_arr [stage_cnt];

				const float    freq_hpf =
					float (_state_set.get_val_tgt_nat (base + ParamStage_HPF_PRE ));
				const float    bias     =
					float (_state_set.get_val_end_nat (base + ParamStage_BIAS    ));
				const int      type     = fstb::round_int (
					_state_set.get_val_tgt_nat (base + ParamStage_TYPE)
				);
				const float    gain     =
					float (_state_set.get_val_end_nat (base + ParamStage_GAIN    ));
				const float    freq_lpf =
					float (_state_set.get_val_tgt_nat (base + ParamStage_LPF_POST));

				stage.set_bias (bias);
				stage.set_type (static_cast <DistoStage::Type> (type));
				stage.set_hpf_pre_cutoff (freq_hpf);
				stage.set_lpf_post_cutoff (freq_lpf);

				const float    gain_post = 0.125f;
				stage.set_gain_pre (gain * (1 / gain_post));
				stage.set_gain_post (           gain_post );
			}
		}
	}
}



void	Disto2x::clear_buffers ()
{
	for (auto &chn : _proc->_chn_arr)
	{
		chn._lpf_pre.clear_buffers ();
	}
	_proc->_trans_ana.clear_buffers ();
	_proc->_freq_split.clear_buffers ();
	for (auto &stage : _proc->_stage_arr)
	{
		stage.clear_buffers ();
	}

	set_next_buffer ();
}



void	Disto2x::set_next_buffer ()
{
	_mix_s12_old = _mix_s12_cur;
	_mix_lb_old  = _mix_lb_cur;
}



void	Disto2x::update_lpf_pre ()
{
	static const float   b_s [3] = { 1,                   0, 0 };
	static const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float    k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_lpf_pre * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _proc->_chn_arr)
	{
		chn._lpf_pre.set_z_eq (b_z, a_z);
	}
}



const std::array <int, Disto2x::_nbr_stages>	Disto2x::_param_stage_base_arr =
{{
	Param_S1_BASE,
	Param_S2_BASE
}};



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
