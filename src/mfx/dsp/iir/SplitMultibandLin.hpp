/*****************************************************************************

        SplitMultibandLin.hpp
        Author: Laurent de Soras, 2021

In ---+----------------------> LPF0 -----+----------> Band 0, low
      |                                  |   
      |                                  `-----.
      |                                        v -
      +---> Delay D0-D1 -----> LPF1 -----+--->(+)--->
      |                                  |   +
      :                                  :
      :                                  :
      :                                  :
      |                                  `-----.
      |                                        v -
      +---> Delay D0-Dn-2 ---> LPFn-2 ---+--->(+)--->
      |                                  |   +
      |                                  `-----.
      |                                        v -
      `---> Delay D0 ------------------------>(+)---> Band n-1, high
                                             +

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitMultibandLin_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandLin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int O>
SplitMultibandLin <T, O>::SplitMultibandLin ()
{
	mix::Generic::setup ();
}



/*
==============================================================================
Name: set_sample_freq
Description:
	Sets the sampling rate. Call this before anything else.
	When the sampling rate of a working filterbank is changed, the crossover
	frequencies may shift because of the quantization.
Input parameters:
	- sample_freq: sampling rate, in Hz. > 0
Throws: std::vector-related exceptions
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = T (sample_freq);
	const int      max_dly_time_spl =
		fstb::round_int (float (_max_dly_time * _sample_freq));
	_delay.setup (max_dly_time_spl, _max_buf_size);
	
	if (! _split_arr.empty ())
	{
		update_all ();
	}
}



/*
==============================================================================
Name: reserve
Description:
	Reserves memory for a given maximum number of bands.
	Once this function is called, set_nbr_band() should be allocation-free
	for any specified number of bands lower or equal to this function
	parameter.
Input parameters:
	- nbr_bands: Maximum number of preallocated bands, >= 2
Throws: std::vector-related exceptions
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::reserve (int nbr_bands)
{
	assert (nbr_bands >= 2);

	const int      nbr_split = nbr_bands - 1;
	_band_arr.reserve (nbr_bands);
	_split_arr.reserve (nbr_split);
}



/*
==============================================================================
Name: set_nbr_bands
Description:
	Sets the number of bands and the corresponding output buffers.
	Output buffers are required even for single-sample processing, in which
	case the buffers are 1-sample long.
	Important notes:
	- This is a mandatory call before calling any other function excepted
		set_sample_freq().
	- This function allocates memory so it is not RT-safe. However it is
		possible to preallocate memory with the reserve() function.
	- If the number of bands is changed, there is not signal continuity.
		You should rebuild the crossover filters with as many calls to
		set_splitter_coef() as necessary.
Input parameters:
	- nbr_bands: Number of bands, >= 2
	- band_ptr_arr: array containing pointers on the output buffers for each
		band. It should contain nbr_bands pointers.
Throws: std::vector-related exceptions, if memory is allocated
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::set_nbr_bands (int nbr_bands, T * const band_ptr_arr [])
{
	assert (nbr_bands >= 2);
	assert (band_ptr_arr != nullptr);
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	const int      nbr_split = nbr_bands - 1;
	_band_arr.resize (nbr_bands);
	_split_arr.resize (nbr_split);

	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		Band &         band = _band_arr [band_idx];
		band._out_ptr = band_ptr_arr [band_idx];
	}

	if (_sample_freq > 0)
	{
		update_all ();
	}
}



/*
==============================================================================
Name: get_nbr_bands
Description:
	Returns the current number of bands.
	It must have been explicitly set before.
Returns: The number of bands, >= 2
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
int	SplitMultibandLin <T, O>::get_nbr_bands () const noexcept
{
	assert (! _band_arr.empty ());

	return int (_band_arr.size ());
}



/*
==============================================================================
Name: set_splitter_coef
Description:
	Sets the s-plane coefficients for a given crossover filter. The
	coefficients are normalized in frequency, meaning that the filter is
	centered around the unit frequency, and transposed with the freq parameter.
	So it's possible to call the function with the same coefficients for all
	the crossover splitters, only varying the freq parameter.
	Depending on the filter response, the crossover frequency (where the
	magnitudes of low- and high-pass filters are the same) may be slightly
	shifted relative to the prescription. This shift is not taken into account
	anywhere in this class and must be dealt with by the client during the
	design stage.
	Important notes:
	- The crossover frequencies are quantized. The quantization is most
		significant in the high frequencies. Call get_actual_xover_freq() to
		know the actual crossover frequency. The highest frequencies may not
		be reachable at all.
Input parameters:
	- split_idx: Crossover index, >= 0. Crossovers should be sorted by
		ascending frequencies.
	- freq: Target crossover frequency, Hz
	- coef_arr: Pointer on an array containing the filter coefficients. Layout:
		- First come groups of coefficients for the 2nd-order sections (if any),
			each group consisting of a numerator followed by a denominator:
			{ b0, b1, b2, a0, a1, a2 }
			The numbers are the coefficient orders.
			a0 and a2 are expected to be 1.
		- Then the coefficients for the 1st-order section, if the global filter
			order is odd. Numerator followed by the denominator:
			{ b0, b1, a0, a1 }
			Here again, a0 and a1 are expected to be 1.
		The product of the higher-order coefficients (a2 for 2nd order, a1
		for 1st order) should also be 1 so the equation is a normalised monic
		form, as specified in eq. 21 (Lipshitz83).
	- dly_ofs: offset for the delay, relative to the crossover delay time.
		With some filter designs, slightly increasing the delay may create a
		notch in the high-pass filter and make the slope steeper near the cutoff
		frequency. However the asymptotic slope drops to 6 dB/octave.
		Positive values increase the delay, negative ones decrease it.
		The offset value should be kept small (a few percents).
		0 is the neutral value (recommended).
Returns:
	The actual crossover frequency, in Hz. It might have been changed to take
	delay constraints into account.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::set_splitter_coef (int split_idx, T freq, const T coef_arr [O], T dly_ofs) noexcept
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);
	assert (coef_arr != nullptr);
	assert (dly_ofs > -1);

	Splitter &     split = _split_arr [split_idx];

	split._freq_tgt = freq;
	split._dly_ofs  = dly_ofs;

	// Stores the coefficients at the right places
	int            coef_ofs = 0;
	for (auto &filt : split._f2p_arr)
	{
		coef_ofs += filt._eq_s.fill_with (coef_arr + coef_ofs);
	}
	for (auto &filt : split._f1p_arr)
	{
		coef_ofs += filt._eq_s.fill_with (coef_arr + coef_ofs);
	}

	// Computes the 1st-order coefficient for the denominator of the whole
	// filter. This is b1 in eq. 21
	T              b1 = 0;
	static constexpr int _nbr_flt = _nbr_2p + _nbr_1p;
	for (int k = 0; k < _nbr_flt; ++k)
	{
		T              prod = 1;
		for (int j = 0; j < _nbr_flt; ++j)
		{
			const int      order = (j == k) ? 1 : 0;
			const T        coef  =
				  (j < _nbr_2p)
				? split._f2p_arr [j          ]._eq_s._a [order]
				: split._f1p_arr [j - _nbr_2p]._eq_s._a [order];
			prod *= coef;
		}
		b1 += prod;
	}
	split._b1 = b1;

	update_single_splitter (split_idx);
	update_post ();
}



/*
==============================================================================
Name: get_actual_xover_freq
Description:
	Retrieves the actual crossover frequency, after the splitter target
	frequency has been set.
Input parameters:
	- split_idx: Crossover index, >= 0.
Returns:
	The actual crossover frequency, in Hz.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
T	SplitMultibandLin <T, O>::get_actual_xover_freq (int split_idx) const noexcept
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));

	const Splitter &  split = _split_arr [split_idx];
	assert (split._freq_act > 0);

	return T (TransSZBilin::unwarp_freq (split._freq_act, _sample_freq));
}



/*
==============================================================================
Name: get_global_delay
Description:
	Retrieves the global filterbank delay. Effective only when all crossover
	have been set.
Returns: The delay, in samples. > 0
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
int	SplitMultibandLin <T, O>::get_global_delay () const noexcept
{
	assert (_sample_freq > 0);
	assert (! _band_arr.empty ());

	return _max_delay;
}



/*
==============================================================================
Name: set_band_ptr_one
Description:
	Sets the output buffer for a single band.
Input parameters:
	- band_idx: index of the band in question, [0 ; nbr_bands - 1].
	- out_ptr: Pointer on the output buffer for this band.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::set_band_ptr_one (int band_idx, T *out_ptr) noexcept
{
	assert (band_idx >= 0);
	assert (band_idx < get_nbr_bands ());
	assert (out_ptr != nullptr);

	_band_arr [band_idx]._out_ptr = out_ptr;
}



/*
==============================================================================
Name: set_band_ptr
Description:
	Sets the output buffers for all the bands.
Input parameters:
	- band_ptr_arr: array containing pointers on the output buffers for each
		band. It should contain nbr_bands pointers.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::set_band_ptr (T * const band_ptr_arr []) noexcept
{
	assert (! _band_arr.empty ());
	assert (band_ptr_arr != nullptr);
	const int      nbr_bands = get_nbr_bands ();
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		_band_arr [band_idx]._out_ptr = band_ptr_arr [band_idx];
	}
}



/*
==============================================================================
Name: offset_band_ptr
Description:
	Adds a value to the pointers of all the band output buffers.
	Important: the offset is measured in T, not bytes.
Input parameters:
	- offset: value to add to the pointers
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::offset_band_ptr (ptrdiff_t offset) noexcept
{
	assert (! _band_arr.empty ());

	const int      nbr_bands = get_nbr_bands ();
	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		_band_arr [band_idx]._out_ptr += offset;
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears all the filter and delay states.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::clear_buffers () noexcept
{
	_delay.clear_buffers ();
	for (auto &split : _split_arr)
	{
		for (auto &filter : split._f2p_arr)
		{
			filter._flt.clear_buffers ();
		}
		for (auto &filter : split._f1p_arr)
		{
			filter._flt.clear_buffers ();
		}
	}
}



/*
==============================================================================
Name: process_sample
Description:
	Splits a single sample into several bands. The output samples are stored
	at the beginning of the band output buffers.
Input parameters:
	- x: sample to process.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::process_sample (T x) noexcept
{
	assert (_sample_freq > 0);
	assert (! _band_arr.empty ());

	const int      nbr_split = int (_split_arr.size ());

	_delay.write_sample (x);

	// Special case for the lowest frequency band
	_band_arr [0]._buf_tmp [0] = x;

	// Delays, on bands 1 to N-1
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		Splitter &     split = _split_arr [split_idx];
		Band &         band  = _band_arr [split_idx + 1];
		band._buf_tmp [0] = _delay.read_at (split._dly_b);
	}
	_delay.step (1);

	// Filter
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		Splitter &     split = _split_arr [split_idx];
		Band &         band  = _band_arr [split_idx];
		auto           y     = band._buf_tmp [0];
		for (auto &filter : split._f2p_arr)
		{
			y = filter._flt.process_sample (y);
		}
		for (auto &filter : split._f1p_arr)
		{
			y = filter._flt.process_sample (y);
		}
		band._buf_tmp [0] = y;
	}

	// Output
	_band_arr [0]._out_ptr [0] = _band_arr [0]._buf_tmp [0];
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		const Band &   b0 = _band_arr [split_idx    ];
		const Band &   b1 = _band_arr [split_idx + 1];
		b1._out_ptr [0]   = b1._buf_tmp [0] - b0._buf_tmp [0];
	}
}



/*
==============================================================================
Name: process_block
Description:
	Splits a block of samples into several bands. The output blocks are stored
	in the band output buffers; make sure they are large enough.
Input parameters:
	- src_ptr: Pointer on the input samples.
	- nbr_spl: Number of samples to process, > 0.
Throws: Nothing
==============================================================================
*/

