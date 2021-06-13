/*****************************************************************************

        RingFifo.h
        Author: Laurent de Soras, 2021

Simple FIFO implemented as a ring buffer.

Template parameters:

- T: Stored data type.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_RingFifo_HEADER_INCLUDED)
#define mfx_dsp_dly_RingFifo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <type_traits>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace dly
{



template <typename T>
class RingFifo
{
	static_assert (std::is_nothrow_copy_assignable <T>::value, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;

	void           setup (int capacity);
	inline int     get_capacity () const noexcept;

	inline int     get_size () const noexcept;
	inline bool    is_empty () const noexcept;
	inline int     get_room () const noexcept;

	inline void    push (T x) noexcept;
	void           push (const T src_ptr [], int nbr_spl) noexcept;
	inline T       pop () noexcept;
	void           pop (T dst_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <T> Buffer;

	void           update_buf ();
	fstb_FORCEINLINE void
	               step (int nbr_spl) noexcept;
	int            write_block_internal (const T src_ptr [], int nbr_spl) noexcept;
	void           read_block_at (T dst_ptr [], int d, int nbr_spl) const noexcept;

	// Ring buffer
	Buffer         _buf      = Buffer (1 << 6, T (0));

	// Buffer length in samples, power of 2, > 0
	int            _len      = int (_buf.size ());

	// Logical AND mask for buffer positinos
	int            _mask     = _len - 1;

	// Writing position, [0 ; _len[
	int            _pos_w    = 0;

	// Specified capacity in elements, <= _len. 0 = FIFO not setup
	int            _capacity = 0;

	// Number of elements stored in the FIFO
	int            _nbr_elt  = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const RingFifo &other) const = delete;
	bool           operator != (const RingFifo &other) const = delete;

}; // class RingFifo



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/RingFifo.hpp"



#endif   // mfx_dsp_dly_RingFifo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
