/*****************************************************************************

        PhaseHalfPiSse.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (hiir_PhaseHalfPiSse_CURRENT_CODEHEADER)
	#error Recursive inclusion of PhaseHalfPiSse code header.
#endif
#define hiir_PhaseHalfPiSse_CURRENT_CODEHEADER

#if ! defined (hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED)
#define hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/StageProcSseV4.h"

#include <mmintrin.h>

#include <cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
constexpr int 	PhaseHalfPiSse <NC>::_nbr_chn;
template <int NC>
constexpr int 	PhaseHalfPiSse <NC>::NBR_COEFS;



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <int NC>
PhaseHalfPiSse <NC>::PhaseHalfPiSse () noexcept
:	_filter ()
,	_prev (0)
,	_phase (0)
{
	for (int phase = 0; phase < _nbr_phases; ++phase)
	{
		for (int i = 0; i < _nbr_stages + 1; ++i)
		{
			_mm_store_ps (_filter [phase] [i]._coef, _mm_setzero_ps ());
		}
	}
	for (int i = NBR_COEFS; i < _nbr_stages * _stage_width; ++i)
	{
		set_single_coef (i, 1);
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
void	PhaseHalfPiSse <NC>::set_coefs (const double coef_arr []) noexcept
{
	assert (coef_arr != nullptr);

	for (int i = 0; i < NBR_COEFS; ++i)
	{
		set_single_coef (i, coef_arr [i]);
	}
}



/*
==============================================================================
Name: process_sample
Description:
	From one input sample, generates two samples with a pi/2 phase shift.
Input parameters:
	- input: The input sample.
Output parameters:
	- out_0: Output sample, ahead.
	- out_1: Output sample, late.
Throws: Nothing
==============================================================================
*/

template <int NC>
hiir_FORCEINLINE void	PhaseHalfPiSse <NC>::process_sample (float &out_0, float &out_1, float input) noexcept
{
	StageDataSse * filter_ptr = &_filter [_phase] [0];

	const auto     spl_in  = _mm_load_ss (&input);
	const auto     prev    = _mm_load_ss (&_prev);
	const auto     comb    = _mm_unpacklo_ps (prev, spl_in);
	const auto     spl_mid = _mm_load_ps (filter_ptr [_nbr_stages]._mem);
	constexpr auto shuf    = (2 << 0) | (3 << 2) | (0 << 4) | (1 << 6);
	auto           y       = _mm_shuffle_ps (spl_mid, comb, shuf);

	auto           mem     = _mm_load_ps (filter_ptr [0]._mem);

	StageProcSseV4 <_nbr_stages>::process_sample_neg (&filter_ptr [0], y, mem);

	_mm_store_ps (filter_ptr [_nbr_stages]._mem, y);

	out_1 = _mm_cvtss_f32 (_mm_shuffle_ps (y, y, 1));
	out_0 = _mm_cvtss_f32 (y);

	_prev  = input;
	_phase = 1 - _phase;
}



/*
==============================================================================
Name: process_block
Description:
	From a block of samples, generates two blocks of samples, with a pi/2
	phase shift between these signals.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl samples.
	- nbr_spl: Number of input samples to process, > 0
Output parameters:
	- out_0_ptr: Output sample array (ahead), capacity: nbr_spl samples.
	- out_1_ptr: Output sample array (late), capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	PhaseHalfPiSse <NC>::process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl) noexcept
{
	assert (out_0_ptr != nullptr);
	assert (out_1_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (out_0_ptr <= in_ptr || out_0_ptr >= in_ptr + nbr_spl);
	assert (out_1_ptr <= in_ptr || out_1_ptr >= in_ptr + nbr_spl);
	assert (out_0_ptr + nbr_spl <= out_1_ptr || out_1_ptr + nbr_spl <= out_0_ptr);
	assert (nbr_spl > 0);

	long           pos = 0;
	do
	{
		process_sample (out_0_ptr [pos], out_1_ptr [pos], in_ptr [pos]);
		++ pos;
	}
	while (pos < nbr_spl);
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
void	PhaseHalfPiSse <NC>::clear_buffers () noexcept
{
	for (int phase = 0; phase < _nbr_phases; ++phase)
	{
		for (int i = 0; i < _nbr_stages + 1; ++i)
		{
			_mm_store_ps (_filter [phase] [i]._mem, _mm_setzero_ps ());
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
constexpr int	PhaseHalfPiSse <NC>::_stage_width;
template <int NC>
constexpr int	PhaseHalfPiSse <NC>::_nbr_stages;
template <int NC>
constexpr int	PhaseHalfPiSse <NC>::_nbr_phases;
template <int NC>
constexpr int	PhaseHalfPiSse <NC>::_coef_shift;



template <int NC>
void	PhaseHalfPiSse <NC>::set_single_coef (int index, double coef) noexcept
{
	assert (index >= 0);
	assert (index < _nbr_stages * _stage_width);

	const int      stage = (index / _stage_width) + 1;
	const int      pos   = (index ^ _coef_shift) & (_stage_width - 1);
	for (int phase = 0; phase < _nbr_phases; ++phase)
	{
		_filter [phase] [stage]._coef [pos] = DataType (coef);
	}
}



}  // namespace hiir



#endif   // hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED

#undef hiir_PhaseHalfPiSse_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
