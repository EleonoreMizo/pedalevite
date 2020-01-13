/*****************************************************************************

        Kaiser.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_Kaiser_HEADER_INCLUDED)
#define mfx_dsp_wnd_Kaiser_HEADER_INCLUDED

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
class Kaiser
:	public WndInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_alpha (double alpha);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	void           do_make_win (T data_ptr [], int len) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static double  compute_wp (int pos, int len, double alpha);
	static double  compute_i0 (double x);

	double         _alpha = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Kaiser <T> &other) const = delete;
	bool           operator != (const Kaiser <T> &other) const = delete;

}; // class Kaiser



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/Kaiser.hpp"



#endif   // mfx_dsp_wnd_Kaiser_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

