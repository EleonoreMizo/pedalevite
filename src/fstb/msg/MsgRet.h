/*****************************************************************************

        MsgRet.h
        Author: Laurent de Soras, 2019

Template parameters:

- T: the message's payload. Should implement void T::clear () noexcept; to
explicitely free the allocated resources, if any.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_msg_MsgRet_HEADER_INCLUDED)
#define fstb_msg_MsgRet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/LockFreeQueue.h"

#include <memory>



namespace fstb
{
namespace msg
{



template <class T>
class MsgRet
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T ContentType;
	typedef conc::LockFreeQueue <MsgRet <T> > QueueType;
	typedef conc::LockFreeCell <MsgRet <T> > CellType;
	typedef std::shared_ptr <QueueType> QueueSPtr;

	void           clear () noexcept;
	void           set_ret_queue (QueueSPtr ret_queue_sptr, CellType &cell) noexcept;
	void           ret () noexcept;
	bool           is_attached_to_queue () const noexcept;

	T              _content;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	QueueSPtr      _ret_queue_sptr;
	CellType *     _cell_ptr = nullptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MsgRet <T> &other) const = delete;
	bool           operator != (const MsgRet <T> &other) const = delete;

}; // class MsgRet



}  // namespace msg
}  // namespace fstb



#include "fstb/msg/MsgRet.hpp"



#endif   // fstb_msg_MsgRet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
