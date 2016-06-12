/*****************************************************************************

        Rect.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_Rect_HEADER_INCLUDED)
#define mfx_uitk_Rect_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Vec2d.h"

#include <array>



namespace mfx
{
namespace uitk
{



class Rect
:	public std::array <Vec2d, 2>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::array <Vec2d, 2> Inherited;

	inline         Rect (int x1, int y1, int x2, int y2);
	inline         Rect (Vec2d c0, Vec2d c1);
	               Rect ()                        = default;
	               Rect (const Rect &other)       = default;
	               ~Rect ()                       = default;

	Rect &         operator = (const Rect &other) = default;

	inline Rect &  intersect (const Rect &other);
	inline Rect &  merge (const Rect &other);
	inline Vec2d   get_size () const;

	inline Rect &  operator += (const Vec2d &other);
	inline Rect &  operator -= (const Vec2d &other);

	inline bool    empty () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:


}; // class Rect



}  // namespace uitk
}  // namespace mfx



inline mfx::uitk::Rect  operator + (const mfx::uitk::Rect &lhs, const mfx::uitk::Vec2d &rhs);
inline mfx::uitk::Rect  operator - (const mfx::uitk::Rect &lhs, const mfx::uitk::Vec2d &rhs);



#include "mfx/uitk/Rect.hpp"



#endif   // mfx_uitk_Rect_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
