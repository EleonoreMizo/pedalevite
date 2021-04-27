/*****************************************************************************

        DesignElliptic.cpp
        Author: Laurent de Soras, 2021

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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/iir/DesignElliptic.h"
#include "mfx/dsp/iir/Zpk.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_spec
Description:
	Set minimum filter specifications. They are like two "no man's land"
	quarter-plane in the |H(s)| response plot, one bounding the (-oo ; -oo)
	corner, and the other one bounding the (+oo ; +oo) corner.
	The first one stops at (PB freq, -PB ripple) and the second one at (SB
	freq, -SB ripple).                
Input parameters:
	- passband_ripple: dB of ripple in the passband, > 0
	- stopband_ripple: dB of ripple in the stopband, > PB ripple
	- passband_freq: Frequency of the right passband limit, > 0 
	- stopband_freq: Frequency of the left stopband limit, > PB freq
Throws: Nothing
==============================================================================
*/

void	DesignElliptic::set_spec (double passband_ripple, double stopband_ripple, double passband_freq, double stopband_freq) noexcept
{
	assert (passband_ripple > 0);
	assert (stopband_ripple > passband_ripple);
	assert (passband_freq > 0);
	assert (stopband_freq > passband_freq);

	_passband_ripple = passband_ripple;
	_stopband_ripple = stopband_ripple;
	_passband_freq   = passband_freq;
	_stopband_freq   = stopband_freq;

	_valid_precomputation_flag = false;
}



/*
==============================================================================
Name: compute_min_order
Description:
	Compute the minimum order to realize the previously specified filter.
	If specifications are too strict, filter coefficents could't be calculated.
	Also pre-compute some variables used for the z/p/k generation.
Returns:
	- Filter order, > 0, or
	- A negative number if specs are too strict.
Throws: Nothing
==============================================================================
*/

int	DesignElliptic::compute_min_order () noexcept
{
	_valid_precomputation_flag = false;
	_selec_factor              = _passband_freq / _stopband_freq;

	const double   x  = pow ((1 - _selec_factor * _selec_factor), 0.25);
	const double   u  = (1 - x) / (2 * (1 + x));
	const double   u2 = u  * u;
	const double   u4 = u2 * u2;

	// u + 2*u^5 + 15*u^9 + 150*u^13
	_modular_const = u * (1 + u4 * (2 + u4 * (15 + u4 * 150)));

	const double   discrim_factor =
		  (exp (_stopband_ripple * (fstb::LN10 / 10)) - 1.0)
		/ (exp (_passband_ripple * (fstb::LN10 / 10)) - 1.0);

	// Test whether specifications are too strict
	const double   k1p = sqrt (1 - 1 / discrim_factor);
	if (k1p == 1)
	{
		_min_order = -1;
	}
	else
	{
		_min_order = fstb::ceil_int (
			-log (16.0 * discrim_factor) / log (_modular_const)
		);
		assert (_min_order > 0);
		_order = _min_order;

		_valid_precomputation_flag = true;
	}

	return _min_order;
}



/*
==============================================================================
Name: compute_coefs
Description:
	Entry point to compute zeros, poles and gain for the previously specificed
	filter. Specs are checked, and default order used if not explicitely set.
	Memory is allocated to the result object if needed.
	Poles and zeros are grouped by conjugate pairs (then possibly comes the
	real pole), and each pair of poles and zeros are grouped to benefit from
	the best digital implementation (nearest grouping). Low-Q biquads come
	first.
Output parameters:
	- zpk: Object used to store zeros, poles and gain.
	- order: Order for the filter. It must be greater or equal to the minimum
		order, or negative to directly use this minimum order.
Returns:
	0 if all is ok, or -1 if specs are too strict.
Throws:
	Nothing if provided zpk is big enough to store results, otherwise it's
	unspecified.
==============================================================================
*/

