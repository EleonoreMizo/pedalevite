/*****************************************************************************

        PageInterface.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PageInterface::FontSet::FontSet (const ui::Font &fnt_t, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
:	_t (fnt_t)
,	_s (fnt_s)
,	_m (fnt_m)
,	_l (fnt_l)
{
	// Nothing
}



const ui::Font &	PageInterface::FontSet::use (Type t) const
{
	switch (t)
	{
	case Type_T: return _t;
	case Type_S: return _s;
	case Type_M: break;
	case Type_L: return _l;
	default:
		assert (false);
		break;
	}

	return _m;
}



void	PageInterface::connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	assert (page.get_nbr_nodes () == 0);
	assert (page_size [0] > 0);
	assert (page_size [1] > 0);

	do_connect (model, view, page, page_size, usr_ptr, fnt);
}



void	PageInterface::disconnect ()
{
	do_disconnect ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
