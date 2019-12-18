/*****************************************************************************

        EndMsg.h
        Author: Laurent de Soras, 2016

usr_ptr points on a raw null-terminated string.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_EndMsg_HEADER_INCLUDED)
#define mfx_uitk_pg_EndMsg_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <memory>
#include <string>
#include <vector>



namespace mfx
{

class CmdLine;

namespace uitk
{
namespace pg
{



class EndMsg
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum EndType
	{
		EndType_NONE = 0,
		EndType_RESTART,
		EndType_REBOOT,
		EndType_SHUTDOWN,

		EndType_NBR_ELT
	};

	explicit       EndMsg (const CmdLine &cmd_line);
	virtual        ~EndMsg () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	const CmdLine& _cmd_line;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;

	TxtArray       _line_list;
	EndType        _end_type;

	std::chrono::microseconds
	               _action_date;

	Question::QArg _msg_arg;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EndMsg ()                               = delete;
	               EndMsg (const EndMsg &other)            = delete;
	EndMsg &       operator = (const EndMsg &other)        = delete;
	bool           operator == (const EndMsg &other) const = delete;
	bool           operator != (const EndMsg &other) const = delete;

}; // class EndMsg



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/EndMsg.hpp"



#endif   // mfx_uitk_pg_EndMsg_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
