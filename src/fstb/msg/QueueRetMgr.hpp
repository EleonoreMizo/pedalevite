/*****************************************************************************

        QueueRetMgr.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_msg_QueueRetMgr_CODEHEADER_INCLUDED)
#define fstb_msg_QueueRetMgr_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>

#include <cassert>



namespace fstb
{
namespace msg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class M>
QueueRetMgr <M>::~QueueRetMgr () noexcept
{
	// Client should ensure that all queues are flushed and all cells are
	// returned to the pool before the object is destructed. This code is only
	// here for minimal consistency and may do wrong things, because cell's
	// resources will be freed from a possibly inappropriate thread.
	CellType *     cell_ptr = nullptr;
	do
	{
		cell_ptr = _queue_fwd.dequeue ();
		if (cell_ptr != nullptr)
		{
			cell_ptr->_val.ret ();
		}
	}
	while (cell_ptr != nullptr);

	for (auto &q_sptr : _queue_list)
	{
		flush_ret_queue (*q_sptr);
		q_sptr.reset ();
	}
}



template <class M>
typename QueueRetMgr <M>::QueueSPtr	QueueRetMgr <M>::create_new_ret_queue ()
{
	std::lock_guard <std::mutex>  lock (_queue_list_mtx);

	QueueSPtr      q_sptr { std::make_shared <Queue> () };
	_queue_list.push_back (q_sptr);

	return q_sptr;
}



// Client has to make sure that all the messages were returned before
// calling this function.
template <class M>
void	QueueRetMgr <M>::kill_ret_queue (QueueSPtr &queue_sptr)
{
	assert (queue_sptr.get () != nullptr);

	flush_ret_queue (*queue_sptr);

	std::lock_guard <std::mutex>  lock (_queue_list_mtx);
	typename QueueList::iterator  it = find_queue (*queue_sptr);
	_queue_list.erase (it);

	// At this point, queue_sptr should be the unique owner of the queue.
	// If it isn't, it means that other external shared pointers still manage
	// it or that sent messages weren't returned to the queue.
	assert (queue_sptr.unique ());
	queue_sptr.reset ();
}



template <class M>
typename QueueRetMgr <M>::Pool &	QueueRetMgr <M>::use_pool () noexcept
{
	return _pool;
}



template <class M>
void	QueueRetMgr <M>::enqueue (CellType &cell, QueueSPtr ret_queue_sptr) noexcept
{
	assert (find_queue (*ret_queue_sptr) != _queue_list.end ());

	cell._val.set_ret_queue (ret_queue_sptr, cell);
	_queue_fwd.enqueue (cell);
}



template <class M>
typename QueueRetMgr <M>::CellType *	QueueRetMgr <M>::dequeue () noexcept
{
	return _queue_fwd.dequeue ();
}



template <class M>
void	QueueRetMgr <M>::flush_ret_queue (Queue &queue) noexcept
{
	assert (find_queue (queue) != _queue_list.end ());

	CellType *     cell_ptr = nullptr;
	do
	{
		cell_ptr = queue.dequeue ();
		if (cell_ptr != nullptr)
		{
			cell_ptr->_val.clear ();
			assert (! cell_ptr->_val.is_attached_to_queue ());
			_pool.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != nullptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class M>
typename QueueRetMgr <M>::QueueList::iterator	QueueRetMgr <M>::find_queue (Queue &queue) noexcept
{
	return std::find_if (
		_queue_list.begin (),
		_queue_list.end (),
		[&queue] (const QueueSPtr &q_sptr)
		{
			return (q_sptr.get () == &queue);
		}
	);
}



}  // namespace msg
}  // namespace fstb



#endif   // fstb_msg_QueueRetMgr_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
