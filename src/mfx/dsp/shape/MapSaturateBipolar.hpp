/*****************************************************************************

        MapSaturateBipolar.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_MapSaturateBipolar_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_MapSaturateBipolar_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/shape/MapSaturate.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, class C, class XS, class YS>
T	MapSaturateBipolar <T, C, XS, YS>::saturate (T x)
{
	static const int  sgn_x = (std::ratio_less <XS, R0>::value) ? -1 : 1;

	typedef MapSaturate <
		T, C,
		std::ratio < sgn_x * XS::num, XS::den>,
		std::ratio < sgn_x * YS::num, YS::den>
	> OpPos;
	typedef MapSaturate <
		T, C,
		std::ratio <-sgn_x * XS::num, XS::den>,
		std::ratio <-sgn_x * YS::num, YS::den>
	> OpNeg;

	if (x < 0)
	{
		return OpNeg::saturate (x);
	}

	return OpPos::saturate (x);
}



template <typename T, class C, class XS, class YS>
T	MapSaturateBipolar <T, C, XS, YS>::desaturate (T y)
{
	static const int  sgn_y = (std::ratio_less <YS, R0>::value) ? -1 : 1;

	typedef MapSaturate <
		T, C,
		std::ratio < sgn_y * XS::num, XS::den>,
		std::ratio < sgn_y * YS::num, YS::den>
	> OpPos;
	typedef MapSaturate <
		T, C,
		std::ratio <-sgn_y * XS::num, XS::den>,
		std::ratio <-sgn_y * YS::num, YS::den>
	> OpNeg;

	if (y < 0)
	{
		return OpNeg::desaturate (y);
	}

	return OpPos::desaturate (y);
}



template <typename T, class C, class XS, class YS>
T	MapSaturateBipolar <T, C, XS, YS>::get_c ()
{
	return T (C::num) / T (C::den);
}

template <typename T, class C, class XS, class YS>
T	MapSaturateBipolar <T, C, XS, YS>::get_xs ()
{
	return T (XS::num) / T (XS::den);
}

template <typename T, class C, class XS, class YS>
T	MapSaturateBipolar <T, C, XS, YS>::get_ys ()
{
	return T (YS::num) / T (YS::den);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_MapSaturateBipolar_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
