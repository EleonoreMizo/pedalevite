/*****************************************************************************

        WsAsym11.h
        Author: Laurent de Soras, 2018

Function fits in the following constraints:
- Asymmetric
- Bounded
- Monotonic
- Continuous C1
- Interesting input range in [-1.2 ; +2.8]
- Output range is roughly scaled in [-0.8 ; +1.3]
- f(0) = 0
- The (0, 0) point is located to the point where the slope is maximum.
- Slope is 1 at 0
Cold biasing
Curve: https://www.desmos.com/calculator/vdcy3npihw

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsAsym11_HEADER_INCLUDED)
#define mfx_dsp_shape_WsAsym11_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace shape
{



class WsAsym11
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline double  operator () (double x) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static inline double
	               fnc (double x);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const WsAsym11 &other) const = delete;
	bool           operator != (const WsAsym11 &other) const = delete;

}; // class WsAsym11



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/WsAsym11.hpp"



#endif   // mfx_dsp_shape_WsAsym11_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
