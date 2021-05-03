/*****************************************************************************

        SineQPoly3.h
        Author: Laurent de Soras, 2019

Maps a number in [0 ; X scale] into a number in [0 ; Y scale].
X and Y can be negative.

The function reproduces the shape of a sine in the range [0 ; pi/2].
First derivates are the same.
Maximum error: 0.0108
Maximum relative error: 1.63 %
Suitable for fast evaluation.

Template parameters:

- T: Numeric type for all operations, must be floating point.
- XSN / XSD: X scale, non zero
- YSN / YSD: Y scale, non zero

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_SineQPoly3_HEADER_INCLUDED)
#define mfx_dsp_shape_SineQPoly3_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename T, long XSN, long XSD, long YSN, long YSD>
class SineQPoly3
{
	static_assert (XSN != 0, "");
	static_assert (XSD != 0, "");
	static_assert (YSN != 0, "");
	static_assert (YSD != 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	fstb_FORCEINLINE T
	               operator () (T x) const noexcept;
	static fstb_FORCEINLINE T
	               approximate (T x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SineQPoly3 &other) const = delete;
	bool           operator != (const SineQPoly3 &other) const = delete;

}; // class SineQPoly3



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/SineQPoly3.hpp"



#endif   // mfx_dsp_shape_SineQPoly3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
