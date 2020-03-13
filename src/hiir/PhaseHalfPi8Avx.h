/*****************************************************************************

        PhaseHalfPi8Avx.h
        Author: Laurent de Soras, 2020

From the input signal, generates two signals with a pi/2 phase shift, using
AVX instruction set. Works on vectors of 8 float.

This object must be aligned on a 32-byte boundary!

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
#if ! defined (hiir_PhaseHalfPi8Avx_HEADER_INCLUDED)
#define hiir_PhaseHalfPi8Avx_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataAvx.h"

#include <immintrin.h>

#include <array>



namespace hiir
{



template <int NC>
class PhaseHalfPi8Avx
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef float DataType;
	static const int  _nbr_chn = 8;

	enum {         NBR_COEFS = NC };

	               PhaseHalfPi8Avx ();
	               PhaseHalfPi8Avx (const PhaseHalfPi8Avx <NC> &other) = default;
	               PhaseHalfPi8Avx (PhaseHalfPi8Avx <NC> &&other)      = default;
	               ~PhaseHalfPi8Avx ()                                 = default;

	PhaseHalfPi8Avx <NC> &
	               operator = (const PhaseHalfPi8Avx <NC> &other)      = default;
	PhaseHalfPi8Avx <NC> &
	               operator = (PhaseHalfPi8Avx <NC> &&other)           = default;

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
	               process_sample (__m256 &out_0, __m256 &out_1, __m256 input);
	void           process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_phases = 2;

	typedef std::array <StageDataAvx, NBR_COEFS + 2> Filter;   // Stages 0 and 1 contain only input memories

	typedef	std::array <Filter, _nbr_phases>	FilterBiPhase;

	FilterBiPhase  _bifilter;
	union
	{
		__m256         _prev8;     // Just to ensure alignement
		float          _prev [_nbr_chn];
	};
	int            _phase;			// 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseHalfPi8Avx &other) const = delete;
	bool           operator != (const PhaseHalfPi8Avx &other) const = delete;

}; // class PhaseHalfPi8Avx



}  // namespace hiir



#include "hiir/PhaseHalfPi8Avx.hpp"



#endif   // hiir_PhaseHalfPi8Avx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
