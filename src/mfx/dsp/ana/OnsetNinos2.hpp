/*****************************************************************************

        OnsetNinos2.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ana_OnsetNinos2_CODEHEADER_INCLUDED)
#define mfx_dsp_ana_OnsetNinos2_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class FFT>
constexpr float	OnsetNinos2 <FFT>::_min_resol_s;
template <class FFT>
constexpr float	OnsetNinos2 <FFT>::_max_delay_s;



/*
==============================================================================
Name: ctor
Throws: standard container related exceptions
==============================================================================
*/

template <class FFT>
OnsetNinos2 <FFT>::OnsetNinos2 ()
{
	_fo_ana.setup (_fft_len, _hop_size, 0);
	_win.setup (_fft_len);

	// Reserves 1 second
	const int      max_dly = fstb::ceil_int (_max_delay_s / _min_resol_s);
	_odf_mem.setup (max_dly, 1);
}



/*
==============================================================================
Name: set_sample_freq
Description:
	Sets the sampling rate. Mandatory call before any other function.
Input parameters:
	- sample_freq: sampling frequency, Hz. > 0.
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	update_peak_pick_cst ();
	update_theta ();
}



/*
==============================================================================
Name: set_resol
Description:
	Sets the analysis resolution. The higher the resolution, the higher the
	processing load.
Input parameters:
	- resol_s: resolution in seconds, >= 1 ms. It is recommended to set it
		below 10 ms.
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_resol (float resol_s) noexcept
{
	assert (_sample_freq > 0);
	assert (resol_s >= _min_resol_s);
	assert (resol_s * _sample_freq < float (_fft_len) + 0.5f);

	_hop_size = fstb::round_int (resol_s * _sample_freq);
	_fo_ana.setup (_fft_len, _hop_size, 0);
	update_peak_pick_cst ();
	update_theta ();
}



/*
==============================================================================
Name: set_gamma
Description:
	Sets the gamma parameter, the ratio of the bin magnitudes kept for
	analysis. The remaining few bins are the most energised ones (fundamental
	and strong harmonics) and are discarded on purpose.
Input parameters:
	- gamma: ratio, in ]0 ; 1]. Good values are closer to 1, default is 0.94.
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_gamma (float gamma) noexcept
{
	assert (gamma > 0);
	assert (gamma <= 1);

	_gamma         = gamma;
	_g_n           = compute_g_n (_gamma);
	_odf_norm_fact = compute_odf_norm_factor (_g_n);
}



/*
==============================================================================
Name: set_pp_param
Description:
	Adjusts the peak-picking time parameters. They define the time windows
	for local maximum and moving average measurements. Beta and b values are
	particularly senstive because they define the algorithm latency.
	The windows late extents are limited to 1 s.
Input parameters:
	- alpha: late extent of the local maximum window, in seconds. >= 0.
	- beta: lookahead extent of the local max window, in seconds. >= 0.
	- a: late extent of the moving average window, in seconds. >= 0.
	- b: lookahead extent of the moving average window, in seconds. >= 0.
	- deadzone: minimum time between two onsets, in seconds. >= 0.
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_pp_param (float alpha, float beta, float a, float b) noexcept
{
	assert (_sample_freq > 0);
	assert (alpha >= 0);
	assert (beta >= 0);
	assert (alpha + beta <= _max_delay_s);
	assert (a >= 0);
	assert (b >= 0);
	assert (a + b <= _max_delay_s);
	assert (alpha + b <= _max_delay_s);
	assert (a + beta <= _max_delay_s);

	_alpha_s = alpha;
	_beta_s  = beta;
	_a_s     = a;
	_b_s     = b;
	update_peak_pick_cst ();
}



/*
==============================================================================
Name: set_deadzone
Description:
	Sets the minimum required time between two onset detections.
	Usually, it is equivalent to the frame size.
Input parameters:
	- deadzone: minimum time between two onsets, in seconds. >= 0.
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_deadzone (float deadzone) noexcept
{
	assert (deadzone >= 0);

	_theta_s = deadzone;
	update_theta ();
}



/*
==============================================================================
Name: set_thr_add
Description:
	Sets the delta parameter of the algorithm. While the main threshold is
	lambda with set_thr_mul(), this delta helps removing false positive at
	very low volumes, ignoring instrument noises. 0.1 is a good value for a
	signal whose peaks are close to 0 dBFS.
Input parameters:
	- delta: tunable threshold, >= 0
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_thr_add (float delta) noexcept
{
	assert (delta >= 0);

	_delta = delta;
}



/*
==============================================================================
Name: set_thr_mul
Description:
	Sets the lambda parameter of the algorithm. Too low values give false
	positives, and too high values miss onsets. 1.5 seems a good overall value.
	Lambda does not depend on the signal level.
Input parameters:
	- lambda: tunable threshold, >= 1
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::set_thr_mul (float lambda) noexcept
{
	assert (lambda >= 1);

	_lambda = lambda;
}



/*
==============================================================================
Name: compute_latency
Description:
	Computes the latency of the algorithm. When an onset is detected at sample
	N, it actually occured at sample N minus latency. The default settings
	exhibit a significant latency, in the 90-100 ms range.
Returns:
	The latency in samples, > 0.
Throws: Nothing
==============================================================================
*/

