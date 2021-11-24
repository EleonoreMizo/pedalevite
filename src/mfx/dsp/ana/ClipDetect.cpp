/*****************************************************************************

        ClipDetect.cpp
        Author: Laurent de Soras, 2019

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
#include "mfx/dsp/ana/ClipDetect.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ClipDetect::set_clip_val (float val) noexcept
{
	assert (val > 0);

	set_clip_val (-val, val);
}



void	ClipDetect::set_clip_val (float val_min, float val_max) noexcept
{
	assert (val_min < val_max);

	_val_min = val_min;
	_val_max = val_max;
}



bool	ClipDetect::process_sample (float x) const noexcept
{
	return (x < _val_min || x > _val_max);
}



bool	ClipDetect::process_block (const float src_ptr [], int nbr_spl) const noexcept
{
#if 1
	int            res_mask = 0;
	const int      ns8 = nbr_spl & ~7;

	const auto     mi = fstb::ToolsSimd::set1_f32 (_val_min);
	const auto     ma = fstb::ToolsSimd::set1_f32 (_val_max);
	for (int pos = 0; pos < ns8 && res_mask == 0; pos += 8)
	{
		const auto     x0   = fstb::ToolsSimd::loadu_f32 (src_ptr + pos    );
		const auto     x4   = fstb::ToolsSimd::loadu_f32 (src_ptr + pos + 4);
		const auto     inf0 = (x0 > ma);
		const auto     sup0 = (x0 < mi);
		const auto     inf4 = (x4 > ma);
		const auto     sup4 = (x4 < mi);
		const auto     msk  = (inf0 | sup0) | (inf4 | sup4);
		res_mask = fstb::ToolsSimd::movemask_f32 (msk);
	}
	bool           clip_flag = (res_mask != 0);

	for (int pos = ns8; pos < nbr_spl && ! clip_flag; ++pos)
	{
		clip_flag = process_sample (src_ptr [pos]);
	}

	return clip_flag;

#else // Reference implementation

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		if (process_sample (src_ptr [pos]))
		{
			return true;
		}
	}

	return false;

#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
