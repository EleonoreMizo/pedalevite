/*****************************************************************************

        MapSaturate.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_MapSaturate_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_MapSaturate_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, class C, class XS, class YS>
T	MapSaturate <T, C, XS, YS>::saturate (T x)
{
	assert (x / get_xs () >= 0);
	assert (x / get_xs () <= 1);

	const T        sx = get_xs ();
	const T        sy = get_ys ();
	const T        cc = get_c ();
	const T        c1 = cc + T (1);

	const T        y  = c1*sy - cc*c1*sx*sy / (x + cc*sx);

	assert (y / get_ys () >= 0);
	assert (y / get_ys () <= 1);

	return y;
}



template <typename T, class C, class XS, class YS>
T	MapSaturate <T, C, XS, YS>::desaturate (T y)
{
	assert (y / get_ys () >= 0);
	assert (y / get_ys () <= 1);

	const T        sx = get_xs ();
	const T        sy = get_ys ();
	const T        cc = get_c ();
	const T        c1 = cc + T (1);

	const T        x  = cc*c1*sx*sy / (c1*sy - y) - cc*sx;

	assert (x / get_xs () >= 0);
	assert (x / get_xs () <= 1);

	return x;
}



template <typename T, class C, class XS, class YS>
T	MapSaturate <T, C, XS, YS>::get_c ()
{
	return T (C::num) / T (C::den);
}

template <typename T, class C, class XS, class YS>
T	MapSaturate <T, C, XS, YS>::get_xs ()
{
	return T (XS::num) / T (XS::den);
}

template <typename T, class C, class XS, class YS>
T	MapSaturate <T, C, XS, YS>::get_ys ()
{
	return T (YS::num) / T (YS::den);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_MapSaturate_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
