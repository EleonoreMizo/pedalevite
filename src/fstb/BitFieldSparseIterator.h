/*****************************************************************************

        BitFieldSparseIterator.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_BitFieldSparseIterator_HEADER_INCLUDED)
#define fstb_BitFieldSparseIterator_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



class BitFieldSparse;

class BitFieldSparseIterator
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit inline
	               BitFieldSparseIterator (const BitFieldSparse &bfs) noexcept;
	               BitFieldSparseIterator (const BitFieldSparseIterator &other) = default;
	               BitFieldSparseIterator (BitFieldSparseIterator &&other)      = default;

	inline void    start (int pos_start = 0, int pos_end = -1) noexcept;
	inline bool    is_rem_elt () const noexcept;
	inline void    iterate () noexcept;

	inline int     get_bit_index () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	const BitFieldSparse &
	               _bfs;
	int            _bit_index = -1; // Negative: terminated
	int            _pos_end   =  0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BitFieldSparseIterator ()                               = delete;
	BitFieldSparseIterator &
	               operator = (const BitFieldSparseIterator &other)        = delete;
	BitFieldSparseIterator &
	               operator = (BitFieldSparseIterator &&other)             = delete;
	bool           operator == (const BitFieldSparseIterator &other) const = delete;
	bool           operator != (const BitFieldSparseIterator &other) const = delete;

}; // class BitFieldSparseIterator



}  // namespace fstb



#include "fstb/BitFieldSparseIterator.hpp"



#endif   // fstb_BitFieldSparseIterator_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
