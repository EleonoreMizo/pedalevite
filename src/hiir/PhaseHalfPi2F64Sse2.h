/*****************************************************************************

        PhaseHalfPi2F64Sse2.h
        Author: Laurent de Soras, 2020

From the input signal, generates two signals with a pi/2 phase shift, using
SSE instruction set. Works on vectors of 2 double.

This object must be aligned on a 16-byte boundary!

Template parameters:
	- NC: number of coefficients, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (hiir_PhaseHalfPi2F64Sse2_HEADER_INCLUDED)
#define hiir_PhaseHalfPi2F64Sse2_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataF64Sse2.h"

#include <emmintrin.h>

#include <array>



namespace hiir
{



template <int NC>
class PhaseHalfPi2F64Sse2
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef double DataType;
	static const int  _nbr_chn = 2;

	enum {         NBR_COEFS = NC };

	               PhaseHalfPi2F64Sse2 ();
	               PhaseHalfPi2F64Sse2 (const PhaseHalfPi2F64Sse2 <NC> &other) = default;
	               PhaseHalfPi2F64Sse2 (PhaseHalfPi2F64Sse2 <NC> &&other)      = default;
	               ~PhaseHalfPi2F64Sse2 ()                            = default;

	PhaseHalfPi2F64Sse2 <NC> &
	               operator = (const PhaseHalfPi2F64Sse2 <NC> &other) = default;
	PhaseHalfPi2F64Sse2 <NC> &
	               operator = (PhaseHalfPi2F64Sse2 <NC> &&other)      = default;

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
	               process_sample (__m128d &out_0, __m128d &out_1, __m128d input);
	void           process_block (double out_0_ptr [], double out_1_ptr [], const double in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_phases = 2;

	typedef std::array <StageDataF64Sse2, NBR_COEFS + 2> Filter;   // Stages 0 and 1 contain only input memories

	typedef	std::array <Filter, _nbr_phases>	FilterBiPhase;

	FilterBiPhase  _bifilter;
	union
	{
		__m128d        _prev4;     // Just to ensure alignement
		double         _prev [4];
	};
	int            _phase;			// 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseHalfPi2F64Sse2 &other) const   = delete;
	bool           operator != (const PhaseHalfPi2F64Sse2 &other) const   = delete;

}; // class PhaseHalfPi2F64Sse2



}  // namespace hiir



#include "hiir/PhaseHalfPi2F64Sse2.hpp"



#endif   // hiir_PhaseHalfPi2F64Sse2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
