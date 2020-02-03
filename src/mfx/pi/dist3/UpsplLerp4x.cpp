/*****************************************************************************

        UpsplLerp4x.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/pi/dist3/UpsplLerp4x.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist3
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	UpsplLerp4x::clear_buffers ()
{
	_val_old = 0;
}



void	UpsplLerp4x::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr));
	assert (fstb::DataAlign <true>::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	fstb::ToolsSimd::VectF32   old = fstb::ToolsSimd::set1_f32 (_val_old);
	const auto     c1234 = fstb::ToolsSimd::set_f32 (0.25f, 0.5f, 0.75f, 1.0f);
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const auto     cur = fstb::ToolsSimd::set1_f32 (src_ptr [pos]);
		fstb::ToolsSimd::mac (old, cur - old, c1234);
		fstb::ToolsSimd::store_f32 (dst_ptr + pos * 4, old);
		old = cur;
	}
	_val_old = fstb::ToolsSimd::Shift <0>::extract (old);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
