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
	fstb_ToolsSimd_INV4 (0x000, 1), fstb_ToolsSimd_INV4 (0x010, 0),
	fstb_ToolsSimd_INV4 (0x020, 0), fstb_ToolsSimd_INV4 (0x030, 0),
	fstb_ToolsSimd_INV4 (0x040, 0), fstb_ToolsSimd_INV4 (0x050, 0),
	fstb_ToolsSimd_INV4 (0x060, 0), fstb_ToolsSimd_INV4 (0x070, 0),
	fstb_ToolsSimd_INV4 (0x080, 0), fstb_ToolsSimd_INV4 (0x090, 0),
	fstb_ToolsSimd_INV4 (0x0A0, 0), fstb_ToolsSimd_INV4 (0x0B0, 0),
	fstb_ToolsSimd_INV4 (0x0C0, 0), fstb_ToolsSimd_INV4 (0x0D0, 0),
	fstb_ToolsSimd_INV4 (0x0E0, 0), fstb_ToolsSimd_INV4 (0x0F0, 0),
	fstb_ToolsSimd_INV4 (0x100, 0), fstb_ToolsSimd_INV4 (0x110, 0),
	fstb_ToolsSimd_INV4 (0x120, 0), fstb_ToolsSimd_INV4 (0x130, 0),
	fstb_ToolsSimd_INV4 (0x140, 0), fstb_ToolsSimd_INV4 (0x150, 0),
	fstb_ToolsSimd_INV4 (0x160, 0), fstb_ToolsSimd_INV4 (0x170, 0),
	fstb_ToolsSimd_INV4 (0x180, 0), fstb_ToolsSimd_INV4 (0x190, 0),
	fstb_ToolsSimd_INV4 (0x1A0, 0), fstb_ToolsSimd_INV4 (0x1B0, 0),
	fstb_ToolsSimd_INV4 (0x1C0, 0), fstb_ToolsSimd_INV4 (0x1D0, 0),
	fstb_ToolsSimd_INV4 (0x1E0, 0), fstb_ToolsSimd_INV4 (0x1F0, 0),
	fstb_ToolsSimd_INV4 (0x200, 0), fstb_ToolsSimd_INV4 (0x210, 0),
	fstb_ToolsSimd_INV4 (0x220, 0), fstb_ToolsSimd_INV4 (0x230, 0),
	fstb_ToolsSimd_INV4 (0x240, 0), fstb_ToolsSimd_INV4 (0x250, 0),
	fstb_ToolsSimd_INV4 (0x260, 0), fstb_ToolsSimd_INV4 (0x270, 0),
	fstb_ToolsSimd_INV4 (0x280, 0), fstb_ToolsSimd_INV4 (0x290, 0),
	fstb_ToolsSimd_INV4 (0x2A0, 0), fstb_ToolsSimd_INV4 (0x2B0, 0),
	fstb_ToolsSimd_INV4 (0x2C0, 0), fstb_ToolsSimd_INV4 (0x2D0, 0),
	fstb_ToolsSimd_INV4 (0x2E0, 0), fstb_ToolsSimd_INV4 (0x2F0, 0),
	fstb_ToolsSimd_INV4 (0x300, 0), fstb_ToolsSimd_INV4 (0x310, 0),
	fstb_ToolsSimd_INV4 (0x320, 0), fstb_ToolsSimd_INV4 (0x330, 0),
	fstb_ToolsSimd_INV4 (0x340, 0), fstb_ToolsSimd_INV4 (0x350, 0),
	fstb_ToolsSimd_INV4 (0x360, 0), fstb_ToolsSimd_INV4 (0x370, 0),
	fstb_ToolsSimd_INV4 (0x380, 0), fstb_ToolsSimd_INV4 (0x390, 0),
	fstb_ToolsSimd_INV4 (0x3A0, 0), fstb_ToolsSimd_INV4 (0x3B0, 0),
	fstb_ToolsSimd_INV4 (0x3C0, 0), fstb_ToolsSimd_INV4 (0x3D0, 0),
	fstb_ToolsSimd_INV4 (0x3E0, 0), fstb_ToolsSimd_INV4 (0x3F0, 0),
	4.0f / 0x400
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