int	DesignElliptic::compute_coefs (Zpk &zpk, int order)
{
	int            ret_val = 0;

	if (! _valid_precomputation_flag)
	{
		if (compute_min_order () <= 0)
		{
			ret_val = -1;
		}
		else
		{
			_order = _min_order;
		}
	}

	if (ret_val == 0 && order > 0)
	{
		if (order < _min_order)
		{
			ret_val = -1;
		}
		else
		{
			_order = order;
		}
	}

	if (ret_val == 0)
	{
		ret_val = compute_coefs_main (zpk);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DesignElliptic::compute_coefs_main (Zpk &zpk)
{
	zpk.clear ();

	int            ret_val = 0;
	const int      nbr_biq = _order / 2;

	// Eq 6.12
	const double   pb_lin  = exp (_passband_ripple * (fstb::LN10 / 20));
	const double   vv      = log ((pb_lin + 1) / (pb_lin - 1)) / (2 * _order);

	// Eq 6.13
	double         p_sub_zero    = fabs (compute_eq_6_13 (vv));

	// Eq 6.14
	const double   p_sub_zero_sq = p_sub_zero * p_sub_zero;
	const double   ww = sqrt (
		  (1 + p_sub_zero_sq * _selec_factor)
		* (1 + p_sub_zero_sq / _selec_factor)
	);

	const double   ww_sq            = ww * ww;
	const double   passband_freq_sq = _passband_freq * _passband_freq;

	double         h_sub_zero = 1;
	const double   mu_base    = 0.5 + 0.5 * (_order & 1);
	for (int biq = 0; biq < nbr_biq; ++biq)
	{
		const double   mu    = mu_base + biq;

		// Eq 6.15
		const double   xx    = compute_eq_6_15 (mu);

		// Eq 6.16
		const double   xx_sq = xx * xx;
		const double   yy    = sqrt (
			  (1 - xx_sq * _selec_factor)
			* (1 - xx_sq / _selec_factor)
		);

		// Eq 6.17
		const double   aa    = passband_freq_sq / xx_sq;

		// Eq 6.18
		double         denom = 1 + p_sub_zero_sq * xx_sq;
		const double   bb    = 2 * _passband_freq * p_sub_zero * yy / denom;

		// Eq 6.19
		denom *= denom;
		const double   yy_sq = yy * yy;
		const double   numer = yy_sq * p_sub_zero_sq + xx_sq * ww_sq;
		const double   cc    = passband_freq_sq * numer / denom;

		h_sub_zero *= cc / aa;

		// Compute pair of pole locations by finding roots of s^2 + bb*s + cc = 0
		zpk.add_conj_poles (std::complex <double> {
			-bb * 0.5,
			sqrt (cc - bb * bb * 0.25)
		});

		// Compute pair of zero locations by finding roots of s^2 + aa = 0
		const double   abs_aa_sqrt = sqrt (fabs (aa));
		if (aa < 0)
		{
			zpk.add_zero ( abs_aa_sqrt);
			zpk.add_zero (-abs_aa_sqrt);
		}
		else
		{
			zpk.add_conj_zeros (std::complex <double> { 0, abs_aa_sqrt });
		}
	}

	// Last pole and finish
	if ((_order & 1) == 0)
	{
		h_sub_zero /= pb_lin;
	}
	else
	{
		p_sub_zero *= _passband_freq; 
		h_sub_zero *= p_sub_zero;
		zpk.add_pole (-p_sub_zero);
	}
	zpk.set_gain (h_sub_zero);

	assert (int (zpk.use_poles ().size ()) == _order);
	assert (int (zpk.use_zeros ().size ()) == (_order & ~1));

	return ret_val;
}



double	DesignElliptic::compute_eq_6_13 (double vv)
{
	constexpr int  upper_summation_limit = 10;

	int            sign = 1;
	double         s = 0;
	for (int m = 0; m < upper_summation_limit; ++m)
	{
		double         term = fstb::ipowp (_modular_const, m * (m + 1));
		term *=  sign;
		term *=  sinh ((2 * m + 1) * vv);
		s    +=  term;
		sign  = -sign;
	}                 

	sign = -1;
	double         c = 0;
	for (int m = 1; m < upper_summation_limit; ++m)
	{
		double         term = fstb::ipowp (_modular_const, m * m);
		term *=  sign;
		term *=  cosh (2 * m * vv);
		c    +=  term;
		sign  = -sign;
	}               

	return s * sqrt (sqrt (_modular_const)) / (0.5 + c);
}



double	DesignElliptic::compute_eq_6_15 (double mu)
{
	constexpr int  upper_summation_limit = 10;

	int            sign = 1;
	double         s = 0;
	for (int m = 0; m < upper_summation_limit; ++m)
	{
		double         term = fstb::ipowp (_modular_const, m * (m + 1));
		term *=  sign;
		term *=  sin ((2 * m + 1) * fstb::PI * mu / _order);
		s    +=  term;
		sign  = -sign;
	}            

	sign = -1;
	double         c = 0;
	for (int m = 1; m < upper_summation_limit; ++m)
	{
		double         term = fstb::ipowp (_modular_const, m * m);
		term *=  sign;
		term *=  cos (2 * m * fstb::PI * mu / _order);
		c    +=  term;
		sign  = -sign;
	}

	return s * sqrt (sqrt (_modular_const)) / (0.5 + c);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
