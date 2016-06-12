/*****************************************************************************

        Vec2d.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_uitk_Vec2d_CODEHEADER_INCLUDED)
#define mfx_uitk_Vec2d_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <utility>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vec2d::Vec2d ()
:	Inherited ({{ 0, 0 }})
{
	// Nothing
}



Vec2d::Vec2d (int x, int y)
:	Inherited ({{ x, y }})
{
	// Nothing
}



Vec2d &	Vec2d::operator += (const Vec2d &other)
{
	(*this) [0] += other [0];
	(*this) [1] += other [1];

	return *this;
}



Vec2d &	Vec2d::operator -= (const Vec2d &other)
{
	(*this) [0] -= other [0];
	(*this) [1] -= other [1];

	return *this;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace uitk
}  // namespace mfx



mfx::uitk::Vec2d  operator + (const mfx::uitk::Vec2d &lhs, const mfx::uitk::Vec2d &rhs)
{
	mfx::uitk::Vec2d  res (lhs);
	res += rhs;
	return std::move (res);
}



mfx::uitk::Vec2d  operator - (const mfx::uitk::Vec2d &lhs, const mfx::uitk::Vec2d &rhs)
{
	mfx::uitk::Vec2d  res (lhs);
	res -= rhs;
	return std::move (res);
}



#endif   // mfx_uitk_Vec2d_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
