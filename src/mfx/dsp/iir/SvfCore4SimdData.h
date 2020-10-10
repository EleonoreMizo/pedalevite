/*****************************************************************************

        SvfCore4SimdData.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SvfCore4SimdData_HEADER_INCLUDED)
#define mfx_dsp_iir_SvfCore4SimdData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



class SvfCore4SimdData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units = 4;   // Number of processing units, running parallel or serial

	typedef float VectFloat4 [4];

	// Coefficients
	alignas (16) VectFloat4
	               _g0;
	alignas (16) VectFloat4
	               _g1;
	alignas (16) VectFloat4
	               _g2;

	// Mixers
	alignas (16) VectFloat4
	               _v0m;
	alignas (16) VectFloat4
	               _v1m;
	alignas (16) VectFloat4
	               _v2m;

	// Internal states
	alignas (16) VectFloat4
	               _ic1eq;
	alignas (16) VectFloat4
	               _ic2eq;
	alignas (16) VectFloat4 // In    2x2 mode: mix state from this unit
	               _y;      // In serial mode: mix state from the previous unit in the processing chain



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SvfCore4SimdData &other) const = delete;
	bool           operator != (const SvfCore4SimdData &other) const = delete;

}; // class SvfCore4SimdData



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/SvfCore4SimdData.hpp"



#endif   // mfx_dsp_iir_SvfCore4SimdData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
