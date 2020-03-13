/*****************************************************************************

        PhaseHalfPiFpuTpl.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (hiir_PhaseHalfPiFpuTpl_CURRENT_CODEHEADER)
	#error Recursive inclusion of PhaseHalfPiFpuTpl code header.
#endif
#define hiir_PhaseHalfPiFpuTpl_CURRENT_CODEHEADER

#if ! defined (hiir_PhaseHalfPiFpuTpl_CODEHEADER_INCLUDED)
#define hiir_PhaseHalfPiFpuTpl_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/StageProcFpu.h"



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <int NC, typename DT>
PhaseHalfPiFpuTpl <NC, DT>::PhaseHalfPiFpuTpl ()
:	_coef ()
,	_mem ()
,	_prev (0)
,	_phase (0)
{
	for (int i = 0; i < NBR_COEFS; ++i)
	{
		_coef [i] = 0;
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

template <int NC, typename DT>
void	PhaseHalfPiFpuTpl <NC, DT>::set_coefs (const double coef_arr [])
{
	assert (coef_arr != nullptr);

	for (int i = 0; i < NBR_COEFS; ++i)
	{
		_coef [i] = DataType (coef_arr [i]);
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

template <int NC, typename DT>
void	PhaseHalfPiFpuTpl <NC, DT>::process_sample (DataType &out_0, DataType &out_1, DataType input)
{
	out_0 = input;   // Even coefs
	out_1 = _prev;   // Odd coefs

	#if defined (_MSC_VER)
		#pragma inline_depth (255)
	#endif   // _MSC_VER

	StageProcFpu <NBR_COEFS, DataType>::process_sample_neg (
		NBR_COEFS,
		out_0,
		out_1,
		&_coef [0],
		&_mem [_phase]._x [0],
		&_mem [_phase]._y [0]
	);

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

template <int NC, typename DT>
void	PhaseHalfPiFpuTpl <NC, DT>::process_block (DataType out_0_ptr [], DataType out_1_ptr [], const DataType in_ptr [], long nbr_spl)
{
	assert (out_0_ptr != nullptr);
	assert (out_1_ptr != nullptr);
	assert (in_ptr    != nullptr);
	assert (out_0_ptr <= in_ptr || out_0_ptr >= in_ptr + nbr_spl);
	assert (out_1_ptr <= in_ptr || out_1_ptr >= in_ptr + nbr_spl);
	assert (out_0_ptr + nbr_spl <= out_1_ptr || out_1_ptr + nbr_spl <= out_0_ptr);
	assert (nbr_spl > 0);

	long           pos = 0;
	if (_phase == 1)
	{
		process_sample (out_0_ptr [0], out_1_ptr [0], in_ptr [0]);
		++ pos;
	}

	const long     end = ((nbr_spl - pos) & -NBR_PHASES) + pos;
	while (pos < end)
	{
		const DataType input_0 = in_ptr [pos];
		out_0_ptr [pos] = input_0;
		out_1_ptr [pos] = _prev;
		StageProcFpu <NBR_COEFS, DataType>::process_sample_neg (
			NBR_COEFS,
			out_0_ptr [pos],
			out_1_ptr [pos],
			&_coef [0],
			&_mem [0]._x [0],
			&_mem [0]._y [0]
		);

		const DataType input_1 = in_ptr [pos + 1];
		out_0_ptr [pos + 1] = input_1;
		out_1_ptr [pos + 1] = input_0;	// _prev
		StageProcFpu <NBR_COEFS, DataType>::process_sample_neg (
			NBR_COEFS,
			out_0_ptr [pos + 1],
			out_1_ptr [pos + 1],
			&_coef [0],
			&_mem [1]._x [0],
			&_mem [1]._y [0]
		);
		_prev = input_1;

		pos += 2;
	}

	if (pos < nbr_spl)
	{
		assert (pos + 1 == nbr_spl);
		process_sample (out_0_ptr [pos], out_1_ptr [pos], in_ptr [pos]);
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

template <int NC, typename DT>
void	PhaseHalfPiFpuTpl <NC, DT>::clear_buffers ()
{
	for (int phase = 0; phase < NBR_PHASES; ++phase)
	{
		for (int i = 0; i < NBR_COEFS; ++i)
		{
			_mem [phase]._x [i] = 0;
			_mem [phase]._y [i] = 0;
		}
	}

	_prev  = 0;
	_phase = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hiir



#endif   // hiir_PhaseHalfPiFpuTpl_CODEHEADER_INCLUDED

#undef hiir_PhaseHalfPiFpuTpl_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
