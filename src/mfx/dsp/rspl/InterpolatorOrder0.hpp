/*****************************************************************************

        InterpolatorOrder0.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpolatorOrder0_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpolatorOrder0_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <algorithm>

#include <cstring>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InterpolatorOrder0::do_set_ovrspl_l2 (int ovrspl_l2)
{
	fstb::unused (ovrspl_l2);
	// Nothing
}



int	InterpolatorOrder0::do_get_impulse_len () const noexcept
{
	return _impulse_len;
}



fstb::FixedPoint	InterpolatorOrder0::do_get_group_delay () const noexcept
{
	return fstb::FixedPoint (-1, uint32_t (1) << 31); // -0.5
}



void	InterpolatorOrder0::do_start (int nbr_chn) noexcept
{
	_nbr_chn = nbr_chn;
}



int	InterpolatorOrder0::do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step) noexcept
{
	const int      pos_dest_old = pos_dest;
	const int      src_limit    = end_src - (_impulse_len - 1);
	int            pos_src_int  = pos_src.get_int_val ();

	const float *  src_ptr      = src_ptr_arr [0];
	float *        dest_ptr     = dest_ptr_arr [0];

	if (   rate_step.get_val_int64 () == 0
	    && rate.get_int_val () == 1
	    && rate.get_frac_val () == 0)
	{
		const int      max_len_dst = end_dest  - pos_dest;
		const int      max_len_src = src_limit - pos_src_int;
		const int      len         = std::min (max_len_dst, max_len_src);
		memcpy (
			dest_ptr + pos_dest,
			src_ptr + pos_src_int,
			len * sizeof (*dest_ptr)
		);
		pos_dest += len;
	}
	else
	{
		do
		{
			dest_ptr [pos_dest] = src_ptr [pos_src_int];

			pos_src += rate;
			rate    += rate_step;
			++ pos_dest;

			pos_src_int = pos_src.get_int_val ();
		}
		while (   pos_dest < end_dest
		       && pos_src_int < src_limit
		       && pos_src_int >= beg_src);
	}

	return pos_dest - pos_dest_old;
}



float	InterpolatorOrder0::do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate) noexcept
{
	fstb::unused (rate);

	const int32_t  pos_int = pos_src.get_int_val ();

	return src_ptr [pos_int];
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpolatorOrder0_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
