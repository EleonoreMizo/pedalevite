/*****************************************************************************

        DolphCheby.h
        Author: Laurent de Soras, 2018

From the following article:
The Dolph-Chebyshev Window - A Simple Optimal Filter
Peter Lynch, January 1996
http://www.maths.tcd.ie/~plynch/Publications/Dolph.pdf


--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_DolphCheby_HEADER_INCLUDED)
#define mfx_dsp_wnd_DolphCheby_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <class T>
class DolphCheby
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_ripple_ratio (double r);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	void           do_make_win (T data_ptr [], int len) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static double  cheby_poly (double x, int n);

	double         _r = 0.5;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DolphCheby <T> &other) const = delete;
	bool           operator != (const DolphCheby <T> &other) const = delete;

}; // class DolphCheby



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/DolphCheby.hpp"



#endif   // mfx_dsp_wnd_DolphCheby_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

