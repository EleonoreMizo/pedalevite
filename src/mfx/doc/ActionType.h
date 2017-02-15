/*****************************************************************************

        ActionType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_ActionType_HEADER_INCLUDED)
#define mfx_doc_ActionType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace doc
{



enum ActionType
{

	ActionType_BANK = 0,
	ActionType_PRESET,
	ActionType_TOGGLE_TUNER,
	ActionType_TOGGLE_FX,
	ActionType_LOOP_REC,
	ActionType_LOOP_PLAY_STOP,
	ActionType_LOOP_UNDO,
	ActionType_PARAM,
	ActionType_TEMPO,          // Tap
	ActionType_SETTINGS,
	ActionType_EVENT,
	ActionType_TEMPO_SET,

	ActionType_NBR_ELT

}; // class ActionType



}  // namespace doc
}  // namespace mfx



#endif   // mfx_doc_ActionType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
