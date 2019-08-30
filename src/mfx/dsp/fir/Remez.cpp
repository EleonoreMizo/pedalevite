/*****************************************************************************

        Remez.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/fir/Remez.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: compute_coefs
Description:
	Find FIR coefficients for a given specification. Frequency constraints are
	strict whereas pass- and stopband ripples are not fixed and depend on the
	number of coefficients and bandweight specifications. Pass/stop ripple
	ratio is also fixed.
Input parameters:
	- coef_list_ptr: Pointer on a pre-reserved array for the coefficients.
	- len: Length of the coefficient array.
	- spec: List of specified bands, ordered by increasing frequency. See
		RemezPoint header for details.
	- type: Type of the filter. See corresponding enum for details.
Returns:
	0 : OK.
	-1: algorithm couldn't find something. It may be caused by too loose
		specification for the given number of coefficients.
Throws: std::vector related exceptions
==============================================================================
*/

int	Remez::compute_coefs (double coef_list_ptr [], int len, const RemezSpec &spec, Type type)
{
	assert (coef_list_ptr != 0);
	assert (len > 0);
	assert (&spec != 0);
	assert (spec.size () > 0);
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	int            ret_val = 0;

	const Symmetry symmetry =
		  (type == Type_BANDPASS)
		? Symmetry_POS
		: Symmetry_NEG;

	// Number of extrema
	const int      half_len = len / 2;
	int            r        = half_len;
	if (   (len & 1) != 0
	    && symmetry == Symmetry_POS)
	{
		++r;
	}

	// Dynamically allocate memory for arrays with proper sizes
	const int      r_plus_one = r + 1;
	_ext.resize (r_plus_one);
	_taps.resize (r_plus_one);
	_x.resize (r_plus_one);
	_y.resize (r_plus_one);
	_ad.resize (r_plus_one);

	// Predict dense grid size in advance for memory allocation
	const int      nbr_bands = int (spec.size ());
	int            gridsize = 0;
	{
#if ! defined (NDEBUG)
		double         prev_freq = 0;
#endif // NDEBUG
		for (int i = 0; i < nbr_bands; ++i)
		{
			const double   lower_freq = spec [i].get_lower_freq ();
			const double   upper_freq = spec [i].get_upper_freq ();
			const double   freq_dif   = upper_freq - lower_freq;
			assert (freq_dif >= 0);
#if ! defined (NDEBUG)
			assert (upper_freq >= prev_freq);
#endif // NDEBUG
			gridsize += fstb::round_int ((2 * r * GRID_DENSITY) * freq_dif);
#if ! defined (NDEBUG)
			prev_freq = upper_freq;
#endif // NDEBUG
		}
	}
	if (symmetry == Symmetry_NEG)
	{
		-- gridsize;
	}

	// Dynamically allocate memory for arrays with proper sizes
	_grid.resize (gridsize);
	_d.resize (gridsize);
	_w.resize (gridsize);
	_e.resize (gridsize);

	// Create dense frequency grid
	create_dense_grid (r, len, spec, gridsize, symmetry);
	initial_guess (r, gridsize);

	// For Differentiator: (fix grid)
	if (type == Type_DIFFERENTIATOR)
	{
		for (int i = 0; i < gridsize; ++i)
		{
			// _d [i] = _d [i] * _grid [i];
			if (_d [i] > 0.0001)
			{
				_w [i] /= _grid [i];
			}
		}
	}

	// For odd or Negative symmetry filters, alter the
	// _d[] and _w[] according to Parks McClellan
	if (symmetry == Symmetry_POS)
	{
		if ((len & 1) == 0)
		{
			for (int i = 0; i < gridsize; ++i)
			{
				const double	c = cos (fstb::PI * _grid [i]);
				_d [i] /= c;
				_w [i] *= c; 
			}
		}
	}
	else
	{
		if ((len & 1) != 0)
		{
			for (int i = 0; i < gridsize; ++i)
			{
				const double	c = sin ((fstb::PI * 2) * _grid [i]);
				_d [i] /= c;
				_w [i] *= c;
			}
		}
		else
		{
			for (int i = 0; i < gridsize; ++i)
			{
				const double	c = sin (fstb::PI * _grid [i]);
				_d [i] /= c;
				_w [i] *= c;
			}
		}
	}

	// Perform the Remez Exchange algorithm
	bool           done_flag = false;
	for (int iter = 0; iter < MAX_NBR_IT && ! done_flag; ++iter)
	{
		calc_params (r);
		calc_error (r, gridsize);
		search (r, gridsize);
		done_flag = is_done (r);
	}

	// Reached maximum iteration count. Results may be bad.
	if (! done_flag)
	{
		ret_val = -1;
	}

	calc_params (r);

	// Find the 'taps' of the filter for use with Frequency
	// Sampling. If odd or Negative symmetry, fix the taps
	// according to Parks McClellan
	for (int i = 0; i <= half_len; ++i)
	{
		double         c        = 0;
		const double   frac_pos = double (i) / len;

		if (symmetry == Symmetry_POS)
		{
			if ((len & 1) != 0)
			{
				c = 1;
			}
			else
			{
				c = cos (fstb::PI * frac_pos);
			}
		}
		else
		{
			if ((len & 1) != 0)
			{
				c = sin ((fstb::PI * 2) * frac_pos);
			}
			else
			{
				c = sin (fstb::PI * frac_pos);
			}
		}

		_taps [i] = compute_a (frac_pos, r) * c;
	}

	// Frequency sampling design with calculated taps
	freq_sample (len, coef_list_ptr, symmetry);

	return ret_val;
}



