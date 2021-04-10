/*****************************************************************************

        SplitMultibandBustad.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitMultibandBustad_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandBustad_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Simd.h"

#include <cassert>

#include <algorithm>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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

template <int O>
void	SplitMultibandBustad <O>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

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

template <int O>
void	SplitMultibandBustad <O>::reserve (int nbr_bands)
{
	assert (nbr_bands >= 2);

	const int      nbr_split = nbr_bands - 1;
	_band_arr.reserve (nbr_bands);
	_split_arr.reserve (nbr_split);
	_filter_arr.reserve (nbr_split);
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
	- If the number of bands is changed, the lower bands are preserved.
		If you increased the number of bands, you should call
		set_splitter_coef() on the new bands.
Input parameters:
	- nbr_bands: Number of bands, >= 2
	- band_ptr_arr: array containing pointers on the output buffers for each
		band. It should contain nbr_bands pointers.
Throws: std::vector-related exceptions, if memory is allocated
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::set_nbr_bands (int nbr_bands, float * const band_ptr_arr [])
{
	assert (nbr_bands >= 2);
	assert (band_ptr_arr != nullptr);
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	const int      nbr_split = nbr_bands - 1;
	_band_arr.resize (nbr_bands);
	_split_arr.resize (nbr_split);
	_filter_arr.resize (nbr_split);

	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		Band &         band = _band_arr [band_idx];
		band._spl_ptr = band_ptr_arr [band_idx];
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

template <int O>
int	SplitMultibandBustad <O>::get_nbr_bands () const noexcept
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
Input parameters:
	- split_idx: Crossover index, >= 0. Crossovers should be sorted by
		ascending frequencies.
	- freq: Target crossover frequency, Hz
	- lpf_coef_arr: Pointer on an array containing the filter coefficients
		for the low-pass filter of the crossover. Layout:
		- First come groups of coefficients for the 2nd-order sections (if any),
			each group consisting of a numerator followed by a denominator:
			{ b0, b1, b2, a0, a1, a2 }
			The numbers are the coefficient orders.
		- Then the coefficients for the 1st-order section, if the global filter
			order is odd. Numerator followed by the denominator:
			{ b0, b1, a0, a1 }
	- hpf_coef_arr: coefficients for the high-pass filter. Same layout as
		lpf_coef_arr.
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::set_splitter_coef (int split_idx, float freq, const float lpf_coef_arr [O], const float hpf_coef_arr [O]) noexcept
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);
	assert (lpf_coef_arr != nullptr);
	assert (hpf_coef_arr != nullptr);

	Split &        split = _split_arr [split_idx];
	split._freq = freq;
	split._lpf.fill_with (lpf_coef_arr);
	split._hpf.fill_with (hpf_coef_arr);

	update_xover_coefs (split_idx);
}



/*
==============================================================================
Name: compute_group_delay
Description:
	Computes the group delay at a specified frequency. Effective only when all
	crossover have been set.
Returns: The group delay, in samples. > 0
Throws: Nothing
==============================================================================
*/

template <int O>
float	SplitMultibandBustad <O>::compute_group_delay (float freq) const noexcept
{
	assert (_sample_freq > 0);
	assert (! _split_arr.empty ());
	assert (freq >= 0);

	double         latency = 0;
	const int      nbr_split = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		const Split &        split = _split_arr [split_idx];
		const SplitFilter &  sfilt = _filter_arr [split_idx];

		using DesEq = mfx::dsp::iir::DesignEq2p;

		const Filter & filter = (split._freq < freq) ? sfilt._hpf : sfilt._lpf;
		for (auto &unit : filter._f2p_arr)
		{
			float          bz [3] { 1 };
			float          az [3] { 1 };
			unit.get_z_eq (bz, az);
			const double      gd = DesEq::compute_group_delay (
				bz, az, _sample_freq, freq
			);
			latency += gd;
		}
		for (auto &unit : filter._f1p_arr)
		{
			float          bz [2] { 1 };
			float          az [2] { 1 };
			unit.get_z_eq (bz, az);
			const double      gd = DesEq::compute_group_delay_1p (
				bz, az, _sample_freq, freq
			);
			latency += gd;
		}
	}

	return float (latency);
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

template <int O>
void	SplitMultibandBustad <O>::set_band_ptr_one (int band_idx, float *band_ptr) noexcept
{
	assert (band_idx >= 0);
	assert (band_idx < get_nbr_bands ());
	assert (band_ptr != nullptr);

	_band_arr [band_idx]._spl_ptr = band_ptr;
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

template <int O>
void	SplitMultibandBustad <O>::set_band_ptr (float * const band_ptr_arr []) noexcept
{
	assert (! _band_arr.empty ());
	assert (band_ptr_arr != nullptr);
	const int      nbr_bands = get_nbr_bands ();
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		_band_arr [band_idx]._spl_ptr = band_ptr_arr [band_idx];
	}
}



