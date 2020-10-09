/*****************************************************************************

        PhaseHalfPiFpuTpl.h
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



#if ! defined (hiir_PhaseHalfPiFpuTpl_HEADER_INCLUDED)
#define hiir_PhaseHalfPiFpuTpl_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataFpu.h"

#include <array>



namespace hiir
{



template <int NC, typename DT>
class PhaseHalfPiFpuTpl
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef DT DataType;
	static constexpr int _nbr_chn  = 1;
	static constexpr int NBR_COEFS = NC;

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
	               process_sample (DataType &out_0, DataType &out_1, DataType input);
	void           process_block (DataType out_0_ptr [], DataType out_1_ptr [], const DataType in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _nbr_phases = 2;

	// Stages 0 and 1 contain only input memories
	typedef std::array <StageDataFpu <DataType>, NBR_COEFS + 2> Filter;

	typedef	std::array <Filter, _nbr_phases>	FilterBiPhase;

	FilterBiPhase  _bifilter;
	DataType       _prev;
	int            _phase;			// 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseHalfPiFpuTpl <NC, DT> &other) = delete;
	bool           operator != (const PhaseHalfPiFpuTpl <NC, DT> &other) = delete;

}; // class PhaseHalfPiFpuTpl



}  // namespace hiir



#include "hiir/PhaseHalfPiFpuTpl.hpp"



#endif   // hiir_PhaseHalfPiFpuTpl_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
