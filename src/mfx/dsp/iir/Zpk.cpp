/*****************************************************************************

        Zpk.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/iir/Zpk.h"

#include <algorithm>

#include <cassert>
#include <cfloat>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Zpk::reserve (int order)
{
	assert (order > 0);

	_z.reserve (order);
	_p.reserve (order);
}



Zpk::RootVect &	Zpk::use_zeros () noexcept
{
	return _z;
}



const Zpk::RootVect &	Zpk::use_zeros () const noexcept
{
	return _z;
}



Zpk::RootVect &	Zpk::use_poles () noexcept
{
	return _p;
}



const Zpk::RootVect &	Zpk::use_poles () const noexcept
{
	return _p;
}



void	Zpk::set_zero (int pos, const std::complex <double> &val) noexcept
{
	assert (pos >= 0);
	assert (pos < int (_z.size ()));

	_z [pos] = val;
}



std::complex <double>	Zpk::get_zero (int pos) const noexcept
{
	assert (pos >= 0);
	assert (pos < int (_z.size ()));

	return _z [pos];
}



void	Zpk::set_pole (int pos, const std::complex <double> &val) noexcept
{
	assert (pos >= 0);
	assert (pos < int (_p.size ()));

	_p [pos] = val;
}



std::complex <double>	Zpk::get_pole (int pos) const noexcept
{
	assert (pos >= 0);
	assert (pos < int (_p.size ()));

	return _p [pos];
}



void	Zpk::add_zero (const std::complex <double> &val)
{
	_z.push_back (val);
}



void	Zpk::add_pole (const std::complex <double> &val)
{
	_p.push_back (val);
}



void	Zpk::add_conj_zeros (const std::complex <double> &val)
{
	_z.push_back (std::conj (val));
	_z.push_back (val);
}



void	Zpk::add_conj_poles (const std::complex <double> &val)
{
	_p.push_back (std::conj (val));
	_p.push_back (val);
}



void	Zpk::set_gain (double val) noexcept
{
	_k = val;
}



double	Zpk::get_gain () const noexcept
{
	return _k;
}



int	Zpk::get_order () const
{
	return int (std::max (_p.size (), _z.size ()));
}



void	Zpk::clear () noexcept
{
	_z.clear ();
	_p.clear ();
	_k = 1.0;
}



bool	Zpk::has_real_coefs () const noexcept
{
	constexpr double  eps = DBL_EPSILON * 64;

	return eq_has_real_coef (_z, eps) && eq_has_real_coef (_p, eps);
}



void	Zpk::group_conj () noexcept
{
	group_conj_zeros ();
	group_conj_poles ();
}



void	Zpk::group_conj_zeros () noexcept
{
	group_conj (_z, DBL_EPSILON * 64);
}



void	Zpk::group_conj_poles () noexcept
{
	group_conj (_p, DBL_EPSILON * 64);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: group_conj
Description:
	Group complex numbers into complex conjugate pairs.
	Pure real numbers are sorted at the end of the vector.
	Numbers are NOT sorted by increasing real part, as they are with the
	equivalent Matlab function.
Input parameters:
	- tol: tolerance for conjugate test, >= 0.
Input/output parameters:
	- root_list: List of roots. There should be only real numbers or conjugate
		pairs of complex numbers.
Throws: Nothing
==============================================================================
*/

void	Zpk::group_conj (RootVect &root_list, double tol) noexcept
{
	assert (tol >= 0);

	const int      nbr_roots = int (root_list.size ());
	int            src       = 0;
	for (int dest = 0; dest < nbr_roots; dest += 2)
	{
		// Find next complex root
		src = std::max (src, dest);
		while (src < nbr_roots && fstb::is_null (root_list [src].imag (), tol))
		{
			++ src;
		}

		// No more complex root, 1st part is terminated
		if (src >= nbr_roots)
		{
			break;
		}

		else
		{
			assert (dest + 1 < nbr_roots);
			assert (src + 1 < nbr_roots);
			std::complex <double> & first  = root_list [dest    ];
			std::complex <double> & second = root_list [dest + 1];

			// Swap roots: the real root is sent further in the vector
			std::swap (first, root_list [src]);

			// Find conjugate complex root
			int				src_conj = src + 1;
			while (   ! fstb::is_eq_rel (root_list [src_conj].real (),  first.real (), tol)
			       || ! fstb::is_eq_rel (root_list [src_conj].imag (), -first.imag (), tol))
			{
				++ src_conj;
				assert (src_conj < nbr_roots);	// Can't find conjugate
			}

			// Swap roots
			std::swap (second, root_list [src_conj]);

			// Negative imaginary part on the first root
			const auto     real_part = first.real ();
			const auto     imag_part = fabs (first.imag ());
			second = std::complex <double> (real_part, imag_part);
			first  = std::conj (second);
		}
	}
}



/*
==============================================================================
Name: eq_has_real_coef
Description:
	Checks if the root list correspond to a real-valued equation
Input parameters:
	- root_list: the root list to test
	- tol: tolerance for conjugate test, >= 0.
Returns: true if the corresponding equation coefficients are real, or if the
	root list is empty.
Throws: Nothing
==============================================================================
*/

bool	Zpk::eq_has_real_coef (const RootVect &root_list, double tol) noexcept
{
	assert (tol >= 0);

	bool				real_flag = true;

	const int      nbr_roots = int (root_list.size ());
	for (int base_index = 0; base_index < nbr_roots && real_flag; ++base_index)
	{
		const auto     base = std::complex <double> (
			root_list [base_index].real (),
			fabs (root_list [base_index].imag ())
		);

		if (base.imag () > 0)
		{
			const int		balance = count_conjugates (base, root_list, tol);
			real_flag = (balance == 0);
		}
	}

	return real_flag;
}



/*
==============================================================================
Name: count_conjugates
Description:
	Calculates a balance sum of the roots that are equal or conjugate to the
	argument.
Input parameters:
	- base: root to check. Imaginary part must be not 0.
	- root_list: list of root to check
	- tol: tolerance for conjugate test, >= 0.
Returns:
	A positive number if the positive imaginary numbers are in excess,
	a negative number if the negative imaginary numbers are in excess,
	0 if there are only conjugate pairs corresponding to base.
Throws: Nothing
==============================================================================
*/

int	Zpk::count_conjugates (const std::complex <double> &base, const RootVect &root_list, double tol) noexcept
{
	assert (! fstb::is_null (base.imag (), tol));
	assert (tol >= 0);

	int				balance  = 0;
	const auto     imag_abs = fabs (base.imag ());

	for (const auto &test : root_list)
	{
		const auto     test_imag = test.imag ();
		if (   fstb::is_eq_rel (test.real ()    , base.real (), tol)
		    && fstb::is_eq_rel (fabs (test_imag), imag_abs    , tol))
		{
			if (test_imag > 0)
			{
				++ balance;
			}
			else
			{
				-- balance;
			}
		}
	}

	return balance;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
