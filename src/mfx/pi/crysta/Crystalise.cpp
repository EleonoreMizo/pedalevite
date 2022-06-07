/*****************************************************************************

        Crystalise.cpp
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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#if defined (fstb_HAS_SIMD)
	#include "fstb/ToolsSimd.h"
	#include "fstb/Vf32.h"
#endif // fstb_HAS_SIMD
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/wnd/XFadeEqPowPoly8.h"
#include "mfx/pi/crysta/Param.h"
#include "mfx/pi/crysta/Crystalise.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>
#include <complex>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace crysta
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Crystalise::Crystalise (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();
	dsp::mix::Generic::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_AMOUNT, 0);

	_state_set.add_observer (Param_AMOUNT, _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Crystalise::do_get_state () const
{
	return _state;
}



double	Crystalise::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Crystalise::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	compute_fft_var (sample_freq);
	_fft.set_length (_p._fft_len_l2);
	_buf_pcm.resize (_p._fft_len);

	// Makes sure the PCM buffer can be recycled to precompute the magnitudes
	// for the crystalise effect.
	assert (_p._fft_len >= _p._nbr_bins + _cryst_rad * 2);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (int chn_idx = 0; chn_idx < _max_nbr_chn; ++chn_idx)
	{
		auto &         chn = _chn_arr [chn_idx];

		int            ofs_com = 0;
		int            ofs_syn = 0;

		// Because we use small processing block size, the calculation load
		// is unevenly spread across the blocks. With multiple channels, we
		// can make the load more homogenous by interleaving the FFT frames
		// of each channel.
		// We have no real latency issue here, so we can interleave
		// analysis and synthesis too.
		// Ideally we should use bit reversal if _max_nbr_chn is high.
		if (max_buf_len < _p._hop_size)
		{
			ofs_com = _p._hop_size * chn_idx / _max_nbr_chn;
			ofs_syn = max_buf_len;
		}

		chn._fo_ana.setup (_p._fft_len, _p._hop_size, ofs_com);
		chn._fo_syn.setup (_p._fft_len, _p._hop_size, ofs_com + ofs_syn);
		chn._buf_bins.resize (_p._fft_len);
		chn._weight_arr.resize (_p._nbr_bins);
	}

	_frame_win.setup (_p._fft_len);

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Crystalise::do_process_block (piapi::ProcInfo &proc)
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
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn = _chn_arr [chn_index];

		int            pos = 0;
		do
		{
			const int      rem_len_a = chn._fo_ana.get_len_before_next_frame ();
			const int      rem_len_s = chn._fo_syn.get_len_before_next_frame ();
			const int      rem_len   = std::min (rem_len_a, rem_len_s);
			const int      work_len  = std::min (rem_len, nbr_spl - pos);

			chn._fo_ana.process_block (proc._src_arr [chn_index] + pos, work_len);
			chn._fo_syn.process_block (proc._dst_arr [chn_index] + pos, work_len);

			const bool     req_ana_flag = chn._fo_ana.is_frame_proc_required ();
			const bool     req_syn_flag = chn._fo_syn.is_frame_proc_required ();

			if (req_ana_flag)
			{
				// Input frame extraction and windowing
				chn._fo_ana.get_frame (_buf_pcm.data ());
				_frame_win.process_frame_mul (_buf_pcm.data ());

				// PCM -> frequency bins
				_fft.do_fft (chn._buf_bins.data (), _buf_pcm.data ());
			}

			if (req_syn_flag)
			{
				// Synthesis
				synthesise_bins (chn);

				// Frequency bins -> PCM
				_fft.rescale (chn._buf_bins.data ());
				_fft.do_ifft (chn._buf_bins.data (), _buf_pcm.data ());

				// Output frame windowing and overlap
				_frame_win.process_frame_mul (_buf_pcm.data ());
				chn._fo_syn.set_frame (_buf_pcm.data ());
			}

			pos += work_len;
		}
		while (pos < nbr_spl);
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



void	Crystalise::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._fo_ana.clear_buffers ();
		chn._fo_syn.clear_buffers ();
		std::fill (chn._buf_bins.begin (), chn._buf_bins.end (), 0.f);
		std::fill (chn._weight_arr.begin (), chn._weight_arr.end (), 0);
	}
}



void	Crystalise::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		_cryst_amt = float (_state_set.get_val_end_nat (Param_AMOUNT));
	}
}



void	Crystalise::compute_fft_var (double sample_freq) noexcept
{
	_p.update (sample_freq, _fft_len_l2_min);

#if defined (fstb_HAS_SIMD)
	// Vector and scalar ranges and indexes
	_bin_end_vec = _p._bin_beg + ((_p._bin_end - _p._bin_beg) & ~(_simd_w - 1));
	_bin_beg_sca = _bin_end_vec;
#endif
}



void	Crystalise::synthesise_bins (Channel &chn) noexcept
{
	if (_cryst_amt >= 1e-3f)
	{
		crystalise_precomp_mag (_buf_pcm, chn._buf_bins);
		crystalise_analyse (chn._weight_arr, _buf_pcm);
		crystalise_decimate (chn._buf_bins, chn._weight_arr);
	}
}



// Zeroes the look-up margins for the precomputed squared bin magnitudes.
// Recycles _buf_pcm for this use
void	Crystalise::crystalise_precomp_mag (std::vector <float> &mag_arr, const std::vector <float> &buf_bins) noexcept
{
	const auto     it_sq_mag_beg = mag_arr.begin ();
	std::fill (
		it_sq_mag_beg,
		it_sq_mag_beg + _cryst_ofs + _p._bin_beg,
		0.f
	);
	std::fill (
		it_sq_mag_beg + _cryst_ofs + _p._bin_end,
		it_sq_mag_beg + _cryst_ofs + _p._bin_end + _cryst_rad,
		0.f
	);

	// Precomputes the squared bin magnitudes
	const int      img_ofs = _p._nbr_bins;

#if defined (fstb_HAS_SIMD)

	for (int bin_idx = _p._bin_beg; bin_idx < _bin_end_vec; bin_idx += _simd_w)
	{
		const auto     img_idx = bin_idx + img_ofs;
		const auto     re = fstb::Vf32::loadu (&buf_bins [bin_idx]);
		const auto     im = fstb::Vf32::loadu (&buf_bins [img_idx]);
		const auto     ma = re * re + im * im;
		ma.storeu (&mag_arr [_cryst_ofs + bin_idx]);
	}

#endif // fstb_HAS_SIMD

	for (int bin_idx = _bin_beg_sca; bin_idx < _p._bin_end; ++bin_idx)
	{
		const auto     img_idx = bin_idx + img_ofs;
		const auto     mag_sq  =
			  fstb::sq (buf_bins [bin_idx])
			+ fstb::sq (buf_bins [img_idx]);
		mag_arr [_cryst_ofs + bin_idx] = mag_sq;
	}
}



// Analysis: finds to which extent each bin is a local maximum.
// Gives 0 for a local minimum and _cryst_rad * 2 for the largest span
void	Crystalise::crystalise_analyse (std::vector <int32_t> &weight_arr, const std::vector <float> &mag_arr) noexcept
{
#if defined (fstb_HAS_SIMD)

	using Vf32 = fstb::Vf32;
	using Vs32 = fstb::Vs32;

	for (int main_idx = _p._bin_beg; main_idx < _bin_end_vec; main_idx += _simd_w)
	{
		const auto     bin_mag_sq =
			Vf32::loadu (&mag_arr [_cryst_ofs + main_idx]);

		// Contains the local maximum aera { forward, backwards }.
		// Defaults at the maximum possible
		std::array <Vs32, 2>  dist_arr { Vs32 (_cryst_rad), Vs32 (_cryst_rad) };

		// Scans in both directions
		int            dir = 1; // Starts with forward search
		for (int dir_idx = 0; dir_idx < 2; ++dir_idx)
		{
			// We use a negative increment ("inc_neg") because using 0 or -1
			// (instead of +1) makes it a valid mask for testing with or_h().
			auto           dist    = Vs32::zero ();
			auto           inc_neg = Vs32 (-1);
			for (int k = 1; k <= _cryst_rad && inc_neg.or_h (); ++k)
			{
				const auto     pos_rel    = k * dir;
				const auto     tst_idx    = main_idx + pos_rel;
				const auto     tst_mag_sq =
					Vf32::loadu (&mag_arr [_cryst_ofs + tst_idx]);
				const auto     ge_mask    = (bin_mag_sq >= tst_mag_sq);
				inc_neg &= fstb::ToolsSimd::cast_s32 (ge_mask);
				dist -= inc_neg;
			}
			dist_arr [dir_idx] = dist;

			dir = -1; // Goes backwards at the next iteration
		}

		// Stores the result
		const auto     dist_tot = dist_arr [0] + dist_arr [1];
		dist_tot.storeu (&weight_arr [main_idx]);
	}

#endif // fstb_HAS_SIMD

	for (int main_idx = _bin_beg_sca; main_idx < _p._bin_end; ++main_idx)
	{
		const auto     bin_mag_sq = mag_arr [_cryst_ofs + main_idx];

		// Contains the local maximum aera { forward, backwards }.
		// Defaults at the maximum possible
		std::array <int, 2>  dist_arr { _cryst_rad, _cryst_rad };

		// Scans in both directions
		int            dir = 1; // Starts with forward search
		for (int dir_idx = 0; dir_idx < 2; ++dir_idx)
		{
			for (int k = 1; k <= _cryst_rad; ++k)
			{
				const auto     pos_rel    = k * dir;
				const auto     tst_idx    = main_idx + pos_rel;
				const auto     tst_mag_sq = mag_arr [_cryst_ofs + tst_idx];
				if (bin_mag_sq < tst_mag_sq)
				{
					// Another maximum: stops here and updates the area
					dist_arr [dir_idx] = k - 1;
					break;
				}
			}
			dir = -1; // Goes backwards at the next iteration
		}

		// Stores the result
		const auto     dist_sum = int32_t (dist_arr [0] + dist_arr [1]);
		weight_arr [main_idx] = dist_sum; 
	}
}



// Floating point threshold with a smooth step function
void	Crystalise::crystalise_decimate (std::vector <float> &buf_bins, const std::vector <int32_t> &weight_arr) noexcept
{
	// Curvature for the amount -> threshold mapping
	constexpr auto curve   = 0.625f;

	// The step spans over 2 units, making the transition a bit smoother
	constexpr auto step_sz = 2.f;
	constexpr auto scale_x = 1.f / step_sz;

	constexpr auto max_dia = float (_cryst_rad * 2);
	const auto     amt_map = _cryst_amt * (_cryst_amt * curve + (1.f - curve));
	const auto     thr     = amt_map * max_dia;
	const auto     img_ofs = _p._nbr_bins;

#if defined (fstb_HAS_SIMD)

	using Vf32 = fstb::Vf32;
	using Vs32 = fstb::Vs32;

	const auto     zero  = Vf32::zero ();
	const auto     one   = Vf32 (1);
	const auto     two   = Vf32 (2);
	const auto     three = Vf32 (3);
	const auto     thr_v = Vf32 (thr);
	const auto     scx_v = Vf32 (scale_x);

	for (int bin_idx = _p._bin_beg; bin_idx < _bin_end_vec; bin_idx += _simd_w)
	{
		const auto     img_idx = bin_idx + img_ofs;

		const auto     weight  = fstb::ToolsSimd::conv_s32_to_f32 (
			Vs32::loadu (&weight_arr [bin_idx])
		);
		const auto     dif  = thr_v - weight;
		const auto     s_in = limit (dif * scx_v, zero, one);

		// Sigmoid, inverted: 0 -> 1 and 1 -> 0
		const auto     gate = one - fstb::sq (s_in) * (three - s_in * two);

		const auto     re   = Vf32::loadu (&buf_bins [bin_idx]) * gate;
		const auto     im   = Vf32::loadu (&buf_bins [img_idx]) * gate;
		re.storeu (&buf_bins [bin_idx]);
		im.storeu (&buf_bins [img_idx]);
	}

#endif // fstb_HAS_SIMD

	for (int bin_idx = _bin_beg_sca; bin_idx < _p._bin_end; ++bin_idx)
	{
		const auto     weight = float (weight_arr [bin_idx]);
		if (weight < thr)
		{
			const auto     img_idx = bin_idx + img_ofs;
			float          gate    = 0;
			const auto     dif     = thr - weight;
			const auto     s_in    = dif * scale_x;
			if (s_in < 1)
			{
				// Sigmoid, inverted: 0 -> 1 and 1 -> 0
				gate = 1 - fstb::sq (dif) * (3 - dif * 2);
			}
			buf_bins [bin_idx] *= gate;
			buf_bins [img_idx] *= gate;
		}
	}
}



}  // namespace crysta
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
