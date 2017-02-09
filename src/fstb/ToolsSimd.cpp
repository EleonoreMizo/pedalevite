/*****************************************************************************

        ToolsSimd.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


#define fstb_ToolsSimd_INV4(x, y) \
	4.0f / (x +  y), 4.0f / (x +  1), 4.0f / (x +  2), 4.0f / (x +  3), \
	4.0f / (x +  4), 4.0f / (x +  5), 4.0f / (x +  6), 4.0f / (x +  7), \
	4.0f / (x +  8), 4.0f / (x +  9), 4.0f / (x + 10), 4.0f / (x + 11), \
	4.0f / (x + 12), 4.0f / (x + 13), 4.0f / (x + 14), 4.0f / (x + 15)

const float	ToolsSimd::_inv_table_4 [_inv_table_4_len] =
{
	fstb_ToolsSimd_INV4 (0x00, 1), fstb_ToolsSimd_INV4 (0x10, 0),
	fstb_ToolsSimd_INV4 (0x20, 0), fstb_ToolsSimd_INV4 (0x30, 0),
	fstb_ToolsSimd_INV4 (0x40, 0), fstb_ToolsSimd_INV4 (0x50, 0),
	fstb_ToolsSimd_INV4 (0x60, 0), fstb_ToolsSimd_INV4 (0x70, 0),
	fstb_ToolsSimd_INV4 (0x80, 0), fstb_ToolsSimd_INV4 (0x90, 0),
	fstb_ToolsSimd_INV4 (0xA0, 0), fstb_ToolsSimd_INV4 (0xB0, 0),
	fstb_ToolsSimd_INV4 (0xC0, 0), fstb_ToolsSimd_INV4 (0xD0, 0),
	fstb_ToolsSimd_INV4 (0xE0, 0), fstb_ToolsSimd_INV4 (0xF0, 0)
};

#undef fstb_ToolsSimd_INV4

const int32_t	ToolsSimd::_log2_sub = 128;
const float	ToolsSimd::_log2_coef_a = -1.f / 3;
const float	ToolsSimd::_log2_coef_b =  2.f;
const float	ToolsSimd::_log2_coef_c = -2.f / 3;

const int32_t	ToolsSimd::_exp2_add = 127;
const float	ToolsSimd::_exp2_coef_a =  1.f / 3;
const float	ToolsSimd::_exp2_coef_b =  2.f / 3;
const float	ToolsSimd::_exp2_coef_c =  1.f;



}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
