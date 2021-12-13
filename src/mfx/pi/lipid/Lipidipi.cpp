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
#include "mfx/dsp/iir/DesignEq2p.h"
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



constexpr int	Cst::_max_groups;
constexpr int	Cst::_max_pitch;



Lipidipi::Lipidipi (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FAT   ,  4);
	_state_set.set_val_nat (desc_set, Param_GREASE, 16);
	_state_set.set_val_nat (desc_set, Param_SUET  ,  0.45);
	_state_set.set_val_nat (desc_set, Param_LARD  ,  1);

	_state_set.add_observer (Param_FAT   , _param_change_flag);
	_state_set.add_observer (Param_GREASE, _param_change_flag);
	_state_set.add_observer (Param_SUET  , _param_change_flag);
	_state_set.add_observer (Param_LARD  , _param_change_flag);
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

	const auto     vol_ramp_len = fstb::ceil_int (sample_freq * 0.001);
	const auto     val_step     = fstb::rcp_uint <float> (vol_ramp_len);
	_vol_dry.set_time (vol_ramp_len, val_step);
	_vol_wet.set_time (vol_ramp_len, val_step);

	const auto     f_beg = _f_beg_hz / sample_freq; // Relative to Fs
	_f_beg_l2 = float (log2 (f_beg));

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

	int            nbr_chn_proc   = nbr_chn_dst;
	const bool     mono_proc_flag = (! _stereo_flag && nbr_chn_src == 1);
	if (mono_proc_flag)
	{
		nbr_chn_proc = 1;
	}

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

	const int      nbr_chn_dly = (_stereo_flag) ? nbr_chn_dst : 1;
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

		_vol_dry.tick (blk_len);
		_vol_wet.tick (blk_len);

		for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
		{
			const int      chn_src_idx = std::min (chn_idx, nbr_chn_src - 1);
			const int      chn_dly_idx = std::min (chn_idx, nbr_chn_dly - 1);

			Channel &      chn     = _chn_arr [chn_idx    ];
			const Channel& chn_dly = _chn_arr [chn_dly_idx];
			const float *  src_ptr = proc._src_arr [chn_src_idx] + blk_pos;
			float *        dst_ptr = proc._dst_arr [chn_idx    ] + blk_pos;

			// Inserts incoming data into the delay lines
			chn._delay.push_block (src_ptr, blk_len);

			// Dry signal
			dsp::mix::Align::copy_1_1_vlrauto (
				dst_ptr, src_ptr, blk_len,
				_vol_dry.get_beg (), _vol_dry.get_end ()
			);

			// Processes all voices
			const int      nbr_voices = _nbr_groups * _vc_per_grp;
			for (int vc_idx = 0; vc_idx < nbr_voices; ++vc_idx)
			{
				const auto &   vc_dly      = chn_dly._voice_arr [vc_idx];
				const auto     dly_dif     = vc_dly._delay_end - vc_dly._delay_beg;
				const auto     dly_beg_seg = vc_dly._delay_beg + rel_beg * dly_dif;
				const auto     dly_end_seg = vc_dly._delay_beg + rel_end * dly_dif;

				auto &         vc = chn._voice_arr [vc_idx];

				// Reads the line
				chn._delay.read_block (
					_buf_dly.data (), blk_len,
					dly_beg_seg, dly_end_seg, -blk_len
				);

				// Voice filtering
				vc._bpf.process_block (_buf_dly.data (), _buf_dly.data (), blk_len);

				// Voice mixing
				if (vc_idx == 0)
				{
					dsp::mix::Align::copy_1_1 (
						_buf_mix.data (), _buf_dly.data (), blk_len
					);
				}
				else
				{
					dsp::mix::Align::mix_1_1 (
						_buf_mix.data (), _buf_dly.data (), blk_len
					);
				}
			}

			// Final mix
			dsp::mix::Align::mix_1_1_vlrauto (
				dst_ptr, _buf_mix.data (), blk_len,
				_vol_wet.get_beg (), _vol_wet.get_end ()
			);
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
constexpr int	Lipidipi::_seg_len;
constexpr int	Lipidipi::_seg_msk;
constexpr int	Lipidipi::_vc_per_grp;
constexpr int	Lipidipi::_max_voices;
constexpr float	Lipidipi::_f1_hz;
constexpr double	Lipidipi::_f_beg_hz;
constexpr double	Lipidipi::_f_end_hz;



void	Lipidipi::clear_buffers () noexcept
{
	for (int chn_idx = 0; chn_idx < _max_nbr_chn; ++chn_idx)
	{
		auto &         chn = _chn_arr [chn_idx];

		chn._delay.clear_buffers ();

		for (int vc_idx = 0; vc_idx < _max_voices; ++vc_idx)
		{
			auto &         vc = chn._voice_arr [vc_idx];
			vc._rnd_state = compute_initial_rnd_state (chn_idx, vc_idx);
			vc._delay_beg = float (_avg_dly);
			vc._delay_end = float (_avg_dly);
			const auto     seed = 12345 + chn_idx * 6789 + vc_idx;
			const uint32_t h    = fstb::Hash::hash (uint32_t (seed));
			const float    val  = float (int16_t (h));
			const std::array <float, 2> s { val, val };
			for (auto &filter : vc._lpf_arr)
			{
				filter.clear_buffers ();
				filter.set_state (s.data (), s.data ());
			}
			vc._bpf.clear_buffers ();
		}
	}

	_vol_dry.clear_buffers ();
	_vol_wet.clear_buffers ();
}



void	Lipidipi::update_param (bool force_flag) noexcept
{
	if (_param_change_flag (true) || force_flag)
	{
		_fatness = float (_state_set.get_val_end_nat (Param_FAT));
		const auto     grease = float (_state_set.get_val_end_nat (Param_GREASE));
		const auto     suet   = float (_state_set.get_val_end_nat (Param_SUET));
		_stereo_flag = _state_set.get_val_bool (Param_LARD);

		const auto     mix    = fstb::sq (suet);
		const int      nbr_grp_full = fstb::floor_int (_fatness);
		_nbr_groups = fstb::ceil_int (_fatness);
		// Makes sure rounding errors are not an issue
		_nbr_groups = std::min (_nbr_groups, int (Cst::_max_groups));
		const float    gr_oct = grease * (1 / 1200.0f);

		float          gain_fix = 1;
		if (_nbr_groups > 0)
		{
			// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
			// Delay LFOs

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
			// We subtract about 4.5 dB (1.68 linear) to take the true peak value
			// of the white noise into account.
			const float    wn_tp_amp    = 1.68f;

			const double   filter_scale =
				sqrt (block_fs * 0.5f / (cutoff_freq * wn_tp_amp));

			constexpr double  scale_rnd = 1.0 / double (1 << 15);
			const auto     dly_scale    =
				float (scale_rnd * amplitude * filter_scale);

			constexpr std::array <double, 3>  d_bs { 1, 0, 0 };
			constexpr std::array <double, 3>  d_as { 1, fstb::SQRT2, 1 };
			std::array <float, 3>   d_bz;
			std::array <float, 3>   d_az;
			dsp::iir::TransSZBilin::map_s_to_z (
				d_bz.data (), d_az.data (),
				d_bs.data (), d_as.data (),
				cutoff_freq, block_fs
			);

			for (auto &chn : _chn_arr)
			{
				for (auto &vc : chn._voice_arr)
				{
					for (auto &filter : vc._lpf_arr)
					{
						filter.set_z_eq (d_bz.data (), d_az.data ());
					}
				}
			}

			// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
			// Band-pass filters

			// Formula: https://www.desmos.com/calculator/oxasvqwzmz

			const auto     fade     = _fatness - nbr_grp_full; // 0 -> 1
			constexpr int  order    = 16;
			const float    fade_vol =
				1 - fstb::ipowpc <order> (1 - fade) * (float (order) * fade + 1);

			auto           lvl_w    = mix;
			if (nbr_grp_full == 0)
			{
				lvl_w *= fade_vol;
			}
			const auto     lvl_w_sq = float (_vc_per_grp) * fstb::sq (lvl_w);
			const auto     lvl_d_sq = fstb::sq (1 - mix);
			gain_fix = fstb::Approx::rsqrt <1> (lvl_d_sq + lvl_w_sq);

			constexpr auto pi_f  = float (fstb::PI);
			// Relative to Fs/pi.
			// We use the taylor approx because f1_hz remains low.
			float          f1_w  =
				fstb::Approx::tan_taylor5 (_f1_hz * _inv_fs * pi_f);

			const auto     rmul    = 1.f / std::max (_fatness - 1, 1.f);
			const auto     f_r_mul = _f_rat_l2 * rmul;
			for (int grp_idx = 0; grp_idx < _nbr_groups; ++grp_idx)
			{
				// Computes the bandpass parameters for the given voice.
				// Splitting frequencies are log-spaced from 100 to 3200 Hz.
				// First voice starts at 5 Hz.
				// The upper voice extends up to nyquist frequency * 0.90.
				// When a new voice is introduced, the penultimate band has its
				// upper frequency shifting down while the new one fades in and
				// its upper bound starts from F_n * 0.95 down to F_n * 0.9.
				constexpr auto nyq_r = 0.5f;
				constexpr auto r0    = nyq_r * 0.95f;
				constexpr auto r1    = nyq_r * 0.90f;
				const bool     last_grp_flag = (grp_idx == nbr_grp_full);
				const float    level = last_grp_flag ? fade_vol : 1.f; // Linear
				float          f2    = 0;
				if (last_grp_flag)
				{
					f2 = fstb::lerp (r0, r1, fade);
				}
				else
				{
					f2 = fstb::Approx::exp2 (_f_beg_l2 + f_r_mul * float (grp_idx));
					if (grp_idx == nbr_grp_full - 1)
					{
						const float    s = 1 - fstb::ipowpc <3> (1 - fade);
						f2 = fstb::lerp (r1, std::min (f2, r1), s);
					}
				}

				// Computes the bandwidth and center frequencies
				const auto     f2_w   = fstb::Approx::tan_pade33 (f2 * pi_f);
				assert (f1_w < f2_w);
				const auto     f0sq_w = f1_w * f2_w;
				const auto     f0_w   = sqrtf (f0sq_w);
				const auto     q      = f0_w / (f2_w - f1_w);

				// Computes the filter coefficients
				std::array <float, 3>   bs;
				std::array <float, 3>   as;
				dsp::iir::DesignEq2p::make_band_pass (bs.data (), as.data (), q);
#if 1 // Simplified
				// k = 1 / tan (pi * f0) = 1 / f0_w = rsqrt (f0sq_w)
				const auto     k = fstb::Approx::rsqrt <1> (f0sq_w);
#else // Reference formula
				const auto     f0 = atanf (f0_w) * (1.f / pi_f); // Rel. to Fs
				const auto     k  = dsp::iir::TransSZBilin::compute_k_approx (f0);
#endif
				std::array <float, 3>   bz;
				std::array <float, 3>   az;
				dsp::iir::TransSZBilin::map_s_to_z_approx (
					bz.data (), az.data (), bs.data (), as.data (), k
				);

				// Merges the band level into the filter coefficients
				for (auto &coef : bz) { coef *= level; }

				// Sets voice parameters
				const int      vc_beg = grp_idx * _vc_per_grp;
				for (auto &chn : _chn_arr)
				{
					for (int vc_cnt = 0; vc_cnt < _vc_per_grp; ++vc_cnt)
					{
						const auto     vc_idx = vc_beg + vc_cnt;
						auto &         vc     = chn._voice_arr [vc_idx];
						vc._dly_scale = dly_scale;
						vc._rate_min  = rate_min;
						vc._rate_max  = rate_max;
						vc._bpf.set_z_eq (bz.data (), az.data ());
					}
				}

				// Next group
				f1_w = f2_w;
			} // for grp_idx
		} // _nbr_groups > 0

		_vol_dry.set_val (gain_fix * (1 - mix));
		_vol_wet.set_val (gain_fix *      mix );
	}
}



void	Lipidipi::start_new_segment () noexcept
{
	// Computes the delay states for the end of the block
	const int      nbr_voices = _nbr_groups * _vc_per_grp;
	for (auto &chn : _chn_arr)
	{
		for (int vc_idx = 0; vc_idx < nbr_voices; ++vc_idx)
		{
			auto &         vc = chn._voice_arr [vc_idx];

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
			delay  = fstb::limit (delay, float (-_max_depth), float (+_max_depth));
			delay += float (_avg_dly);

#if 0 // Not really necessary actually
			// Takes the rate limit into account
			delay  = fstb::limit (
				delay,
				vc._delay_beg + vc._rate_min,
				vc._delay_beg + vc._rate_max
			);
#endif

			vc._delay_end = delay;
		}
	}
}



uint32_t	Lipidipi::compute_initial_rnd_state (int chn_idx, int vc_idx) noexcept
{
	assert (chn_idx >= 0);
	assert (vc_idx >= 0);

	return uint32_t ((chn_idx << 24) + (vc_idx << 16));
}



const float	Lipidipi::_f_rat_l2 = float (log2 (_f_end_hz / _f_beg_hz));



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