/*
==============================================================================
Name: offset_band_ptr
Description:
	Adds a value to the pointers of all the band output buffers.
	Important: the offset is measured in samples, not bytes.
Input parameters:
	- offset: value to add to the pointers
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::offset_band_ptr (ptrdiff_t offset) noexcept
{
	assert (! _band_arr.empty ());

	const int      nbr_bands = get_nbr_bands ();
	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		_band_arr [band_idx]._spl_ptr += offset;
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears all the filter states.
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::clear_buffers () noexcept
{
	for (auto &split_filter : _filter_arr)
	{
		split_filter._lpf.clear_buffers ();
		split_filter._hpf.clear_buffers ();
		split_filter._fix.clear_buffers ();
	}
}



/*
==============================================================================
Name: process_sample_split
Description:
	Splits a single sample into several bands. The output samples are stored
	at the beginning of the band output buffers.
Input parameters:
	- x: sample to process.
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::process_sample_split (float x) noexcept
{
	assert (_sample_freq > 0);
	assert (! _split_arr.empty ());

	const int      nbr_split = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		SplitFilter &  filter = _filter_arr [split_idx];
		const float    lo     = filter._lpf.process_sample (x);
		x = filter._hpf.process_sample (x);
		*(_band_arr [split_idx]._spl_ptr) = lo;
	}
	*(_band_arr [nbr_split]._spl_ptr) = x;
}



/*
==============================================================================
Name: process_sample_merge
Description:
	Merges all the bands into a single sample, with the required phase
	compensation in order to obtain magnitude-unity.
Returns: the summed bands.
Throws: Nothing
==============================================================================
*/

template <int O>
float	SplitMultibandBustad <O>::process_sample_merge () noexcept
{
	assert (_sample_freq > 0);
	assert (! _split_arr.empty ());

	float          x = *(_band_arr [0]._spl_ptr);
	const int      nbr_split = int (_split_arr.size ());
	for (int split_idx = 1; split_idx < nbr_split; ++split_idx)
	{
		SplitFilter &  filter = _filter_arr [split_idx];
		const Band &   band   = _band_arr [split_idx];

		x  = filter._fix.process_sample (x);
		x += *(band._spl_ptr);
	}

	x += *(_band_arr [nbr_split]._spl_ptr);

	return x;
}



