/*****************************************************************************

        WsSmthAbs.h
        Author: Laurent de Soras, 2020

https://www.desmos.com/calculator/1cus9jejml

Template parameters:

- A: size of the curvature as a std::ratio. > 0

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectS32 VD::load_s32 (const void *ptr);
	static void VD::store_s32 (void *ptr, const fstb::ToolsSimd::VectS32 val);

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectS32 VS::load_s32 (const void *ptr);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsSmthAbs_HEADER_INCLUDED)
#define mfx_dsp_shape_WsSmthAbs_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <ratio>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename A>
class WsSmthAbs
{
	static_assert (
		std::ratio_greater <A, std::ratio <0, 1> >::value,
		"WsSmthAbs: A > 0"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline double  operator () (double x);

	static inline float
	               process_sample (float x);
	template <typename VD, typename VS>
	static void    process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsSmthAbs



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/WsSmthAbs.hpp"



#endif   // mfx_dsp_shape_WsSmthAbs_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
