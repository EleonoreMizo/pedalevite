/*****************************************************************************

        LockFreeQueue.hpp
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (conc_LockFreeQueue_CODEHEADER_INCLUDED)
#define	conc_LockFreeQueue_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace conc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
LockFreeQueue <T>::LockFreeQueue ()
:	_m_ptr ()
{
	_m_ptr->_dummy._next_ptr = nullptr;
	_m_ptr->_head.set (&_m_ptr->_dummy, 0);
	_m_ptr->_tail.set (&_m_ptr->_dummy, 0);
}



template <class T>
void	LockFreeQueue <T>::enqueue (CellType &cell)
{
	cell._next_ptr = nullptr;  // set the cell next pointer to NULL

	CellType *     tail_ptr = nullptr;
	ptrdiff_t      icount   = 0;

	bool           cont_flag = true;
	do	// try until enqueue is done
	{
		icount   = _m_ptr->_tail.get_val ();	// read the tail modification count
		tail_ptr = _m_ptr->_tail.get_ptr ();	// read the tail cell

		// try to link the cell to the tail cell
		void *         old_ptr = tail_ptr->_next_ptr.cas (&cell, nullptr);

		if (old_ptr == nullptr)
		{
			cont_flag = false;	// enqueue is done, exit the loop
		}
		else
		{
			// tail was not pointing to the last cell, try to set tail to the next cell
			_m_ptr->_tail.cas2 (tail_ptr->_next_ptr, icount + 1, tail_ptr, icount);
		}
	}
	while (cont_flag);

	_m_ptr->_tail.cas2 (&cell, icount + 1, tail_ptr, icount);
}



// Returns 0 if the queue is empty.
template <class T>
typename LockFreeQueue <T>::CellType *	LockFreeQueue <T>::dequeue ()
{
	ptrdiff_t      ocount   = 0;
	ptrdiff_t      icount   = 0;
	CellType *     head_ptr = nullptr;
	CellType *     next_ptr = nullptr;

	do	// try until dequeue is done
	{
		ocount   = _m_ptr->_head.get_val ();   // read the head modification count
		icount   = _m_ptr->_tail.get_val ();   // read the tail modification count
		head_ptr = _m_ptr->_head.get_ptr ();   // read the head cell
		next_ptr = head_ptr->_next_ptr;        // read the next cell

		if (ocount == _m_ptr->_head.get_val ())  // ensures that next is a valid pointer to avoid failure when reading next value
		{
			if (head_ptr == _m_ptr->_tail.get_ptr ())   // is queue empty or tail falling behind ?
			{
				if (next_ptr == nullptr)   // is queue empty ?
				{
					return nullptr; // queue is empty: return NULL
				}
				// tail is pointing to head in a non empty queue, try to set tail to the next cell
				_m_ptr->_tail.cas2 (next_ptr, icount + 1, head_ptr, icount);
			}
			else if (next_ptr != nullptr) // if we are not competing on the dummy next
			{
				// try to set tail to the next cell
				if (_m_ptr->_head.cas2 (next_ptr, ocount + 1, head_ptr, ocount))
				{
					break;   // dequeue done, exit the loop
				}
			}
		}
	}
	while (true);

	if (head_ptr == &_m_ptr->_dummy)   // check wether we’re trying to pop the dummy cell
	{
		enqueue (*head_ptr);    // this is the dummy cell: push it back to the fifo
		head_ptr = dequeue ();  // and pop a cell again
	}

	return head_ptr;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace conc



#endif	// conc_LockFreeQueue_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
