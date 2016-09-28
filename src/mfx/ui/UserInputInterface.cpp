/*****************************************************************************

        UserInputInterface.cpp
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

#include "mfx/ui/UserInputInterface.h"

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	UserInputInterface::get_nbr_param (UserInputType type) const
{
	assert (type >= 0);
	assert (type < UserInputType_NBR_ELT);

	const int      nbr_param = do_get_nbr_param (type);
	assert (nbr_param >= 0);

	return nbr_param;
}



// queue_ptr can be 0
void	UserInputInterface::set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr)
{
	assert (type >= 0);
	assert (type < UserInputType_NBR_ELT);
	assert (index >= 0);
	assert (index < get_nbr_param (type));

	do_set_msg_recipient (type, index, queue_ptr);
}



void	UserInputInterface::return_cell (MsgCell &cell)
{
	do_return_cell (cell);
}



// In microseconds. Reference date not specified.
std::chrono::microseconds	UserInputInterface::get_cur_date () const
{
	return do_get_cur_date ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
