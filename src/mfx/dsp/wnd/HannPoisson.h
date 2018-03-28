/*****************************************************************************

        HannPoisson.h
        Author: Laurent de Soras, 2018

Hanning-Poisson window

       1                n                    |n|
w(n) = -(1 + cos (pi * ---)) * exp (-alpha * ---)
       2               M/2                   M/2

with n in [-M/2 ; M/2-1].

It has no side-lobe for alpha >= 2 and its smallest value of R = 0.926 for
alpha = 2. Common alpha values are in the range [0.5 ; 2.0].

Found in "Extraction Of Spectral Peak Parameters Using A Short-Time Fourier
Transform Modeling And No Sidelobe Windows" by Ph. Depalle and T. Helie, 1997

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_HannPoisson_HEADER_INCLUDED)
#define mfx_dsp_wnd_HannPoisson_HEADER_INCLUDED

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
class HannPoisson
:	public WndInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               HannPoisson ()                             = default;
	               HannPoisson (const HannPoisson <T> &other) = default;
	HannPoisson <T> &
	virtual        ~HannPoisson ()                            = default;
	               operator = (const HannPoisson <T> &other)  = default;

	void           set_alpha (double alpha);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	virtual void   do_make_win (T data_ptr [], int len);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _alpha = 2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const HannPoisson <T> &other) const = delete;
	bool           operator != (const HannPoisson <T> &other) const = delete;

}; // class HannPoisson



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/HannPoisson.hpp"



#endif   // mfx_dsp_wnd_HannPoisson_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

