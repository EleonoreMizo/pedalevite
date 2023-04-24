/*****************************************************************************

        ArrayAlign.hpp
        Author: Laurent de Soras, 2012

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_ArrayAlign_CODEHEADER_INCLUDED)
#define	fstb_ArrayAlign_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <new>
#include <type_traits>

#include <cassert>



namespace fstb
{



template <bool D>
class DestroyAux
{
public:
	template <typename T>
	static inline void
	               destroy_elt (T *ptr)
	{
		ptr->~T ();
	}
};

template <>
class DestroyAux <true>
{
public:
	template <typename T>
	static inline void
	               destroy_elt (T *ptr)
	{
		// Nothing
		fstb::unused (ptr);
	}
};



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, long LEN, long AL>
ArrayAlign <T, LEN, AL>::ArrayAlign ()
:/*	_data ()
,*/	_data_ptr (0)
{
	_data_ptr = reinterpret_cast <Element *> (
		reinterpret_cast <ptrdiff_t> (&_data [ALIGNMENT - 1]) & -ALIGNMENT
	);

	for (long pos = 0; pos < NBR_ELT; ++pos)
	{
		new (&_data_ptr [pos]) Element ();
	}
}



template <typename T, long LEN, long AL>
ArrayAlign <T, LEN, AL>::ArrayAlign (const ArrayAlign <T, LEN, AL> &other)
:/*	_data ()
,*/	_data_ptr (0)
{
	_data_ptr = reinterpret_cast <Element *> (
		reinterpret_cast <ptrdiff_t> (&_data [ALIGNMENT - 1]) & -ALIGNMENT
	);

	for (long pos = 0; pos < NBR_ELT; ++pos)
	{
		new (&_data_ptr [pos]) Element (other [pos]);
	}
}



template <typename T, long LEN, long AL>
ArrayAlign <T, LEN, AL>::~ArrayAlign ()
{
	assert (_data_ptr != 0);

	for (long pos = 0; pos < NBR_ELT; ++pos)
	{
		Element *   ptr =
			reinterpret_cast <Element *> (&_data_ptr [pos]);
		typedef DestroyAux <std::is_trivially_destructible <Element>::value> Destr;
		Destr::destroy_elt (ptr);
	}
}



template <typename T, long LEN, long AL>
ArrayAlign <T, LEN, AL> &	ArrayAlign <T, LEN, AL>::operator = (const ArrayAlign <T, LEN, AL> &other)
{
	if (this != &other)
	{
		for (long pos = 0; pos < NBR_ELT; ++pos)
		{
			(*this) [pos] = other [pos];
		}
	}

	return *this;
}



template <typename T, long LEN, long AL>
const typename ArrayAlign <T, LEN, AL>::Element &	ArrayAlign <T, LEN, AL>::operator [] (long pos) const noexcept
{
	assert (_data_ptr != 0);
	assert (pos >= 0);
	assert (pos < NBR_ELT);

	return _data_ptr [pos];
}



template <typename T, long LEN, long AL>
typename ArrayAlign <T, LEN, AL>::Element &	ArrayAlign <T, LEN, AL>::operator [] (long pos) noexcept
{
	assert (_data_ptr != 0);
	assert (pos >= 0);
	assert (pos < NBR_ELT);

	return _data_ptr [pos];
}



template <typename T, long LEN, long AL>
const typename ArrayAlign <T, LEN, AL>::Element *	ArrayAlign <T, LEN, AL>::data () const noexcept
{
	assert (_data_ptr != 0);

	return _data_ptr;
}



template <typename T, long LEN, long AL>
typename ArrayAlign <T, LEN, AL>::Element *	ArrayAlign <T, LEN, AL>::data () noexcept
{
	assert (_data_ptr != 0);

	return _data_ptr;
}



template <typename T, long LEN, long AL>
long	ArrayAlign <T, LEN, AL>::size () noexcept
{
	return NBR_ELT;
}



template <typename T, long LEN, long AL>
long	ArrayAlign <T, LEN, AL>::length () noexcept
{
	return NBR_ELT;
}



template <typename T, long LEN, long AL>
long	ArrayAlign <T, LEN, AL>::get_alignment () noexcept
{
	return ALIGNMENT;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace fstb



#endif	// fstb_ArrayAlign_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
