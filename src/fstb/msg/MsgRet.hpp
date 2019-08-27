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
void	MsgRet <T>::set_ret_queue (QueueSPtr ret_queue_sptr, CellType &cell)
{
	assert (_ret_queue_sptr.get () == nullptr);
	assert (ret_queue_sptr.get ()  != nullptr);
	assert (&cell._val             == this);

	_ret_queue_sptr = ret_queue_sptr;
	_cell_ptr      = &cell;
}



template <class T>
void	MsgRet <T>::ret ()
{
	assert (_ret_queue_sptr.get () != nullptr);
	assert (_cell_ptr              != nullptr);

	QueueSPtr      tmp_sptr = _ret_queue_sptr;
	_ret_queue_sptr.reset ();
	tmp_sptr->enqueue (*_cell_ptr);
}



template <class T>
bool	MsgRet <T>::is_attached_to_queue () const
{
	return (_ret_queue_sptr.get () != 0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace msg
}  // namespace fstb



#endif   // fstb_msg_MsgRet_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
