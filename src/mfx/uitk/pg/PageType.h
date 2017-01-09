/*****************************************************************************

        PageType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PageType_HEADER_INCLUDED)
#define mfx_uitk_pg_PageType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace uitk
{
namespace pg
{



enum PageType
{
	PageType_INVALID = -1,

	PageType_BOOT    = 0,
	PageType_TUNER,
	PageType_CUR_PROG,
	PageType_MENU_MAIN,
	PageType_EDIT_PROG,
	PageType_PARAM_LIST,
	PageType_PARAM_EDIT,
	PageType_NOT_YET,
	PageType_QUESTION,
	PageType_PARAM_CONTROLLERS,
	PageType_CTRL_EDIT,
	PageType_MENU_SLOT,
	PageType_EDIT_TEXT,
	PageType_SAVE_PROG,
	PageType_END_MSG,
	PageType_LEVELS,
	PageType_PEDALBOARD_CONFIG,
	PageType_PEDAL_ACTION_TYPE,
	PageType_CTRL_PROG,
	PageType_BANK_MENU,
	PageType_MOVE_FX,
	PageType_PEDAL_EDIT_GROUP,
	PageType_PEDAL_EDIT_CYCLE,

	PageType_NBR_ELT

}; // enum PageType



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PageType.hpp"



#endif   // mfx_uitk_pg_PageType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
