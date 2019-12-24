/*****************************************************************************

        Phaser.cpp
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

#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/pi/phase1/Param.h"
#include "mfx/pi/phase1/Phaser.h"
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
namespace phase1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Phaser::Phaser ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_flag_osc ()
,	_param_change_flag_fdbk ()
,	_param_change_flag_bpf ()
,	_param_change_flag_mix ()
,	_param_change_flag_set ()
,	_param_change_flag_apd ()
,	_phased_voice_arr (PhasedVoice::_nbr_chn_out)
,	_tmp_buf ()
,	_tmp_buf_pv ()
,	_mbl_align (0)
,	_phase_mix_cur (0.5f)
,	_phase_mix_old (0.5f)
,	_mono_mix_flag (false)
,	_stereo_out (StereoOut_SPAT_MIX)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SPEED     , 2);
	_state_set.set_val_nat (desc_set, Param_DEPTH     , 0);
	_state_set.set_val_nat (desc_set, Param_FDBK_LEVEL, 0);
	_state_set.set_val_nat (desc_set, Param_FDBK_COLOR, 0);
	_state_set.set_val_nat (desc_set, Param_PHASE_MIX , 0.5);
	_state_set.set_val_nat (desc_set, Param_MANUAL    , 0);
	_state_set.set_val_nat (desc_set, Param_PHASE_SET , 0);
	_state_set.set_val_nat (desc_set, Param_HOLD      , 0);
	_state_set.set_val_nat (desc_set, Param_BPF_CUTOFF, 640);
	_state_set.set_val_nat (desc_set, Param_BPF_Q     , 0.1);
	_state_set.set_val_nat (desc_set, Param_DIR       , 0);
	_state_set.set_val_nat (desc_set, Param_OP_MONO   , 0);
	_state_set.set_val_nat (desc_set, Param_OP_STEREO , StereoOut_SPAT_MIX);
	_state_set.set_val_nat (desc_set, Param_AP_DELAY  , 0);
	_state_set.set_val_nat (desc_set, Param_AP_COEF   , -0.5);

	_state_set.add_observer (Param_SPEED     , _param_change_flag_osc);
	_state_set.add_observer (Param_DEPTH     , _param_change_flag_osc);
	_state_set.add_observer (Param_FDBK_LEVEL, _param_change_flag_fdbk);
	_state_set.add_observer (Param_FDBK_COLOR, _param_change_flag_fdbk);
	_state_set.add_observer (Param_PHASE_MIX , _param_change_flag_mix);
	_state_set.add_observer (Param_MANUAL    , _param_change_flag_osc);
	_state_set.add_observer (Param_PHASE_SET , _param_change_flag_set);
	_state_set.add_observer (Param_HOLD      , _param_change_flag_osc);
	_state_set.add_observer (Param_BPF_CUTOFF, _param_change_flag_bpf);
	_state_set.add_observer (Param_BPF_Q     , _param_change_flag_bpf);
	_state_set.add_observer (Param_DIR       , _param_change_flag_osc);
	_state_set.add_observer (Param_OP_MONO   , _param_change_flag_mix);
	_state_set.add_observer (Param_OP_STEREO , _param_change_flag_mix);
	_state_set.add_observer (Param_AP_DELAY  , _param_change_flag_apd);
	_state_set.add_observer (Param_AP_COEF   , _param_change_flag_apd);

	_param_change_flag_osc .add_observer (_param_change_flag);
	_param_change_flag_fdbk.add_observer (_param_change_flag);
	_param_change_flag_bpf .add_observer (_param_change_flag);
	_param_change_flag_mix .add_observer (_param_change_flag);
	_param_change_flag_set .add_observer (_param_change_flag);
	_param_change_flag_apd .add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Phaser::do_get_state () const
{
	return _state;
}



double	Phaser::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Phaser::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_osc .set ();
	_param_change_flag_fdbk.set ();
	_param_change_flag_bpf .set ();
	_param_change_flag_mix .set ();
	_param_change_flag_set .set ();
	_param_change_flag_apd .set ();

	_mbl_align = (max_buf_len + 3) & -4;
	_tmp_buf.resize (_mbl_align * Buf_NBR_ELT);
	_tmp_buf_pv.resize (_mbl_align * PhasedVoice::Buf_NBR_ELT);

	for (auto &voice : _phased_voice_arr)
	{
		voice.reset (_sample_freq, max_buf_len, &_tmp_buf_pv [0]);
	}

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Phaser::do_clean_quick ()
{
	clear_buffers ();
}



void	Phaser::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
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

	int            pos = 0;
	do
	{
		// We need this intermediate varaible because for some reason GCC
		// fails to link when _update_resol is directly used in std::min.
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		_state_set.process_block (work_len);
		update_param ();

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Signal processing

		// Mono input for the phased signal
		const float *  src_ptr = proc._src_arr [0] + pos;
		if (nbr_chn_src > 1)
		{
			assert (nbr_chn_dst > 1);
			if (_stereo_out == StereoOut_SPAT_MIX)
			{
				dsp::mix::Simd <
					fstb::DataAlign <true>,
					fstb::DataAlign <false>
				>::copy_2_1_v (
					&_tmp_buf [_mbl_align * Buf_SRC],
					proc._src_arr [0] + pos,
					proc._src_arr [1] + pos,
					work_len, 0.5f
				);
				src_ptr = &_tmp_buf [_mbl_align * Buf_SRC];
			}
		}

		// Input mix
		int            chn_in     = 0;
		int            chn_in_inc = (nbr_chn_src >= nbr_chn_dst) ? 1 : 0;
		for (int chn_cnt = 0; chn_cnt < nbr_chn_dst; ++chn_cnt)
		{
			dsp::mix::Simd <
				fstb::DataAlign <false>,
				fstb::DataAlign <false>
			>::copy_1_1_vlrauto (
				proc._dst_arr [chn_cnt] + pos,
				proc._src_arr [chn_in ] + pos,
				work_len,
				1 - _phase_mix_old,
				1 - _phase_mix_cur
			);
			chn_in += chn_in_inc;
		}

		const int      nbr_pv =
			  (nbr_chn_src > 1 && _stereo_out != StereoOut_SPAT_MIX)
			? PhasedVoice::_nbr_chn_out
			: 1;

		float *        trash_ptr = &_tmp_buf [_mbl_align * Buf_TRASH];
		for (int pv_cnt = 0; pv_cnt < nbr_pv; ++pv_cnt)
		{
			if (pv_cnt > 0)
			{
				assert (nbr_chn_src > 1);
				src_ptr = proc._src_arr [pv_cnt] + pos;
			}

			// Phasing stage
			float *        dst_ptr_arr [PhasedVoice::_nbr_chn_out] =
			{
				&_tmp_buf [_mbl_align * Buf_PH_L],
				&_tmp_buf [_mbl_align * Buf_PH_R]
			};
			if (   (nbr_chn_dst == 1 && ! _mono_mix_flag)
			    || (nbr_chn_src >  1 && _stereo_out == StereoOut_SPAT_SEP))
			{
				dst_ptr_arr [PhasedVoice::_nbr_chn_out - 1 - pv_cnt] = trash_ptr;
			}
			else if (nbr_chn_dst > 1 && _stereo_out == StereoOut_BIMONO && ! _mono_mix_flag)
			{
				dst_ptr_arr [1] = trash_ptr;
			}

			_phased_voice_arr [pv_cnt].process_block (
				dst_ptr_arr, src_ptr, work_len
			);

			// Phase mix
			int            chn_src     = 0;
			int            chn_src_inc = 1;
			if (nbr_chn_dst > 1 && _stereo_out == StereoOut_BIMONO && ! _mono_mix_flag)
			{
				chn_src_inc = 0;
			}
			int            chn_out     = 0;
			int            chn_out_inc =
				(PhasedVoice::_nbr_chn_out <= nbr_chn_dst) ? 1 : 0;
			if (nbr_chn_src > 1 && _stereo_out != StereoOut_SPAT_MIX)
			{
				chn_out     = pv_cnt;
				chn_out_inc = 0;
			}
			for (int chn_cnt = 0; chn_cnt < PhasedVoice::_nbr_chn_out; ++chn_cnt)
			{
				if (dst_ptr_arr [chn_src] != trash_ptr)
				{
					dsp::mix::Simd <
						fstb::DataAlign <true>,
						fstb::DataAlign <false>
					>::mix_1_1_vlrauto (
						proc._dst_arr [chn_out] + pos,
						dst_ptr_arr [chn_src],
						work_len,
						_phase_mix_old,
						_phase_mix_cur
					);
				}
				chn_src += chn_src_inc;
				chn_out += chn_out_inc;
			}
		}

		_phase_mix_old = _phase_mix_cur;

		pos += work_len;
	}
	while (pos < proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Phaser::clear_buffers ()
{
	for (auto &voice : _phased_voice_arr)
	{
		voice.clear_buffers ();
	}
	_phase_mix_old = _phase_mix_cur;
}



void	Phaser::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_osc (true) || force_flag)
		{
			const float    speed = float (_state_set.get_val_end_nat (Param_SPEED));
			const int      depth =
				fstb::round_int (_state_set.get_val_tgt_nat (Param_DEPTH));
			const float    manual = float (_state_set.get_val_end_nat (Param_MANUAL));
			const bool     hold_flag =
				(_state_set.get_val_tgt_nat (Param_HOLD) >= 0.5f);
			const bool     neg_flag  =
				(_state_set.get_val_tgt_nat (Param_DIR) >= 0.5f);
			
			for (auto &voice : _phased_voice_arr)
			{
				voice.set_speed (speed);
				voice.set_depth (depth);
				voice.set_phase_shift (manual);
				voice.set_hold (hold_flag);
				voice.set_polarity (neg_flag);
			}
		}

		if (_param_change_flag_fdbk (true) || force_flag)
		{
			const float    level =
				float (_state_set.get_val_end_nat (Param_FDBK_LEVEL));
			const float    color =
				float (_state_set.get_val_end_nat (Param_FDBK_COLOR));

			for (auto &voice : _phased_voice_arr)
			{
				voice.set_fdbk_level (level);
				voice.set_fdbk_color (color);
			}
		}

		if (_param_change_flag_bpf (true) || force_flag)
		{
			const float    freq =
				float (_state_set.get_val_end_nat (Param_BPF_CUTOFF));
			const float    q = float (_state_set.get_val_end_nat (Param_BPF_Q));

			for (auto &voice : _phased_voice_arr)
			{
				voice.set_bpf_cutoff (freq);
				voice.set_bpf_q (q);
			}
		}

		if (_param_change_flag_mix (true) || force_flag)
		{
			_phase_mix_cur = float (_state_set.get_val_end_nat (Param_PHASE_MIX));
			_mono_mix_flag = (_state_set.get_val_tgt_nat (Param_OP_MONO) >= 0.5f);
			_stereo_out    = static_cast <StereoOut> (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_OP_STEREO)
			));
		}

		if (_param_change_flag_set (true) || force_flag)
		{
			const float    phase =
				float (_state_set.get_val_end_nat (Param_PHASE_SET));
			for (auto &voice : _phased_voice_arr)
			{
				voice.set_phase (phase);
			}
		}

		if (_param_change_flag_apd (true) || force_flag)
		{
			const float    dly  =
				float (_state_set.get_val_end_nat (Param_AP_DELAY));
			const float    coef =
				float (_state_set.get_val_end_nat (Param_AP_COEF));
			for (auto &voice : _phased_voice_arr)
			{
				voice.set_ap_delay (dly);
				voice.set_ap_coef (coef);
			}
		}
	}
}



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
