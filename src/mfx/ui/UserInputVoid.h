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



class UserInputVoid final
:	public UserInputInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               UserInputVoid ();
	               ~UserInputVoid () = default;

	void           send_message (std::chrono::microseconds date, UserInputType type, int index, float val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ui::UserInputInterface
	int            do_get_nbr_param (UserInputType type) const final;
	void           do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr) final;
	void           do_return_cell (MsgCell &cell) final;
	std::chrono::microseconds
	               do_get_cur_date () const final;




/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <MsgQueue *> QueueArray;
	typedef std::array <QueueArray, UserInputType_NBR_ELT> RecipientList;

	RecipientList  _recip_list;

	std::chrono::high_resolution_clock
	               _clk;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               UserInputVoid (const UserInputVoid &other)     = delete;
	               UserInputVoid (UserInputVoid &&other)          = delete;
	UserInputVoid &
	               operator = (const UserInputVoid &other)        = delete;
	UserInputVoid &
	               operator = (UserInputVoid &&other)             = delete;
	bool           operator == (const UserInputVoid &other) const = delete;
	bool           operator != (const UserInputVoid &other) const = delete;

}; // class UserInputVoid



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/UserInputVoid.hpp"



#endif   // mfx_ui_UserInputVoid_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
