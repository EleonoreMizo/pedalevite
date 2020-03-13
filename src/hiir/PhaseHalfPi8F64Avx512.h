/*****************************************************************************

        PhaseHalfPi8F64Avx512.h
        Author: Laurent de Soras, 2020

From the input signal, generates two signals with a pi/2 phase shift, using
AVX-512 instruction set. Works on vectors of 8 double.

This object must be aligned on a 64-byte boundary!

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
#if ! defined (hiir_PhaseHalfPi8F64Avx512_HEADER_INCLUDED)
#define hiir_PhaseHalfPi8F64Avx512_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataF64Avx512.h"

#include <immintrin.h>

#include <array>



namespace hiir
{



template <int NC>
class PhaseHalfPi8F64Avx512
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef double DataType;
	static const int  _nbr_chn = 8;

	enum {         NBR_COEFS = NC };

	               PhaseHalfPi8F64Avx512 ();
	               PhaseHalfPi8F64Avx512 (const PhaseHalfPi8F64Avx512 <NC> &other) = default;
	               PhaseHalfPi8F64Avx512 (PhaseHalfPi8F64Avx512 <NC> &&other)      = default;
	               ~PhaseHalfPi8F64Avx512 ()                            = default;

	PhaseHalfPi8F64Avx512 <NC> &
	               operator = (const PhaseHalfPi8F64Avx512 <NC> &other) = default;
	PhaseHalfPi8F64Avx512 <NC> &
	               operator = (PhaseHalfPi8F64Avx512 <NC> &&other)      = default;

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
	               process_sample (__m512d &out_0, __m512d &out_1, __m512d input);
	void           process_block (double out_0_ptr [], double out_1_ptr [], const double in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_phases = 2;

	typedef std::array <StageDataF64Avx512, NBR_COEFS + 2> Filter;   // Stages 0 and 1 contain only input memories

	typedef	std::array <Filter, _nbr_phases>	FilterBiPhase;

	FilterBiPhase  _bifilter;
	union
	{
		__m512d        _prev8;     // Just to ensure alignement
		double         _prev [_nbr_chn];
	};
	int            _phase;			// 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseHalfPi8F64Avx512 &other) const  = delete;
	bool           operator != (const PhaseHalfPi8F64Avx512 &other) const  = delete;

}; // class PhaseHalfPi8F64Avx512



}  // namespace hiir



#include "hiir/PhaseHalfPi8F64Avx512.hpp"



#endif   // hiir_PhaseHalfPi8F64Avx512_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
