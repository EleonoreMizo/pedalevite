/*****************************************************************************

        SplitMultibandLinSimd.hpp
        Author: Laurent de Soras, 2021

Rationale:

The SIMD operations apply on the N-2 low bands. The highest band N-1 is
processed separately because it does need filtering.

The input feeds the delay, then the delay is read back in temporary buffers
(linear), excepted for the lowest band (not delayed).

The for each SIMD pack (total N-1 channels):

- The bands are interleaved for SIMD filtering in a global buffer.
The lowest band is read directly from the source. The highest band is not
processed here, kept intact in its temp buf.

- SIMD filtering occurs

- Bands are delinterleaved in their respective temporary buffers.

When all the SIMD packs are processed, band subtraction is done in a linear
way.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitMultibandLinSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandLinSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/mix/Simd.h"

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



template <int O>
constexpr int	SplitMultibandLinSimd <O>::_order;
template <int O>
constexpr int	SplitMultibandLinSimd <O>::_nbr_2p;
template <int O>
constexpr int	SplitMultibandLinSimd <O>::_nbr_1p;



template <int O>
SplitMultibandLinSimd <O>::SplitMultibandLinSimd ()
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

template <int O>
void	SplitMultibandLinSimd <O>::set_sample_freq (double sample_freq)
{
	if (this->set_sample_freq_internal (sample_freq, _max_buf_size))
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
void	SplitMultibandLinSimd <O>::reserve (int nbr_bands)
{
	assert (nbr_bands >= 2);

	const int      nbr_split = nbr_bands - 1;
	const int      nbr_packs = (nbr_split + _simd_w - 1) >> _simd_w_l2;
	_band_arr.reserve (nbr_bands);
	this->_split_arr.reserve (nbr_split);
	_flt_pack_arr.reserve (nbr_packs);
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

template <int O>
void	SplitMultibandLinSimd <O>::set_nbr_bands (int nbr_bands, float * const band_ptr_arr [])
{
	assert (nbr_bands >= 2);
	assert (band_ptr_arr != nullptr);
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	const int      nbr_split = nbr_bands - 1;
	const int      nbr_packs = (nbr_split + _simd_w - 1) >> _simd_w_l2;
	_band_arr.resize (nbr_bands);
	this->_split_arr.resize (nbr_split);
	_flt_pack_arr.resize (nbr_packs);

	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		Band &         band = _band_arr [band_idx];
		band._out_ptr = band_ptr_arr [band_idx];
	}

	if (this->_sample_freq > 0)
	{
		update_all ();
	}

	// Updates the pointers on temporary buffers
	for (int pack_idx = 0; pack_idx < nbr_packs; ++pack_idx)
	{
		const int      split_idx = pack_idx << _simd_w_l2;
		auto &         pack      = _flt_pack_arr [pack_idx];

		// Collects the pointers on the linear buffers
		for (int elt_idx = 0; elt_idx < _simd_w; ++elt_idx)
		{
			const int       b0_idx = split_idx + elt_idx;
			if (b0_idx >= nbr_bands - 1)
			{
				pack._buf_s_ptr_arr [elt_idx] =
					_band_arr [nbr_bands - 1]._buf_tmp.data ();
				pack._buf_d_ptr_arr [elt_idx] = _buf_trash.data ();
			}
			else
			{
				auto            buf_ptr = _band_arr [b0_idx]._buf_tmp.data ();
				pack._buf_s_ptr_arr [elt_idx] = buf_ptr;
				pack._buf_d_ptr_arr [elt_idx] = buf_ptr;
			}
		}
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
int	SplitMultibandLinSimd <O>::get_nbr_bands () const noexcept
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
			a0 is expected to be 1.
		- Then the coefficients for the 1st-order section, if the global filter
			order is odd. Numerator followed by the denominator:
			{ b0, b1, a0, a1 }
			Here again, a0 is expected to be 1.
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
Throws: Nothing
==============================================================================
*/

