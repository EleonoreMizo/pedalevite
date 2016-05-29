/*****************************************************************************

        UserInputVoid.cpp
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

#include "mfx/ui/UserInputVoid.h"

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



UserInputVoid::UserInputVoid ()
:	_recip_list ()
{
	for (int i = 0; i < UserInputType_NBR_ELT; ++i)
	{
		const int      nbr_dev =
			do_get_nbr_param (static_cast <UserInputType> (i));
		_recip_list [i].resize (nbr_dev, 0);
	}
}



void	UserInputVoid::send_message (int64_t date, UserInputType type, int index, float val)
{
	// The cell well be lost but we don't care, this is for debugging.
	conc::LockFreeCell <UserInputMsg> * cell_ptr =
		new conc::LockFreeCell <UserInputMsg>;
	cell_ptr->_next_ptr = 0;
	cell_ptr->_val.set (date, type, index, val);
	_recip_list [type] [index]->enqueue (*cell_ptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	UserInputVoid::do_get_nbr_param (UserInputType /*type*/) const
{
	return (64);
}



void	UserInputVoid::do_set_msg_recipient (UserInputType type, int index, MsgQueue * queue_ptr)
{
	_recip_list [type] [index] = queue_ptr;
}



void	UserInputVoid::do_return_cell (MsgCell &/*cell*/)
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