int	Remez::compute_coefs (CoefList &coef_list, int len, const RemezSpec &spec, Type type)
{
	assert (&coef_list != 0);
	assert (len > 0);
	assert (&spec != 0);
	assert (spec.size () > 0);
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	coef_list.resize (len);

	return compute_coefs (&coef_list [0], len, spec, type);
}



void	Remez::free_resources ()
{
	IntArray ().swap (_ext);
	FltArray ().swap (_taps);
	FltArray ().swap (_x);
	FltArray ().swap (_y);
	FltArray ().swap (_ad);
	FltArray ().swap (_grid);
	FltArray ().swap (_w);
	FltArray ().swap (_d);
	FltArray ().swap (_e);
}



int	Remez::estimate_order (double ripple_db, double atten_db, double trans_band)
{
	assert (ripple_db > 0);
	assert (atten_db >= ripple_db);
	assert (trans_band > 0);
	assert (trans_band < 0.5);

	const double   delta_p = 0.5 * (1.0 - pow (10.0, ripple_db / -20.0));
	const double   delta_s =              pow (10.0,  atten_db / -20.0);

	const int      order = fstb::round_int (
		  (-10.0 * log10 (delta_p * delta_s) - 13)
		/ ( 14.6 * trans_band)
	);

	return order;
}



