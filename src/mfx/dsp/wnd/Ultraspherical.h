/*****************************************************************************

        Ultraspherical.h
        Author: Laurent de Soras, 2018

From the following articles:

Stuart W. A. Bergen and Andreas Antoniou,
Design of Ultraspherical Window Functions with Prescribed Spectral
Characteristics,
EURASIP Journal on Applied Signal Processing 2004:13, 2053–2065, 2004-10

Stuart W. A. Bergen and Andreas Antoniou
Generation Of Ultraspherical Window Functions
Proceedings of the 11th European Signal Processing Conference (EUSIPCO),
Volume II p. 607-610, 2002

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_Ultraspherical_HEADER_INCLUDED)
#define mfx_dsp_wnd_Ultraspherical_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/wnd/WndInterface.h"



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <class T>
class Ultraspherical
:	public WndInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_mu (double mu) noexcept;
	void           set_x_mu (double x_mu) noexcept;

	static double  compute_x_mu_for_prescribed_null_to_null_width (int len, double mu, double w) noexcept;
	static double  compute_x_mu_for_prescribed_main_lobe_width (int len, double mu, double w) noexcept;
	static double  compute_x_mu_for_prescribed_ripple_ratio (int len, double mu, double r) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	void           do_make_win (T data_ptr [], int len) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr double
	               msgn (double x) noexcept;
	static double  compute_t (int n, double x) noexcept;
	static void    compute_t (double &t_n, double &t_n_m_1, int n, double x) noexcept;
	static double  compute_c (int n, double mu, double x) noexcept;
	static void    compute_c (double &c_n, double &c_n_m_1, int n, double mu, double x) noexcept;
	static double  compute_a (int len, double mu, double epsilon) noexcept;
	static double  compute_b (int len, double mu, double epsilon) noexcept;
	static double  compute_v (int len, double mu, double epsilon, int l) noexcept;
	static double  compute_zero_of_c (int n, int l, double mu, double param, double epsilon, double r) noexcept;

	double         _mu   = 1;
	double         _x_mu = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Ultraspherical <T> &other) const = delete;
	bool           operator != (const Ultraspherical <T> &other) const = delete;

}; // class Ultraspherical



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/Ultraspherical.hpp"



#endif   // mfx_dsp_wnd_Ultraspherical_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

