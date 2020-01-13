/*****************************************************************************

        Rect.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_uitk_Rect_CODEHEADER_INCLUDED)
#define mfx_uitk_Rect_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Rect::Rect (int x1, int y1, int x2, int y2)
:	Inherited ({{ Vec2d (x1, y1) , Vec2d (x2, y2) }})
{
	// Nothing
}



Rect::Rect (Vec2d c0, Vec2d c1)
:	Inherited ({{ c0, c1 }})
{
	// Nothing
}



// Returns a null rect if there is no intersection
Rect &	Rect::intersect (const Rect &other)
{
	Rect &         lhs = *this;

	if (   lhs [1] [0] <= other [0] [0]
	    || lhs [0] [0] >= other [1] [0]
	    || lhs [1] [1] <= other [0] [1]
	    || lhs [0] [1] >= other [1] [1])
	{
		lhs = Rect ();
	}
	else
	{
		lhs [0] [0] = std::max (lhs [0] [0], other [0] [0]);
		lhs [0] [1] = std::max (lhs [0] [1], other [0] [1]);
		lhs [1] [0] = std::min (lhs [1] [0], other [1] [0]);
		lhs [1] [1] = std::min (lhs [1] [1], other [1] [1]);
	}

	return lhs;
}



Rect &	Rect::merge (const Rect &other)
{
	Rect &         lhs = *this;

	const Vec2d    sz_lhs = lhs.get_size ();
	const Vec2d    sz_rhs = other.get_size ();
	if (sz_lhs [0] <= 0 || sz_lhs [1] <= 0)
	{
		lhs = other;
	}
	else if (sz_rhs [0] > 0 && sz_rhs [1] > 0)
	{
		lhs [0] [0] = std::min (lhs [0] [0], other [0] [0]);
		lhs [0] [1] = std::min (lhs [0] [1], other [0] [1]);
		lhs [1] [0] = std::max (lhs [1] [0], other [1] [0]);
		lhs [1] [1] = std::max (lhs [1] [1], other [1] [1]);
	}

	return lhs;
}



Vec2d	Rect::get_size () const
{
	return (*this) [1] - (*this) [0];
}



Rect &	Rect::operator += (const Vec2d &other)
{
	(*this) [0] += other;
	(*this) [1] += other;

	return *this;
}



Rect &	Rect::operator -= (const Vec2d &other)
{
	(*this) [0] -= other;
	(*this) [1] -= other;

	return *this;
}



bool	Rect::is_empty () const
{
	const Rect &   tmp = *this;

	return (tmp [1] [0] <= tmp [0] [0] || tmp [1] [1] <= tmp [0] [1]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace uitk
}  // namespace mfx



mfx::uitk::Rect  operator + (const mfx::uitk::Rect &lhs, const mfx::uitk::Vec2d &rhs)
{
	mfx::uitk::Rect   res (lhs);
	res += rhs;

	return res;
}



mfx::uitk::Rect  operator - (const mfx::uitk::Rect &lhs, const mfx::uitk::Vec2d &rhs)
{
	mfx::uitk::Rect   res (lhs);
	res -= rhs;

	return res;
}



#endif   // mfx_uitk_Rect_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
