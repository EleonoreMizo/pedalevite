/*****************************************************************************

        UserInputVoid.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_UserInputVoid_HEADER_INCLUDED)
#define mfx_ui_UserInputVoid_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/UserInputInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace ui
{



class UserInputVoid
:	public UserInputInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               UserInputVoid ();
	virtual        ~UserInputVoid () = default;

	void           send_message (int64_t date, UserInputType type, int index, float val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ui::UserInputInterface
	virtual int    do_get_nbr_param (UserInputType type) const;
	virtual void   do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr);
	virtual void   do_return_cell (MsgCell &cell);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <MsgQueue *> QueueArray;
	typedef std::array <QueueArray, UserInputType_NBR_ELT> RecipientList;

	RecipientList  _recip_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               UserInputVoid (const UserInputVoid &other)     = delete;
	UserInputVoid &
	               operator = (const UserInputVoid &other)        = delete;
	bool           operator == (const UserInputVoid &other) const = delete;
	bool           operator != (const UserInputVoid &other) const = delete;

}; // class UserInputVoid



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/UserInputVoid.hpp"



#endif   // mfx_ui_UserInputVoid_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