template <typename T, int O>
void	SplitMultibandLin <T, O>::process_block (const T src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (! _band_arr.empty ());
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const int      nbr_split = int (_split_arr.size ());

	int            pos = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, _max_buf_size);

		_delay.write_block (src_ptr + pos, work_len);

		// Special case for the lowest frequency band
		mix::Generic::copy_1_1 (
			_band_arr [0]._buf_tmp.data (), src_ptr + pos, work_len
		);

		// Delays, on bands 1 to N-1
		for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
		{
			Splitter &     split = _split_arr [split_idx];
			Band &         band  = _band_arr [split_idx + 1];
			_delay.read_block_at (band._buf_tmp.data (), split._dly_b, work_len);
		}
		_delay.step (work_len);

		// Filter
		for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
		{
			Splitter &     split = _split_arr [split_idx];
			Band &         band  = _band_arr [split_idx];
			for (auto &filter : split._f2p_arr)
			{
				filter._flt.process_block (
					band._buf_tmp.data (), band._buf_tmp.data (), work_len
				);
			}
			for (auto &filter : split._f1p_arr)
			{
				filter._flt.process_block (
					band._buf_tmp.data (), band._buf_tmp.data (), work_len
				);
			}
		}

		// Output
		mix::Generic::copy_1_1 (
			_band_arr [0]._out_ptr + pos, _band_arr [0]._buf_tmp.data (), work_len
		);
		for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
		{
			const Band &   b0 = _band_arr [split_idx    ];
			const Band &   b1 = _band_arr [split_idx + 1];
			const float * fstb_RESTRICT   b0_ptr  = b0._buf_tmp.data ();
			const float * fstb_RESTRICT   b1_ptr  = b1._buf_tmp.data ();
			float * fstb_RESTRICT         out_ptr = b1._out_ptr + pos;
			for (int k = 0; k < work_len; ++k)
			{
				out_ptr [k] = b1_ptr [k] - b0_ptr [k];
			}
		}

		// Next sub-block
		pos += work_len;
	}
	while (pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// 100 ms should be enough for most uses.
template <typename T, int O>
const double	SplitMultibandLin <T, O>::_max_dly_time = 0.100;



template <typename T, int O>
template <int N>
int	SplitMultibandLin <T, O>::FilterEq <N>::fill_with (const T coef_ptr [SplitMultibandLin <T, O>::FilterEq <N>::_nbr_coef]) noexcept
{
	for (int k = 0; k < N; ++k)
	{
		_b [k] = coef_ptr [    k];
		_a [k] = coef_ptr [N + k];
	}
	assert (fstb::is_eq (_a [0    ], T (1)));
	assert (fstb::is_eq (_a [N - 1], T (1)));

	return _nbr_coef;
}



template <typename T, int O>
void	SplitMultibandLin <T, O>::update_all ()
{
	assert (_sample_freq > 0);

	const int      nbr_split = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		update_single_splitter (split_idx);
	}

	update_post ();
}



