/*****************************************************************************

        HannPoisson.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_HannPoisson_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_HannPoisson_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

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
void	HannPoisson <T>::set_alpha (double alpha)
{
	assert (alpha > 0);

	_alpha = alpha;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	HannPoisson <T>::do_make_win (T data_ptr [], int len)
{
	const double   len_half = double (len) * 0.5;

	for (long pos = 0; pos < len; ++pos)
	{
		const double   n   = pos - len_half;
		const double   n_d = n / len_half;
		const double   c   = 1 + cos (fstb::PI * n_d);
		const double   e   = exp (-_alpha * fabs (n_d));
		const double   val = 0.5 * c * e;

		data_ptr [pos] = T (val);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_HannPoisson_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

