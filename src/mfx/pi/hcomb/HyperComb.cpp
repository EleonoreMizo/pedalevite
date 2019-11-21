/*****************************************************************************

        HyperComb.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/hcomb/HyperComb.h"
#include "mfx/pi/hcomb/Param.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace hcomb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



HyperComb::HyperComb ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq ()
,	_inv_fs ()
,	_param_change_flag ()
,	_param_change_flag_comb ()
,	_param_change_flag_filt ()
,	_param_change_flag_misc ()
,	_tune (0)
,	_decay (1)
,	_volume (1)
,	_voice_arr ()
,	_buf_tmp ()
,	_buf_dst_arr ()
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_TUNE    , 0);
	_state_set.set_val_nat (desc_set, Param_TIME    , 1);
	_state_set.set_val_nat (desc_set, Param_VOLUME  , 1);
	_state_set.set_val_nat (desc_set, Param_FLT_FREQ, 4000);
	_state_set.set_val_nat (desc_set, Param_FLT_RESO, 0);
	_state_set.set_val_nat (desc_set, Param_FLT_DAMP, 0.5);

	_state_set.add_observer (Param_TUNE    , _param_change_flag_comb);
	_state_set.add_observer (Param_TIME    , _param_change_flag_comb);
	_state_set.add_observer (Param_VOLUME  , _param_change_flag_misc);
	_state_set.add_observer (Param_FLT_FREQ, _param_change_flag_filt);
	_state_set.add_observer (Param_FLT_RESO, _param_change_flag_filt);
	_state_set.add_observer (Param_FLT_DAMP, _param_change_flag_filt);

	_param_change_flag_comb.add_observer (_param_change_flag);
	_param_change_flag_filt.add_observer (_param_change_flag);
	_param_change_flag_misc.add_observer (_param_change_flag);

	static const std::array <float, Cst::_nbr_voices>  note_arr =
	{{ 48, 52, 55, 59, 60, 62, 67, 72 }};

	for (int index = 0; index < Cst::_nbr_voices; ++index)
	{
		VoicePack &    vp   = _voice_arr [index];
		const int      base = HyperCombDesc::get_base_voice (index);

		vp._active_flag = (index == 0);

		_state_set.set_val_nat (desc_set, base + ParamVoice_POLARITY , (vp._active_flag) ? 1 : 0);
		_state_set.set_val_nat (desc_set, base + ParamVoice_NOTE     , note_arr [index]);
		_state_set.set_val_nat (desc_set, base + ParamVoice_FINETUNE , 0);
		_state_set.set_val_nat (desc_set, base + ParamVoice_TIME_MULT, 1);
		_state_set.set_val_nat (desc_set, base + ParamVoice_GAIN     , 1);

		_state_set.add_observer (base + ParamVoice_POLARITY , vp._param_change_flag_comb);
		_state_set.add_observer (base + ParamVoice_NOTE     , vp._param_change_flag_comb);
		_state_set.add_observer (base + ParamVoice_FINETUNE , vp._param_change_flag_comb);
		_state_set.add_observer (base + ParamVoice_TIME_MULT, vp._param_change_flag_comb);
		_state_set.add_observer (base + ParamVoice_GAIN     , vp._param_change_flag_misc);

		vp._param_change_flag_comb.add_observer (vp._param_change_flag);
		vp._param_change_flag_misc.add_observer (vp._param_change_flag);

		vp._param_change_flag.add_observer (_param_change_flag);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	HyperComb::do_get_state () const
{
	return _state;
}



double	HyperComb::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	HyperComb::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_tmp.resize (mbs_align);
	for (auto &buf : _buf_dst_arr)
	{
		buf.resize (mbs_align);
	}

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_comb.set ();
	_param_change_flag_filt.set ();
	_param_change_flag_misc.set ();

	for (auto &vp : _voice_arr)
	{
		vp._voice.reset (sample_freq, max_buf_len, &_buf_tmp [0]);

		vp._param_change_flag_comb.set ();
		vp._param_change_flag_misc.set ();
		vp._vol_final.set_time (64, 1.0f / 64);
	}

	update_param (true);
	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	HyperComb::do_clean_quick ()
{
	clear_buffers ();
}



void	HyperComb::do_process_block (piapi::ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._nbr_chn_arr [piapi::Dir_IN ];
	const int      nbr_chn_dst = proc._nbr_chn_arr [piapi::Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

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

	const int      nbr_spl = proc._nbr_spl;

	// Parameters
	_state_set.process_block (nbr_spl);
	update_param (false);

	// Signal
	std::array <float *, _max_nbr_chn>  buf_arr;
	for (int chn = 0; chn < nbr_chn_proc; ++chn)
	{
		buf_arr [chn] = &_buf_dst_arr [chn] [0];
	}
	bool           mix_flag = false;
	for (int vc_cnt = 0; vc_cnt < Cst::_nbr_voices; ++vc_cnt)
	{
		VoicePack &    vp = _voice_arr [vc_cnt];
		if (vp._active_flag)
		{
			vp._voice.process_block (
				&buf_arr [0],
				proc._src_arr,
				nbr_spl,
				nbr_chn_proc
			);

			vp._vol_final.tick (nbr_spl);

			for (int chn = 0; chn < nbr_chn_proc; ++chn)
			{
				if (mix_flag)
				{
					dsp::mix::Align::mix_1_1_vlrauto (
						proc._dst_arr [chn],
						buf_arr [chn],
						nbr_spl,
						vp._vol_final.get_beg (),
						vp._vol_final.get_end ()
					);
				}
				else
				{
					dsp::mix::Align::copy_1_1_vlrauto (
						proc._dst_arr [chn],
						buf_arr [chn],
						nbr_spl,
						vp._vol_final.get_beg (),
						vp._vol_final.get_end ()
					);
				}
			}

			mix_flag = true;
		}
	}

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



void	HyperComb::clear_buffers ()
{
	for (auto &vp : _voice_arr)
	{
		vp._voice.clear_buffers ();
		vp._vol_final.clear_buffers ();
	}
}



void	HyperComb::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		bool           comb_glob_flag = false;
		bool           vol_glob_flag  = false;

		if (_param_change_flag_comb (true) || force_flag)
		{
			_tune  = float (_state_set.get_val_end_nat (Param_TUNE));
			_decay = float (_state_set.get_val_end_nat (Param_TIME));
			comb_glob_flag = true;
		}

		if (_param_change_flag_filt (true) || force_flag)
		{
			const float    freq = float (
				_state_set.get_val_end_nat (Param_FLT_FREQ)
			);
			const float    reso = float (
				_state_set.get_val_end_nat (Param_FLT_RESO)
			);
			const float    damp = float (
				_state_set.get_val_end_nat (Param_FLT_DAMP)
			);

			float          biq_b_z [3];
			float          biq_a_z [3];
			float          shf_b_z [2];
			float          shf_a_z [2];
			_voice_arr [0]._voice.compute_filt_param (
				freq, reso, damp,
				biq_b_z, biq_a_z,
				shf_b_z, shf_a_z
			);
			for (int index = 0; index < Cst::_nbr_voices; ++index)
			{
				VoicePack &    vp = _voice_arr [index];
				vp._voice.set_filt_param (
					freq, reso, damp,
					biq_b_z, biq_a_z,
					shf_b_z, shf_a_z
				);
			}
		}

		if (_param_change_flag_misc (true) || force_flag)
		{
			_volume = float (_state_set.get_val_end_nat (Param_VOLUME));
			vol_glob_flag = true;
		}

		for (int index = 0; index < Cst::_nbr_voices; ++index)
		{
			VoicePack &    vp   = _voice_arr [index];
			const int      base = HyperCombDesc::get_base_voice (index);
			bool           comb_voice_flag = false;
			bool           vol_voice_flag  = false;

			if (vp._param_change_flag (true) || force_flag)
			{
				if (vp._param_change_flag_comb (true) || force_flag)
				{
					vp._note     = float (
						_state_set.get_val_tgt_nat (base + ParamVoice_NOTE)
					);
					vp._finetune = float (
						_state_set.get_val_tgt_nat (base + ParamVoice_FINETUNE)
					);

					const int      pol = fstb::round_int (
						_state_set.get_val_tgt_nat (base + ParamVoice_POLARITY)
					);
					vp._active_flag = (pol > 0);
					vp._voice.set_polarity (pol == 2);

					vp._decay_mult = float (
						_state_set.get_val_tgt_nat (base + ParamVoice_TIME_MULT)
					);

					comb_voice_flag = true;
				}

				if (vp._param_change_flag_misc (true) || force_flag)
				{
					vp._vol = float (
						_state_set.get_val_tgt_nat (base + ParamVoice_GAIN)
					);
					vol_voice_flag = true;
				}
			}

			if (comb_glob_flag || comb_voice_flag)
			{
				const float    note_final = vp._note + vp._finetune + _tune;
				vp._voice.set_note (note_final);

				const float    decay_final = _decay * vp._decay_mult;
				vp._voice.set_decay_rt60 (decay_final);
			}

			if (vol_glob_flag || vol_voice_flag)
			{
				const float    vol_final = vp._vol * _volume;
				vp._vol_final.set_val (vol_final);
			}
		}
	}
}



}  // namespace hcomb
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
