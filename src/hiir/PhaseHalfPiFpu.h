/*****************************************************************************

        PhaseHalfPiFpu.h
        Author: Laurent de Soras, 2005

From the input signal, generates two signals with a pi/2 phase shift, using
FPU.

Template parameters:
	- NC: number of coefficients, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_PhaseHalfPiFpu_HEADER_INCLUDED)
#define hiir_PhaseHalfPiFpu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"

#include <array>



namespace hiir
{



template <int NC>
class PhaseHalfPiFpu
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {         NBR_COEFS = NC };

	               PhaseHalfPiFpu ();

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
	               process_sample (float &out_0, float &out_1, float input);
	void           process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {         NBR_PHASES = 2 };

	typedef std::array <float, NBR_COEFS> HyperGluar;

	class Memory
	{
	public:
		HyperGluar     _x;
		HyperGluar     _y;
	};

	typedef	std::array <Memory, NBR_PHASES>	MemoryBiPhase;

	HyperGluar     _coef;
	MemoryBiPhase  _mem;
	float          _prev;
	int            _phase;			// 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseHalfPiFpu <NC> &other);
	bool           operator != (const PhaseHalfPiFpu <NC> &other);

}; // class PhaseHalfPiFpu



}  // namespace hiir



#include "hiir/PhaseHalfPiFpu.hpp"



#endif   // hiir_PhaseHalfPiFpu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
