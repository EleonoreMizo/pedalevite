/*****************************************************************************

        PageInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_PageInterface_HEADER_INCLUDED)
#define mfx_uitk_PageInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/MsgHandlerInterface.h"
#include "mfx/uitk/Vec2d.h"
#include "mfx/ModelObserverDefault.h"



namespace mfx
{

class Model;
class View;

namespace ui
{
	class Font;
}

namespace uitk
{



class PageMgrInterface;

class PageInterface
:	public MsgHandlerInterface
,	public ModelObserverDefault
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class FontSet
	{
	public:
		explicit       FontSet (const ui::Font &fnt_t, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l);
		               FontSet (const FontSet &other)    = default;
		FontSet &      operator = (const FontSet &other) = default;
		const ui::Font &  _t;
		const ui::Font &  _s;
		const ui::Font &  _m;
		const ui::Font &  _l;
	};

	virtual        ~PageInterface () = default;

	void           connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	void           disconnect ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) = 0;
	virtual void   do_disconnect () = 0;



}; // class PageInterface



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/PageInterface.hpp"



#endif   // mfx_uitk_PageInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
