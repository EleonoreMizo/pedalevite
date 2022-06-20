/*****************************************************************************

        ArrayAlign.h
        Author: Laurent de Soras, 2010

Template parameters:

- T: Element type
- LEN: Number of elements, > 0
- AL: Desired memory alignement, in bytes. > 0 and must a power of 2.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_ArrayAlign_HEADER_INCLUDED)
#define	fstb_ArrayAlign_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <climits>
#include <cstdint>



namespace fstb
{



template <typename T, long LEN, long AL>
class ArrayAlign
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	T	Element;

	static const int  NBR_ELT   = LEN;
	static const int  ALIGNMENT = AL;

	               ArrayAlign ();
	               ArrayAlign (const ArrayAlign &other);
	               ~ArrayAlign ();

	ArrayAlign &   operator = (const ArrayAlign &other);

	inline const Element &
	               operator [] (long pos) const noexcept;
	inline Element &
	               operator [] (long pos) noexcept;

	inline const Element *
	               data () const noexcept;
	inline Element *
	               data () noexcept;

	static inline long
	               size () noexcept;
	static inline long
	               length () noexcept;
	static inline long
	               get_alignment () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  ELT_SIZE_BYTE = int (sizeof (Element) * CHAR_BIT / 8);

	uint8_t        _data [NBR_ELT * ELT_SIZE_BYTE + ALIGNMENT - 1];
	Element *      _data_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ArrayAlign &other) const = delete;
	bool           operator != (const ArrayAlign &other) const = delete;

};	// class ArrayAlign



}	// namespace fstb



#include "fstb/ArrayAlign.hpp"



#endif	// fstb_ArrayAlign_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
