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
#include "mfx/Cst.h"

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
	assert (nbr_param <= Cst::_max_input_param);

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



void	UserInputInterface::assign_queues_to_input_dev (MsgQueue &queue_cmd, MsgQueue &queue_gui, MsgQueue &queue_audio)
{
	for (int type = 0; type < UserInputType_NBR_ELT; ++type)
	{
		const int      nbr_param = get_nbr_param (
			static_cast <UserInputType> (type)
		);
		for (int index = 0; index < nbr_param; ++index)
		{
			MsgQueue *     queue_ptr = &queue_cmd;
			switch (Cst::_queue_type_arr [type] [index])
			{
			case Cst::UserInputQueueType_GUI:
				queue_ptr = &queue_gui;
				break;
			case Cst::UserInputQueueType_AUDIO:
				queue_ptr = &queue_audio;
				break;
			case Cst::UserInputQueueType_CMD:
			default:
				break;
			}
			set_msg_recipient (
				static_cast <UserInputType> (type), index, queue_ptr
			);
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
