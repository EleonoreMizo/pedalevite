/*****************************************************************************

        PhaseHalfPiNeon.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_PhaseHalfPiNeon_CODEHEADER_INCLUDED)
#define hiir_PhaseHalfPiNeon_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/StageProcNeon.h"

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
PhaseHalfPiNeon <NC>::PhaseHalfPiNeon ()
:	_filter ()
,	_prev (0)
,	_phase (0)
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			_filter [phase] [i]._coef4 = vdupq_n_f32 (0);
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
void	PhaseHalfPiNeon <NC>::set_coefs (const double coef_arr [])
{
	assert (coef_arr != 0);

   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_COEFS; ++i)
		{
			const int      stage = (i / STAGE_WIDTH) + 1;
			const int      pos   = (i ^ 1) & (STAGE_WIDTH - 1);
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
hiir_FORCEINLINE void	PhaseHalfPiNeon <NC>::process_sample (float &out_0, float &out_1, float input)
{
	StageDataNeon *   filter_ptr = &_filter [_phase] [0];

	const float32x2_t comb    = vset_lane_f32 (input, vdup_n_f32 (_prev), 1);
	const float32x2_t spl_mid = vget_low_f32 (filter_ptr [NBR_STAGES]._mem4);
	float32x4_t       y       = vcombine_f32 (comb, spl_mid);
	float32x4_t       mem     = filter_ptr [0]._mem4;

	StageProcNeon <NBR_STAGES>::process_sample_neg (&filter_ptr [0], y, mem);
	filter_ptr [NBR_STAGES]._mem4 = y;

	out_0  = vgetq_lane_f32 (y, 3);
	out_1  = vgetq_lane_f32 (y, 2);

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
void	PhaseHalfPiNeon <NC>::process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl)
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
void	PhaseHalfPiNeon <NC>::clear_buffers ()
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			_filter [phase] [i]._mem4 = vdupq_n_f32 (0);
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hiir



#endif   // hiir_PhaseHalfPiNeon_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
