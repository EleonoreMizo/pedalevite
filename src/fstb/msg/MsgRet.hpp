/*****************************************************************************

        MsgRet.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_msg_MsgRet_CODEHEADER_INCLUDED)
#define fstb_msg_MsgRet_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace fstb
{
namespace msg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	MsgRet <T>::clear ()
{
	_content.clear ();
}



template <class T>
void	MsgRet <T>::set_ret_queue (QueueType &ret_queue, CellType &cell)
{
	assert (_ret_queue_ptr == nullptr);
	assert (&cell._val     == this);

	_ret_queue_ptr = &ret_queue;
	_cell_ptr      = &cell;
}



template <class T>
void	MsgRet <T>::ret ()
{
	assert (_ret_queue_ptr != nullptr);
	assert (_cell_ptr      != nullptr);

	QueueType *    tmp_ptr = _ret_queue_ptr;
	_ret_queue_ptr = 0;
	tmp_ptr->enqueue (*_cell_ptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace msg
}  // namespace fstb



#endif   // fstb_msg_MsgRet_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
