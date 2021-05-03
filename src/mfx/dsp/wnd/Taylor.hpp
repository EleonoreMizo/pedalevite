/*****************************************************************************

        Taylor.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_Taylor_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_Taylor_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

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
void	Taylor <T>::set_side_lobe_lvl (double lvl) noexcept
{
	assert (lvl > 0);
	assert (lvl < 1);

	_sl_lvl = lvl;
}



template <class T>
void	Taylor <T>::set_nbar (int nbar) noexcept
{
	assert (nbar > 0);

	_nbar = nbar;
}



template <class T>
int	Taylor <T>::compute_nbar_min (double lvl) noexcept
{
	const double   a = compute_a (lvl);

	return fstb::ceil_int (2 * a * a + 0.5);
}



// The the specified sidelobe level is guaranteed only if the function is true
template <class T>
bool	Taylor <T>::is_side_lobe_lvl_guaranteed () const noexcept
{
	const double   a = compute_a (_sl_lvl);

	return (_nbar >= 2 * a * a + 0.5);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	Taylor <T>::do_make_win (T data_ptr [], int len)
{
	assert (_nbar * 2 <= len);

	const double   a     = compute_a (_sl_lvl);

	// Taylor pulse widening (dilation) factor
	const double   sp2   =
		  fstb::sq (double (_nbar))
		/ (a * a + fstb::sq (_nbar - 0.5));

	// Precomputes fm values
	std::vector <double> fm_arr (_nbar);
	for (int m = 1; m < _nbar; ++m)
	{
		fm_arr [m] = compute_fm (m, sp2, a);
	}

	double         scale = 1;
	const int      hlen  = (len + 1) / 2;
	const int      mid_p =  len      / 2;
	const int      mid_n = (len - 1) / 2;
	for (int idx = 0; idx < hlen; ++idx)
	{
		const double   pos = mid_n - idx;
		const double   xi  = (pos - 0.5 * len + 0.5) / len;
		const double   mul = 2 * fstb::PI * xi;
		double         sum = 0;
		for (int m = 1; m < _nbar; ++m)
		{
			const double   fm = fm_arr [m];
			sum += fm * cos (m * mul);
		}
		double         w = 1 + 2 * sum;
		if (idx == 0)
		{
			scale = 1 / w;
			w = 1;
		}
		else
		{
			w *= scale;
		}

		data_ptr [mid_p + idx] = T (w);
		data_ptr [mid_n - idx] = T (w);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Computes the cosine weight
template <class T>
double	Taylor <T>::compute_fm (int m, double sp2, double a) noexcept
{
	assert (m > 0);
	assert (sp2 > 0);
	assert (a > 0);

	double         num = 1;
	double         den = 1;
	for (int n = 1; n < _nbar; ++n)
	{
		const double   m2 = double (m) * m;
		num *= 1 - m2 / (sp2 * (a * a + fstb::sq (n - 0.5)));
		if (n != m)
		{
			const double   n2 = double (n) * n;
			den *= 1 - m2 / n2;
		}
	}

	const int      s  = (((m + 1) & 1) == 0) ? 1 : -1;
	const double   fm = (s * num) / (2 * den);

	return fm;
}



template <class T>
double	Taylor <T>::compute_a (double lvl) noexcept
{
	assert (lvl > 0);
	assert (lvl < 1);

	return acosh (1 / lvl) / fstb::PI;
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_Taylor_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
