/*****************************************************************************

        Taylor.h
        Author: Laurent de Soras, 2020

Taylor windows are similar to Chebyshev windows. A Chebyshev window has the
narrowest possible mainlobe for a specified sidelobe level, but a Taylor
window allows you to make tradeoffs between the mainlobe width and the
sidelobe level. The Taylor distribution avoids edge discontinuities, so
Taylor window sidelobes decrease monotonically.

Ref:
Matlab implementation
toolbox/signal/signal/taylorwin.m

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_Taylor_HEADER_INCLUDED)
#define mfx_dsp_wnd_Taylor_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/wnd/WndInterface.h"



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <class T>
class Taylor
:	public WndInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_side_lobe_lvl (double lvl);
	void           set_nbar (int nbar);

	int            compute_nbar_min (double lvl);
	bool           is_side_lobe_lvl_guaranteed () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	void           do_make_win (T data_ptr [], int len) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         compute_fm (int m, double sp2, double a);

	static double  compute_a (double lvl);

	// Maximum linear level of the side lobes, relative to the main lobe.
	// Range: ]0 ; 1[
	double         _sl_lvl  = 0.03162277660168379331998893544433; // -30 dB

	// Number of nearly constant-level sidelobes adjacent to the mainlobe. > 0
	int            _nbar    = 4;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Taylor &other) const = delete;
	bool           operator != (const Taylor &other) const = delete;

}; // class Taylor



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/Taylor.hpp"



#endif   // mfx_dsp_wnd_Taylor_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
