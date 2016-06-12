/*****************************************************************************

        Vec2d.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_Vec2d_HEADER_INCLUDED)
#define mfx_uitk_Vec2d_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace mfx
{
namespace uitk
{



class Vec2d
:	public std::array <int, 2>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef public std::array <int, 2> Inherited;

	inline         Vec2d ();
	inline         Vec2d (int x, int y);
	               Vec2d (const Vec2d &other)      = default;
	               ~Vec2d ()                       = default;
	Vec2d &        operator = (const Vec2d &other) = default;

	inline Vec2d & operator += (const Vec2d &other);
	inline Vec2d & operator -= (const Vec2d &other);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



}; // class Vec2d



}  // namespace uitk
}  // namespace mfx



inline mfx::uitk::Vec2d  operator + (const mfx::uitk::Vec2d &lhs, const mfx::uitk::Vec2d &rhs);
inline mfx::uitk::Vec2d  operator - (const mfx::uitk::Vec2d &lhs, const mfx::uitk::Vec2d &rhs);



#include "mfx/uitk/Vec2d.hpp"



#endif   // mfx_uitk_Vec2d_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
