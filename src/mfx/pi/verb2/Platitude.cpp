/*****************************************************************************

        Platitude.cpp
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
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/verb2/Cst.h"
#include "mfx/pi/verb2/Param.h"
#include "mfx/pi/verb2/Platitude.h"
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
namespace verb2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Platitude::Platitude ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_lvl ()
,	_param_change_flag_early ()
,	_param_change_flag_dif_inp ()
,	_param_change_flag_dif_tnk ()
,	_param_change_flag_tank ()
,	_param_change_flag_line ()
,	_chn_arr ()
,	_lvl_dry (1)
,	_lvl_early (0)
,	_lvl_diffuse (0)
,	_flush_flag (false)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_LVL_DRY    ,     1.0);
	_state_set.set_val_nat (desc_set, Param_LVL_EARLY  ,     sqrt (0.1));
	_state_set.set_val_nat (desc_set, Param_LVL_DFS    ,     0.1);
	_state_set.set_val_nat (desc_set, Param_PREDELAY   ,     0.025);
	_state_set.set_val_nat (desc_set, Param_EARLY_NBR  ,    16);
	_state_set.set_val_nat (desc_set, Param_EARLY_DUR  ,     0.25);
	_state_set.set_val_nat (desc_set, Param_EARLY_DCY  ,     0.1);
	_state_set.set_val_nat (desc_set, Param_EARLY_STR  ,     1);
	_state_set.set_val_nat (desc_set, Param_DFS_INP_LC ,    80);
	_state_set.set_val_nat (desc_set, Param_DFS_INP_HC , 20480);
	_state_set.set_val_nat (desc_set, Param_DFS_INP_DIF,     1);
	_state_set.set_val_nat (desc_set, Param_DFS_DECAY  ,     0.5);
	_state_set.set_val_nat (desc_set, Param_DFS_SIZE   ,     1);
	_state_set.set_val_nat (desc_set, Param_DFS_PITCH  ,     0);
	_state_set.set_val_nat (desc_set, Param_DFS_PMODE  ,     1);
	_state_set.set_val_nat (desc_set, Param_DFS_TNK_LC ,     2.5);
	_state_set.set_val_nat (desc_set, Param_DFS_TNK_HC , 40960);
	_state_set.set_val_nat (desc_set, Param_DFS_TNK_DIF,     1);
	_state_set.set_val_nat (desc_set, Param_DFS_FREEZE ,     0);
	_state_set.set_val_nat (desc_set, Param_DFS_FLUSH  ,     0);

	_state_set.add_observer (Param_LVL_DRY    , _param_change_flag_lvl);
	_state_set.add_observer (Param_LVL_EARLY  , _param_change_flag_lvl);
	_state_set.add_observer (Param_LVL_DFS    , _param_change_flag_lvl);
	_state_set.add_observer (Param_PREDELAY   , _param_change_flag_early);
	_state_set.add_observer (Param_EARLY_NBR  , _param_change_flag_early);
	_state_set.add_observer (Param_EARLY_DUR  , _param_change_flag_early);
	_state_set.add_observer (Param_EARLY_DCY  , _param_change_flag_early);
	_state_set.add_observer (Param_EARLY_STR  , _param_change_flag_early);
	_state_set.add_observer (Param_DFS_INP_LC , _param_change_flag_dif_inp);
	_state_set.add_observer (Param_DFS_INP_HC , _param_change_flag_dif_inp);
	_state_set.add_observer (Param_DFS_INP_DIF, _param_change_flag_dif_inp);
	_state_set.add_observer (Param_DFS_DECAY  , _param_change_flag_tank);
	_state_set.add_observer (Param_DFS_SIZE   , _param_change_flag_tank);
	_state_set.add_observer (Param_DFS_PITCH  , _param_change_flag_tank);
	_state_set.add_observer (Param_DFS_PMODE  , _param_change_flag_tank);
	_state_set.add_observer (Param_DFS_TNK_LC , _param_change_flag_dif_tnk);
	_state_set.add_observer (Param_DFS_TNK_HC , _param_change_flag_dif_tnk);
	_state_set.add_observer (Param_DFS_TNK_DIF, _param_change_flag_dif_tnk);
	_state_set.add_observer (Param_DFS_FREEZE , _param_change_flag_line);
	_state_set.add_observer (Param_DFS_FLUSH  , _param_change_flag_line);

	_param_change_flag_lvl.add_observer (_param_change_flag);
	_param_change_flag_early.add_observer (_param_change_flag);
	_param_change_flag_dif_inp.add_observer (_param_change_flag);
	_param_change_flag_tank.add_observer (_param_change_flag);
	_param_change_flag_tank.add_observer (_param_change_flag);
	_param_change_flag_dif_tnk.add_observer (_param_change_flag);
	_param_change_flag_line.add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_LVL_DRY    , 0.010f);
	_state_set.set_ramp_time (Param_LVL_EARLY  , 0.010f);
	_state_set.set_ramp_time (Param_LVL_DFS    , 0.010f);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Platitude::do_get_state () const
{
	return _state;
}



double	Platitude::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Platitude::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_reverb.set_sample_freq (sample_freq);

	const int      mbs_align = (max_buf_len + 3) & ~3;
	for (auto &chn : _chn_arr)
	{
		chn._early_reflections.reset (
			sample_freq, Cst::_max_predelay_time, Cst::_max_er_duration
		);
		chn._buf_tmp.resize (mbs_align);
		chn._buf_erf.resize (mbs_align);
		chn._buf_dif.resize (mbs_align);
		chn._dc_kill_out.set_sample_freq (sample_freq);
		chn._dc_kill_out.set_cutoff_freq (5.f);
	}

	_param_change_flag_lvl.set ();
	_param_change_flag_early.set ();
	_param_change_flag_dif_inp.set ();
	_param_change_flag_dif_tnk.set ();
	_param_change_flag_tank.set ();
	_param_change_flag_line.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Platitude::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	const int      nbr_spl = proc._nbr_spl;
	_lvl_dry.tick (nbr_spl);
	_lvl_early.tick (nbr_spl);
	_lvl_diffuse.tick (nbr_spl);

	// Signal processing
	for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
	{
		Channel &      chn         = _chn_arr [chn_index];
		const int      chn_src_idx = std::min (chn_index, nbr_chn_src - 1);
		chn._early_reflections.process_block (
			chn._buf_tmp.data (),
			chn._buf_erf.data (),
			proc._src_arr [chn_src_idx],
			nbr_spl
		);
	}

	dsp::mix::Align::mix_2_2_vlrauto (
		_chn_arr [0]._buf_tmp.data (),
		_chn_arr [1]._buf_tmp.data (),
		_chn_arr [0]._buf_erf.data (),
		_chn_arr [1]._buf_erf.data (),
		nbr_spl,
		_lvl_early.get_beg (),
		_lvl_early.get_end ()
	);
	_reverb.process_block (
		_chn_arr [0]._buf_dif.data (),
		_chn_arr [1]._buf_dif.data (),
		_chn_arr [0]._buf_tmp.data (),
		_chn_arr [1]._buf_tmp.data (),
		nbr_spl
	);

	if (nbr_chn_dst < 2)
	{
		assert (nbr_chn_src == 1);
		dsp::mix::Align::copy_1_1_vlrauto (
			proc._dst_arr [0],
			proc._src_arr [0],
			nbr_spl,
			_lvl_dry.get_beg (),
			_lvl_dry.get_end ()
		);
		dsp::mix::Align::mix_2_1_vlrauto (
			proc._dst_arr [0],
			_chn_arr [0]._buf_erf.data (),
			_chn_arr [1]._buf_erf.data (),
			nbr_spl,
			_lvl_early.get_beg () * 0.5f,
			_lvl_early.get_end () * 0.5f
		);
		dsp::mix::Align::mix_2_1_vlrauto (
			proc._dst_arr [0],
			_chn_arr [0]._buf_dif.data (),
			_chn_arr [1]._buf_dif.data (),
			nbr_spl,
			_lvl_diffuse.get_beg () * 0.5f,
			_lvl_diffuse.get_end () * 0.5f
		);
	}
	else // nbr_chn_dst >= 2
	{
		dsp::mix::Align::copy_2_2_vlrauto (
			proc._dst_arr [0],
			proc._dst_arr [1],
			proc._src_arr [0],
			proc._src_arr [std::min (1, nbr_chn_src - 1)],
			nbr_spl,
			_lvl_dry.get_beg (),
			_lvl_dry.get_end ()
		);
		dsp::mix::Align::mix_2_2_vlrauto (
			proc._dst_arr [0],
			proc._dst_arr [1],
			_chn_arr [0]._buf_erf.data (),
			_chn_arr [1]._buf_erf.data (),
			nbr_spl,
			_lvl_early.get_beg (),
			_lvl_early.get_end ()
		);
		dsp::mix::Align::mix_2_2_vlrauto (
			proc._dst_arr [0],
			proc._dst_arr [1],
			_chn_arr [0]._buf_dif.data (),
			_chn_arr [1]._buf_dif.data (),
			nbr_spl,
			_lvl_diffuse.get_beg (),
			_lvl_diffuse.get_end ()
		);
	}

	// Kills DC on output
	for (int chn_idx = 0; chn_idx < nbr_chn_dst; ++chn_idx)
	{
		_chn_arr [chn_idx]._dc_kill_out.process_block (
			proc._dst_arr [chn_idx],
			proc._dst_arr [chn_idx],
			nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Platitude::clear_buffers ()
{
	_reverb.clear_buffers ();

	for (auto &chn : _chn_arr)
	{
		chn._early_reflections.clear_buffers ();
	}

	_lvl_dry.clear_buffers ();
	_lvl_early.clear_buffers ();
	_lvl_diffuse.clear_buffers ();
	_flush_flag = false;
}



void	Platitude::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_lvl (true) || force_flag)
		{
			const float    lvl_dry =
				float (_state_set.get_val_end_nat (Param_LVL_DRY));
			const float    lvl_erf =
				float (_state_set.get_val_end_nat (Param_LVL_EARLY));
			const float    lvl_dfs =
				float (_state_set.get_val_end_nat (Param_LVL_DFS));

			_lvl_dry.set_val (lvl_dry);
			_lvl_early.set_val (lvl_erf);
			_lvl_diffuse.set_val (lvl_dfs);
		}

		if (_param_change_flag_early (true) || force_flag)
		{
			const float    predelay_time =
				float (_state_set.get_val_tgt_nat (Param_PREDELAY));
			const int      nbr_taps =
				_state_set.get_val_int (Param_EARLY_NBR);
			const float    duration =
				float (_state_set.get_val_tgt_nat (Param_EARLY_DUR));
			const float    decay    =
				float (_state_set.get_val_tgt_nat (Param_EARLY_DCY));
			const float    stereo   =
				float (_state_set.get_val_tgt_nat (Param_EARLY_STR));

			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				auto &      chn = _chn_arr [chn_cnt];
				chn._early_reflections.generate_taps (
					chn_cnt, nbr_taps, duration, decay, 1
				);
				chn._early_reflections.set_predelay (predelay_time);
			}
			_chn_arr [1]._early_reflections.morph_taps_to (
				_chn_arr [0]._early_reflections, 1 - stereo
			);
		}

		if (_param_change_flag_dif_inp (true) || force_flag)
		{
			float          freq_lc   =
				float (_state_set.get_val_tgt_nat (Param_DFS_INP_LC));
			float          freq_hc   =
				float (_state_set.get_val_tgt_nat (Param_DFS_INP_HC));
			const float    diffusion =
				float (_state_set.get_val_tgt_nat (Param_DFS_INP_DIF));
			_reverb.set_filter_input_bp (freq_lc, freq_hc);
			_reverb.set_diffusion_input (diffusion);
		}

		if (_param_change_flag_dif_tnk (true) || force_flag)
		{
			float          freq_lc   =
				float (_state_set.get_val_tgt_nat (Param_DFS_TNK_LC));
			float          freq_hc   =
				float (_state_set.get_val_tgt_nat (Param_DFS_TNK_HC));
			const float    diffusion =
				float (_state_set.get_val_tgt_nat (Param_DFS_TNK_DIF));

			freq_lc = std::min (freq_lc, _sample_freq * 0.4999f);
			freq_hc = std::min (freq_hc, _sample_freq * 0.4999f);
			_reverb.set_filter_tank_bp (freq_lc, freq_hc);
			_reverb.set_diffusion_tank (diffusion);
		}

		if (_param_change_flag_tank (true) || force_flag)
		{
			const float    decay       =
				float (_state_set.get_val_tgt_nat (Param_DFS_DECAY));
			const float    room_size   =
				float (_state_set.get_val_tgt_nat (Param_DFS_SIZE));
			const float    pitch_cents =
				float (_state_set.get_val_tgt_nat (Param_DFS_PITCH));
			const bool     pitch_all_flag =
				bool (_state_set.get_val_int (Param_DFS_PMODE));
			_reverb.set_decay (decay);
			_reverb.set_room_size (room_size);
			_reverb.set_shimmer_pitch (pitch_cents, pitch_all_flag);
		}

		if (_param_change_flag_line (true) || force_flag)
		{
			const bool     freeze_flag =
				bool (_state_set.get_val_int (Param_DFS_FREEZE));
			const bool     flush_flag  =
				bool (_state_set.get_val_int (Param_DFS_FLUSH));
			_reverb.freeze_tank (freeze_flag);
			if (flush_flag && ! _flush_flag)
			{
				_reverb.flush_tank ();
			}
			_flush_flag = flush_flag;
		}
	}
}



}  // namespace verb2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
