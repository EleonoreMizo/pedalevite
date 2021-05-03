/*****************************************************************************

        Kaiser.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_Kaiser_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_Kaiser_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <vector>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	Kaiser <T>::set_alpha (double alpha) noexcept
{
	assert (alpha > 0);

	_alpha = alpha;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	Kaiser <T>::do_make_win (T data_ptr [], int len)
{
   const int      half_len = len >> 1;
	std::vector <double> wp_sum (half_len + 1);

   double         den = 0;
   for (int pos = 0; pos <= half_len; ++pos)
   {
      den += compute_wp (pos, len, _alpha);
      wp_sum [pos] = den;
   }

	data_ptr [half_len] = 1;
   for (int pos = 0; pos < half_len; ++pos)
   {
      const double   num = wp_sum [pos];
      const T        val = T (sqrt (num / den));
      data_ptr [          pos] = val;
      data_ptr [len - 1 - pos] = val;
   }
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: compute_wp
Description:
   Computes W'(n, alpha) * I0 (Pi * alpha).
   The original formula for kaiser window uses just W', but because it is
   used in a ratio of sums of W' with constant alpha, I0 (PI * alpha) can
   be factored and thus removed - there is no need to compute it.

                                                             n - N/4 2
W'(n, alpha) * I0 (Pi * alpha) = I0 (Pi * alpha * sqrt (1 - (-------) ) )
                                                               N/4
Input parameters:
	- pos: n in the formula. [0 ; len/2]
	- len: N in the formula. > 0
	- alpha: Kernel Alpha Factor, > 0
Returns: Value of W'(n, alpha) * I0 (Pi * alpha) 
Throws: Nothing
==============================================================================
*/

template <class T>
double	Kaiser <T>::compute_wp (int pos, int len, double alpha) noexcept
{
   assert (pos >= 0);
   assert (pos <= len / 2);
   assert (len > 0);
   assert (alpha > 0);

   const double	len_q = len * 0.25;
   const double	r     = (pos - len_q) / len_q;
   const double	x     = fstb::PI * alpha * sqrt (1 - r * r);
   const double	wp    = compute_i0 (x);

   return wp;
}



template <class T>
double	Kaiser <T>::compute_i0 (double x) noexcept
{
   int            k    = 1;
   double         sum  = 1;
   double         prod = 1;
   double         fact = 1;
   double         ratio;

   x *= 0.5;
   do
   {
      prod  *= x;
      fact  *= k;
      ratio  = prod / fact;
      ratio *= ratio;
      sum   += ratio;
      ++ k;
   }
   while (sum < ratio * 1e10);

   return (sum);
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_Kaiser_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

