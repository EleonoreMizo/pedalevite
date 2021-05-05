/*****************************************************************************

        SpectralCrusher.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_scrush_SpectralCrusher_HEADER_INCLUDED)
#define mfx_pi_scrush_SpectralCrusher_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTReal.h"
#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/spec/FrameOverlapAna.h"
#include "mfx/dsp/spec/FrameOverlapSyn.h"
#include "mfx/dsp/wnd/ProcHann.h"
#include "mfx/pi/scrush/Cst.h"
#include "mfx/pi/scrush/SpectralCrusherDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{
namespace pi
{
namespace scrush
{



class SpectralCrusher final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SpectralCrusher ();
	               ~SpectralCrusher () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// log2 of the ratio frame size / hop size. >= 1
	static constexpr int _hop_ratio_l2  = 2;

	// First bin to process. Bin 0 is DC
	static constexpr int _bin_beg       = 1;

	static constexpr int _nbr_fft_sizes =
		Cst::_fft_len_l2_max + 1 - Cst::_fft_len_l2_min;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		// Analysis and synthesis overlappers
		dsp::spec::FrameOverlapAna <float>
		               _fo_ana;
		dsp::spec::FrameOverlapSyn <float>
		               _fo_syn;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	typedef ffft::FFTReal <float> FftType;
	typedef std::unique_ptr <FftType> FftUPtr;

	void           clear_buffers () noexcept;
	void           update_param (bool force_flag = false) noexcept;
	void           mutilate_bins () noexcept;
	void           set_fft_param (int fft_len_l2) noexcept;
	void           update_bin_range () noexcept;
	int            conv_freq_to_bin (float f) const noexcept;

	State          _state = State_CREATED;

	SpectralCrusherDesc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	float          _inv_fs      = 0;    // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_freq;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_res;

	ChannelArray   _chn_arr;

	std::array <FftUPtr, _nbr_fft_sizes>
	               _fft_uptr_arr;
	FftType *      _fft_cur_ptr = nullptr;
	dsp::wnd::ProcHann <float>
	               _frame_win;
	BufAlign       _buf_pcm;
	BufAlign       _buf_bins;

	// Threshold, added to the bin magnitude.
	// Actually it's a squared value. > 0.
	float          _threshold   = 1e-10f;

	// Quantization step, applied to the log2 of the squared magnitude. > 0.
	float          _qt_step     = 2;
	float          _qt_step_inv = 1.f / _qt_step;

	// Quantization rounding bias (0 = floor, 0.5 = round, 0.999.. = ceil)
	float          _qt_bias     = 0.5f;

	// Shape of the step of the quantization difference.
	// 0 = none, 1 = plain quantization, > 1 = inverted
	float          _qt_shape    = 1;

	// Amount of original bin subtracted to the modified bin. [0 ; 1]
	float          _lin_dif     = 0;

	// Frequency range for the effect, Hz
	float          _freq_min    = 20;
	float          _freq_max    = 20480;

	// Amplification limit, linear value
	float          _amp_limit   = 10;

	// Base-2 log of the FFT length, in samples
	int            _fft_len_l2  = Cst::_fft_len_l2_min;
	int            _fft_len     = 1 << _fft_len_l2;

	// Base-2 log of the overlap, in samples. Must be <= _fft_len_l2
	int            _hop_size_l2 = _fft_len_l2 - _hop_ratio_l2;
	int            _hop_size    = 1 << _hop_size_l2;

	// Range of "useful" bins. DC is 0 and Nyquist is _bin_end
	int            _nbr_bins    = _fft_len / 2;
	int            _bin_end     = _nbr_bins;

	// Bin indexes defining the range to be processed. [1 ; _nbr_bins[
	// Other bins are left untouched
	int            _bin_pbeg    = _bin_beg;
	int            _bin_pend    = _bin_end;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SpectralCrusher (const SpectralCrusher &other)   = delete;
	               SpectralCrusher (SpectralCrusher &&other)        = delete;
	SpectralCrusher &
	               operator = (const SpectralCrusher &other)        = delete;
	SpectralCrusher &
	               operator = (SpectralCrusher &&other)             = delete;
	bool           operator == (const SpectralCrusher &other) const = delete;
	bool           operator != (const SpectralCrusher &other) const = delete;

}; // class SpectralCrusher



}  // namespace scrush
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/scrush/SpectralCrusher.hpp"



#endif   // mfx_pi_scrush_SpectralCrusher_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