template <class FFT>
int	OnsetNinos2 <FFT>::compute_latency () const noexcept
{
	assert (_sample_freq > 0);

	return _hop_size * _delay + (_fft_len >> 1);
}



/*
==============================================================================
Name: analyse_sample
Description:
	Processes one input samples and checks if an onset is detected.
Input parameters:
	- x: input sample
Returns:
	true if an onset is detected
Throws: Nothing
==============================================================================
*/

template <class FFT>
bool	OnsetNinos2 <FFT>::analyse_sample (float x) noexcept
{
	assert (_sample_freq > 0);

	bool           onset_flag = false;

	if (_fo_ana.process_sample (x))
	{
		onset_flag = analyse_new_frame ();
	}

	return onset_flag;
}



/*
==============================================================================
Name: analyse_block
Description:
	Processes a block of samples and checks if at least one onset is detected.
Input parameters:
	- src_ptr: pointer on the block of samples.
	- nbr_spl: number of samples to analyse.
Returns:
	true if at leat one onset is detected
Throws: Nothing
==============================================================================
*/

template <class FFT>
bool	OnsetNinos2 <FFT>::analyse_block (const float src_ptr [], int nbr_spl) noexcept
{
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	bool           onset_flag = false;

	do
	{
		const auto     info = _fo_ana.process_block (src_ptr, nbr_spl);
		if (info._frame_flag && analyse_new_frame ())
		{
			onset_flag = true;
		}

		src_ptr += nbr_spl - info._nbr_spl_rem;
		nbr_spl  = info._nbr_spl_rem;
	}
	while (nbr_spl > 0);

	return onset_flag;
}



/*
==============================================================================
Name: clear_buffers
Description:
	Flushes the detector memory.
Throws: Nothing
==============================================================================
*/