// update_post() must be called afterwards
template <typename T, int O>
void	SplitMultibandLin <T, O>::update_single_splitter (int split_idx)
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));

	auto &         split = _split_arr [split_idx];

	const T        f = split._freq_tgt; // Hz

	// Is the band already set?
	if (f > 0)
	{
		// Group delay at DC
		// Eq. 22 with bilinear frequency prewarping
		split._freq_warp = T (TransSZBilin::prewarp_freq (f, _sample_freq));
		split._dly_comp  = split._b1 / (2 * split._freq_warp);
		split._dly_comp *= 1 + split._dly_ofs;

		split._dly_int   = fstb::round_int (float (split._dly_comp));

		// For the highest frequencies, the rounded delay may be 0.
		// Keep it greater than 0.
		split._dly_int   = std::max (split._dly_int, 1);

		// Evaluates the actual cutoff frequency corresponding to this
		// rounded delay time
		T              ratio = 1;
		ratio = split._dly_comp / split._dly_int;
		split._freq_act = split._freq_warp * ratio;

		// Produces the z-plane coefficients using the bilinear transform

		// 2-pole sections
		for (auto &filt : split._f2p_arr)
		{
			Eq2p           eq_z;
			bilinear_2p (eq_z, filt._eq_s, split._freq_act);
			filt._flt.set_z_eq (eq_z._b.data (), eq_z._a.data ());
		}

		// 1-pole sections
		for (auto &filt : split._f1p_arr)
		{
			Eq1p           eq_z;
			bilinear_1p (eq_z, filt._eq_s, split._freq_act);
			filt._flt.set_z_eq (eq_z._b.data (), eq_z._a.data ());
		}
	}
}



