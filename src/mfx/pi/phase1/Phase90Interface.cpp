/*****************************************************************************

        Phase90Interface.cpp
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

#include "mfx/pi/phase1/Phase90Interface.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace phase1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Phase90Interface::get_nbr_coefs () const
{
	return do_get_nbr_coefs ();
}



void	Phase90Interface::set_coefs (const double coef_arr [])
{
	assert (coef_arr != 0);

	do_set_coefs (coef_arr);
}



void	Phase90Interface::clear_buffers ()
{
	do_clear_buffers ();
}



void	Phase90Interface::process_block (float dst_0_ptr [], float dst_1_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_0_ptr != 0);
	assert (dst_1_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	do_process_block (dst_0_ptr, dst_1_ptr, src_ptr, nbr_spl);
}



void	Phase90Interface::process_sample (float &dst_0, float &dst_1, float src)
{
	do_process_sample (dst_0, dst_1, src);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
