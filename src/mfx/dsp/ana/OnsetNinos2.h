/*****************************************************************************

        OnsetNinos2.h
        Author: Laurent de Soras, 2021

Onset detector tuned for the guitar using a slightly modified NINOS2
algorithm.

The modification is about the delta additive parameter, which is assisted
with the multiplicative parameter lambda. This makes the algorithm more
independant of the overal volume of the input.

Template parameters:

- FFT: an ffft::FFTRealFixLen class. Recommended size: 2048 at 44.1 kHz

Reference:

Mina Mounir, Peter Karsmakers, Toon van Waterschoot,
Guitar Note Onset Detection Based on a Spectral Sparsity Measure,
24th European Signal Processing Conference (EUSIPCO), 2016, pp. 978-982,
doi: 10.1109/EUSIPCO.2016.7760394

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_OnsetNinos2_HEADER_INCLUDED)
#define mfx_dsp_ana_OnsetNinos2_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/spec/FrameOverlapAna.h"
#include "mfx/dsp/wnd/ProcHann.h"

#include <algorithm>
#include <array>



namespace mfx
{
namespace dsp
{
namespace ana
{



template <class FFT>
class OnsetNinos2
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef FFT FftType;

	static_assert (
		(FftType::FFT_LEN & (FftType::FFT_LEN - 1)) == 0,
		"FFT length must be a power of 2"
	);

	// Minimum analysis resolution, in seconds
	static constexpr float _min_resol_s = 0.001f;

	// Maximum delay (for the peak-picking parameters)
	static constexpr float _max_delay_s = 1.0f;

	               OnsetNinos2 ();
	               OnsetNinos2 (const OnsetNinos2 &other) = default;
	               OnsetNinos2 (OnsetNinos2 &&other)      = default;
						~OnsetNinos2 ()                        = default;
	OnsetNinos2 &  operator = (const OnsetNinos2 &other)  = default;
	OnsetNinos2 &  operator = (OnsetNinos2 &&other)       = default;

	void           set_sample_freq (double sample_freq) noexcept;

	void           set_resol (float resol_s) noexcept;
	void           set_gamma (float gamma) noexcept;
	void           set_pp_param (float alpha, float beta, float a, float b) noexcept;
	void           set_deadzone (float deadzone) noexcept;
	void           set_thr_add (float delta) noexcept;
	void           set_thr_mul (float lambda) noexcept;

	int            compute_latency () const noexcept;

	bool           analyse_sample (float x) noexcept;
	bool           analyse_block (const float src_ptr [], int nbr_spl) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _fft_len  = FftType::FFT_LEN;
	static constexpr int _nbr_bins = _fft_len / 2;

	inline int     compute_g_n (float gamma) const noexcept;
	inline float   compute_odf_norm_factor (int g_n) const noexcept;
	void           update_peak_pick_cst () noexcept;
	void           update_theta () noexcept;
	bool           analyse_new_frame () noexcept;
	float          compute_odf () const noexcept;
	float          compute_odf_local_max () const noexcept;
	float          compute_odf_avg () const noexcept;

	// Sampling rate, Hz. > 0
	float          _sample_freq = 0;

	// Slicer for the analysis frames
	spec::FrameOverlapAna <float>
	               _fo_ana;

	wnd::ProcHann <float>
	               _win;

	// FFT object for time-frequency conversions
	FftType        _fft;

	// Buffer for the frame in the time domain
	std::array <float, _fft_len>
	               _buf_frame;

	// Spectral buffer, mainly for the FFT bin squared magnitudes.
	// After magnitudes have been sorted, only the first _g_n values are valid.
	std::array <float, _fft_len>
	               _buf_mag2;

	// Distance between two analysis frames, in ]0 ; _fft_len]
	int            _hop_size = std::min <int> (256, _fft_len);

	// Ratio of magnitudes kept for analysis. ]0 ; 1]
	float          _gamma    = 0.94f;

	// Same as _gamma, but in number of bins. [1 ; _nbr_bins - 1].
	// Called J in the paper
	int            _g_n      = compute_g_n (_gamma);

	// Normalisation factor for the ODF. Depends on the number of magnitudes
	// taken into account.
	float          _odf_norm_fact = compute_odf_norm_factor (_g_n);

	// Peak-picking constants, in seconds. Default values taken from:
	// Sebastian Boeck, Gerhard Widmer,
	// Local Group Delay Based Vibrato and Tremolo Suppression for
	// Onset Detection,
	// Proc. 14th Int. Soc. Music Information Retrieval Conf. (ISMIR), 2013,
	// pp. 361-366
	float          _alpha_s  = 0.030f; // Local max, pre
	float          _beta_s   = 0.030f; // Local max, post
	float          _a_s      = 0.100f; // Moving average, pre
	float          _b_s      = 0.070f; // Moving average, post

	// Minimum time interval between two onsets. Defaulted to the frame
	// duration. Negative: not initialised
	float          _theta_s  = -1.f;

	// Peak-picking constants, in frames. Initialised after the sample rate
	// is set.
	int            _alpha    = 0;
	int            _beta     = 0;
	int            _a        = 0;
	int            _b        = 0;
	int            _theta    = 0;
	int            _delay    = 0;

	// Tunable threshold, additive. >= 0
	float          _delta    = 0.f;

	// Tunable threshold, multiplicative. >= 1
	float          _lambda   = 1.5f;

	// Number of frames since the last offset.
	// Initialised to a big number > _theta.
	int            _nfsl     = 1'000'000;

	dly::DelaySimple <float>
	               _odf_mem;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OnsetNinos2 &other) const = delete;
	bool           operator != (const OnsetNinos2 &other) const = delete;

}; // class OnsetNinos2



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ana/OnsetNinos2.hpp"



#endif   // mfx_dsp_ana_OnsetNinos2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
