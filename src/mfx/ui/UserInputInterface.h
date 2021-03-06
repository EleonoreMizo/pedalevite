/*****************************************************************************

        UserInputInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_UserInputInterface_HEADER_INCLUDED)
#define mfx_ui_UserInputInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/LockFreeCell.h"
#include "conc/LockFreeQueue.h"
#include "mfx/ui/UserInputMsg.h"
#include "mfx/ui/UserInputType.h"

#include <chrono>



namespace mfx
{
namespace ui
{



class UserInputInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef conc::LockFreeCell <UserInputMsg>  MsgCell;
	typedef conc::LockFreeQueue <UserInputMsg> MsgQueue;

	               UserInputInterface ()                           = default;
	               UserInputInterface (const UserInputInterface &other) = default;
	               UserInputInterface (UserInputInterface &&other) = default;

	virtual        ~UserInputInterface ()                          = default;

	virtual UserInputInterface &
	               operator = (const UserInputInterface &other)    = default;
	virtual UserInputInterface &
	               operator = (UserInputInterface &&other)         = default;

	int            get_nbr_param (UserInputType type) const;
	void           set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr);
	void           return_cell (MsgCell &cell);
	std::chrono::microseconds
	               get_cur_date () const;

	// Utility functions
	void           assign_queues_to_input_dev (MsgQueue &queue_cmd, MsgQueue &queue_gui, MsgQueue &queue_audio);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_get_nbr_param (UserInputType type) const = 0;
	virtual void   do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr) = 0;
	virtual void   do_return_cell (MsgCell &cell) = 0;
	virtual std::chrono::microseconds
	               do_get_cur_date () const = 0;



}; // class UserInputInterface



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/UserInputInterface.hpp"



#endif   // mfx_ui_UserInputInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
