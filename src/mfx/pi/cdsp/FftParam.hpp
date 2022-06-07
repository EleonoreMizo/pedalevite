/*****************************************************************************

        FftParam.hpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FftParam_CODEHEADER_INCLUDED)
#define mfx_pi_cdsp_FftParam_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cdsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int LL2_MIN, int LL2_MAX>
constexpr double FftParam <LL2_MIN, LL2_MAX>::_thr_fs;
template <int LL2_MIN, int LL2_MAX>
constexpr double FftParam <LL2_MIN, LL2_MAX>::_bandwidth;
template <int LL2_MIN, int LL2_MAX>
constexpr int FftParam <LL2_MIN, LL2_MAX>::_bin_beg;
template <int LL2_MIN, int LL2_MAX>
constexpr int FftParam <LL2_MIN, LL2_MAX>::_fft_len_l2_min;
template <int LL2_MIN, int LL2_MAX>
constexpr int FftParam <LL2_MIN, LL2_MAX>::_fft_len_l2_max;



template <int LL2_MIN, int LL2_MAX>
FftParam <LL2_MIN, LL2_MAX>::FftParam ()
{
	update (44100, _fft_len_l2_min);
}



template <int LL2_MIN, int LL2_MAX>
void	FftParam <LL2_MIN, LL2_MAX>::update (double sample_freq, int target_ll2)
{
	assert (sample_freq > 0);
	assert (target_ll2 >= _fft_len_l2_min);
	assert (target_ll2 <= _fft_len_l2_max);

	// Computes the FFT length for the given sampling rate.
	// Over 50 kHz, we double the default length each octave, so the bandwidth
	// per bin is kept more or less constant.
	_fft_len_l2 = target_ll2;
	while (
		   _fft_len_l2 < _fft_len_l2_max
	   && sample_freq > _thr_fs * double (1 << (_fft_len_l2 - _fft_len_l2_min))
	)
	{
		++ _fft_len_l2;
	}

	_fft_len  = 1 << _fft_len_l2;

	// Range for all bins. DC is 0 and Nyquist is _bin_top
	_nbr_bins = _fft_len / 2;
	_bin_top  = _nbr_bins;

	// Base-2 log of the hop size, in samples. Must be <= _fft_len_l2 - 2
	_hop_size_l2 = _fft_len_l2 - 2;
	_hop_size    = 1 << _hop_size_l2;
	_hop_ratio   = 1 << (_fft_len_l2 - _hop_size_l2);

	// FFT normalisation factor combined with window scaling to compensate
	// for the amplitude change caused by the overlap.
	const auto    scale_win =
		(_hop_ratio <= 2) ? 1.f : 8.f / float (3 * _hop_ratio);
	const auto    scale_fft = 1.f / _fft_len;
	_scale_amp   =  scale_fft * scale_win;

	// Last bin + 1 being processed. Other bins (ultrasonic content) are cleared
	const auto     nyquist_freq = sample_freq * 0.5;
	const auto     bin_end_raw  = fstb::ceil_int (
		double (_bin_top) * _bandwidth / nyquist_freq
	);
	_bin_end     = std::min (bin_end_raw, _bin_top);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#endif // mfx_pi_cdsp_FftParam_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
