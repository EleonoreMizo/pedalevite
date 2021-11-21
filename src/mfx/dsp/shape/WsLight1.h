/*****************************************************************************

        WsLight1.h
        Author: Laurent de Soras, 2020

https://www.desmos.com/calculator/n8vtrhf0da

Template parameters:

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr);
	static fstb::Vf32 VD::load_f32 (const void *ptr);
	static void VD::store_f32 (void *ptr, const fstb::Vf32 val);

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr);
	static fstb::Vf32 VS::load_f32 (const void *ptr);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsLight1_HEADER_INCLUDED)
#define mfx_dsp_shape_WsLight1_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace shape
{



class WsLight1
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static inline float
	               process_sample (float x) noexcept;
	template <typename VD, typename VS>
	static void    process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	union Combo
	{
		int32_t        _i;
		float          _f;
	};

	static const int32_t _m_mask = 0x007FFFFF; // 23 bits
	static const int32_t _m_invm = 0xFF800000; // exponent and sign



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsLight1



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/WsLight1.hpp"



#endif   // mfx_dsp_shape_WsLight1_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
