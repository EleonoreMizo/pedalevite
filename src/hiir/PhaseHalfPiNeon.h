/*****************************************************************************

        PhaseHalfPiNeon.h
        Author: Laurent de Soras, 2016

From the input signal, generates two signals with a pi/2 phase shift, using
NEON instruction set.

This object must be aligned on a 16-byte boundary!

If the number of coefficients is 2 or 3 modulo 4, the output is delayed from
1 sample, compared to the theoretical formula (or FPU implementation).

Template parameters:
	- NC: number of coefficients, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (hiir_PhaseHalfPiNeon_HEADER_INCLUDED)
#define hiir_PhaseHalfPiNeon_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataNeon.h"

#include <arm_neon.h>

#include <array>



namespace hiir
{



template <int NC>
class PhaseHalfPiNeon
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {         NBR_COEFS = NC };

	               PhaseHalfPiNeon ();
	               PhaseHalfPiNeon (const PhaseHalfPiNeon &other) = default;
	PhaseHalfPiNeon &
	               operator = (const PhaseHalfPiNeon &other)      = default;

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
	               process_sample (float &out_0, float &out_1, float input);
	void           process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {         STAGE_WIDTH = 4 };
	enum {         NBR_STAGES  = (NBR_COEFS + STAGE_WIDTH-1) / STAGE_WIDTH };
	enum {         NBR_PHASES  = 2 };

	typedef std::array <StageDataNeon, NBR_STAGES + 1> Filter;  // Stage 0 contains only input memory
   typedef std::array <Filter, NBR_PHASES> FilterBiPhase;

	FilterBiPhase  _filter;    // Should be the first member (thus easier to align)
	float          _prev;
	int            _phase;     // 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseHalfPiNeon <NC> &other) const = delete;
	bool           operator != (const PhaseHalfPiNeon <NC> &other) const = delete;

}; // class PhaseHalfPiNeon



}  // namespace hiir



#include "hiir/PhaseHalfPiNeon.hpp"



#endif   // hiir_PhaseHalfPiNeon_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
