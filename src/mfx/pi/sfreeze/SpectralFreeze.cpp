/*****************************************************************************

        SpectralFreeze.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/wnd/XFadeEqPowPoly8.h"
#include "mfx/pi/sfreeze/Param.h"
#include "mfx/pi/sfreeze/SpectralFreeze.h"
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
namespace sfreeze
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpectralFreeze::SpectralFreeze ()
{
	dsp::mix::Align::setup ();
	dsp::mix::Generic::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
	{
		const int      base = Param_SLOT_BASE + slot_idx * ParamSlot_NBR_ELT;
		auto &         pcf  = _param_change_flag_slot_arr [slot_idx];

		_state_set.set_val_nat (desc_set, base + ParamSlot_FREEZE, 0);
		_state_set.set_val_nat (desc_set, base + ParamSlot_GAIN  , 1);

		_state_set.add_observer (base + ParamSlot_FREEZE, pcf);
		_state_set.add_observer (base + ParamSlot_GAIN  , pcf);

		pcf.add_observer (_param_change_flag);
	}

	_state_set.set_val_nat (desc_set, Param_XFADE , 0);
	_state_set.set_val_nat (desc_set, Param_XFGAIN, 0);
	_state_set.set_val_nat (desc_set, Param_PHASE , 0);

	_state_set.add_observer (Param_XFADE , _param_change_flag_misc);
	_state_set.add_observer (Param_XFGAIN, _param_change_flag_misc);
	_state_set.add_observer (Param_PHASE , _param_change_flag_misc);

	_param_change_flag_misc.add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	SpectralFreeze::do_get_state () const
{
	return _state;
}



double	SpectralFreeze::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	SpectralFreeze::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	/*** To do:
		Resampling to 44.1 or 48 kHz when the sampling rate is higher, so the
		sound remains more or less the same whatever the sampling rate.
	***/

	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

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
		// We have no real latency issue for a freeze, so we can interleave
		// analysis and synthesis too.
		// Ideally we should use bit reversal if _max_nbr_chn is high.
		if (max_buf_len < _hop_size)
		{
			ofs_com = _hop_size * chn_idx / _max_nbr_chn;
			ofs_syn = max_buf_len;
		}

		chn._fo_ana.setup (_fft_len, _hop_size, ofs_com);
		chn._fo_syn.setup (_fft_len, _hop_size, ofs_com + ofs_syn);

		for (auto &slot : chn._slot_arr)
		{
			slot._frz_state = FreezeState::NONE;
			slot._nbr_hops  = 0;
		}
	}

	_frame_win.setup (_fft_len);

	for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
	{
		auto &         pcf  = _param_change_flag_slot_arr [slot_idx];
		pcf.set ();
	}

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	SpectralFreeze::do_process_block (piapi::ProcInfo &proc)
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
			dsp::mix::Generic::mix_1_1 (
				proc._dst_arr [chn_index] + pos,
				proc._src_arr [chn_index] + pos,
				work_len
			);

			const bool     req_ana_flag = chn._fo_ana.is_frame_proc_required ();
			const bool     req_syn_flag = chn._fo_syn.is_frame_proc_required ();

			if (req_ana_flag)
			{
				// Input frame extraction and windowing
				chn._fo_ana.get_frame (_buf_pcm.data ());
				_frame_win.process_frame_mul (_buf_pcm.data ());

				// PCM -> frequency bins
				_fft.do_fft (_buf_bins.data (), _buf_pcm.data ());

				// Analysis
				analyse_bins (chn);
			}

			if (req_syn_flag)
			{
				// Synthesis
				synthesise_bins (chn);

				// Frequency bins -> PCM
				_fft.do_ifft (_buf_bins.data (), _buf_pcm.data ());

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



void	SpectralFreeze::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._fo_ana.clear_buffers ();
		chn._fo_syn.clear_buffers ();
	}
}



