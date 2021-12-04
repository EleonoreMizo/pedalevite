/*****************************************************************************

        Lipidipi.cpp
        Author: Laurent de Soras, 2021

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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/lipid/Param.h"
#include "mfx/pi/lipid/Lipidipi.h"
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
namespace lipid
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr int	Cst::_max_voice_pairs;
constexpr int	Cst::_max_pitch;



Lipidipi::Lipidipi (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FAT   , 4);
	_state_set.set_val_nat (desc_set, Param_GREASE, 10);

	_state_set.add_observer (Param_FAT   , _param_change_flag);
	_state_set.add_observer (Param_GREASE, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Lipidipi::do_get_state () const
{
	return _state;
}



double	Lipidipi::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Lipidipi::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	const int      mbl_align = (max_buf_len + 3) & ~3;
	_buf_dly.resize (mbl_align);
	_buf_mix.resize (mbl_align);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();
	_xfade_shape.set_sample_freq (sample_freq);
	_xfade_shape.set_duration (_win_dur);

	// When the delay is set to the maximum, we need room to push first
	// the new data, then read the delayed data.
	const double   add_dly  = max_buf_len / sample_freq;
	// Shifted by Cst::_max_pitch_up because at +1 octave, we read the line
	// at twice the speed.
	const double   max_rate = std::exp2 (double (Cst::_max_pitch) / 1200);
	const double   max_dly  = _win_dur * max_rate + add_dly;

	for (auto &chn : _chn_arr)
	{
		chn._delay.set_interpolator (_interp);
		chn._delay.set_sample_freq (sample_freq, 0);
		chn._delay.set_max_delay_time (max_dly);

		for (auto &reader : chn._reader_arr)
		{
			reader.set_tmp_buf (&_buf_dly [0], int (_buf_dly.size ()));
			reader.set_delay_line (chn._delay);
			reader.set_resampling_range (float (1.0 / max_rate), float (max_rate));
			reader.set_crossfade_normal (
				_xfade_shape.get_len (),
				_xfade_shape.use_shape ()
			);
			reader.set_crossfade_pitchshift (
				_xfade_shape.get_len (),
				_xfade_shape.use_shape ()
			);
			assert (reader.is_ready ());
		}
	}

	// We add a microsecond because of possible rounding errors
	_min_dly_time = float (_chn_arr [0]._delay.get_min_delay_time () + 1e-6);

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Lipidipi::do_process_block (piapi::ProcInfo &proc)
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
	const int      nbr_spl = proc._nbr_spl;
	for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
	{
		// Inserts incoming data into the delay lines
		Channel &      chn = _chn_arr [chn_idx];
		chn._delay.push_block (proc._src_arr [chn_idx], nbr_spl);

		// Dry signal
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_idx], proc._src_arr [chn_idx], nbr_spl
		);

		// Reads the lines
		const int      nbr_voices = _nbr_vc_pairs * 2;
		for (int vc_idx = 0; vc_idx < nbr_voices; ++vc_idx)
		{
			auto &         reader = chn._reader_arr [vc_idx];
			reader.read_data (_buf_mix.data (), nbr_spl, -nbr_spl);
			dsp::mix::Align::mix_1_1 (
				proc._dst_arr [chn_idx], _buf_mix.data (), nbr_spl
			);
		}
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



constexpr double	Lipidipi::_win_dur;



void	Lipidipi::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		for (auto &reader : chn._reader_arr)
		{
			reader.clear_buffers ();
		}
	}
}



void	Lipidipi::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const auto     fat    = float (_state_set.get_val_end_nat (Param_FAT));
		const auto     grease = float (_state_set.get_val_end_nat (Param_GREASE));

		_nbr_vc_pairs = fstb::ceil_int (fat);
		const float    gr_oct = grease * (1 / 1200.0f);

		if (_nbr_vc_pairs > 0)
		{
			const float    oct_per_vc =
				gr_oct * fstb::rcp_uint <float> (std::max (_nbr_vc_pairs - 1, 1));

			// Frequency difference between two consecutive pairs, Hz
			for (int pair_idx = 0; pair_idx < _nbr_vc_pairs; ++pair_idx)
			{
				float          rate  =
					fstb::Approx::exp2 (oct_per_vc * (_nbr_vc_pairs - pair_idx));

				for (int pol = 0; pol < 2; ++pol)
				{
					const int      rd_idx = pair_idx * 2 + pol;
					const float    dly_s  =
						std::max (float (2 * _win_dur * (rate - 1)), _min_dly_time);
					for (auto &chn : _chn_arr)
					{
						auto &         reader = chn._reader_arr [rd_idx];
						reader.set_delay_time (dly_s, 0);
						reader.set_grain_pitch (rate);
					}

					rate = 1.f / rate;
				}
			}
		}
	}
}



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
