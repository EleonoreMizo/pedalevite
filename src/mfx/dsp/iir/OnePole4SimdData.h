/*****************************************************************************

        OnePole4SimdData.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_OnePole4SimdData_HEADER_INCLUDED)
#define mfx_dsp_iir_OnePole4SimdData_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



class OnePole4SimdData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units = 4;   // Number of processing units, running parallel or serial

	typedef float VectFloat4 [4];

	alignas (16) VectFloat4
	               _z_eq_b [2];   // Direct coefficients, order z^(-n)
	alignas (16) VectFloat4
	               _z_eq_a [2];   // Recursive coefficients, order z^(-n). Content of index 0 is actually ignored.
	alignas (16) VectFloat4
	               _mem_x;        // Buffer for input memory
	alignas (16) VectFloat4
	               _mem_y;        // Buffer for output memory




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OnePole4SimdData &other) const = delete;
	bool           operator != (const OnePole4SimdData &other) const = delete;

}; // class OnePole4SimdData



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/OnePole4SimdData.hpp"



#endif   // mfx_dsp_iir_OnePole4SimdData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
