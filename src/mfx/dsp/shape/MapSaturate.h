/*****************************************************************************

        MapSaturate.h
        Author: Laurent de Soras, 2016

Maps a positive number in [0 ; X scale] into a number in [0 ; Y scale].

saturate() gives a "saturated" curve:
	y = (1+c) - (1+c)*c / (x + c)
	y'(0) = (1+c) / c

desaturate() gives a kind of exponential curve:
	x = -c - (1+c)*c / (y - (1+c))
	x'(0) = c / (1+c)

Mapping is done in order to realize:
- x == desaturate (saturate (x))
- x == saturate (desaturate (x))

Computation is very fast (1 div, 2 add).

T: Numeric type for all operations, must be floating point.
C: std::ratio, curve, must be > 0. The smaller, the steeper is the curve
XS: X scale, std::ratio
YS: Y scale, std::ratio

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_MapSaturate_HEADER_INCLUDED)
#define mfx_dsp_shape_MapSaturate_HEADER_INCLUDED

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
class MapSaturate
{

	static_assert (std::ratio_greater <C, std::ratio <0, 1> >::value, "");
	static_assert (XS::num != 0, "");
	static_assert (XS::den != 0, "");
	static_assert (YS::num != 0, "");
	static_assert (YS::den != 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef C  ValC;
	typedef XS ValXS;
	typedef YS ValYS;

	static inline T
	               saturate (T x);
	static inline T
	               desaturate (T y);

	static inline T
	               get_c ();
	static inline T
	               get_xs ();
	static inline T
	               get_ys ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MapSaturate ()                               = delete;
	               MapSaturate (const MapSaturate &other)       = delete;
	virtual        ~MapSaturate ()                              = delete;
	MapSaturate &  operator = (const MapSaturate &other)        = delete;
	bool           operator == (const MapSaturate &other) const = delete;
	bool           operator != (const MapSaturate &other) const = delete;

}; // class MapSaturate



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/MapSaturate.hpp"



#endif   // mfx_dsp_shape_MapSaturate_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
