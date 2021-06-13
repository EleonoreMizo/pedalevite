/*****************************************************************************

        RingFifo.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_RingFifo_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_RingFifo_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: setup
Description:
	Sets the upper bound for the FIFO length. This function must be called
	before calling other functions.
Input parameters:
	- capacity: maximum number of elements that the FIFO can store. > 0.
Throws: depends on std::vector
==============================================================================
*/

template <typename T>
void	RingFifo <T>::setup (int capacity)
{
	assert (capacity > 0);

	_capacity = capacity;
	_nbr_elt  = 0;
	update_buf ();
}



/*
==============================================================================
Name: get_capacity
Description:
	Retrieve the upper bound for the FIFO length.
Returns:
	Maximum number of elements that the FIFO can store. > 0.
Throws: Nothing
==============================================================================
*/

template <typename T>
int	RingFifo <T>::get_capacity () const noexcept
{
	assert (_capacity > 0);

	return _capacity;
}



/*
==============================================================================
Name: get_size
Description:
	Returns the current number of elements stored in the FIFO.
Returns:
	The number of elements, >= 0.
Throws: Nothing
==============================================================================
*/

template <typename T>
int	RingFifo <T>::get_size () const noexcept
{
	assert (_capacity > 0);

	return _nbr_elt;
}



/*
==============================================================================
Name: is_empty
Description:
	Checks if there is no element at all in the FIFO
Returns:
	true if it is empty.
Throws: Nothing
==============================================================================
*/

template <typename T>
bool	RingFifo <T>::is_empty () const noexcept
{
	assert (_capacity > 0);

	return (_nbr_elt > 0);
}



/*
==============================================================================
Name: get_room
Description:
	Computes the available free slots.
Returns:
	The maximum number of elements that can be inserted from now. >= 0.
Throws: Nothing
==============================================================================
*/

template <typename T>
int	RingFifo <T>::get_room () const noexcept
{
	assert (_capacity > 0);

	return _capacity - _nbr_elt;
}



/*
==============================================================================
Name: push
Description:
	Adds one element. The client must ensure there is room enough.
Input parameters:
	- x: element to add to the queue.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	RingFifo <T>::push (T x) noexcept
{
	assert (_capacity > 0);
	assert (get_room () >= 1);

	_buf [_pos_w] = x;
	step (1);
	++ _nbr_elt;
}



/*
==============================================================================
Name: push
Description:
	Adds multiple elements. The client must ensure there is room enough.
Input parameters:
	- src_ptr: pointer on the block of elements. The first element of the block
		is inserted first.
	- nbr_spl: number of elements to insert. > 0.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	RingFifo <T>::push (const T src_ptr [], int nbr_spl) noexcept
{
	assert (_capacity > 0);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= get_room ());

	_pos_w    = write_block_internal (src_ptr, nbr_spl);
	_nbr_elt += nbr_spl;
}



/*
==============================================================================
Name: pop
Description:
	Reads and removes an element from the queue. The client must ensure there
	is at least one element in the queue.
Returns:
	The element.
Throws: Nothing
==============================================================================
*/

template <typename T>
T	RingFifo <T>::pop () noexcept
{
	assert (_capacity > 0);
	assert (! is_empty ());

	const T        x = _buf [(_pos_w - _nbr_elt) & _mask];
	-- _nbr_elt;

	return x;
}



/*
==============================================================================
Name: pop
Description:
	Reads and removes a block of elements from the queue. The client must
	ensure there is enough elements in the queue.
Input parameters:
	- nbr_spl: number of elements to read, > 0.
Output parameters:
	- dst_ptr: pointer on a buffer receiving the read elements.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	RingFifo <T>::pop (T dst_ptr [], int nbr_spl) noexcept
{
	assert (_capacity > 0);
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= get_size ());

	read_block_at (dst_ptr, _nbr_elt, nbr_spl);
	_nbr_elt -= nbr_spl;
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears the FIFO.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	RingFifo <T>::clear_buffers () noexcept
{
	_nbr_elt = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	RingFifo <T>::update_buf ()
{
	const int      len_min = _capacity;
	_len    = 1 << fstb::get_next_pow_2 (len_min);
	_mask   = _len - 1;
	_pos_w &= _mask;
	_buf.resize (_len);
}



template <typename T>
void	RingFifo <T>::step (int nbr_spl) noexcept
{
	assert (nbr_spl > 0);
	assert (nbr_spl <= _capacity);

	_pos_w = (_pos_w + nbr_spl) & _mask;
}



template <typename T>
int	RingFifo <T>::write_block_internal (const T src_ptr [], int nbr_spl) noexcept
{
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _capacity);

	int            pos     = 0;
	T * const      buf_ptr = _buf.data ();
	RingBufVectorizer rbv (_len);

	for (rbv.start (nbr_spl, _pos_w); rbv.end (); rbv.next ())
	{
		const int      work_len = rbv.get_seg_len ();
		const int      pos_w    = rbv.get_curs_pos (0);

		fstb::copy_no_overlap (buf_ptr + pos_w, src_ptr + pos, work_len);

		pos += work_len;
	}

	return rbv.get_curs_pos (0);
}



template <typename T>
void	RingFifo <T>::read_block_at (T dst_ptr [], int d, int nbr_spl) const noexcept
{
	assert (d >= 0);
	assert (d <= _capacity);
	assert (nbr_spl > 0);
	assert (nbr_spl <= d + 1);

	int            pos_r = (_pos_w - d) & _mask;
	const int      room  = _len - pos_r;
	const int      len_1 = std::min (nbr_spl, room);
	const int      len_2 = nbr_spl - len_1;
	fstb::copy_no_overlap (dst_ptr, &_buf [pos_r], len_1);
	if (len_2 > 0)
	{
		fstb::copy_no_overlap (dst_ptr + len_1, _buf.data (), len_2);
	}
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_RingFifo_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
