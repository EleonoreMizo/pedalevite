/*****************************************************************************

        Biquad4SimdData.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Biquad4SimdData_HEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4SimdData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



class Biquad4SimdData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units = 4;   // Number of processing units, running parallel or serial

	typedef float VectFloat4 [4];
	fstb_TYPEDEF_ALIGN (16, VectFloat4, VectFloat4Aligned);

	VectFloat4Aligned
	               _z_eq_b [3];   // Direct coefficients, order z^(-n)
	VectFloat4Aligned
	               _z_eq_a [3];   // Recursive coefficients, order z^(-n). Content of index 0 is actually ignored.
	VectFloat4Aligned
	               _mem_x [2];    // Ring buffer for input memory
	VectFloat4Aligned
	               _mem_y [2];    // Ring buffer for output memory

	union
	{
		int            _mem_pos;   // Write position in ring buffers. 0 or 1.
		VectFloat4Aligned
		               _pad;
	};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Biquad4SimdData &other) const = delete;
	bool           operator != (const Biquad4SimdData &other) const = delete;

}; // class Biquad4SimdData



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/Biquad4SimdData.hpp"



#endif   // mfx_dsp_iir_Biquad4SimdData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