// We need to compute the maximum delay time for the whole filterbank,
// take all the actual delays into account and recompute the crossover
// frequencies according to these changes.
// Then we can compute the z-plane equations for all filters.
template <typename T, int O>
void	SplitMultibandLin <T, O>::update_post ()
{
	assert (_sample_freq > 0);

	_max_delay = _split_arr [0]._dly_int;
	assert (_max_delay <= _delay.get_max_delay ());

	T              freq_tgt_old = 0;
	const int      nbr_split    = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		Splitter &     split = _split_arr [split_idx];
		if (split._freq_tgt <= freq_tgt_old)
		{
			// Splitter not set or invalid set of frequencies: stop updating
			break;
		}

		// The difference between the maximum delay and LPF DC group delay
		// on the upper band.
		split._dly_b = _max_delay;
		if (split_idx + 1 < nbr_split)
		{
			split._dly_b -= _split_arr [split_idx + 1]._dly_int;
		}

		freq_tgt_old = split._freq_tgt;
	}
}



// Simplified bilinear transforms:
// - No frequency prewarping
// - Assumes a0 == 1 and aN == 1
// f0_pi_fs = f0 * pi / fs
template <typename T, int O>
void	SplitMultibandLin <T, O>::bilinear_2p (Eq2p &eq_z, const Eq2p &eq_s, T f0_pi_fs) noexcept
{
	assert (fstb::is_eq (eq_s._a [0], T (1)));
	assert (fstb::is_eq (eq_s._a [2], T (1)));

	const double   k  = 1 / f0_pi_fs;
	const double   kk = k*k;

	const double   b1k  = eq_s._b [1] * k;
	const double   b2kk = eq_s._b [2] * kk;
	const double   b2kk_plus_b0 = b2kk + eq_s._b [0];
	const double   b0z = b2kk_plus_b0 + b1k;
	const double   b2z = b2kk_plus_b0 - b1k;
	const double   b1z = 2 * (eq_s._b [0] - b2kk);

	const double   a1k  = eq_s._a [1] * k;
	const double   a2kk = kk;
	const double   a2kk_plus_a0 = a2kk + 1;
	const double   a0z = a2kk_plus_a0 + a1k;
	const double   a2z = a2kk_plus_a0 - a1k;
	const double   a1z = 2 * (1 - a2kk);

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double	mult = 1 / a0z;

	eq_z._b [0] = T (b0z * mult);
	eq_z._b [1] = T (b1z * mult);
	eq_z._b [2] = T (b2z * mult);

	eq_z._a [0] = T (1);
	eq_z._a [1] = T (a1z * mult);
	eq_z._a [2] = T (a2z * mult);
}



template <typename T, int O>
void	SplitMultibandLin <T, O>::bilinear_1p (Eq1p &eq_z, const Eq1p &eq_s, T f0_pi_fs) noexcept
{
	assert (fstb::is_eq (eq_s._a [0], T (1)));
	assert (fstb::is_eq (eq_s._a [1], T (1)));

	const double   k   = 1 / f0_pi_fs;
	const double   b1k = eq_s._b [1] * k;
	const double   b1z = eq_s._b [0] - b1k;
	const double   b0z = eq_s._b [0] + b1k;

	const double   a1k = k;
	const double   a1z = 1 - a1k;
	const double   a0z = 1 + a1k;

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double   mult = 1 / a0z;

	eq_z._b [0] = T (b0z * mult);
	eq_z._b [1] = T (b1z * mult);

	eq_z._a [0] = T (1);
	eq_z._a [1] = T (a1z * mult);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitMultibandLin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