double	Remez::compute_ripple_ratio (double ripple_db, double atten_db)
{
	assert (ripple_db > 0);
	assert (atten_db >= ripple_db);

	const double   delta_p = 0.5 * (1.0 - pow (10.0, ripple_db / -20.0));
	const double   delta_s =              pow (10.0,  atten_db / -20.0);

	return delta_p / delta_s;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*******************
 * create_dense_grid
 *=================
 * Creates the dense grid of frequencies from the specified bands.
 * Also creates the Desired Frequency Response function (_d[]) and
 * the Weight function (_w[]) on that dense grid
 *
 *
 * INPUT:
 * ------
 * int      r         - 1/2 the number of filter coefficients
 * int      nbr_coefs - Number of taps in the resulting filter
 * int      numband   - Number of bands in user specification
 * double   bands[]   - User-specified band edges [2*numband]
 * double   des[]     - Desired response per band [numband]
 * double   weight[]  - Weight per band [numband]
 * int    gridsize    - Number of elements in the dense frequency grid
 * int      symmetry  - Symmetry of filter - used for grid check
 *
 * OUTPUT:
 * -------
 * double _grid[]     - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double _d[]        - Desired response on the dense grid [gridsize]
 * double _w[]        - Weight function on the dense grid [gridsize]
 *******************/

void	Remez::create_dense_grid (int r, int nbr_coefs, const RemezSpec &spec, int gridsize, Symmetry symmetry)
{
	const int      nbr_bands = int (spec.size ());
	const double   delf      = 0.5 / (GRID_DENSITY * r);

	int            j = 0;
	for (int band = 0; band < nbr_bands; ++band)
	{
		double         lowf  = spec [band].get_lower_freq ();
		const double   highf = spec [band].get_upper_freq ();

		// For differentiator, hilbert,
		// symmetry is odd and _grid [0] = max (delf, band [0])
		if (band == 0 && symmetry == Symmetry_NEG && lowf < delf)
		{
			lowf = delf;
		}

		int            k = fstb::round_int ((highf - lowf) / delf);
		_grid [j] = lowf;
		for (int i = 0; i < k; ++i)
		{
			_d [j]    = spec [band].get_amp ();
			_w [j]    = spec [band].get_weight ();
			_grid [j] = lowf;
			lowf     += delf;
			++j;
		}
		if (j > 0)
		{
			_grid [j - 1] = highf;
		}
	}

	// Similar to above, if odd symmetry, last grid point can't be 0.5
	// - but, if there are even taps, leave the last grid point at 0.5
	if (   symmetry == Symmetry_NEG
	    && _grid [gridsize - 1] > (0.5 - delf)
	    && (nbr_coefs & 1) != 0)
	{
		_grid [gridsize - 1] = 0.5 - delf;
	}
}



/********************
 * initial_guess
 *==============
 * Places Extremal Frequencies evenly throughout the dense grid.
 *
 *
 * INPUT: 
 * ------
 * int r        - 1/2 the number of filter coefficients
 * int gridsize - Number of elements in the dense frequency grid
 *
 * OUTPUT:
 * -------
 * int _ext[]   - Extremal indexes to dense frequency grid [r+1]
 ********************/

void	Remez::initial_guess (int r, int gridsize)
{
   for (int i = 0; i <= r; ++i)
	{
      _ext [i] = i * (gridsize - 1) / r;
	}
}



/***********************
 * calc_params
 *===========
 *
 *
 * INPUT:
 * ------
 * int    r       - 1/2 the number of filter coefficients
 * int    _ext[]  - Extremal indexes to dense frequency grid [r+1]
 * double _grid[] - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double _d[]    - Desired response on the dense grid [gridsize]
 * double _w[]    - Weight function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double _ad[]   - 'b' in Oppenheim & Schafer [r+1]
 * double _x[]    - [r+1]
 * double _y[]    - 'C' in Oppenheim & Schafer [r+1]
 ***********************/

void	Remez::calc_params (int r)
{
	// Find _x []
	{
		for (int i = 0; i <= r; ++i)
		{
			assert (_ext [i] >= 0);
			_x [i] = cos ((fstb::PI * 2) * _grid [_ext [i]]);
		}
	}

	// Calculate _ad[]  - Oppenheim & Schafer eq 7.132
	//#* LDS: d'ou il vient ce 15 ???
	const int      ld = (r - 1) / 15 + 1;	// Skips around to avoid round errors
	{
		for (int i = 0; i <= r; ++i)
		{
			double         denom = 1.0;
			const double   xi    = _x [i];
			for (int j = 0; j < ld; ++j)
			{
				for (int k = j; k <= r; k += ld)
				{
					if (k != i)
					{
						denom *= 2.0 * (xi - _x [k]);
					}
				}
			}
			if (fabs (denom) < 0.00001)
			{
				denom = 0.00001;
			}
			_ad [i] = 1.0 / denom;
		}
	}

	// Calculate delta  - Oppenheim & Schafer eq 7.131
	double         numer = 0;
	double         denom = 0;
	double         sign  = 1;
	{
		for (int i = 0; i <= r; ++i)
		{
			numer += _ad [i] * _d [_ext [i]];
			denom += sign * _ad [i] / _w [_ext [i]];
			sign = -sign;
		}
	}
	const double   delta = numer / denom;
	sign = 1;

	// Calculate _y []  - Oppenheim & Schafer eq 7.133b
	{
		for (int i = 0; i <= r; ++i)
		{
			_y [i] = _d [_ext [i]] - sign * delta / _w [_ext [i]];
			sign   = -sign;
		}
	}
}



/*********************
 * compute_a
 *==========
 * Using values calculated in calc_params, compute_a calculates the
 * actual filter response at a given frequency (freq).  Uses
 * eq 7.133a from Oppenheim & Schafer.
 *
 *
 * INPUT:
 * ------
 * double freq  - Frequency (0 to 0.5) at which to calculate A
 * int    r     - 1/2 the number of filter coefficients
 * double _ad[] - 'b' in Oppenheim & Schafer [r+1]
 * double _x[]  - [r+1]
 * double _y[]  - 'C' in Oppenheim & Schafer [r+1]
 *
 * OUTPUT:
 * -------
 * Returns double value of A[freq]
 *********************/

double	Remez::compute_a (double freq, int r) const
{
	double         den       = 0;
	double         num       = 0;
	double         xc        = cos ((fstb::PI * 2) * freq);
	bool           cont_flag = true;
	for (int i = 0; i <= r && cont_flag; ++i)
	{
		double         c = xc - _x [i];
		if (fabs (c) < 1.0e-7)
		{
			num       = _y [i];
			den       = 1;
			cont_flag = false;
		}

		else
		{
			c    = _ad [i] / c;
			den += c;
			num += c * _y [i];
		}
	}

	return num / den;
}



/************************
 * calc_error
 *===========
 * Calculates the Error function from the desired frequency response
 * on the dense grid (_d[]), the weight function on the dense grid (_w[]),
 * and the present response calculation (A[])
 *
 *
 * INPUT:
 * ------
 * int    r       - 1/2 the number of filter coefficients
 * double _ad[]   - [r+1]
 * double _x[]    - [r+1]
 * double _y[]    - [r+1]
 * int gridsize   - Number of elements in the dense frequency grid
 * double _grid[] - Frequencies on the dense grid [gridsize]
 * double _d[]    - Desired response on the dense grid [gridsize]
 * double _w[]    - Weight function on the desnse grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double _e[]    - Error function on dense grid [gridsize]
 ************************/

void	Remez::calc_error (int r, int gridsize)
{
	for (int i = 0; i < gridsize; ++i)
	{
		const double   a = compute_a (_grid [i], r);
		_e [i] = _w [i] * (_d [i] - a);
	}
}



/************************
 * search
 *========
 * Searches for the maxima/minima of the error curve.  If more than
 * r+1 extrema are found, it uses the following heuristic (thanks
 * Chris Hanson):
 * 1) Adjacent non-alternating extrema deleted first.
 * 2) If there are more than one excess extrema, delete the
 *    one with the smallest error.  This will create a non-alternation
 *    condition that is fixed by 1).
 * 3) If there is exactly one excess extremum, delete the smaller
 *    of the first/last extremum
 *
 *
 * INPUT:
 * ------
 * int    r        - 1/2 the number of filter coefficients
 * int    _ext[]   - Indexes to _grid[] of extremal frequencies [r+1]
 * int    gridsize - Number of elements in the dense frequency grid
 * double _e[]     - Array of error values.  [gridsize]
 * OUTPUT:
 * -------
 * int    _ext[]   - New indexes to extremal frequencies [r+1]
 ************************/

void	Remez::search (int r, int gridsize)
{
	// Array of found extremals
	IntArray       found_ext;

	// Check for extremum at 0.
	if (   ((_e [0] > 0.0) && (_e [0] > _e [1]))
	    || ((_e [0] < 0.0) && (_e [0] < _e [1])))
	{
		found_ext.push_back (0);
	}

	// Check for extrema inside dense grid
	for (int i = 1; i < gridsize - 1; ++i)
	{
		if (   ((_e [i] >= _e [i-1]) && (_e [i] > _e [i+1]) && (_e [i] > 0.0)) 
		    || ((_e [i] <= _e [i-1]) && (_e [i] < _e [i+1]) && (_e [i] < 0.0)))
		{
			found_ext.push_back (i);
		}
	}

	// Check for extremum at 0.5
	const int		j = gridsize - 1;
	if (   ((_e [j] > 0.0) && (_e [j] > _e [j-1]))
	    || ((_e [j] < 0.0) && (_e [j] < _e [j-1])))
	{
		found_ext.push_back (j);
	}

	// Remove extra extremals
	int            k     = int (found_ext.size ());
	int            extra = k - (r + 1);

	while (extra > 0)
	{
		// First one is a maxima or minima
		bool           up       = (_e [found_ext [0]] > 0.0);

		int            l        = 0;
		bool           alt_flag = true;
		{
			for (int m = 1; m < k && alt_flag; ++m)
			{
				if (fabs (_e [found_ext [m]]) < fabs (_e [found_ext [l]]))
				{
					l = m;	// New smallest error.
				}
				if (up && _e [found_ext [m]] < 0.0)
				{
					up = false;	// Switch to a minima
				}
				else if (! up && _e [found_ext [m]] > 0.0)
				{
					up = true;	// Switch to a maxima
				}
				else
				{ 
					// Ooops, found two non-alternating extrema. Delete smallest of
					// them if the loop finishes, all extrema are alternating
					alt_flag = false;
				}
			}
		}

		// If there's only one extremal and all are alternating,
		// delete the smallest of the first/last extremals.
		if (alt_flag && extra == 1)
		{
			if (fabs (_e [found_ext [k - 1]]) < fabs (_e [found_ext [0]]))
			{
				l = k - 1;  // Delete last extremal
			}
			else
			{
				l = 0;      // Delete first extremal
			}
		}

		found_ext.erase (found_ext.begin () + l);

		-- k;
		-- extra;
	}

	const int      nbr_elt = std::min (k, r + 1);
	for (int i = 0; i < nbr_elt; ++i)
	{
		_ext [i] = found_ext [i];	// Copy found extremals to _ext[]
		assert (_ext [i] >= 0);
		assert (_ext [i] < gridsize);
	}
}



/*********************
 * freq_sample
 *============
 * Simple frequency sampling algorithm to determine the impulse
 * response h[] from A's found in compute_a
 *
 *
 * INPUT:
 * ------
 * int      N        - Number of filter coefficients
 * double   _taps[]  - Sample points of desired response [N/2]
 * int      symmetry - Symmetry of desired filter
 *
 * OUTPUT:
 * -------
 * double h[] - Impulse Response of final filter [N]
 *********************/
void	Remez::freq_sample (int N, double h [], int symm)
{
	const double   M = (N - 1) / 2.0;
	if (symm == Symmetry_POS)
	{
		if ((N & 1) != 0)
		{
			for (int n = 0; n < N; ++n)
			{
				double         val = _taps [0];
				const double   x   = (fstb::PI * 2) * (n - M) / N;
				for (int k = 1; k <= M; ++k)
				{
					val += 2.0 * _taps [k] * cos (x * k);
				}
				h [n] = val / N;
			}
		}
		else
		{
			for (int n = 0; n < N; ++n)
			{
				double         val = _taps [0];
				const double   x   = (fstb::PI * 2) * (n - M) / N;
				for (int k = 1; k <= (N / 2 - 1); ++k)
				{
					val += 2.0 * _taps [k] * cos (x * k);
				}
				h [n] = val / N;
			}
		}
	}
	else
	{
		if ((N & 1) != 0)
		{
			for (int n = 0; n < N; ++n)
			{
				double         val = 0;
				const double   x   = (fstb::PI * 2) * (n - M) / N;
				for (int k = 1; k <= M; ++k)
				{
					val += 2.0 * _taps [k] * sin (x * k);
				}
				h [n] = val / N;
			}
		}
		else
		{
			for (int n = 0; n < N; ++n)
			{
				double         val = _taps [N / 2] * sin (fstb::PI * (n - M));
				const double   x   = (fstb::PI * 2) * (n - M) / N;
				for (int k = 1; k <= (N / 2 - 1); ++k)
				{
					val += 2.0 * _taps [k] * sin (x * k);
				}
				h [n] = val / N;
			}
		}
	}
}

/*******************
 * is_done
 *========
 * Checks to see if the error function is small enough to consider
 * the result to have converged.
 *
 * INPUT:
 * ------
 * int    r      - 1/2 the number of filter coeffiecients
 * int    _ext[] - Indexes to extremal frequencies [r+1]
 * double _e[]   - Error function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * Returns 1 if the result converged
 * Returns 0 if the result has not converged
 ********************/

bool	Remez::is_done (int r) const
{
	double         ma = fabs (_e [_ext [0]]);
	double         mi = ma;
	for (int i = 1; i <= r; ++i)
	{
		const double   current = fabs (_e [_ext [i]]);
		mi = std::min (current, mi);
		ma = std::max (current, ma);
	}

	return (((ma - mi) / ma) < 0.0001);
}



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
