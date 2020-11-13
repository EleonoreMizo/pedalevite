/*****************************************************************************

        CompexGainFnc.cpp
        Author: Laurent de Soras, 2020

Knee formula calculation
------------------------

hard knee is supposed to be at (0, 0)
rl = ratio, low, expressed as a slope (0 = infinite ratio)
rh = ratio, high (slope too)
k = knee radius, relative to the input level. Knee operates in [-k ; +k]

f(x) = a*x^3 + b*x^2 + c*x + d
f'(x) = 3*a*x^2 + 2*b*x + c

f(k) = k * rh  	<=>	a*k^3 + b*k^2 + c*k + d = k * rh
f(-k) = -k * rl	<=>	-a*k^3 + b*k^2 - c*k + d = -k * rl
f'(k) = rh     	<=>	3*a*k^2 + 2*b*k + c = rh
f'(-k) = rl    	<=>	3*a*k^2 - 2*b*k + c = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
  -k^3 * a +   k^2 * b -   k * c + d = -k * rl
 3*k^2 * a + 2*k   * b +       c     = rh
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
 2*k^3 * a             + 2*k * c     = k * (rh + rl)		[l1 - l2]
 3*k^2 * a + 2*k   * b +       c     = rh
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
 2*k^2 * a             + 2   * c     = rh + rl      		[l2 / k]
 3*k^2 * a + 2*k   * b +       c     = rh
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
-4*k^2 * a + 4*k   * b               = rh - rl      		[l2 - 2 * l4]
             4*k   * b               = rh - rl   			[l3 - l4]
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
-4*k^2 * a + 4*k   * b               = rh - rl
                     b               = (rh - rl) / (4*k)
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
         a                           = 0
                     b               = (rh - rl) / (4*k)
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
         a                           = 0
                     b               = (rh - rl) / (4*k)
                               c     = (rh + rl) / 2

                                   d = (rh - rl) * k / 4
         a                           = 0
                     b               = (rh - rl) / (4*k)
                               c     = (rh + rl) / 2

Conclusion:
a = 0
b = (rh - rl) / (k * 4)
c = (rh + rl) / 2
d = (rh - rl) * k / 4
2nd order polynomial is enough.

Example:
rl = 3, rh = 1/5, k = 2
b = -7/20
c = 8/5
d = -7/5
f(x) = -7/20*x^2 + 8/5*x - 7/5

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/cpx/CompexGainFnc.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cpx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CompexGainFnc::update_curve (float rl, float rh, float thr_l2, float gain_l2, float knee_l2, bool autofix_flag)
{
	assert (rl > 0);
	assert (rh > 0);
	assert (knee_l2 >= 0);

	knee_l2 = std::max (knee_l2, 0.01f);

	const float    k = knee_l2 * 0.5f;  // 0.5 for diameter to radius conversion

	_knee_th_abs = k;
	_ratio_lo    = rl;
	_ratio_hi    = rh;

	const float    c2 = (rh - rl) * 0.25f / k;
	const float    c1 = (rh + rl) * 0.5f;
	const float    c0 = (rh - rl) * 0.25f * k;

	_knee_coef_arr [0] = c0;
	_knee_coef_arr [1] = c1;
	_knee_coef_arr [2] = c2;

	_vol_offset_pre = thr_l2;

	float          fixed_gain = gain_l2;
	if (autofix_flag)
	{
		const float    thr_dist     = thr_l2 - _lvl_ref_l2;
		const float    ratio_at_ref = (thr_dist < 0) ? rh : 1;
		const float    gain_at_ref  = thr_dist * (1 - ratio_at_ref);
		fixed_gain -= gain_at_ref;
	}

	_vol_offset_post = fixed_gain;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
