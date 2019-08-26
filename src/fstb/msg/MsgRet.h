/*****************************************************************************

        MsgRet.h
        Author: Laurent de Soras, 2019

Template parameters:

- T: the message's payload. Should implement void T::clear(); to explicitely
free the allocated resources, if any.

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

	               MsgRet ()                            = default;
	               MsgRet (const MsgRet <T> &other)     = default;
	               ~MsgRet ()                           = default;
	MsgRet <T> &   operator = (const MsgRet <T> &other) = default;

	void           clear ();
	void           set_ret_queue (QueueType &ret_queue, CellType &cell);
	void           ret ();

	T              _content;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	QueueType * volatile
	               _ret_queue_ptr = nullptr;
	CellType * volatile
	               _cell_ptr      = nullptr;



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
