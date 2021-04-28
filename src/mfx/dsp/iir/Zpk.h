/*****************************************************************************

        Zpk.h
        Author: Laurent de Soras, 2021

Class holding a filter equation stored as ZPK (zero roots, pole roots, gain).
The equation plane (s or z) is unspecified.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Zpk_HEADER_INCLUDED)
#define mfx_dsp_iir_Zpk_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <complex>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace iir
{



class Zpk
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::vector <std::complex <double> > RootVect;

	void           reserve (int order);
	RootVect &     use_zeros () noexcept;
	const RootVect &
	               use_zeros () const noexcept;
	RootVect &     use_poles () noexcept;
	const RootVect &
	               use_poles () const noexcept;

	void           set_zero (int pos, const std::complex <double> &val) noexcept;
	std::complex <double>
	               get_zero (int pos) const noexcept;
	void           set_pole (int pos, const std::complex <double> &val) noexcept;
	std::complex <double>
	               get_pole (int pos) const noexcept;

	void           add_zero (const std::complex <double> &val);
	void           add_pole (const std::complex <double> &val);
	void           add_conj_zeros (const std::complex <double> &val);
	void           add_conj_poles (const std::complex <double> &val);

	void           set_gain (double val) noexcept;
	double         get_gain () const noexcept;

	int            get_order () const;

	void           clear () noexcept;

	bool           has_real_coefs () const noexcept;
	void           group_conj () noexcept;
	void           group_conj_zeros () noexcept;
	void           group_conj_poles () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    group_conj (RootVect &root_list, double tol) noexcept;
	static bool    eq_has_real_coef (const RootVect &root_list, double tol) noexcept;
	static int     count_conjugates (const std::complex <double> &base, const RootVect &root_list, double tol) noexcept;

	RootVect       _z;         // Zero list
	RootVect       _p;         // Pole list
	double         _k = 1.0;   // Overall gain



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Zpk &other) const = delete;
	bool           operator != (const Zpk &other) const = delete;

}; // class Zpk



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/Zpk.hpp"



#endif   // mfx_dsp_iir_Zpk_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