template <class FFT>
void	OnsetNinos2 <FFT>::clear_buffers () noexcept
{
	_fo_ana.clear_buffers ();
	_odf_mem.clear_buffers ();
	_nfsl = _theta + 1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class FFT>
int	OnsetNinos2 <FFT>::compute_g_n (float gamma) const noexcept
{
	int            g_n = fstb::floor_int (gamma * (_nbr_bins - 1));
	g_n = fstb::limit (g_n, 1, _nbr_bins - 1);

	return g_n;
}



// Eq. 5
template <class FFT>
float	OnsetNinos2 <FFT>::compute_odf_norm_factor (int g_n) const noexcept
{
	const float    x             = sqrtf (float (g_n));
	const float    odf_norm_fact = 1.f / sqrtf (x);

	return odf_norm_fact;
}



template <class FFT>
void	OnsetNinos2 <FFT>::update_peak_pick_cst () noexcept
{
	assert (_sample_freq > 0);

	const float    mult = _sample_freq / _hop_size;
	_alpha = fstb::round_int (_alpha_s * mult);
	_beta  = fstb::round_int (_beta_s  * mult);
	_a     = fstb::round_int (_a_s     * mult);
	_b     = fstb::round_int (_b_s     * mult);

	assert (_alpha + 1 + _beta <= _odf_mem.get_max_delay ());
	assert (_a     + 1 + _b    <= _odf_mem.get_max_delay ());

	_delay = std::max (_beta, _b);
}



template <class FFT>
void	OnsetNinos2 <FFT>::update_theta () noexcept
{
	assert (_sample_freq > 0);

	if (_theta_s < 0)
	{
		_theta_s = _fft_len / _sample_freq;
	}
	_theta = fstb::ceil_int (_theta_s * _sample_freq / _hop_size);
}



// Returns true if an onset is detected
template <class FFT>
bool	OnsetNinos2 <FFT>::analyse_new_frame () noexcept
{
	assert (_sample_freq > 0);

	bool           onset_flag = false;

	// Retrieves the frame
	_fo_ana.get_frame (_buf_frame.data ());

	// Applies the Hann window
	_win.process_frame_mul (_buf_frame.data ());

	// Converts it to frequency domain
	_fft.do_fft (_buf_mag2.data (), _buf_frame.data ());

	// Converts the bins into simple squared magnitudes. DC/Nyquist bin is
	// processed for simplicity but will be ignored later.
	for (int k = 0; k < _nbr_bins; ++k)
	{
		const auto     re = _buf_mag2 [k            ];
		const auto     im = _buf_mag2 [k + _nbr_bins];
		_buf_mag2 [k] = re * re + im * im;
	}

	// Sorts the magnitudes in ascending order and keeps only the first ones
	_buf_mag2 [0] = _buf_mag2 [_nbr_bins - 1]; // Discards DC
	auto           it_mag = _buf_mag2.begin ();
	std::nth_element (it_mag, it_mag + _g_n, it_mag + _nbr_bins - 1);

	// Computes the onset detection function (ODF)
	const float    aleph = compute_odf ();
	_odf_mem.write_sample (aleph);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	// Peak-picking
	const float    odf_ref = _odf_mem.read_at (_delay);
	const float    odf_max = compute_odf_local_max ();
	const float    odf_avg = compute_odf_avg ();
	if (   odf_ref >= odf_max
	    && odf_ref > odf_avg * _lambda + _delta
	    &&   _nfsl > _theta)
	{
		onset_flag = true;
		_nfsl      = 0;
	}

	// Next
	++ _nfsl;
	_odf_mem.step (1);

	return onset_flag;
}



// Eq. 5
template <class FFT>
float	OnsetNinos2 <FFT>::compute_odf () const noexcept
{
	float          sum2 = 0;
	float          sum4 = 0;
	for (int k = 0; k < _g_n; ++k)
	{
		const float    m2 = _buf_mag2 [k];
		const float    m4 = fstb::sq (m2);
		sum2 += m2;
		sum4 += m4;
	}
	const float   norm2sq = sum2;
	const float   norm4   = sqrtf (sqrtf (sum4));

	const float   aleph   = _odf_norm_fact * norm2sq / norm4;

	return aleph;
}



template <class FFT>
float	OnsetNinos2 <FFT>::compute_odf_local_max () const noexcept
{
	float          odf_max = 0;
	for (int k = _delay + _alpha; k >= _delay - _beta; --k)
	{
		const float    odf_tst = _odf_mem.read_at (k);
		odf_max = std::max (odf_tst, odf_max);
	}

	return odf_max;
}



template <class FFT>
float	OnsetNinos2 <FFT>::compute_odf_avg () const noexcept
{
	float          sum     = 0;
	for (int k = _delay + _a; k >= _delay - _b; --k)
	{
		const float    odf = _odf_mem.read_at (k);
		sum += odf;
	}
	const float    avg = sum / (_a + 1 + _b);

	return avg;
}



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ana_OnsetNinos2_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
