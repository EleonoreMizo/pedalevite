/*****************************************************************************

        Approx.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Approx_CODEHEADER_INCLUDED)
#define fstb_Approx_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cmath>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// f(x) ~ sin (x * pi/2)
// x in [-1 ; 1]
// Max error: 2.411e-8
ToolsSimd::VectF32	Approx::sin_rbj_halfpi (ToolsSimd::VectF32 x)
{
	const auto    a = ToolsSimd::set1_f32 ( 0.0001530302f);
	const auto    b = ToolsSimd::set1_f32 (-0.0046768800f);
	const auto    c = ToolsSimd::set1_f32 ( 0.0796915849f);
	const auto    d = ToolsSimd::set1_f32 (-0.6459640619f);
	const auto    e = ToolsSimd::set1_f32 ( 1.5707963268f);
	const auto    x2 = x * x;

	return (((((a * x2 + b) * x2 + c) * x2 + d) * x2) + e) * x;
}



// f(x) ~ sin (x * pi)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
ToolsSimd::VectF32	Approx::sin_rbj_pi (ToolsSimd::VectF32 x)
{
	const auto     one   = ToolsSimd::set1_f32 (1);
	const auto     two_x = one - ToolsSimd::abs (one - x - x);

	return sin_rbj_halfpi (two_x);
}



// f1(x) ~ sin (x * pi)
// f2(x) ~ sin (x * pi/2)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
void	Approx::sin_rbj_halfpi_pi (float &sx, float &s2x, float x)
{
	const auto     xv = ToolsSimd::set_2f32 (x, 1 - fabs (1 - 2*x));
	const auto     yv = sin_rbj_halfpi (xv);
	sx  = ToolsSimd::Shift <0>::extract (yv);
	s2x = ToolsSimd::Shift <1>::extract (yv);
}



// f1(x) ~ sin (x)
// x in [-pi ; pi]
// Max error: 0.919e-3
float	Approx::sin_nick (float x)
{
	const float    b = float ( 4 /  fstb::PI);
	const float    c = float (-4 / (fstb::PI * fstb::PI));
	const float    d = 0.22401f;

	const float    y = b * x + c * x * fabs (x);
	const float    z = d * (y * fabs (y) - y) + y;

	return z;
}

ToolsSimd::VectF32	Approx::sin_nick (ToolsSimd::VectF32 x)
{
	const auto     b = ToolsSimd::set1_f32 (float ( 4 /  fstb::PI));
	const auto     c = ToolsSimd::set1_f32 (float (-4 / (fstb::PI * fstb::PI)));
	const auto     d = ToolsSimd::set1_f32 (0.22401f);

	const auto     y = b * x + c * x * ToolsSimd::abs (x);
	const auto     z = d * (y * ToolsSimd::abs (y) - y) + y;

	return z;
}



// f1(x) ~ sin (x * 2 * pi)
// x in [-0.5 ; 0.5]
// Max error: 0.919e-3
float	Approx::sin_nick_2pi (float x)
{
	const float    b =   8;
	const float    c = -16;
	const float    d =   0.22401f;

	const float    y = b * x + c * x * fabs (x);
	const float    z = d * (y * fabs (y) - y) + y;

	return z;
}

ToolsSimd::VectF32	Approx::sin_nick_2pi (ToolsSimd::VectF32 x)
{
	const auto     b = ToolsSimd::set1_f32 (  8);
	const auto     c = ToolsSimd::set1_f32 (-16);
	const auto     d = ToolsSimd::set1_f32 (  0.22401f);

	const auto     y = b * x + c * x * ToolsSimd::abs (x);
	const auto     z = d * (y * ToolsSimd::abs (y) - y) + y;

	return z;
}



float	Approx::log2 (float val)
{
	assert (val > 0);

	union
	{
		int32_t        _i;
		float          _f;
	}              combo;
	combo._f = val;
	int				x = combo._i;
	const int		log_2 = ((x >> 23) & 255) - 128;
	x &= ~(255 << 23);
	x += 127 << 23;
	combo._i = x;
	val  = combo._f;

	const float		k = 0.5f;
	const float		a = (k - 1) / (k + 1);
	const float		b = (4 - 2*k) / (k + 1);	// 1 - 3*a
	const float		c = 2*a;
	val = (a * val + b) * val + c;

	return (val + log_2);
}



float	Approx::exp2 (float val)
{
	// Truncated val for integer power of 2
	const long		tx = floor_int (val);

	// Float remainder of power of 2
	val -= static_cast <float> (tx);

	// Quadratic approximation of 2^x in [0 ; 1]
	const float		a = 1.0f / 3.0f;
	const float		b = 2.0f / 3.0f;
	const float		c = 1.0f;
	val = (a * val + b) * val + c;

	union
	{
		int32_t        _i;
		float          _f;
	}              combo;
	combo._f = val;

	// Add integer power of 2 to exponent
	combo._i += tx << 23;
	val = combo._f;

	assert (val >= 0);

	return (val);
}



float	Approx::tan_taylor5 (float x)
{
	const float    x_2 = x * x;
	const float    c_1 = 1;
	const float    c_3 = 1.0f / 3;
	const float    c_5 = 2.0f / 15;

	return ((c_5 * x_2 + c_3) * x_2 + c_1) * x;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_Approx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
