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
#include "fstb/Hash.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/iir/TransSZBilin.h"
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



constexpr int	Cst::_max_voices;
constexpr int	Cst::_max_pitch;



Lipidipi::Lipidipi (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FAT   , 4);
	_state_set.set_val_nat (desc_set, Param_GREASE, 16);

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

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	constexpr std::array <double, 3>  bs { 1, 0, 0 };
	constexpr std::array <double, 3>  as { 1, fstb::SQRT2, 1 };
	std::array <float, 3>   bz;
	std::array <float, 3>   az;
	dsp::iir::TransSZBilin::map_s_to_z (
		bz.data (), az.data (),
		bs.data (), as.data (),
		_lpf_cutoff_freq, sample_freq
	);

	for (int vc_idx = 0; vc_idx < Cst::_max_voices; ++vc_idx)
	{
		auto &         vc = _voice_arr [vc_idx];

		vc._rnd_state = compute_initial_rnd_state (vc_idx);
		for (auto &filter : vc._lpf_arr)
		{
			filter.set_z_eq (bz.data (), az.data ());
		}
	}

	// When the delay is set to the maximum, we need room to push first
	// the new data, then read the delayed data.
	const double   add_dly       = max_buf_len / sample_freq;
	const double   max_dly_final = _avg_dly * 2 + add_dly;

	for (auto &chn : _chn_arr)
	{
		chn._delay.set_interpolator (_interp);
		chn._delay.set_sample_freq (sample_freq, 0);
		chn._delay.set_max_delay_time (max_dly_final);
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
	int            blk_pos = 0;
	do
	{
		const int      blk_len = std::min (
			nbr_spl - blk_pos,
			_seg_len - _seg_pos
		);

		if (_seg_pos == 0)
		{
			start_new_segment ();
		}

		constexpr auto seg_mul = fstb::rcp_uint <float> (_seg_len);
		const auto     rel_beg = float (_seg_pos          ) * seg_mul;
		const auto     rel_end = float (_seg_pos + blk_len) * seg_mul;

		for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
		{
			// Inserts incoming data into the delay lines
			Channel &      chn = _chn_arr [chn_idx];
			chn._delay.push_block (proc._src_arr [chn_idx] + blk_pos, blk_len);

			// Dry signal
			dsp::mix::Align::copy_1_1 (
				proc._dst_arr [chn_idx] + blk_pos,
				proc._src_arr [chn_idx] + blk_pos,
				blk_len
			);

			// Reads the lines
			for (int vc_idx = 0; vc_idx < _nbr_voices; ++vc_idx)
			{
				const auto &   vc = _voice_arr [vc_idx];

				const auto     dly_dif     = vc._delay_end - vc._delay_beg;
				const auto     dly_beg_seg = vc._delay_beg + rel_beg * dly_dif;
				const auto     dly_end_seg = vc._delay_beg + rel_end * dly_dif;

				chn._delay.read_block (
					_buf_dly.data (), blk_len,
					dly_beg_seg, dly_end_seg, -blk_len
				);
				dsp::mix::Align::mix_1_1 (
					proc._dst_arr [chn_idx] + blk_pos, _buf_dly.data (), blk_len
				);
			}
		}

		blk_pos  += blk_len;
		_seg_pos += blk_len;
		assert (_seg_pos <= _seg_len);
		_seg_pos &= _seg_msk;
	}
	while (blk_pos < nbr_spl);

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



constexpr double	Lipidipi::_avg_dly;
constexpr double	Lipidipi::_max_depth;
constexpr double	Lipidipi::_lpf_cutoff_freq;



void	Lipidipi::clear_buffers ()
{
	for (int vc_idx = 0; vc_idx < Cst::_max_voices; ++vc_idx)
	{
		auto &         vc = _voice_arr [vc_idx];
		vc._rnd_state = compute_initial_rnd_state (vc_idx);
		vc._delay_beg = float (_avg_dly);
		vc._delay_end = float (_avg_dly);
		for (auto &filter : vc._lpf_arr)
		{
			filter.clear_buffers ();
		}
	}

	for (auto &chn : _chn_arr)
	{
		chn._delay.clear_buffers ();
	}
}



void	Lipidipi::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const auto     fat    = float (_state_set.get_val_end_nat (Param_FAT));
		const auto     grease = float (_state_set.get_val_end_nat (Param_GREASE));

		_nbr_voices = fstb::ceil_int (fat);
		const float    gr_oct = grease * (1 / 1200.0f);

		if (_nbr_voices > 0)
		{
			// Maximum playback rate (1 = no pitch change)
			const float    ratio_max = fstb::Approx::exp2 (gr_oct);
			const float    ratio_min = 1 / ratio_max;

			// Seconds per segment
			const float    seg_dur   = _seg_len * _inv_fs;
			const float    rate_max  = (ratio_max - 1) * seg_dur;
			const float    rate_min  = (ratio_min - 1) * seg_dur;

			// Given the maximum rate and the oscillation depth, we can compute
			// the filter cutoff frequency
			const float    block_fs    = // seg/s
				_sample_freq * fstb::rcp_uint <float> (_seg_len);
			const auto     amplitude   = _max_depth; // s
			const float    cutoff_freq = // (s/seg) * (seg/s) / s
				rate_max * block_fs / float (amplitude * 2 * fstb::PI);

			// Compensates for the energy (and magnitude) loss caused by filtering
			// We subtract 4.5 dB (1.68 linear) to take the true peak value of the
			// white noise into account.
			const float    wn_tp_amp    = 1.68f;
			const double   filter_scale =
				sqrt (block_fs * 0.5f / (cutoff_freq * wn_tp_amp));

			constexpr double  scale_rnd = 1.0 / double (1 << 15);
			const double   dly_scale    = scale_rnd * amplitude * filter_scale;

			for (int vc_idx = 0; vc_idx < _nbr_voices; ++vc_idx)
			{
				auto &         vc = _voice_arr [vc_idx];
				vc._dly_scale = float (dly_scale);
				vc._rate_min  = rate_min;
				vc._rate_max  = rate_max;
			}
		}
	}
}



void	Lipidipi::start_new_segment ()
{
	// Computes the delay states for the end of the block
	for (int vc_idx = 0; vc_idx < _nbr_voices; ++vc_idx)
	{
		auto &         vc = _voice_arr [vc_idx];

		// Starts from the end of the previous block
		vc._delay_beg = vc._delay_end;

		// Random generation and filtering
		++ vc._rnd_state;
		const auto     rnd_val_u = fstb::Hash::hash (vc._rnd_state);
		const auto     rnd_val_s = int16_t (rnd_val_u); // Makes it signed
		float          delay = float (rnd_val_s);
		for (auto &filter : vc._lpf_arr)
		{
			delay = filter.process_sample (delay);
		}

		delay *= vc._dly_scale;
		delay = fstb::limit (delay, float (-_max_depth), float (+_max_depth));
		delay += float (_avg_dly);

		// Takes the rate limit into account
		vc._delay_end = fstb::limit (
			delay,
			vc._delay_beg + vc._rate_min,
			vc._delay_beg + vc._rate_max
		);
	}
}



uint32_t	Lipidipi::compute_initial_rnd_state (int vc_idx) noexcept
{
	return uint32_t (vc_idx) << 16;
}



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
