/*****************************************************************************

        ControlCurve.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/shape/MapSaturateBipolar.h"
#include "mfx/ControlCurve.h"

#include <complex>

#include <cassert>
#include <cmath>



namespace mfx
{



// 3x^2 - 2x^3 - a = 0
// d = cbrt (2 * sqrt (a * (a - 1)) - 2 * a + 1)
// | x = 0.5 + 0.5 * (d + 1 / d)
// | x = -0.25 * (1 - i * sqrt (3)) * d - 0.25*(1 + i * sqrt(3)) / d + 0.5
// | x = -0.25 * (1 + i * sqrt (3)) * d - 0.25*(1 - i * sqrt(3)) / d + 0.5 <-- this one
static double	ControlCurve_invert_s1 (double val)
{
	const double                  a_r = fabs (val);
	const std::complex <double>   a (a_r, 0);
	const std::complex <double>   aam1 (a_r * (a_r - 1.));
	const std::complex <double>   d   =
		std::pow ((std::sqrt (aam1) - a) * 2. + 1., 1 / 3.0);
	const double      s3 = sqrt (3);
	const double      x  =
		(- 0.25 * std::complex <double> (1,  s3) * d
		 - 0.25 * std::complex <double> (1, -s3) / d).real ()
		+ 0.5;
	
	return std::copysign (x, val);
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const char *  ControlCurve_get_name (ControlCurve c)
{
	assert (c >= 0);
	assert (c < ControlCurve_NBR_ELT);

	switch (c)
	{
	case ControlCurve_LINEAR: return ("Linear"); break;
	case ControlCurve_SQ:     return ("Square"); break;
	case ControlCurve_CB:     return ("Cubic" ); break;
	case ControlCurve_SQINV:  return ("Sq inv"); break;
	case ControlCurve_CBINV:  return ("Cb inv"); break;
	case ControlCurve_S1:     return ("S 1"   ); break;
	case ControlCurve_S2:     return ("S 2"   ); break;
	case ControlCurve_FLAT1:  return ("Flat 1"); break;
	case ControlCurve_FLAT2:  return ("Flat 2"); break;
	case ControlCurve_DES1:   return ("Prog 1"); break;
	case ControlCurve_SAT1:   return ("Sat 1" ); break;
	case ControlCurve_DES2:   return ("Prog 2"); break;
	case ControlCurve_SAT2:   return ("Sat 2" ); break;
	case ControlCurve_DES3:   return ("Prog 3"); break;
	case ControlCurve_SAT3:   return ("Sat 3" ); break;
	case ControlCurve_DES4:   return ("Prog 4"); break;
	case ControlCurve_SAT4:   return ("Sat 4" ); break;
	default:
		assert (false);
		break;
	}

	return "\?\?\?";
}



namespace
{
	template <int N> using MapSatBipolN = dsp::shape::MapSaturateBipolar <
		float,
		std::ratio <1, (N << 1) - 1>,
		std::ratio <1, 1>,
		std::ratio <1, 1>
	>;
}

// Input is not range-restricted, it can be bipolar too.
float	ControlCurve_apply_curve (float val, ControlCurve curve, bool invert_flag)
{
	static const int  inv = 1000;
	switch (curve + ((invert_flag) ? inv : 0))
	{
	case ControlCurve_LINEAR:
	case ControlCurve_LINEAR + inv:
		// Nothing
		break;

	case ControlCurve_SQ:
		val = fabs (val) * val;
		break;
	case ControlCurve_SQ + inv:
		val = std::copysign (sqrt (fabs (val)), val);
		break;

	case ControlCurve_CB:
		val = val * val * val;
		break;
	case ControlCurve_CB + inv:
		val = cbrt (val);
		break;

	case ControlCurve_SQINV:
		{
			const float    vx = 1 - fabs (val);
			val = std::copysign (1 - vx * vx, val);
		}
		break;
	case ControlCurve_SQINV + inv:
		{
			const float    vy = fabs (val);
			val = std::copysign (1 - sqrt (1 - vy), val);
		}
		break;

	case ControlCurve_CBINV:
		{
			const float    vx = 1 - fabs (val);
			val = std::copysign (1 - vx * vx * vx, val);
		}
		break;
	case ControlCurve_CBINV + inv:
		{
			const float    vy = fabs (val);
			val = std::copysign (1 - cbrt (1 - vy), val);
		}
		break;

	case ControlCurve_S1:
	case ControlCurve_FLAT1 + inv:
		val = std::copysign (val * val * (3 - 2 * fabs (val)), val);
		break;
	case ControlCurve_S1 + inv:
	case ControlCurve_FLAT1:
		val = float (ControlCurve_invert_s1 (val));
		break;

	case ControlCurve_S2:
	case ControlCurve_FLAT2 + inv:
		val = ControlCurve_apply_curve (val, ControlCurve_S1, false);
		val = ControlCurve_apply_curve (val, ControlCurve_S1, false);
		break;
	case ControlCurve_S2 + inv:
	case ControlCurve_FLAT2:
		val = float (ControlCurve_invert_s1 (ControlCurve_invert_s1 (val)));
		break;

	case ControlCurve_DES1:
	case ControlCurve_SAT1 + inv:
		val = MapSatBipolN <1>::desaturate (val);
		break;
	case ControlCurve_DES1 + inv:
	case ControlCurve_SAT1:
		val = MapSatBipolN <1>::saturate (val);
		break;

	case ControlCurve_DES2:
	case ControlCurve_SAT2 + inv:
		val = MapSatBipolN <2>::desaturate (val);
		break;
	case ControlCurve_DES2 + inv:
	case ControlCurve_SAT2:
		val = MapSatBipolN <2>::saturate (val);
		break;

	case ControlCurve_DES3:
	case ControlCurve_SAT3 + inv:
		val = MapSatBipolN <3>::desaturate (val);
		break;
	case ControlCurve_DES3 + inv:
	case ControlCurve_SAT3:
		val = MapSatBipolN <3>::saturate (val);
		break;

	case ControlCurve_DES4:
	case ControlCurve_SAT4 + inv:
		val = MapSatBipolN <4>::desaturate (val);
		break;
	case ControlCurve_DES4 + inv:
	case ControlCurve_SAT4:
		val = MapSatBipolN <4>::saturate (val);
		break;

	default:
		assert (false);
		break;
	}

	return val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
