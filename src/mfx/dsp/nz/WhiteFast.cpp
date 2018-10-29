/*****************************************************************************

        WhiteFast.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/dsp/nz/WhiteFast.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace nz
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	WhiteFast::set_seed (uint32_t seed)
{
	_state = seed & _mm;
}



void	WhiteFast::set_rough_level (float lvl)
{
	assert (lvl > 0);

	Mixed          both;
	both._f = lvl * (0.25f / 0.7f);
	_exp_or = both._i & 0x7F800000;
}



void	WhiteFast::process_block (float dst_ptr [], int nbr_spl)
{
	uint32_t       state = _state;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = gen_new_val (state);
	}
	_state = state;
}



void	WhiteFast::process_block_add (float dst_ptr [], int nbr_spl)
{
	uint32_t       state = _state;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] += gen_new_val (state);
	}
	_state = state;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
