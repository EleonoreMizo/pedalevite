/*****************************************************************************

        WsTruncMod.h
        Author: Laurent de Soras, 2020

- LB: lower bound of the skipped range as std::ratio

- UB: upper bound of the skipped range as std::ratio

Generally LB is in [-1 ; 0[ range and UB ]0 ; +1]

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsTruncMod_HEADER_INCLUDED)
#define mfx_dsp_shape_WsTruncMod_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



template <class LB, class UB>
class WsTruncMod
{

	static_assert (
		std::ratio_greater_equal <UB, LB>::value,
		"WsTruncMod: LB <= UB"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

   template <typename T>
   fstb_FORCEINLINE T operator () (T x)
   {
      return process_sample (x);
   }

   template <typename T>
   static fstb_FORCEINLINE T process_sample (T x)
   {
		const T        lb = T (LB::num) / T (LB::den);
		const T        ub = T (UB::num) / T (UB::den);
      if (x <= lb)
      {
         x = T (fstb::trunc_int (x) - 1);
      }
      else if (x >= ub)
      {
         x = T (fstb::trunc_int (x) + 1);
      }
      return x;
   }



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsTruncMod



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsTruncMod.hpp"



#endif   // mfx_dsp_shape_WsTruncMod_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
