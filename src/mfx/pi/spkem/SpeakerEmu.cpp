/*****************************************************************************

        SpeakerEmu.cpp
        Author: Laurent de Soras, 2016

TO DO: optimize the channel bypass. Currently everything is processed and
bypassed channels are just overwritten with the input afterwards

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

#include "fstb/fnc.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/spkem/Param.h"
#include "mfx/pi/spkem/SpeakerEmu.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace spkem
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpeakerEmu::SpeakerEmu ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_nbr_chn (0)
,	_channels (~uint32_t (0))
,	_param_change_flag ()
,	_biq_pack ()
,	_config_arr ()
,	_nbr_stages_arr ({{ 8, 16, 12 }})
,	_chn_arr ()
,	_comb_time_spl (24)
,	_write_pos (0)
,	_config (0)
,	_mid_freq (1900)
,	_mid_lvl (powf (10.f, 7 / 20.f))
,	_treble_freq (6500)
,	_treble_lvl (powf (10.f, (-13*3+7) / 20.f))
,	_comb_fdbk (0.125f)
,	_buf_tmp ()
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_biq_pack.reserve (_max_nbr_stages, _max_nbr_chn);

	// Other interesting combination: mid 3kHz/+5dB, comb 25%
	_state_set.set_val_nat (desc_set, Param_TYPE       , 0);
	_state_set.set_val_nat (desc_set, Param_MID_LVL    , powf (10.f,   7 / 20.f));
	_state_set.set_val_nat (desc_set, Param_MID_FREQ   , 1900);
	_state_set.set_val_nat (desc_set, Param_TREBLE_LVL , powf (10.f, -32 / 20.f));
	_state_set.set_val_nat (desc_set, Param_TREBLE_FREQ, 6500);
	_state_set.set_val_nat (desc_set, Param_COMB_LVL   , 0.125f);
	_state_set.set_val_nat (desc_set, Param_CHANNELS   , SpeakerEmuDesc::Channels_ALL);

	_state_set.add_observer (Param_TYPE       , _param_change_flag);
	_state_set.add_observer (Param_MID_LVL    , _param_change_flag);
	_state_set.add_observer (Param_MID_FREQ   , _param_change_flag);
	_state_set.add_observer (Param_TREBLE_LVL , _param_change_flag);
	_state_set.add_observer (Param_TREBLE_FREQ, _param_change_flag);
	_state_set.add_observer (Param_COMB_LVL   , _param_change_flag);
	_state_set.add_observer (Param_CHANNELS   , _param_change_flag);

	// Type 0
	set_peak (   0, 0,   300, powf (10,  -1.2f / 20.f), 1.3f );
	set_peak (   0, 1,  1000, powf (10,   2.2f / 20.f), 1.5f );
	set_peak (   0, 2,  1500, powf (10,  -1.5f / 20.f), 3    );
	set_shelf_h (0, 3,  1900, powf (10,   7    / 20.f), 1    );
	set_peak (   0, 4, 10000, powf (10, -10    / 20.f), 3    );
	for (int i = 0; i < 3; ++i)
	{
		set_shelf_h (0, i + 5,  6500, powf (10, -13    / 20.f), 0.75f);
	}

	// Type 1A
	set_pass_h ( 1,  0,    73, 0.34f,  1.3f  );
	set_pass_h ( 1,  1,    73, 0    ,  1.25f );
	set_peak (   1,  2,   144, 0.5f ,  2     );
	set_peak (   1,  3,   180, 1.5f ,  2     );
	set_peak (   1,  4,   300, 0.85f,  5     );
	set_peak (   1,  5,   620, 0.65f,  7     );
	set_pass_l ( 1,  6,   760, 0.98f, 20     );
	set_peak (   1,  7,   920, 1.6f , 10     );
	set_pass_l ( 1,  8,  1100, 1.02f, 15     );
	set_peak (   1,  9,  1580, 0.34f,  4     );
	set_pass_l ( 1, 10,  2200, 0.25f,  1.6f  );
	set_shelf_h (1, 11,  3300, 0.75f,  0.707f);
	set_pass_l ( 1, 12,  3600, 0    ,  8.5f  );
	set_peak (   1, 13,  5000, 2.25f,  7     );
	set_shelf_h (1, 14,  7000, 1.05f, 18     );
	set_peak (   1, 15,  9700, 3.25f,  4     );

	// Type 1B
	set_pass_h ( 2,  0,    73, 0.34f,  1.3f  );
	set_pass_h ( 2,  1,    73, 0    ,  1.25f );
	set_peak (   2,  2,   130, 0.5f ,  2     );
	set_peak (   2,  3,   170, 2    ,  2.7f  );
	set_shelf_l (2,  4,   300, 0.8f ,  0.5f  ); // Measured level was 0.2f instead of 0.8f. This new setting gives a flatter response in the low-mid range.
	set_shelf_h (2,  5,   870, 1.1f ,  6     );
	set_shelf_h (2,  6,  1780, 1.4f ,  4.7f  );
	set_pass_l ( 2,  7,  3750, 0    ,  0.8f  );
	set_pass_l ( 2,  8,  3720, 0.94f, 20     );
	set_pass_l ( 2,  9,  6000, 0    ,  0.8f  );
	set_pass_l ( 2, 10,  8500, 1.25f,  4     );
	set_pass_l ( 2, 11, 10000, 0.01f,  1     );
//	add_gain (   2, 1.5f);                      // Not needed with the flat response mod

	for (auto &chn : _chn_arr)
	{
		chn._delay.set_unroll_post (0);
		chn._delay.set_unroll_pre (0);
		chn._delay.set_max_delay_time (0.005);
	}

	dsp::mix::Generic::setup ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	SpeakerEmu::do_get_state () const
{
	return _state;
}



double	SpeakerEmu::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	SpeakerEmu::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_nbr_chn     = 0; // Force update

	for (auto &chn : _chn_arr)
	{
		chn._delay.set_extra_len (max_buf_len);
		chn._delay.set_sample_freq (sample_freq);
		chn._delay.update_buffer_size ();
	}
	_comb_time_spl = fstb::round_int (sample_freq * 24 / 44100);

	const int      mbl_align = (max_buf_len + 3) & -4;
	_buf_tmp.resize (mbl_align);

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	SpeakerEmu::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_in =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_out =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	assert (nbr_chn_in <= nbr_chn_out);
	const int      nbr_chn_proc = std::min (nbr_chn_in, nbr_chn_out);
	if (nbr_chn_proc != _nbr_chn)
	{
		_nbr_chn = nbr_chn_proc;
		const int   nbr_stages = _nbr_stages_arr [_config];
		_biq_pack.adapt_config (nbr_stages, _nbr_chn);
		update_filter ();
	}

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
	update_param ();

	// Signal processing
	_biq_pack.process_block (proc._dst_arr, proc._src_arr, 0, proc._nbr_spl);

	const int      dly_len  = _chn_arr [0]._delay.get_len ();
	const int      dly_mask = _chn_arr [0]._delay.get_mask ();
	int            pos      = 0;
	do
	{
		const int      read_pos = (_write_pos - _comb_time_spl) & dly_mask;
		const int      rem_len  = proc._nbr_spl - pos;
		const int      room_w   = dly_len - _write_pos;
		const int      room_r   = dly_len - read_pos;
		const int      work_len = std::min (std::min (room_w, room_r), rem_len);
		for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
		{
			Channel &      chn      = _chn_arr [chn_index];
			float *        data_ptr = chn._delay.get_buffer ();

			// Writes the delay line
			dsp::mix::Generic::copy_1_1 (
				data_ptr + _write_pos,
				proc._dst_arr [chn_index] + pos,
				work_len
			);
			// Original + delayed signals read from the line
			dsp::mix::Generic::mix_1_1_v (
				proc._dst_arr [chn_index] + pos,
				data_ptr + read_pos,
				work_len,
				-_comb_fdbk
			);
		}
		_write_pos = (_write_pos + work_len) & dly_mask;
		pos += work_len;
	}
	while (pos < proc._nbr_spl);

	// Duplicates the remaining output channels
	for (int chn_index = 0; chn_index < nbr_chn_out; ++chn_index)
	{
		const bool     proc_flag = (((_channels >> chn_index) & 1) != 0);
		if (chn_index >= nbr_chn_in || ! proc_flag)
		{
			const float *  from_ptr = proc._dst_arr [0];
			if (! proc_flag)
			{
				if (chn_index >= nbr_chn_in)
				{
					from_ptr = proc._src_arr [0];
				}
				else
				{
					from_ptr = proc._src_arr [chn_index];
				}
			}
			dsp::mix::Align::copy_1_1 (
				proc._dst_arr [chn_index],
				from_ptr,
				proc._nbr_spl
			);
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SpeakerEmu::clear_buffers ()
{
	_write_pos = 0;
	_biq_pack.clear_buffers ();
	for (auto &chn : _chn_arr)
	{
		chn._delay.clear_buffers ();
	}
}



void	SpeakerEmu::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const int      conf_new =
			fstb::round_int (_state_set.get_val_tgt_nat (Param_TYPE));
		_mid_lvl     = float (_state_set.get_val_tgt_nat (Param_MID_LVL    ));
		_mid_freq    = float (_state_set.get_val_tgt_nat (Param_MID_FREQ   ));
		_treble_lvl  = float (_state_set.get_val_tgt_nat (Param_TREBLE_LVL ));
		_treble_freq = float (_state_set.get_val_tgt_nat (Param_TREBLE_FREQ));
		_comb_fdbk   = float (_state_set.get_val_tgt_nat (Param_COMB_LVL   ));

		if (conf_new != _config)
		{
			_config  = conf_new;
			_nbr_chn = 0; // Force udpate
		}

		if (_config == 0)
		{
			set_shelf_h (0, 3, _mid_freq, _mid_lvl, 1);

			const float    treble_lvl_cbrt = cbrt (_treble_lvl);
			for (int i = 0; i < 3; ++i)
			{
				set_shelf_h (0, i + 5, _treble_freq, treble_lvl_cbrt, 0.75f);
			}

			update_filter ();
		}

		const SpeakerEmuDesc::Channels   chn_val = static_cast <SpeakerEmuDesc::Channels> (
			fstb::round_int (_state_set.get_val_tgt_nat (Param_CHANNELS))
		);
		if (chn_val == SpeakerEmuDesc::Channels_ALL)
		{
			_channels = ~uint32_t (0);
		}
		else
		{
			_channels = uint32_t (chn_val);
		}
	}
}



void	SpeakerEmu::update_filter ()
{
	StageArray &   stage_arr  = _config_arr [_config];
	const int      nbr_stages = _nbr_stages_arr [_config];
	for (int stg_index = 0; stg_index < nbr_stages; ++stg_index)
	{
		const Stage &  stage = stage_arr [stg_index];

		float          bz [3];
		float          az [3];
		const float    k = dsp::iir::TransSZBilin::compute_k_approx (
			stage._freq * _inv_fs
		);
		dsp::iir::TransSZBilin::map_s_to_z_approx (
			bz, az,
			&stage._bs [0], &stage._as [0],
			k
		);

		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			_biq_pack.set_biquad (stg_index, chn, bz, az, false);
		}
	}
}



void	SpeakerEmu::set_peak (int conf, int stage, float freq, float lvl, float q)
{
	assert (conf >= 0);
	assert (conf < _nbr_types);
	assert (stage >= 0);
	assert (stage < _max_nbr_stages);
	assert (freq > 0);
	assert (lvl >= 0);
	assert (q > 0);

	Stage &        stg = _config_arr [conf] [stage];
	dsp::iir::DesignEq2p::make_mid_peak (&stg._bs [0], &stg._as [0], q, lvl);
	stg._freq = freq;
}



void	SpeakerEmu::set_shelf_l (int conf, int stage, float freq, float lvl, float q)
{
	assert (conf >= 0);
	assert (conf < _nbr_types);
	assert (stage >= 0);
	assert (stage < _max_nbr_stages);
	assert (freq > 0);
	assert (lvl > 0);
	assert (q > 0);

	Stage &        stg = _config_arr [conf] [stage];
	dsp::iir::DesignEq2p::make_mid_shelf_lo (&stg._bs [0], &stg._as [0], q, lvl);
	stg._freq = freq;
}



void	SpeakerEmu::set_shelf_h (int conf, int stage, float freq, float lvl, float q)
{
	assert (conf >= 0);
	assert (conf < _nbr_types);
	assert (stage >= 0);
	assert (stage < _max_nbr_stages);
	assert (freq > 0);
	assert (lvl > 0);
	assert (q > 0);

	Stage &        stg = _config_arr [conf] [stage];
	dsp::iir::DesignEq2p::make_mid_shelf_hi (&stg._bs [0], &stg._as [0], q, lvl);
	stg._freq = freq;
}



void	SpeakerEmu::set_pass_l (int conf, int stage, float freq, float lvl, float q)
{
	assert (conf >= 0);
	assert (conf < _nbr_types);
	assert (stage >= 0);
	assert (stage < _max_nbr_stages);
	assert (freq > 0);
	assert (lvl >= 0);
	assert (q > 0);

	Stage &        stg = _config_arr [conf] [stage];
	dsp::iir::DesignEq2p::make_3db_shelf_hi (&stg._bs [0], &stg._as [0], q, lvl);
	stg._freq = freq;
}



void	SpeakerEmu::set_pass_h (int conf, int stage, float freq, float lvl, float q)
{
	assert (conf >= 0);
	assert (conf < _nbr_types);
	assert (stage >= 0);
	assert (stage < _max_nbr_stages);
	assert (freq > 0);
	assert (lvl >= 0);
	assert (q > 0);

	Stage &        stg = _config_arr [conf] [stage];
	dsp::iir::DesignEq2p::make_3db_shelf_lo (&stg._bs [0], &stg._as [0], q, lvl);
	stg._freq = freq;
}



void	SpeakerEmu::add_gain (int conf, float gain)
{
	Stage &        stg = _config_arr [conf] [0];
	stg._bs [0] *= gain;
	stg._bs [1] *= gain;
	stg._bs [2] *= gain;
}



}  // namespace spkem
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
