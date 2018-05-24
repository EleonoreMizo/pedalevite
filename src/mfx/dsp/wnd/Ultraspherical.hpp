/*****************************************************************************

        Ultraspherical.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_Ultraspherical_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_Ultraspherical_CODEHEADER_INCLUDED



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
void	Ultraspherical <T>::set_mu (double mu)
{
	_mu = mu;
}



template <class T>
void	Ultraspherical <T>::set_x_mu (double x_mu)
{
	_x_mu = x_mu;
}



// w = main lobe width
template <class T>
double	Ultraspherical <T>::compute_x_mu_for_prescribed_main_lobe_width (int len, double mu, double w)
{
	assert (len >= 3);
	assert (w > 0);
	assert (w < 1);

	const double   a    = compute_a (len, mu, 1e-6);
	const double   x_a  = compute_largest_zero_of_c (len - 1, mu, a, 1e-6);
	const double   x_mu = x_a / cos (w * (fstb::PI * 0.5));

	return x_mu;
}



// r = ripple ratio (max_side_lobe_amp / main_lobe_amp)
template <class T>
double	Ultraspherical <T>::compute_x_mu_for_prescribed_ripple_ratio (int len, double mu, double r)
{
	assert (len >= 3);
	assert (r > 0);
	assert (r < 1);

	const double   a    = compute_a (len, mu, 1e-6);
	const double   x_mu = compute_largest_zero_of_c (len - 1, mu, a / r, 1e-6);

	return x_mu;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	Ultraspherical <T>::do_make_win (T data_ptr [], int len)
{
	const int      m          = len / 2;
	const double   cos_mult_1 = fstb::PI / (2 * m + 1);

	// Precomputes C values
	std::vector <double>	c_arr (m + 1);
	for (int s = 0; s <= m; ++s)
	{
		const double   x = _x_mu * cos (s * cos_mult_1);
		double         c = compute_c (2 * m, _mu, x);
		if (s > 0)
		{
			c *= 2;
		}

		c_arr [s] = c;
	}

	// Generates coefficients
	const double   offset = ((len & 1) == 0) ?     0.5 : 0.0;
	const int      end    = ((len & 1) == 0) ? (m - 1) :   m;
	double         scale  = 1;
	for (int n = 0; n <= end; ++n)
	{
		const double   cos_mult_2 = cos_mult_1 * 2 * (n + offset);
		double         sum        = c_arr [0];
		for (int s = 1; s <= m; ++s)
		{
			const double	p = c_arr [s] * cos (s * cos_mult_2);
			sum += p;
		}

		const double   coef = sum / (2 * m + 1);
		if (n == 0)
		{
			scale = coef;
		}
		const T        coef_norm = T (coef / scale);

		data_ptr [m   + n] = coef_norm;
		data_ptr [end - n] = coef_norm;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
double	Ultraspherical <T>::compute_c (int n, double mu, double x)
{
	assert (n >= 0);

	double         dummy;
	double         c;
	compute_c (c, dummy, n, mu, x);

	return c;
}



template <class T>
void	Ultraspherical <T>::compute_c (double &c_n, double &c_n_m_1, int n, double mu, double x)
{
	assert (&c_n != 0);
	assert (&c_n_m_1 != 0);
	assert (n >= 0);

	if (n == 0)
	{
		c_n = 1;
		c_n_m_1 = 0;   // Not defined actually
	}

	else if (n == 1)
	{
		c_n     = 2 * mu * x;
		c_n_m_1 = 1;
	}

	else
	{
		double         c_n_m_2;
		compute_c (c_n_m_1, c_n_m_2, n - 1, mu, x);

		const double   a = 2 * x * (n - 1 + mu);
		const double   b = n - 2 + 2 * mu;
		c_n = (a * c_n_m_1 - b * c_n_m_2) / n;
	}
}



template <class T>
double	Ultraspherical <T>::compute_a (int len, double mu, double epsilon)
{
	assert (len >= 3);
	assert (epsilon > 0);

	const double	z = compute_largest_zero_of_c (len - 2, mu + 1, 0, 1e-6);
	const double	a = fabs (compute_c (len - 1, mu, z));

	return a;
}



template <class T>
double	Ultraspherical <T>::compute_largest_zero_of_c (int n, double mu, double a, double epsilon)
{
	assert (n >= 1);
	assert (a >= 0);
	assert (epsilon > 0);

	double         y;
	if (mu == 0)
	{
		y = cos (fstb::PI * 0.5 / n);
	}

	else
	{
		const double   n2 = static_cast <double> (n) * n;
		y = sqrt (n2 + 2*n*mu - 2*mu - 1) / (n + mu);

		double         dif;
		do
		{
			const double   c_n_mu    = compute_c (n, mu, y);
			const double   c_nm1_mup1 = compute_c (n - 1, mu + 1, y);
			assert (c_nm1_mup1 != 0);
			dif = (c_n_mu - a) / (2 * mu * c_nm1_mup1);

			y -= dif;
		}
		while (fabs (dif) >= epsilon);
	}

	return y;
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_Ultraspherical_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

