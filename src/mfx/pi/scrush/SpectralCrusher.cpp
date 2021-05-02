/*****************************************************************************

        SpectralCrusher.cpp
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
#include "mfx/pi/scrush/Param.h"
#include "mfx/pi/scrush/SpectralCrusher.h"
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
namespace scrush
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpectralCrusher::SpectralCrusher ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_STEP , 2);
	_state_set.set_val_nat (desc_set, Param_BIAS , 0.5);
	_state_set.set_val (              Param_THR  , 0);
	_state_set.set_val_nat (desc_set, Param_SHAPE, 1);
	_state_set.set_val_nat (desc_set, Param_DIF  , 0);
	_state_set.set_val (              Param_LB   , 0);
	_state_set.set_val (              Param_HB   , 1);

	_state_set.add_observer (Param_STEP , _param_change_flag_misc);
	_state_set.add_observer (Param_BIAS , _param_change_flag_misc);
	_state_set.add_observer (Param_THR  , _param_change_flag_misc);
	_state_set.add_observer (Param_SHAPE, _param_change_flag_misc);
	_state_set.add_observer (Param_DIF  , _param_change_flag_misc);
	_state_set.add_observer (Param_LB   , _param_change_flag_freq);
	_state_set.add_observer (Param_HB   , _param_change_flag_freq);

	_param_change_flag_misc.add_observer (_param_change_flag);
	_param_change_flag_freq.add_observer (_param_change_flag);

	for (auto &chn : _chn_arr)
	{
		chn._fo_ana.setup (_fft_len, _overlap, 0);
		chn._fo_syn.setup (_fft_len, _overlap, 0);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	SpectralCrusher::do_get_state () const
{
	return _state;
}



double	SpectralCrusher::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	SpectralCrusher::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	/*** To do:
		Resampling to 44.1 or 48 kHz when the sampling rate is higher, so the
		sound remains more or less the same whatever the sampling rate.
	***/

	latency      = _fft_len;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_frame_win.setup (_fft_len);

	_param_change_flag_misc.set ();
	_param_change_flag_freq.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	SpectralCrusher::do_process_block (piapi::ProcInfo &proc)
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
			const int      rem_len = chn._fo_ana.get_len_before_next_frame ();
			assert (rem_len == chn._fo_syn.get_len_before_next_frame ());
			const int      work_len = std::min (rem_len, nbr_spl - pos);

			chn._fo_ana.process_block (proc._src_arr [chn_index] + pos, work_len);
			chn._fo_syn.process_block (proc._dst_arr [chn_index] + pos, work_len);

			const bool     frame_req_flag = chn._fo_ana.is_frame_proc_required ();
			assert (frame_req_flag == chn._fo_syn.is_frame_proc_required ());
			if (frame_req_flag)
			{
				// Input frame extraction and windowing
				chn._fo_ana.get_frame (_buf_pcm.data ());
				_frame_win.process_frame_mul (_buf_pcm.data ());

				// PCM -> frequency bins
				_fft.do_fft (_buf_bins.data (), _buf_pcm.data ());

				// Frequency domain processing
				mutilate_bins ();

				// Frequency bins -> PCM
				_fft.rescale (_buf_bins.data ());
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



void	SpectralCrusher::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._fo_ana.clear_buffers ();
		chn._fo_syn.clear_buffers ();
	}
}



void	SpectralCrusher::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		// Misc param
		if (_param_change_flag_misc (true) || force_flag)
		{
			const float    amt = float (_state_set.get_val_end_nat (Param_STEP));
			_qt_step     = amt * 2; // * 2 because we compute squared values
			_qt_step_inv = 1.f / _qt_step;

			const float    bias = float (_state_set.get_val_end_nat (Param_BIAS));
			_qt_bias   = std::min (bias, 0.999f);

			const float    thr = float (_state_set.get_val_end_nat (Param_THR));
			_threshold = (thr * thr) * _fft_len;

			_qt_shape  = float (_state_set.get_val_end_nat (Param_SHAPE));
			_lin_dif   = float (_state_set.get_val_end_nat (Param_DIF));
		}

		// Processed frequency band
		if (_param_change_flag_freq (true) || force_flag)
		{
			float          lb_freq = float (_state_set.get_val_end_nat (Param_LB));
			float          hb_freq = float (_state_set.get_val_end_nat (Param_HB));
			if (hb_freq < lb_freq)
			{
				std::swap (lb_freq, hb_freq);
			}
			_bin_pbeg = std::max (conv_freq_to_bin (lb_freq), _bin_beg);
			_bin_pend = std::min (conv_freq_to_bin (hb_freq), _bin_end);
		}
	}
}



int	SpectralCrusher::conv_freq_to_bin (float f) const noexcept
{
	assert (f >= 0);
	assert (f < _sample_freq * 0.5f);

	return fstb::round_int (f * _inv_fs * _fft_len);
}



void	SpectralCrusher::mutilate_bins () noexcept
{
	for (int bin_idx = _bin_pbeg; bin_idx < _bin_pend; ++bin_idx)
	{
		// Loads bin
		const std::complex <float> bin_org {
			_buf_bins [bin_idx            ],
			_buf_bins [bin_idx + _nbr_bins]
		};
		auto           bin    = bin_org;

		// Squared module, log scale
		const float    n2_old =
			fstb::sq (bin.real ()) + fstb::sq (bin.imag ()) + _threshold;
		const float    n2_old_l2 = fstb::Approx::log2 (n2_old);

		// Quantified
		const float    n2_qnt_l2 =
			_qt_step * fstb::trunc_int (n2_old_l2 * _qt_step_inv + _qt_bias);
		const float    n2_new_l2 =
			n2_old_l2 + _qt_shape * (n2_qnt_l2 - n2_old_l2);

		// Ratio between both amplitudes
		const float    ratio2_l2 = n2_new_l2 - n2_old_l2;
		const float    ratio = fstb::Approx::exp2 (ratio2_l2 * 0.5f);

		// Rescales the bin
		bin *= ratio;
		bin -= bin_org * _lin_dif;

		// Stores bin
		_buf_bins [bin_idx            ] = bin.real ();
		_buf_bins [bin_idx + _nbr_bins] = bin.imag ();
	}
}



}  // namespace scrush
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
