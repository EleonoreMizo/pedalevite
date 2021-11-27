/*****************************************************************************

        DataAlign.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_DataAlign_CODEHEADER_INCLUDED)
#define fstb_DataAlign_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <bool A>
bool	DataAlign <A>::check_ptr (const void *ptr) noexcept
{
	return is_ptr_align_nz (ptr, 16);
}

template <bool A>
Vf32	DataAlign <A>::load_f32 (const void *ptr) noexcept
{
	return Vf32::load (ptr);
}

template <bool A>
Vs32	DataAlign <A>::load_s32 (const void *ptr) noexcept
{
	return Vs32::load (ptr);
}

template <bool A>
void	DataAlign <A>::store_f32 (void *ptr, Vf32 v) noexcept
{
	v.store (ptr);
}

template <bool A>
void	DataAlign <A>::store_s32 (void *ptr, Vs32 v) noexcept
{
	v.store (ptr);
}



template <>
inline bool	DataAlign <false>::check_ptr (const void *ptr) noexcept
{
	return (ptr != nullptr);
}

template <>
inline Vf32	DataAlign <false>::load_f32 (const void *ptr) noexcept
{
	return Vf32::loadu (ptr);
}

template <>
inline Vs32	DataAlign <false>::load_s32 (const void *ptr) noexcept
{
	return Vs32::loadu (ptr);
}

template <>
inline void	DataAlign <false>::store_f32 (void *ptr, Vf32 v) noexcept
{
	v.storeu (ptr);
}

template <>
inline void	DataAlign <false>::store_s32 (void *ptr, Vs32 v) noexcept
{
	v.storeu (ptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_DataAlign_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
