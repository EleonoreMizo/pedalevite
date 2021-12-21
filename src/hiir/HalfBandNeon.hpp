/*****************************************************************************

        HalfBandNeon.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_HalfBandNeon_CODEHEADER_INCLUDED)
#define hiir_HalfBandNeon_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/fnc_neon.h"
#include "hiir/StageProcNeonV4.h"

#include <cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
constexpr int 	HalfBandNeon <NC>::_nbr_chn;
template <int NC>
constexpr int 	HalfBandNeon <NC>::NBR_COEFS;



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <int NC>
HalfBandNeon <NC>::HalfBandNeon () noexcept
:	_filter ()
,	_prev (0)
,	_phase (0)
{
	for (int phase = 0; phase < NBR_PHASES; ++phase)
	{
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			storea (_filter [phase] [i]._coef, vdupq_n_f32 (0));
		}
		if ((NBR_COEFS & 1) != 0)
		{
			const int      pos = (NBR_COEFS ^ 1) & (STAGE_WIDTH - 1);
			_filter [phase] [NBR_STAGES]._coef [pos] = 1;
		}
	}

	clear_buffers ();
}



/*
==============================================================================
Name: set_coefs
Description:
	Sets filter coefficients. Generate them with the PolyphaseIir2Designer
	class.
	Call this function before doing any processing.
Input parameters:
	- coef_arr: Array of coefficients. There should be as many coefficients as
		mentioned in the class template parameter.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	HalfBandNeon <NC>::set_coefs (const double coef_arr []) noexcept
{
	assert (coef_arr != nullptr);

	for (int phase = 0; phase < NBR_PHASES; ++phase)
	{
		for (int i = 0; i < NBR_COEFS; ++i)
		{
			const int      stage = (i / STAGE_WIDTH) + 1;
			const int      pos   = (i ^ 1) & (STAGE_WIDTH - 1);
			_filter [phase] [stage]._coef [pos] = DataType (coef_arr [i]);
		}
	}
}



/*
==============================================================================
Name: process_sample
Description:
	Performs a half-band low-pass filtering on a sample.
Input parameters:
	- in: sample from the stream to be filtered.
Returns: filtered sample.
Throws: Nothing
==============================================================================
*/

template <int NC>
float	HalfBandNeon <NC>::process_sample (float input) noexcept
{
	auto           y = process_2_paths (input);
	y *= vdupq_n_f32 (0.5f);
	const auto     even = vgetq_lane_f32 (y, 3);
	const auto     odd  = vgetq_lane_f32 (y, 2);
	const auto     low  = even + odd;

	return low;
}



/*
==============================================================================
Name: process_block
Description:
	Performs a half-band low-pass filtering on a block of samples.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl samples.
	- nbr_spl: Number of samples to output, > 0
Output parameters:
	- out_ptr: Array for the output samples, capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	HalfBandNeon <NC>::process_block (float out_ptr [], const float in_ptr [], long nbr_spl) noexcept
{
	assert (in_ptr  != nullptr);
	assert (out_ptr != nullptr);
	assert (out_ptr <= in_ptr || out_ptr >= in_ptr + nbr_spl);
	assert (nbr_spl > 0);

	for (long pos = 0; pos < nbr_spl; ++pos)
	{
		out_ptr [pos] = process_sample (in_ptr [pos]);
	}
}



/*
==============================================================================
Name: process_sample_hpf
Description:
	Performs a half-band high-pass filtering on a sample.
Input parameters:
	- in: sample from the stream to be filtered.
Returns: filtered sample.
Throws: Nothing
==============================================================================
*/

template <int NC>
float	HalfBandNeon <NC>::process_sample_hpf (float input) noexcept
{
	auto           y = process_2_paths (input);
	y *= vdupq_n_f32 (0.5f);
	const auto     even = vgetq_lane_f32 (y, 3);
	const auto     odd  = vgetq_lane_f32 (y, 2);
	const auto     hi   = even - odd;

	return hi;
}



