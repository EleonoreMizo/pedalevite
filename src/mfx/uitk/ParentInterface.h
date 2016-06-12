/*****************************************************************************

        ParentInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_ParentInterface_HEADER_INCLUDED)
#define mfx_uitk_ParentInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Rect.h"
#include "mfx/uitk/Vec2d.h"


namespace mfx
{
namespace uitk
{



class ParentInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~ParentInterface () = default;

	Vec2d          get_coord_abs () const;
	void           invalidate (const Rect &zone);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual Vec2d  do_get_coord_abs () const = 0;
	virtual void   do_invalidate (const Rect &zone) = 0;



}; // class ParentInterface



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/ParentInterface.hpp"



#endif   // mfx_uitk_ParentInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
