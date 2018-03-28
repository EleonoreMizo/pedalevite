/*****************************************************************************

        Ultraspherical.h
        Author: Laurent de Soras, 2018

From the following articles:

Generation Of Ultraspherical Window Functions
Stuart W. A. Bergen and Andreas Antoniou, 2002
Proceedings of the 11th European Signal Processing Conference (EUSIPCO),
Volume II p. 607-610
http://www.eurasip.org/content/Eusipco/2002/articles/paper144.html

On The Ultraspherical Family Of Window Functions
Stuart W. A. Bergen and Andreas Antoniou, 2003
University of Victoria

Note: not sure the implementation is totally correct. It seems the functions
using prescribed specifications give wrong results for mu = 0 (Dolph-Chebyshev
case). To be checked more deeply.

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

	               Ultraspherical ()                                = default;
	               Ultraspherical (const Ultraspherical <T> &other) = default;
	virtual        ~Ultraspherical () = default;
	Ultraspherical <T> &
	               operator = (const Ultraspherical <T> &other)     = default;

	void           set_mu (double mu);
	void           set_x_mu (double x_mu);

	static double  compute_x_mu_for_prescribed_main_lobe_width (int len, double mu, double w);
	static double  compute_x_mu_for_prescribed_ripple_ratio (int len, double mu, double r);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	virtual void   do_make_win (T data_ptr [], int len);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static double  compute_c (int n, double mu, double x);
	static void    compute_c (double &c_n, double &c_n_m_1, int n, double mu, double x);
	static double  compute_a (int len, double mu, double epsilon);
	static double  compute_largest_zero_of_c (int n, double mu, double a, double epsilon);

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

