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

#include "fstb/fnc.h"
#include "mfx/dsp/shape/MapSaturateBipolar.h"
#include "mfx/ControlCurve.h"

#include <algorithm>
#include <complex>

#include <cassert>
#include <cmath>



namespace mfx
{



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace
{



// In/out range: [-1 ; 1], with internal mirroring around 0
template <typename T>
static T	ControlCurve_s1 (T x)
{
	const T        xa = T (fabs (x));
	assert (xa <= T (1));

	return std::copysign (x * x * (3 - 2 * xa), x);
}



// In/out range: [-1 ; 1], with internal mirroring around 0
static float	ControlCurve_invert_s1 (float val)
{
	const float    a_r = fabsf (val);
	assert (a_r <= 1);

#if 1

	// https://en.wikipedia.org/wiki/Smoothstep#Inverse_Smoothstep
	// Most likely cheaper than the other version
	const float    x = 0.5f - sinf (asinf (1.f - 2.f * a_r) / 3.f);

#else

	// 3x^2 - 2x^3 - a = 0
	// d = cbrt (2 * sqrt (a * (a - 1)) - 2 * a + 1)
	// | x = 0.5 + 0.5 * (d + 1 / d)
	// | x = -0.25 * (1 - i * sqrt (3)) * d - 0.25*(1 + i * sqrt(3)) / d + 0.5
	// | x = -0.25 * (1 + i * sqrt (3)) * d - 0.25*(1 - i * sqrt(3)) / d + 0.5 <-- this one
	const std::complex <double>   a (a_r, 0);
	const std::complex <double>   aam1 (a_r * (a_r - 1.f));
	const std::complex <double>   d   =
		std::pow ((std::sqrt (aam1) - a) * 2. + 1., 1 / 3.0);
	const double      s3 = sqrt (3);
	const float       x  = 0.5f - 0.25f * float ((
		  std::complex <double> (1,  s3) * d
		+ std::complex <double> (1, -s3) / d
	).real ());

#endif
	
	return std::copysign (x, val);
}



template <typename T, typename FNC>
fstb_FORCEINLINE T	ControlCurve_step1 (T x, FNC f)
{
	if (x < 0.5f)
	{
		return 0.5f - 0.5f * f (1 - 2 * x);
	}
	else
	{
		return 0.5f + 0.5f * f (2 * x - 1);
	}
}



template <typename T, typename FNC>
fstb_FORCEINLINE T	ControlCurve_step2 (T x, FNC f)
{
	if (x < 0.5f)
	{
		if (x < 0.25f)
		{
			return         0.25f * f (4 * x    );
		}
		else
		{
			return 0.50f - 0.25f * f (2 - 4 * x);
		}
	}
	else
	{
		if (x < 0.75f)
		{
			return 0.50f + 0.25f * f (4 * x - 2);
		}
		else
		{
			return 1.00f - 0.25f * f (4 - 4 * x);
		}
	}
}



template <int N> using MapSatBipolN = dsp::shape::MapSaturateBipolar <
	float,
	std::ratio <1, (N << 1) - 1>,
	std::ratio <1, 1>,
	std::ratio <1, 1>
>;



template <typename T, typename FNC>
fstb_FORCEINLINE T	ControlCurve_map_range_bip (T val, FNC f)
{
	if (val >= T (-1) && val <= T (1))
	{
		val = f (val);
	}

	return val;
}



template <typename T, typename FNC>
fstb_FORCEINLINE T	ControlCurve_map_range_mon (T val, FNC f)
{
	const T        va = T (fabs (val));
	if (va <= T (1))
	{
		val = std::copysign (f (va), val);
	}

	return val;
}



} // namespace



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
	case ControlCurve_STEP1:  return ("Step 1"); break;
	case ControlCurve_STEP2:  return ("Step 2"); break;
	default:
		assert (false);
		break;
	}

	return "\?\?\?";
}