void	SpectralFreeze::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		// Slot parameters
		for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
		{
			const int      base = Param_SLOT_BASE + slot_idx * ParamSlot_NBR_ELT;

			if (_param_change_flag_slot_arr [slot_idx] (true) || force_flag)
			{
				const bool     frz_flag =
					_state_set.get_val_bool (base + ParamSlot_FREEZE);
				const float    gain     =
					float (_state_set.get_val_end_nat (base + ParamSlot_GAIN));

				for (auto &chn : _chn_arr)
				{
					auto &         slot = chn._slot_arr [slot_idx];

					if (! frz_flag)
					{
						slot._frz_state = FreezeState::NONE;
					}
					else if (slot._frz_state == FreezeState::NONE)
					{
						slot._frz_state = FreezeState::CAPTURE1;
					}

					slot._gain = gain;
				}
			}
		}

		// Misc
		if (_param_change_flag_misc (true) || force_flag)
		{
			_xfade_pos  = float (_state_set.get_val_end_nat (Param_XFADE ));
			_xfade_gain = float (_state_set.get_val_end_nat (Param_XFGAIN));

			const float   speed   =
				float (_state_set.get_val_end_nat (Param_PHASE));
			const float   hop_dur = _inv_fs * _hop_size;
			// * 0.5f because the phase difference is set in both directions
			_phasing = speed * hop_dur * 0.5f;
		}
	}
}



int	SpectralFreeze::conv_freq_to_bin (float f) const noexcept
{
	assert (f >= 0);
	assert (f < _sample_freq * 0.5f);

	return fstb::round_int (f * _inv_fs * _fft_len);
}



void	SpectralFreeze::analyse_bins (Channel &chn) noexcept
{
	for (auto &slot : chn._slot_arr)
	{
		if (slot._frz_state == FreezeState::CAPTURE1)
		{
			analyse_capture1 (slot);
		}
		else if (slot._frz_state == FreezeState::CAPTURE2)
		{
			analyse_capture2 (slot);
		}
	}
}



void	SpectralFreeze::analyse_capture1 (Slot &slot) noexcept
{
	// Normalizes each bin, so we keep only its phase information
	for (int bin_idx = _bin_beg; bin_idx < _nbr_bins; ++bin_idx)
	{
		std::complex <float> bin {
			_buf_bins [bin_idx            ],
			_buf_bins [bin_idx + _nbr_bins]
		};

		/*** To do: optimise this with a fast inverse square root ***/
		const float    mag = std::abs (bin);
		if (fstb::is_null (mag))
		{
			bin = std::complex <float> { 1, 0 };
		}
		else
		{
			const float    inv_mag = (mag > 0) ? 1.f / mag : 0;
			bin *= inv_mag;
		}

		slot._buf_freeze [bin_idx            ] = bin.real ();
		slot._buf_freeze [bin_idx + _nbr_bins] = bin.imag ();
	}

	slot._frz_state = FreezeState::CAPTURE2;
}



void	SpectralFreeze::analyse_capture2 (Slot &slot) noexcept
{
	// For each bin, evaluates the phase difference between this frame and
	// the previous one.
	for (int bin_idx = _bin_beg; bin_idx < _nbr_bins; ++bin_idx)
	{
		std::complex <float> bin {
			_buf_bins [bin_idx            ],
			_buf_bins [bin_idx + _nbr_bins]
		};
		std::complex <float> bin_old {
			slot._buf_freeze [bin_idx            ],
			slot._buf_freeze [bin_idx + _nbr_bins]
		};

		// Phase difference
		float          arg_n = 0;
		const float    b0r   = bin_old.real ();
		const float    b0i   = bin_old.imag ();
		if (! fstb::is_null (fabsf (b0r) + fabsf (b0i)))
		{
			// Division formula without scaling: bin / bin_old = b1 / b0
			const float    b1r   = bin.real ();
			const float    b1i   = bin.imag ();
			const float    dr    = b1r * b0r + b1i * b0i;
			const float    di    = b1i * b0r - b1r * b0i;

			// Extracts the angle
			// If the 3rd-order approximation is not accurate enough, we can
			// still use the 7th-order one.
			const float    angle = fstb::Approx::atan2_3th (di, dr);

			// Scales the resulting phase from [-pi ; +pi] to [-1/2; +1/2]
			arg_n = angle * float (0.5 / fstb::PI);
		}

		slot._buf_freeze [bin_idx            ] = std::abs (bin);
		slot._buf_freeze [bin_idx + _nbr_bins] = arg_n;
	}

	slot._frz_state = FreezeState::REPLAY;
	slot._nbr_hops  = 1;
}



