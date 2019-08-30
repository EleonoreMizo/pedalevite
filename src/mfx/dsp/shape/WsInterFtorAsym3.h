/*****************************************************************************

        WsInterFtorAsym3.h
        Author: Laurent de Soras, 2018

Function fits in the following constraints:
- Asymmetric
- Bounded
- Monotonic
- Continuous C1
- Interesting input range in [-1.5 ; +2.0]
- Output range is roughly scaled in [-1.3 ; +0.8]
- f(0) = 0
- Slope is 1 at 0
Mild hot biasing
Curve: https://www.desmos.com/calculator/vdcy3npihw

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsInterFtorAsym3_HEADER_INCLUDED)
#define mfx_dsp_shape_WsInterFtorAsym3_HEADER_INCLUDED

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



class WsInterFtorAsym3
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

	bool           operator == (const WsInterFtorAsym3 &other) const = delete;
	bool           operator != (const WsInterFtorAsym3 &other) const = delete;

}; // class WsInterFtorAsym3



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/WsInterFtorAsym3.hpp"



#endif   // mfx_dsp_shape_WsInterFtorAsym3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