// Input is not range-restricted, it can be bipolar too.
// As general rule, "saturating" curves have their input clipped to [-1 ; +1],
// whereas other curves have linear mapping for input values out of [-1 ; +1].
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
		val = fabsf (val) * val;
		break;
	case ControlCurve_SQ + inv:
		val = std::copysign (sqrtf (fabsf (val)), val);
		break;

	case ControlCurve_CB:
		val = val * val * val;
		break;
	case ControlCurve_CB + inv:
		val = cbrt (val);
		break;

	case ControlCurve_SQINV:
		{
			const float    vx = 1 - std::min (fabsf (val), 1.f);
			val = std::copysign (1 - vx * vx, val);
		}
		break;
	case ControlCurve_SQINV + inv:
		{
			val = ControlCurve_map_range_mon (val, [] (float x) {
				return 1 - sqrtf (1 - x);
			});
		}
		break;

	case ControlCurve_CBINV:
		{
			const float    vx = 1 - std::min (fabsf (val), 1.f);
			val = std::copysign (1 - vx * vx * vx, val);
		}
		break;
	case ControlCurve_CBINV + inv:
		{
			val = ControlCurve_map_range_mon (val, [] (float x) {
				return 1 - cbrt (1 - x);
			});
		}
		break;

	case ControlCurve_S1:
	case ControlCurve_FLAT1 + inv:
		val = ControlCurve_s1 (fstb::limit (val, -1.f, +1.f));
		break;
	case ControlCurve_S1 + inv:
	case ControlCurve_FLAT1:
		val = ControlCurve_map_range_bip (val, [] (float x) {
			return ControlCurve_invert_s1 (x);
		});
		break;

	case ControlCurve_S2:
	case ControlCurve_FLAT2 + inv:
		val = ControlCurve_s1 (fstb::limit (val, -1.f, +1.f));
		val = ControlCurve_s1 (val);
		break;
	case ControlCurve_S2 + inv:
	case ControlCurve_FLAT2:
		val = ControlCurve_map_range_bip (val, [] (float x) {
			return ControlCurve_invert_s1 (ControlCurve_invert_s1 (x));
		});
		break;

	case ControlCurve_DES1:
	case ControlCurve_SAT1 + inv:
		val = ControlCurve_map_range_bip (val, [] (float x) {
			return MapSatBipolN <1>::desaturate (x);
		});
		break;
	case ControlCurve_DES1 + inv:
	case ControlCurve_SAT1:
		val = MapSatBipolN <1>::saturate (fstb::limit (val, -1.f, +1.f));
		break;

	case ControlCurve_DES2:
	case ControlCurve_SAT2 + inv:
		val = ControlCurve_map_range_bip (val, [] (float x) {
			return MapSatBipolN <2>::desaturate (x);
		});
		break;
	case ControlCurve_DES2 + inv:
	case ControlCurve_SAT2:
		val = MapSatBipolN <2>::saturate (fstb::limit (val, -1.f, +1.f));
		break;

	case ControlCurve_DES3:
	case ControlCurve_SAT3 + inv:
		val = ControlCurve_map_range_bip (val, [] (float x) {
			return MapSatBipolN <3>::desaturate (x);
		});
		break;
	case ControlCurve_DES3 + inv:
	case ControlCurve_SAT3:
		val = MapSatBipolN <3>::saturate (fstb::limit (val, -1.f, +1.f));
		break;

	case ControlCurve_DES4:
	case ControlCurve_SAT4 + inv:
		val = ControlCurve_map_range_bip (val, [] (float x) {
			return MapSatBipolN <4>::desaturate (x);
		});
		break;
	case ControlCurve_DES4 + inv:
	case ControlCurve_SAT4:
		val = MapSatBipolN <4>::saturate (fstb::limit (val, -1.f, +1.f));
		break;

	case ControlCurve_STEP1:
		val = std::copysign (ControlCurve_step1 (
			std::min (fabsf (val), 1.f),
			[] (float x) { return fstb::ipowpc <4> (x); }
		), val);
		break;
	case ControlCurve_STEP1 + inv:
		val = ControlCurve_map_range_mon (val, [] (float x) {
			return ControlCurve_step1 (x, [] (float x) {
				return sqrtf (sqrtf (x));
			});
		});
		break;

	case ControlCurve_STEP2:
		val = std::copysign (ControlCurve_step2 (
			std::min (fabsf (val), 1.f),
			[] (float x) { return fstb::ipowpc <4> (x); }
		), val);
		break;
	case ControlCurve_STEP2 + inv:
		val = ControlCurve_map_range_mon (val, [] (float x) {
			return ControlCurve_step2 (x, [] (float x) {
				return sqrtf (sqrtf (x));
			});
		});
		break;

	default:
		assert (false);
		break;
	}

	return val;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
