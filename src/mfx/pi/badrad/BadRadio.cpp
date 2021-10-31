/*****************************************************************************

        BadRadio.cpp
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
#include "fstb/Hash.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/badrad/Param.h"
#include "mfx/pi/badrad/BadRadio.h"
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
namespace badrad
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BadRadio::BadRadio (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_GAIN ,   10);
	_state_set.set_val_nat (desc_set, Param_NOISE,    0.6);
	_state_set.set_val_nat (desc_set, Param_HISS ,    0);
	_state_set.set_val_nat (desc_set, Param_NZBW , 6000);
	_state_set.set_val_nat (desc_set, Param_LC   ,  150);
	_state_set.set_val_nat (desc_set, Param_HC   , 8000);

	_state_set.add_observer (Param_GAIN , _param_change_flag);
	_state_set.add_observer (Param_NOISE, _param_change_flag);
	_state_set.add_observer (Param_HISS , _param_change_flag);
	_state_set.add_observer (Param_NZBW , _param_change_flag);
	_state_set.add_observer (Param_LC   , _param_change_flag);
	_state_set.add_observer (Param_HC   , _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	BadRadio::do_get_state () const
{
	return _state;
}



double	BadRadio::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	BadRadio::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	// Otherwise our white noise level subtly changes with sampling rate
	_noise_fix   = float (sqrt (sample_freq / 44100));

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	BadRadio::do_process_block (piapi::ProcInfo &proc) noexcept
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
	_gain.tick (nbr_spl);
	for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
	{
		Channel &      chn   = _chn_arr [chn_idx];
		float          g_cur = _gain.get_beg ();
		const float    g_stp = _gain.get_step ();
		const auto * fstb_RESTRICT s_ptr = proc._src_arr [chn_idx];
		auto       * fstb_RESTRICT d_ptr = proc._dst_arr [chn_idx];

		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const auto     rnd_i = fstb::Hash::hash (_noise_cnt);
			float          noise = float (rnd_i) * fstb::TWOPM32; // [0 ; 1[
			noise = chn._noise_lpf.process_sample (noise * _noise_fix);

			auto           x  = s_ptr [pos];
			const float    gx = x * g_cur;
			const float    kx = x * std::max (_noise_lvl, 1.f);
			// Differences with the original version:
			// - Volume compensation for the noise level (_nz_comp)
			// - kx * gx instead of gx * gx at the denominator for better
			// volume homogeneity across the different gain values
			x *= (1 + _noise_lvl * fabsf (gx) * noise) * _nz_comp / (1 + kx * gx);

			x += (noise - 0.5f) * _hiss_lvl;

			x = chn._out_hpf.process_sample (x);
			x = chn._out_lpf.process_sample (x);

			d_ptr [pos] = x;

			++ _noise_cnt;
			g_cur += g_stp;
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



void	BadRadio::clear_buffers () noexcept
{
	_noise_cnt = 0;
	_gain.clear_buffers ();
	for (auto &chn : _chn_arr)
	{
		chn._noise_lpf.clear_buffers ();
		chn._out_hpf.clear_buffers ();
		chn._out_lpf.clear_buffers ();
	}
}



void	BadRadio::update_param (bool force_flag) noexcept
{
	if (_param_change_flag (true) || force_flag)
	{
		const float    gain = float (_state_set.get_val_end_nat (Param_GAIN));
		_gain.set_val (gain);

		_noise_lvl = float (_state_set.get_val_end_nat (Param_NOISE));
		_nz_comp = 1.f / (1 + _noise_lvl * 0.5f);

		_hiss_lvl  = float (_state_set.get_val_end_nat (Param_HISS));

		const float    freq_nz = float (_state_set.get_val_end_nat (Param_NZBW));
		const float    freq_lc = float (_state_set.get_val_end_nat (Param_LC));
		const float    freq_hc = float (_state_set.get_val_end_nat (Param_HC));
		using Filter = dsp::iir::Biquad;
		udpate_filter (freq_nz, { 1, 0, 0 },
			[] (Channel &c) -> Filter & { return c._noise_lpf; }
		);
		udpate_filter (freq_lc, { 0, 0, 1 },
			[] (Channel &c) -> Filter & { return c._out_hpf; }
		);
		udpate_filter (freq_hc, { 1, 0, 0 },
			[] (Channel &c) -> Filter & { return c._out_lpf; }
		);
	}
}



template <typename F>
void	BadRadio::udpate_filter (float freq, std::array <float, 3> b_s, F use_filter) noexcept
{
	using Tr = dsp::iir::TransSZBilin;
	const float    k = Tr::compute_k_approx (freq * _inv_fs);
	constexpr std::array <float, 3> a_s { 1, float (fstb::SQRT2 * 0.5), 1 };
	std::array <float, 3> b_z;
	std::array <float, 3> a_z;
	Tr::map_s_to_z_approx (
		b_z.data (), a_z.data (),
		b_s.data (), a_s.data (),
		k
	);
	for (auto &chn : _chn_arr)
	{
		auto &      biq = use_filter (chn);
		biq.set_z_eq (b_z.data (), a_z.data ());
	}
}



}  // namespace badrad
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
