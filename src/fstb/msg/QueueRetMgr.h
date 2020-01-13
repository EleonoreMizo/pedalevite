/*****************************************************************************

        QueueRetMgr.h
        Author: Laurent de Soras, 2019

This is a lock-free queue to send messages containing allocated resources.

- Cardinality: N sending threads to M destination threads.
- The resources are allocated and deallocated from the sending threads.
- Messages are returned to the sender for deallocation.
- Deallocation is done via a clear() function and the destructor at the
	message content's level.
- It is possible to create as many return queues as necessary (for example
	one per sending threads).
- Sender is responsible to regulary call flush_ret_queue() with its own
	return queue as parameter to recycle used messages.
- All created queues should be flushed before the object is destructed.

Typical setup:

// Sending thread
q_sptr = mgr.create_new_ret_queue ();
...
while (...)
{
	cell_ptr = mgr.use_pool ().take_cell ();
	cell_ptr->_val._content.set_something ()
	mgr.enqueue (*cell_ptr, q_sptr);
	...
	mgr.flush_ret_queue (*q_sptr);
}
...
mgr.kill_ret_queue (q_sptr);

// Destination thread
while (...)
{
	cell_ptr = mgr.dequeue ();
	if (cell_ptr != 0)
	{
		do_something_with (cell_ptr->_val._content);
		cell_ptr->_val.ret ();
	}
	...
}

Template parameters:

- M: A MsgRet class.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_msg_QueueRetMgr_HEADER_INCLUDED)
#define fstb_msg_QueueRetMgr_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/CellPool.h"
#include "conc/LockFreeCell.h"
#include "conc/LockFreeQueue.h"

#include <memory>
#include <mutex>
#include <vector>



namespace fstb
{
namespace msg
{



template <class M>
class QueueRetMgr
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef typename M::CellType CellType;
	typedef typename M::QueueType Queue;
	typedef typename M::QueueSPtr QueueSPtr;
	typedef conc::CellPool <M> Pool;

	               QueueRetMgr () = default;
	virtual        ~QueueRetMgr ();

	QueueSPtr      create_new_ret_queue ();
	void           kill_ret_queue (QueueSPtr &queue_sptr);
	Pool &         use_pool ();

	// Forward
	void           enqueue (CellType &cell, QueueSPtr ret_queue_sptr);
	CellType *     dequeue ();

	// Return
	void           flush_ret_queue (Queue &queue);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <QueueSPtr> QueueList;

	typename QueueList::iterator
	               find_queue (Queue &queue);

	Pool           _pool;
	Queue          _queue_fwd;
	QueueList      _queue_list;
	std::mutex     _queue_list_mtx;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               QueueRetMgr (const QueueRetMgr &other)       = delete;
	               QueueRetMgr (const QueueRetMgr &&other)      = delete;
	QueueRetMgr &  operator = (const QueueRetMgr &other)        = delete;
	QueueRetMgr &  operator = (const QueueRetMgr &&other)       = delete;
	bool           operator == (const QueueRetMgr &other) const = delete;
	bool           operator != (const QueueRetMgr &other) const = delete;

}; // class QueueRetMgr



}  // namespace msg
}  // namespace fstb



#include "fstb/msg/QueueRetMgr.hpp"



#endif   // fstb_msg_QueueRetMgr_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