/*
==============================================================================
Name: process_block_split
Description:
	Splits a block of samples into several bands. The output blocks are stored
	in the band output buffers; make sure they are large enough.
Input parameters:
	- src_ptr: Pointer on the input samples.
	- nbr_spl: Number of samples to process, > 0.
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::process_block_split (const float src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (! _split_arr.empty ());
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const int      nbr_split = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		SplitFilter &  filter = _filter_arr [split_idx];

		// The lower part goes to the current band, and the higher part
		// propagates to the next band
		float *        lo_ptr = _band_arr [split_idx    ]._spl_ptr;
		float *        hi_ptr = _band_arr [split_idx + 1]._spl_ptr;

		filter._hpf.process_block (hi_ptr, src_ptr, nbr_spl);
		filter._lpf.process_block (lo_ptr, src_ptr, nbr_spl);

		// Next bands will be filtered in-place.
		src_ptr = hi_ptr;
	}
}



/*
==============================================================================
Name: process_block_merge
Description:
	Merges all the bands into a single buffer, with the required phase
	compensation in order to obtain magnitude-unity.
Input parameters:
	- dst_ptr: Pointer on the buffer receiving the summed samples. Should have
		a capacity of nbr_spl samples.
	- nbr_spl: Number of samples to merge. > 0.
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandBustad <O>::process_block_merge (float dst_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (! _split_arr.empty ());
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);

	alignas (16) Buffer  buf_fix;
	alignas (16) Buffer  buf_sum;

	const int      nbr_split = int (_split_arr.size ());
	int            pos       = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, _max_buf_size);

		float *        sum_ptr  = _band_arr [0]._spl_ptr + pos;
		for (int split_idx = 1; split_idx < nbr_split; ++split_idx)
		{
			SplitFilter &  filter = _filter_arr [split_idx];
			const Band &   band   = _band_arr [split_idx];

			// Fix
			filter._fix.process_block (buf_fix.data (), sum_ptr, work_len);

			// Mix
			float *        cur_ptr = band._spl_ptr + pos;
			mix::Simd <
				fstb::DataAlign <true>,
				fstb::DataAlign <false>
			>::copy_2_1 (
				buf_sum.data (), buf_fix.data (), cur_ptr, work_len
			);

			sum_ptr = buf_sum.data ();
		}

		// The last two bands don't need compensation processing
		mix::Simd <
			fstb::DataAlign <false>,
			fstb::DataAlign <false>
		>::copy_2_1 (
			dst_ptr + pos,
			sum_ptr,
			_band_arr [nbr_split]._spl_ptr + pos,
			work_len
		);

		pos += work_len;
	}
	while (pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int O>
template <int N>
int	SplitMultibandBustad <O>::FilterEq <N>::fill_with (const float coef_ptr [SplitMultibandBustad <O>::FilterEq <N>::_nbr_coef]) noexcept
{
	for (int k = 0; k < N; ++k)
	{
		_b [k] = coef_ptr [    k];
		_a [k] = coef_ptr [N + k];
	}

	return _nbr_coef;
}



template <int O>
void	SplitMultibandBustad <O>::Spec::fill_with (const float coef_arr []) noexcept
{
	int            coef_ofs = 0;
	for (auto &eq : _eq_2p)
	{
		coef_ofs += eq.fill_with (coef_arr + coef_ofs);
	}
	for (auto &eq : _eq_1p)
	{
		coef_ofs += eq.fill_with (coef_arr + coef_ofs);
	}
}



template <int O>
void	SplitMultibandBustad <O>::Filter::clear_buffers () noexcept
{
	for (auto &unit : _f2p_arr)
	{
		unit.clear_buffers ();
	}
	for (auto &unit : _f1p_arr)
	{
		unit.clear_buffers ();
	}
}



template <int O>
float	SplitMultibandBustad <O>::Filter::process_sample (float x) noexcept
{
	for (auto &unit : _f2p_arr)
	{
		x = unit.process_sample (x);
	}
	for (auto &unit : _f1p_arr)
	{
		x = unit.process_sample (x);
	}

	return x;
}



template <int O>
void	SplitMultibandBustad <O>::Filter::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	for (auto &unit : _f2p_arr)
	{
		unit.process_block (dst_ptr, src_ptr, nbr_spl);
		src_ptr = dst_ptr;
	}
	for (auto &unit : _f1p_arr)
	{
		unit.process_block (dst_ptr, src_ptr, nbr_spl);
		src_ptr = dst_ptr;
	}
}



template <int O>
void	SplitMultibandBustad <O>::update_all () noexcept
{
	const int      nbr_split = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		update_xover_coefs (split_idx);
	}
}



template <int O>
void	SplitMultibandBustad <O>::update_xover_coefs (int split_idx) noexcept
{
	const Split &  split  = _split_arr [split_idx];
	SplitFilter &  filter = _filter_arr [split_idx];

	const float    f      = split._freq * _inv_fs;
	const float    k      = TransSZBilin::compute_k_approx (f);

	// LPF
	set_filter_coefs (
		split._lpf._eq_2p, filter._lpf._f2p_arr, &filter._fix._f2p_arr,
		&TransSZBilin::map_s_to_z_approx, k
	);
	set_filter_coefs (
		split._lpf._eq_1p, filter._lpf._f1p_arr, &filter._fix._f1p_arr,
		&TransSZBilin::map_s_to_z_one_pole_approx, k
	);

	// HPF
	decltype (&filter._hpf._f2p_arr) n2_ptr = nullptr;
	set_filter_coefs (
		split._hpf._eq_2p, filter._hpf._f2p_arr, n2_ptr,
		&TransSZBilin::map_s_to_z_approx, k
	);
	decltype (&filter._hpf._f1p_arr) n1_ptr = nullptr;
	set_filter_coefs (
		split._hpf._eq_1p, filter._hpf._f1p_arr, n1_ptr,
		&TransSZBilin::map_s_to_z_one_pole_approx, k
	);
}



template <int O>
template <typename EQS, typename FILT>
void	SplitMultibandBustad <O>::set_filter_coefs (const EQS &eq_arr, FILT &filt_arr, FILT *filt2_arr_ptr, void (*blt) (float*, float*, const float*, const float*, float), float k)
{
	typename EQS::value_type   eq_z;
	for (int unit_idx = 0; unit_idx < int (eq_arr.size ()); ++unit_idx)
	{
		const auto &   eq_s = eq_arr [unit_idx];
		blt (
			eq_z._b.data (), eq_z._a.data (),
			eq_s._b.data (), eq_s._a.data (),
			k
		);
		filt_arr [unit_idx].set_z_eq (
			eq_z._b.data (), eq_z._a.data ()
		);
		if (filt2_arr_ptr != nullptr)
		{
			(*filt2_arr_ptr) [unit_idx].set_z_eq (
				eq_z._b.data (), eq_z._a.data ()
			);
		}
	}
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitMultibandBustad_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