template <int O>
void	SplitMultibandLinSimd <O>::set_splitter_coef (int split_idx, float freq, const float coef_arr [O], float dly_ofs) noexcept
{
	this->set_splitter_coef_internal (split_idx, freq, coef_arr, dly_ofs);

	if (this->update_single_splitter (split_idx))
	{
		update_xover_coefs (split_idx);
	}
	this->update_post ();
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
void	SplitMultibandLinSimd <O>::set_band_ptr_one (int band_idx, float *out_ptr) noexcept
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

template <int O>
void	SplitMultibandLinSimd <O>::set_band_ptr (float * const band_ptr_arr []) noexcept
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

template <int O>
void	SplitMultibandLinSimd <O>::offset_band_ptr (ptrdiff_t offset) noexcept
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

template <int O>
void	SplitMultibandLinSimd <O>::clear_buffers () noexcept
{
	this->_delay.clear_buffers ();
	for (auto &pack : _flt_pack_arr)
	{
		for (auto &unit : pack._f2p_arr)
		{
			unit.clear_buffers ();
		}
		for (auto &unit : pack._f1p_arr)
		{
			unit.clear_buffers ();
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

template <int O>
void	SplitMultibandLinSimd <O>::process_sample (float x) noexcept
{
	assert (this->_sample_freq > 0);
	assert (! _band_arr.empty ());

	const int      nbr_split = int (this->_split_arr.size ());
	const int      nbr_packs = int (_flt_pack_arr.size ());

	// Updates some pointers
	_flt_pack_arr [0]._buf_s_ptr_arr [0] = &x;
	_flt_pack_arr [0]._buf_d_ptr_arr [0] = _band_arr [0]._out_ptr;

	this->_delay.write_sample (x);

	// Delays, on bands 1 to N-1
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		Splitter &     split = this->_split_arr [split_idx];
		Band &         band  = _band_arr [split_idx + 1];
		band._buf_tmp [0] = this->_delay.read_at (split._dly_b);
	}
	this->_delay.step (1);

	// Filter
	for (int pack_idx = 0; pack_idx < nbr_packs; ++pack_idx)
	{
		auto &         pack = _flt_pack_arr [pack_idx];

		// Reads and interleaves the bands
		auto           y = fstb::Vf32 (
			*(pack._buf_s_ptr_arr [0]),
			*(pack._buf_s_ptr_arr [1]),
			*(pack._buf_s_ptr_arr [2]),
			*(pack._buf_s_ptr_arr [3])
		);

		// Filtering
		for (auto &filter : pack._f2p_arr)
		{
			y = filter.process_sample_parallel (y);
		}
		for (auto &filter : pack._f1p_arr)
		{
			y = filter.process_sample_parallel (y);
		}

		// Deinterleaves the bands
		*(pack._buf_d_ptr_arr [0]) = y.template extract <0> ();
		*(pack._buf_d_ptr_arr [1]) = y.template extract <1> ();
		*(pack._buf_d_ptr_arr [2]) = y.template extract <2> ();
		*(pack._buf_d_ptr_arr [3]) = y.template extract <3> ();
	}

	// Output
	{
		const Band &   b0 = _band_arr [0];
		const Band &   b1 = _band_arr [1];
		b1._out_ptr [0]   = b1._buf_tmp [0] - b0._out_ptr [0];
	}
	for (int split_idx = 1; split_idx < nbr_split; ++split_idx)
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

template <int O>
void	SplitMultibandLinSimd <O>::process_block (const float src_ptr [], int nbr_spl) noexcept
{
	assert (this->_sample_freq > 0);
	assert (! _band_arr.empty ());
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const int      nbr_split = int (this->_split_arr.size ());
	const int      nbr_packs = int (_flt_pack_arr.size ());

	int            pos = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, _max_buf_size);

		// These buffers may be unaligned
		_flt_pack_arr [0]._buf_s_ptr_arr [0] = src_ptr + pos;
		_flt_pack_arr [0]._buf_d_ptr_arr [0] = _band_arr [0]._out_ptr + pos;

		this->_delay.write_block (src_ptr + pos, work_len);

		// Delays, on bands 1 to N-1
		for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
		{
			Splitter &     split = this->_split_arr [split_idx];
			Band &         band  = _band_arr [split_idx + 1];
			this->_delay.read_block_at (band._buf_tmp.data (), split._dly_b, work_len);
		}
		this->_delay.step (work_len);

		// Filter
		for (int pack_idx = 0; pack_idx < nbr_packs; ++pack_idx)
		{
			auto &         pack = _flt_pack_arr [pack_idx];

			// Interleaves the bands
			if (pack_idx == 0)
			{
				mix::Simd <
					fstb::DataAlign <true>,
					fstb::DataAlign <false>
				>::copy_4_4i (
					reinterpret_cast <float *> (_buf_pack.data ()),
					pack._buf_s_ptr_arr [0],
					pack._buf_s_ptr_arr [1],
					pack._buf_s_ptr_arr [2],
					pack._buf_s_ptr_arr [3],
					work_len
				);
			}
			else
			{
				mix::Simd <
					fstb::DataAlign <true>,
					fstb::DataAlign <true>
				>::copy_4_4i (
					reinterpret_cast <float *> (_buf_pack.data ()),
					pack._buf_s_ptr_arr [0],
					pack._buf_s_ptr_arr [1],
					pack._buf_s_ptr_arr [2],
					pack._buf_s_ptr_arr [3],
					work_len
				);
			}

			// Filtering
			for (auto &filter : pack._f2p_arr)
			{
				filter.process_block_parallel (
					_buf_pack.data (), _buf_pack.data (), work_len
				);
			}
			for (auto &filter : pack._f1p_arr)
			{
				filter.process_block_parallel (
					_buf_pack.data (), _buf_pack.data (), work_len
				);
			}

			// Deinterleaves the bands
			if (pack_idx == 0)
			{
				mix::Simd <
					fstb::DataAlign <false>,
					fstb::DataAlign <true>
				>::copy_4i_4 (
					pack._buf_d_ptr_arr [0],
					pack._buf_d_ptr_arr [1],
					pack._buf_d_ptr_arr [2],
					pack._buf_d_ptr_arr [3],
					reinterpret_cast <const float *> (_buf_pack.data ()),
					work_len
				);
			}
			else
			{
				mix::Simd <
					fstb::DataAlign <true>,
					fstb::DataAlign <true>
				>::copy_4i_4 (
					pack._buf_d_ptr_arr [0],
					pack._buf_d_ptr_arr [1],
					pack._buf_d_ptr_arr [2],
					pack._buf_d_ptr_arr [3],
					reinterpret_cast <const float *> (_buf_pack.data ()),
					work_len
				);
			}
		}

		// Output
		for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
		{
			const Band &   b0      = _band_arr [split_idx    ];
			const Band &   b1      = _band_arr [split_idx + 1];
			const float *  b0_ptr  =
				  (split_idx == 0)
				? b0._out_ptr + pos
				: b0._buf_tmp.data ();
			const float *  b1_ptr  = b1._buf_tmp.data ();
			float *        out_ptr = b1._out_ptr + pos;
			mix::Simd <
				fstb::DataAlign <false>,
				fstb::DataAlign <false>
			>::vec_op_2_1 (
				out_ptr, b1_ptr, b0_ptr, work_len,
				[] (const auto &lhs, const auto &rhs) { return lhs - rhs; },
				[] (const auto &lhs, const auto &rhs) { return lhs - rhs; }
			);
		}

		// Next sub-block
		pos += work_len;
	}
	while (pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int O>
constexpr int	SplitMultibandLinSimd <O>::_simd_alig_l2;
template <int O>
constexpr int	SplitMultibandLinSimd <O>::_simd_alig;
template <int O>
constexpr int	SplitMultibandLinSimd <O>::_simd_w_l2;
template <int O>
constexpr int	SplitMultibandLinSimd <O>::_simd_w;
template <int O>
constexpr int	SplitMultibandLinSimd <O>::_max_buf_size;



template <int O>
void	SplitMultibandLinSimd <O>::update_all () noexcept
{
	assert (this->_sample_freq > 0);

	const int      nbr_split = int (this->_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		if (this->update_single_splitter (split_idx))
		{
			update_xover_coefs (split_idx);
		}
	}

	this->update_post ();
}



// Produces the z-plane coefficients using the bilinear transform
template <int O>
void	SplitMultibandLinSimd <O>::update_xover_coefs (int split_idx) noexcept
{
	assert (this->_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (this->_split_arr.size ()));
	assert (this->_split_arr [split_idx]._freq_tgt > 0);

	auto &         split = this->_split_arr [split_idx];
	const auto     coord { conv_split_to_pack (split_idx) };
	auto &         pack  = _flt_pack_arr [coord._pack_idx];

	// 2-pole sections
	for (int flt_idx = 0; flt_idx < _nbr_2p; ++flt_idx)
	{
		const auto &   eq_s = split._eq_2p [flt_idx];
		auto &         flt  = pack._f2p_arr [flt_idx];
		Eq2p           eq_z;
		this->bilinear_2p (eq_z, eq_s, split._freq_act);
		flt.set_z_eq_one (coord._elt_idx, eq_z._b.data (), eq_z._a.data ());
	}

	// 1-pole sections
	for (int flt_idx = 0; flt_idx < _nbr_1p; ++flt_idx)
	{
		const auto &   eq_s = split._eq_1p [flt_idx];
		auto &         flt  = pack._f1p_arr [flt_idx];
		Eq1p           eq_z;
		this->bilinear_1p (eq_z, eq_s, split._freq_act);
		flt.set_z_eq_one (coord._elt_idx, eq_z._b.data (), eq_z._a.data ());
	}
}



template <int O>
typename SplitMultibandLinSimd <O>::SplitCoord	SplitMultibandLinSimd <O>::conv_split_to_pack (int split_idx) noexcept
{
	assert (split_idx >= 0);

	return {
		split_idx >> _simd_w_l2,
		split_idx & ((1 << _simd_w_l2) - 1)
	};
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitMultibandLinSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
