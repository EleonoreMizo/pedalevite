/*****************************************************************************

        SineQPoly3.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_SineQPoly3_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_SineQPoly3_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, long XSN, long XSD, long YSN, long YSD>
T	SineQPoly3 <T, XSN, XSD, YSN, YSD>::operator () (T x) const noexcept
{
	return approximate (x);
}



template <typename T, long XSN, long XSD, long YSN, long YSD>
T	SineQPoly3 <T, XSN, XSD, YSN, YSD>::approximate (T x) noexcept
{
	const T			sx = T (XSN) / T (XSD);
	const T			sy = T (YSN) / T (YSD);

	assert (x / sx >= 0);
	assert (x / sx <= 1);

	const double	aa = fstb::PI * 0.5;
	const double	bb = 3 - fstb::PI;
	const double	cc = fstb::PI * 0.5 - 2;
	const T			a = sy * T (aa) / sx;
	const T			b = sy * T (bb) / (sx * sx);
	const T			c = sy * T (cc) / (sx * sx * sx);

	const T			y = ((c * x + b) * x + a) * x;

	assert (y / sy >= 0);
	assert (y / sy <= T (1.0000000000000002));	// Rounding error: 1 bit on a 64-bit float
 
	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_SineQPoly3_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
