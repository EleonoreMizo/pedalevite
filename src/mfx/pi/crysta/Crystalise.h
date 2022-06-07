/*****************************************************************************

        Crystalise.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_crysta_Crystalise_HEADER_INCLUDED)
#define mfx_pi_crysta_Crystalise_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/spec/FrameOverlapAna.h"
#include "mfx/dsp/spec/FrameOverlapSyn.h"
#include "mfx/dsp/wnd/ProcHann.h"
#include "mfx/pi/cdsp/FFTRealRange.h"
#include "mfx/pi/crysta/CrystaliseDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace pi
{
namespace crysta
{



class Crystalise final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Crystalise (piapi::HostInterface &host);
	               ~Crystalise () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Base-2 log of the FFT length, in samples
	// _fft_len_l2_min is used at standard rates (< 50 kHz)
	static constexpr int _fft_len_l2_min = 8;
	static constexpr int _fft_len_l2_max = 16;

	// Range for all bins. DC is 0 and Nyquist is _bin_top
	static constexpr int _bin_beg   = 1;

	// Radius for the crystalise effect, in bins.
	static constexpr int _cryst_rad = 16;

	// Offset of the 0th bin in _buf_pcm when used to store the bin magnitudes
	// The offset is required in order to provide some margins to avoid doing
	// tests for boundaries during analysis pass for the Crystalise effect.
	static constexpr int _cryst_ofs = _cryst_rad;

#if defined (fstb_HAS_SIMD)
	static constexpr int _simd_w = 4;
#endif

	class Channel
	{
	public:
		// Analysis and synthesis overlappers
		dsp::spec::FrameOverlapAna <float>
		               _fo_ana;
		dsp::spec::FrameOverlapSyn <float>
		               _fo_syn;
		std::vector <float>          // Length: _fft_len
		               _buf_bins;
		std::vector <int32_t>        // Length: _nbr_bins. Only [_bin_beg ; bin_end[ range is valid
		               _weight_arr;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           compute_fft_var (double sample_freq) noexcept;
	void           synthesise_bins (Channel &chn) noexcept;
	void           crystalise_precomp_mag (std::vector <float> &mag_arr, const std::vector <float> &buf_bins) noexcept;
	void           crystalise_analyse (std::vector <int32_t> &weight_arr, const std::vector <float> &mag_arr) noexcept;
	void           crystalise_decimate (std::vector <float> &buf_bins, const std::vector <int32_t> &weight_arr) noexcept;

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	CrystaliseDesc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	float          _inv_fs      = 0;    // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	// Base-2 log of the FFT length, in samples
	int            _fft_len_l2  = 0;
	int            _fft_len     = 0;

	// Range for all bins. DC is 0 and Nyquist is _bin_top
	int            _nbr_bins    = 0;
	int            _bin_top     = 0;

	// Base-2 log of the hop size, in samples. Must be <= _fft_len_l2
	int            _hop_size_l2 = 0;
	int            _hop_size    = 0;
	int            _hop_ratio   = 0;

	// FFT normalisation factor combined with window scaling to compensate
	// for the amplitude change caused by the overlap.
	float          _scale_amp   = 0;

	// Last bin + 1 being processed. Other bins (ultrasonic content) are cleared
	int            _bin_end     = 0;

	// Vector and scalar ranges and indexes
#if defined (fstb_HAS_SIMD)
	int            _bin_end_vec = 0;
	int            _bin_beg_sca = 0;
#else
	int            _bin_beg_sca = _bin_beg;
#endif

	ChannelArray   _chn_arr;

	cdsp::FFTRealRange <_fft_len_l2_min, _fft_len_l2_max>
	               _fft;
	dsp::wnd::ProcHann <float, false>
	               _frame_win;
	std::vector <float>          // Length: _fft_len. Also hosts the precalculated squared magnitudes for the crystalise effect
	               _buf_pcm;

	float          _cryst_amt  = 0; // Crystalise amount, [0 ; 1]. 0 = disabled.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Crystalise ()                               = delete;
	               Crystalise (const Crystalise &other)        = delete;
	               Crystalise (Crystalise &&other)             = delete;
	Crystalise &   operator = (const Crystalise &other)        = delete;
	Crystalise &   operator = (Crystalise &&other)             = delete;
	bool           operator == (const Crystalise &other) const = delete;
	bool           operator != (const Crystalise &other) const = delete;

}; // class Crystalise



}  // namespace crysta
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/crysta/Crystalise.hpp"



#endif   // mfx_pi_crysta_Crystalise_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
