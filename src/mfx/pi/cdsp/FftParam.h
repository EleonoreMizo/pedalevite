/*****************************************************************************

        FftParam.h
        Author: Laurent de Soras, 2022

update() must be called at least once.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FftParam_HEADER_INCLUDED)
#define mfx_pi_cdsp_FftParam_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace cdsp
{



template <int LL2_MIN, int LL2_MAX>
class FftParam
{
	static_assert (LL2_MIN >= 0, "");
	static_assert (LL2_MIN <= LL2_MAX, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Thershold in Hz for the sampling rate above which we switch to the next
	// FFT length, and so forth for the next Fs * 2^N.
	static constexpr double _thr_fs    = 50'000;

	// Upper bound of the processed bandwidth, in Hz
	static constexpr double _bandwidth = 20'000;

	// Range for all bins. DC is 0 and Nyquist is _bin_top
	static constexpr int _bin_beg = 1;

	// Base-2 log of the FFT length, in samples
	// _fft_len_l2_min is used at standard rates (< 50 kHz)
	static constexpr int _fft_len_l2_min = LL2_MIN;
	static constexpr int _fft_len_l2_max = LL2_MAX;

	               FftParam ();
	               FftParam (const FftParam &other)          = default;
	               FftParam (FftParam &&other)               = default;

	FftParam &     operator = (const FftParam &other)        = default;
	FftParam &     operator = (FftParam &&other)             = default;

	void           update (double sample_freq, int target_ll2, int hop_ratio);

	// Base-2 log of the FFT length, in samples
	int            _fft_len_l2  = 0;
	int            _fft_len     = 0;

	// Range for all bins. DC is 0 and Nyquist is _bin_top
	int            _nbr_bins    = 0;
	int            _bin_top     = 0;

	// Hop size between two transforms, in samples.
	int            _hop_size    = 0;
	int            _hop_ratio   = 0;

	// FFT normalisation factor combined with window scaling to compensate
	// for the amplitude change caused by the overlap.
	float          _scale_amp   = 0;

	// Last bin + 1 being processed. Other bins (ultrasonic content) are cleared
	int            _bin_end     = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class FftParam



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/cdsp/FftParam.hpp"



#endif // mfx_pi_cdsp_FftParam_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
