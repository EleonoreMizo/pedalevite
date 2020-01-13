/*****************************************************************************

        Generic.h
        Author: Laurent de Soras, 2018

Template parameters:

- GEN must have the following function:
	GEN::GEN ();
	GEN::~GEN ();
	double GEN::operator () (double pos);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_Generic_HEADER_INCLUDED)
#define mfx_dsp_wnd_Generic_HEADER_INCLUDED

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



template <class T, class GEN>
class Generic
:	public WndInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef GEN CoefGen;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	void           do_make_win (T data_ptr [], int len) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	CoefGen        _the_thing;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Generic <T, GEN> &other) const = delete;
	bool           operator != (const Generic <T, GEN> &other) const = delete;

}; // class Generic



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/Generic.hpp"



#endif   // mfx_dsp_wnd_Generic_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