/*
==============================================================================
Name: process_block_hpf
Description:
	Performs a half-band high-pass filtering on a block of samples.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl samples.
	- nbr_spl: Number of samples to output, > 0
Output parameters:
	- out_ptr: Array for the output samples, capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	HalfBandNeon <NC>::process_block_hpf (float out_ptr [], const float in_ptr [], long nbr_spl) noexcept
{
	assert (in_ptr  != nullptr);
	assert (out_ptr != nullptr);
	assert (out_ptr <= in_ptr || out_ptr >= in_ptr + nbr_spl);
	assert (nbr_spl > 0);

	for (long pos = 0; pos < nbr_spl; ++pos)
	{
		out_ptr [pos] = process_sample_hpf (in_ptr [pos]);
	}
}



/*
==============================================================================
Name: process_sample_split
Description:
	Splits (spectrum-wise) in half a sample from a stream. Both part are
	results of a low-pass and a high-pass filtering, equivalent to the output
	of process_sample() and process_sample_hpf().
Input parameters:
	- in: sample from the stream to be filtered.
Output parameters:
	- low: output sample, lower part of the spectrum.
	- high: output sample, higher part of the spectrum.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	HalfBandNeon <NC>::process_sample_split (float &low, float &hi, float input) noexcept
{
	auto           y = process_2_paths (input);
	y *= vdupq_n_f32 (0.5f);
	const auto     even = vgetq_lane_f32 (y, 3);
	const auto     odd  = vgetq_lane_f32 (y, 2);
	low = even + odd;
	hi  = even - odd;
}



/*
==============================================================================
Name: process_block_split
Description:
	Splits (spectrum-wise) in half a block of samples. Both part are
	results of a low-pass and a high-pass filtering, equivalent to the output
	of process_sample() and process_sample_hpf().
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl samples.
	- nbr_spl: Number of samples to output, > 0
Output parameters:
	- out_l_ptr: Array for the output samples, lower part of the spectrum
		Capacity: nbr_spl samples.
	- out_h_ptr: Array for the output samples, higher part of the spectrum.
		Capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	HalfBandNeon <NC>::process_block_split (float out_l_ptr [], float out_h_ptr [], const float in_ptr [], long nbr_spl) noexcept
{
	assert (out_l_ptr != nullptr);
	assert (out_h_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (out_l_ptr <= in_ptr || out_l_ptr >= in_ptr + nbr_spl);
	assert (out_h_ptr <= in_ptr || out_h_ptr >= in_ptr + nbr_spl);
	assert (out_l_ptr + nbr_spl <= out_h_ptr || out_h_ptr + nbr_spl <= out_l_ptr);
	assert (nbr_spl > 0);

	for (long pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample_split (out_l_ptr [pos], out_h_ptr [pos], in_ptr [pos]);
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears filter memory, as if it processed silence since an infinite amount
	of time.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	HalfBandNeon <NC>::clear_buffers () noexcept
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			storea (_filter [phase] [i]._mem, vdupq_n_f32 (0));
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
constexpr int	HalfBandNeon <NC>::STAGE_WIDTH;
template <int NC>
constexpr int	HalfBandNeon <NC>::NBR_STAGES;
template <int NC>
constexpr int	HalfBandNeon <NC>::NBR_PHASES;



// Shared processing function, outputs both paths of the all-pass filter pair.
// y [2] = Odd coefs
// y [3] = Even coefs
template <int NC>
float32x4_t	HalfBandSse <NC>::process_2_paths (float input) noexcept
{
	StageDataNeonV4 * filter_ptr = &_filter [_phase] [0];

	const float32x2_t comb    = vset_lane_f32 (input, vdup_n_f32 (_prev), 1);
	const float32x2_t spl_mid =
#if ! defined (__BYTE_ORDER__) || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
		// Requires a little-endian architecture, which is generally the case
		load2a (filter_ptr [NBR_STAGES]._mem);
#else
		// Safe on any platform, but possibly slower.
		vget_low_f32 (load4a (filter_ptr [NBR_STAGES]._mem));
#endif
	float32x4_t       y       = vcombine_f32 (comb, spl_mid);
	float32x4_t       mem     = load4a (filter_ptr [0]._mem);

	StageProcNeonV4 <NBR_STAGES>::process_sample_neg (&filter_ptr [0], y, mem);

	_prev  = input;
	_phase = 1 - _phase;

	return y;
}



}  // namespace hiir



#endif   // hiir_HalfBandNeon_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
