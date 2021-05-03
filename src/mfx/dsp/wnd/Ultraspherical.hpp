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



/*
mu indicates the behaviour of the side lobes with increasing frequency:
mu >  0: side lobes are decreasing
mu <  0: side lobes are increasing
mu == 0: side lobes are flat
*/

template <class T>
void	Ultraspherical <T>::set_mu (double mu) noexcept
{
	_mu = mu;
}



template <class T>
void	Ultraspherical <T>::set_x_mu (double x_mu) noexcept
{
	_x_mu = x_mu;
}



// w = null-to-null half width, in rad/s
// Integer negative mu will cause calculation issues.
template <class T>
double	Ultraspherical <T>::compute_x_mu_for_prescribed_null_to_null_width (int len, double mu, double w) noexcept
{
	assert (len >= 3);
	assert (mu >= 0 || mu != fstb::round (mu));
	assert (w > 0);
	assert (w < fstb::PI);

	const double   z    = compute_zero_of_c (len - 1, 1, mu, 0, 1e-6, 0);

	// Eq. 29
	const double   x_mu = z / cos (w * 0.5);

	return x_mu;
}



// w = main lobe half width, in rad/s
// Integer negative mu will cause calculation issues.
template <class T>
double	Ultraspherical <T>::compute_x_mu_for_prescribed_main_lobe_width (int len, double mu, double w) noexcept
{
	assert (len >= 3);
	assert (mu >= 0 || mu != fstb::round (mu));
	assert (w > 0);
	assert (w < fstb::PI);

	const double   a    = compute_a (len, mu, 1e-6);
	const double   b    = compute_b (len, mu, 1e-6);
	const double   par  = msgn (mu) * std::max (a, b);
	const double   x_a  = compute_zero_of_c (len - 1, 1, mu, par, 1e-6, 0);

	// Eq. 30
	const double   x_mu = x_a / cos (w * 0.5);

	return x_mu;
}



// r = ripple ratio (max_side_lobe_amp / main_lobe_amp)
// Integer negative mu will cause calculation issues.
template <class T>
double	Ultraspherical <T>::compute_x_mu_for_prescribed_ripple_ratio (int len, double mu, double r) noexcept
{
	assert (len >= 3);
	assert (mu >= 0 || mu != fstb::round (mu));
	assert (r > 0);
	assert (r < 1);

	double         x_mu = 1;
	if (fstb::is_null (mu))
	{
		// Eq. 19
		x_mu = cosh (acosh (1 / r) / (len - 1));
	}
	else
	{
		// Eq. 32
		const double   a    = compute_a (len, mu, 1e-6);
		const double   b    = compute_b (len, mu, 1e-6);
		const double   par  = msgn (mu) * std::max (a, b) / r;
		x_mu = compute_zero_of_c (len - 2, 1, mu, par, 1e-6, r);
	}

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
		double         c = 0;
		if (fstb::is_null (_mu))
		{
			c = compute_t (2 * m, x);
		}
		else
		{
			c = compute_c (2 * m, _mu, x);
		}
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
constexpr double	Ultraspherical <T>::msgn (double x) noexcept
{
	return (x < 0) ? -1 : 1;
}



template <class T>
double	Ultraspherical <T>::compute_t (int n, double x) noexcept
{
	assert (n >= 0);

	double         dummy;
	double         t;
	compute_t (t, dummy, n, x);

	return t;
}



template <class T>
void	Ultraspherical <T>::compute_t (double &t_n, double &t_n_m_1, int n, double x) noexcept
{
	assert (n >= 0);

	if (n == 0)
	{
		t_n     = 1;
		t_n_m_1 = 0;   // Not defined actually
	}
	else if (n == 1)
	{
		t_n     = x;
		t_n_m_1 = 1;
	}
	else
	{
		double         t_n_m_2;
		compute_t (t_n_m_1, t_n_m_2, n - 1, x);
		t_n = 2 * x * t_n_m_1 - t_n_m_2;
	}
}



template <class T>
double	Ultraspherical <T>::compute_c (int n, double mu, double x) noexcept
{
	assert (n >= 0);

	double         dummy;
	double         c;
	compute_c (c, dummy, n, mu, x);

	return c;
}



template <class T>
void	Ultraspherical <T>::compute_c (double &c_n, double &c_n_m_1, int n, double mu, double x) noexcept
{
	assert (n >= 0);

	if (n == 0)
	{
		c_n     = 1;
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

		// Eq. 14
		const double   a = 2 * x * (n - 1 + mu);
		const double   b = n - 2 + 2 * mu;
		c_n = (a * c_n_m_1 - b * c_n_m_2) / n;
	}
}



template <class T>
double	Ultraspherical <T>::compute_a (int len, double mu, double epsilon) noexcept
{
	return compute_v (len, mu, epsilon, 1);
}



template <class T>
double	Ultraspherical <T>::compute_b (int len, double mu, double epsilon) noexcept
{
	const int      l = (len - 2 + 1) / 2;

	return compute_v (len, mu, epsilon, l);
}



template <class T>
double	Ultraspherical <T>::compute_v (int len, double mu, double epsilon, int l) noexcept
{
	assert (len >= 3);
	assert (epsilon > 0);
	assert (l >= 1);
	assert (l <= (len - 2 + 1) / 2);

	const double	z = compute_zero_of_c (len - 2, l, mu + 1, 0, epsilon, 0);
	const double	v = fabs (compute_c (len - 1, mu, z));

	return v;
}



// param may be 0, a or a / r
// r > 0 indicates a modification of the starting point for the prescribed
// ripple ratio mode.
template <class T>
double	Ultraspherical <T>::compute_zero_of_c (int n, int l, double mu, double param, double epsilon, double r) noexcept
{
	assert (n >= 1);
	assert (l >= 1);
	assert (epsilon > 0);

	double         y;

	if (fstb::is_null (mu))
	{
		y = cos (fstb::PI * (l - 0.5) / n);
	}

	// This formula looks wrong in some cases.
	// Used in x_mu calculation for a prescribed ripple ratio, it gives an
	// x_mu < 1 for mu = 1. The generic case doesn't have this problem and
	// outputs an x_mu slightly > 1, as expected.
	// Adding an extra condition with r == 0 makes it work.
	else if (fstb::is_eq (mu, 1.0) && r == 0)
	{
		y = cos (l * fstb::PI / (n + 1));
	}

	else
	{
		const double   n2 = static_cast <double> (n) * n;
		if (r > 0)
		{
			// Eq. 33
			y  = cosh (acosh (1 / r) / (n - 1));
		}
		else
		{
			// Eq. 12
			y  = sqrt (n2 + 2*n*mu - 2*mu - 1) / (n + mu);
			y *= cos ((l - 1) * fstb::PI / (n + 1));
		}

		double         dif;
		do
		{
			// Eq. 13
			const double   c_n_mu     = compute_c (n, mu, y);
			const double   c_nm1_mup1 = compute_c (n - 1, mu + 1, y);
			assert (c_nm1_mup1 != 0);
			dif = (c_n_mu - param) / (2 * mu * c_nm1_mup1);

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