void	SpectralFreeze::synthesise_bins (Channel &chn) noexcept
{
	_buf_bins.fill (0.f);

	for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
	{
		auto &         slot = chn._slot_arr [slot_idx];
		if (slot._frz_state == FreezeState::REPLAY)
		{
			float          gain       = slot._gain;

			// 0 = pure slot, >= 1 = silent
			float          xf_pos_rel = std::min (
				fabsf (_xfade_pos - slot_idx),
				fabsf (_xfade_pos - slot_idx - Cst::_nbr_slots)
			);
			if (xf_pos_rel < 1)
			{
#if 1
				// This curve is a bit flat at the top which is great when
				// automating/controlling the crossfade position, so the "pure"
				// slot position is a bit wider and therefore less sensitive to
				// position errors. Calculation complexity is similar to the
				// sine curve.
				const auto     xf_res  =
					dsp::wnd::XFadeEqPowPoly8 <false>::compute_gain (xf_pos_rel);
				const float    gain_xf = xf_res [0];
#else
				// Old version, for reference: sine-based crossfade curve
				const float    gain_xf =
					fstb::Approx::sin_rbj_halfpi (1 - xf_pos_rel);
#endif
				gain += gain_xf * _xfade_gain;
			}

			synthesise_playback (slot, gain);
		}
	}
}



void	SpectralFreeze::synthesise_playback (Slot &slot, float gain) noexcept
{
	++ slot._nbr_hops;

	slot._phase_acc += _phasing;
	if (slot._phase_acc >= 1)
	{
		slot._phase_acc -= 1;
	}

	// FFT normalisation factor combined with window scaling
	constexpr int     hop_ratio = 1 << (_fft_len_l2 - _hop_size_l2);
	constexpr float   scale_win = (hop_ratio <= 2) ? 1 : 8.f / (3 * hop_ratio);
	constexpr float   scale_fft = 1.f / _fft_len;
	constexpr float   scale     = scale_fft * scale_win;

	const float       gain_sc   = gain * scale;
	constexpr float   gain_thr  = 1e-6f; // -120 dB

	if (gain_sc >= gain_thr)
	{
		float          phase_val = slot._phase_acc * ((_bin_beg & 1) * 2 - 1);
		for (int bin_idx = _bin_beg; bin_idx < _nbr_bins; ++bin_idx)
		{
			const float    mag   = slot._buf_freeze [bin_idx            ];
			float          arg_n = slot._buf_freeze [bin_idx + _nbr_bins];

			// Reports the phase difference between the two initial frames
			arg_n *= slot._nbr_hops;

			// Experimental
			arg_n += phase_val;

			// Why 0.5? Got this by trial & error. Maths behind this are clear
			// like mud
			const float    omega = 0.5f * bin_idx;
			arg_n += omega;

			// Keeps [-1/2; +1/2] range
			arg_n -= float (fstb::round_int (arg_n));

			const float    mag_gain = mag * gain_sc;
			const auto     cs = fstb::Approx::cos_sin_nick_2pi (arg_n);
			const float    br = cs [0] * mag_gain;
			const float    bi = cs [1] * mag_gain;

			_buf_bins [bin_idx            ] += br;
			_buf_bins [bin_idx + _nbr_bins] += bi;

			// Next
			phase_val = -phase_val;
		}
	}
}



}  // namespace sfreeze
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
