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
	PageType_PROG_CUR,
	PageType_MENU_MAIN,
	PageType_PROG_EDIT,
	PageType_PARAM_LIST,
	PageType_PARAM_EDIT,
	PageType_NOT_YET,
	PageType_QUESTION,
	PageType_PARAM_CONTROLLERS,
	PageType_CTRL_EDIT,
	PageType_SLOT_MENU,
	PageType_EDIT_TEXT,
	PageType_PROG_SAVE,
	PageType_END_MSG,
	PageType_LEVELS,
	PageType_PEDALBOARD_CONFIG,
	PageType_PEDAL_ACTION_TYPE,
	PageType_PROG_SETTINGS,
	PageType_BANK_MENU,
	PageType_SLOT_MOVE,
	PageType_PEDAL_EDIT_GROUP,
	PageType_PEDAL_EDIT_CYCLE,
	PageType_PEDAL_EDIT_STEP,
	PageType_PEDAL_EDIT_ACTION,
	PageType_EDIT_LABEL,
	PageType_EDIT_FXID,
	PageType_FX_PEQ,
	PageType_SETTINGS_OTHER,
	PageType_PRESET_MENU,
	PageType_PRESET_LIST,
	PageType_BANK_ORGA,
	PageType_BANK_MOVE,
	PageType_PROG_MOVE,
	PageType_FX_LFO,
	PageType_MENU_BACKUP,
	PageType_EDIT_DATE,
	PageType_PROG_CATALOG,
	PageType_REC2DISK,

	PageType_NBR_ELT

}; // enum PageType



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PageType.hpp"



#endif   // mfx_uitk_pg_PageType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
