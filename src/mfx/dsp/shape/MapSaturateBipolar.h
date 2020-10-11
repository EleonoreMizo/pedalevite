/*****************************************************************************

        MapSaturateBipolar.h
        Author: Laurent de Soras, 2016

Bipolar version of MapSaturate. Works from [-X scale ; X scale] to
[-Y scale ; Y scale]

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_MapSaturateBipolar_HEADER_INCLUDED)
#define mfx_dsp_shape_MapSaturateBipolar_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <ratio>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename T, class C, class XS, class YS>
class MapSaturateBipolar
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr inline T
						saturate (T x);
	static constexpr inline T
						desaturate (T y);

	static constexpr inline T
	               get_c ();
	static constexpr inline T
	               get_xs ();
	static constexpr inline T
	               get_ys ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::ratio <0, 1> R0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MapSaturateBipolar &other) const  = delete;
	bool           operator != (const MapSaturateBipolar &other) const  = delete;

}; // class MapSaturateBipolar



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/MapSaturateBipolar.hpp"



#endif   // mfx_dsp_shape_MapSaturateBipolar_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
