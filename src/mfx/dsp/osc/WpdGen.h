/*****************************************************************************

        WpdGen.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_WpdGen_HEADER_INCLUDED)
#define mfx_dsp_osc_WpdGen_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/wnd/WndInterface.h"



namespace mfx
{
namespace dsp
{
namespace osc
{



class WpdGen
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <class T>
	static void    build_blep_pure (T &steptable, wnd::WndInterface <double> &win_gen, float sample_scale = 1);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               WpdGen ()                               = delete;
	               WpdGen (const WpdGen &other)            = delete;
	virtual        ~WpdGen ()                              = delete;
	WpdGen &       operator = (const WpdGen &other)        = delete;
	bool           operator == (const WpdGen &other) const = delete;
	bool           operator != (const WpdGen &other) const = delete;

}; // class WpdGen



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/WpdGen.hpp"



#endif   // mfx_dsp_osc_WpdGen_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
