/*****************************************************************************

        WndInterface.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_WndInterface_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_WndInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Center of the window is exactly at (len - 1) / 2.
// The array does not include the bounds of the window (+/-1), which means
// that usually data_ptr [0] and data_ptr [len - 1] are not null.
template <class T>
void	WndInterface <T>::make_win (T data_ptr [], int len)
{
	assert (data_ptr != nullptr);
	assert (len > 0);

	do_make_win (data_ptr, len);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_WndInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

