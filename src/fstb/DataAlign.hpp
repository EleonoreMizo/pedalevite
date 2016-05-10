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
bool	DataAlign <A>::check_ptr (const void *ptr)
{
	return is_ptr_align_nz (ptr, 16);
}

template <bool A>
ToolsSimd::VectF32	DataAlign <A>::load_f32 (const void *ptr)
{
	return ToolsSimd::load_f32 (ptr);
}

template <bool A>
void	DataAlign <A>::store_f32 (void *ptr, ToolsSimd::VectF32 v)
{
	ToolsSimd::store_f32 (ptr, v);
}



template <>
inline bool	DataAlign <false>::check_ptr (const void *ptr)
{
	return (ptr != 0);
}

template <>
inline ToolsSimd::VectF32	DataAlign <false>::load_f32 (const void *ptr)
{
	return ToolsSimd::loadu_f32 (ptr);
}

template <>
inline void	DataAlign <false>::store_f32 (void *ptr, ToolsSimd::VectF32 v)
{
	ToolsSimd::storeu_f32 (ptr, v);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_DataAlign_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
