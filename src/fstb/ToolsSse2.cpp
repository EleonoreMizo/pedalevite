/*****************************************************************************

        ToolsSse2.cpp
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

#include "ToolsSse2.h"

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const ToolsSse2::VectI32	ToolsSse2::_zero = { 0, 0, 0, 0 };

const ToolsSse2::VectI32	ToolsSse2::_c16_8000 =
{ 0x80008000, 0x80008000, 0x80008000, 0x80008000 };

const ToolsSse2::VectI32	ToolsSse2::_c32_00008000 =
{ 0x8000, 0x8000, 0x8000, 0x8000 };

const ToolsSse2::VectI32	ToolsSse2::_c32_0000ffff =
{ 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

const ToolsSse2::VectI32	ToolsSse2::_mask_lo64 =
{ 0xFFFFFFFF, 0xFFFFFFFF, 0, 0 };

const ToolsSse2::VectI32	ToolsSse2::_mask_abs =
{ 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF };



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
