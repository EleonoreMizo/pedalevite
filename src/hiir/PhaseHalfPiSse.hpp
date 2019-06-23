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

#include "hiir/StageProcSse.h"

#include <mmintrin.h>

#include <cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <int NC>
PhaseHalfPiSse <NC>::PhaseHalfPiSse ()
:	_filter ()
,	_prev (0)
,	_phase (0)
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			_filter [phase] [i]._coef [0] = 0;
			_filter [phase] [i]._coef [1] = 0;
			_filter [phase] [i]._coef [2] = 0;
			_filter [phase] [i]._coef [3] = 0;
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
void	PhaseHalfPiSse <NC>::set_coefs (const double coef_arr [])
{
	assert (coef_arr != 0);

   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_COEFS; ++i)
		{
			const int      stage = (i / STAGE_WIDTH) + 1;
			const int      pos = (i ^ 1) & (STAGE_WIDTH - 1);
			_filter [phase] [stage]._coef [pos] = float (coef_arr [i]);
		}
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
hiir_FORCEINLINE void	PhaseHalfPiSse <NC>::process_sample (float &out_0, float &out_1, float input)
{
	StageDataSse * filter_ptr = &_filter [_phase] [0];

	const __m128   spl_in  = _mm_load_ss (&input);
	const __m128   prev    = _mm_load_ss (&_prev);
	const __m128   comb    = _mm_unpacklo_ps (prev, spl_in);
	const __m128   spl_mid = _mm_load_ps (filter_ptr [NBR_STAGES]._mem);
	__m128         y       = _mm_shuffle_ps (comb, spl_mid, 0x44);

	__m128         mem     = _mm_load_ps (filter_ptr [0]._mem);

	StageProcSse <NBR_STAGES>::process_sample_neg (&filter_ptr [0], y, mem);

	_mm_store_ps (filter_ptr [NBR_STAGES]._mem, y);

	// The latest shufps/movss instruction pairs can be freely inverted
	y = _mm_shuffle_ps (y, y, 0xE3);
	_mm_store_ss (&out_0, y);
	y = _mm_shuffle_ps (y, y, 0xE2);
	_mm_store_ss (&out_1, y);

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
void	PhaseHalfPiSse <NC>::process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_0_ptr != 0);
	assert (out_1_ptr != 0);
	assert (in_ptr != 0);
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
void	PhaseHalfPiSse <NC>::clear_buffers ()
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			_filter [phase] [i]._mem [0] = 0;
			_filter [phase] [i]._mem [1] = 0;
			_filter [phase] [i]._mem [2] = 0;
			_filter [phase] [i]._mem [3] = 0;
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hiir



#endif   // hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED

#undef hiir_PhaseHalfPiSse_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
