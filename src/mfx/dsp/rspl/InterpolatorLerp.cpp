/*****************************************************************************

        InterpolatorLerp.cpp
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

#include	"mfx/dsp/rspl/InterpolatorLerp.h"

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



InterpolatorLerp::InterpolatorLerp ()
:	_nbr_chn (1)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InterpolatorLerp::do_set_ovrspl_l2 (int ovrspl_l2)
{
	// Nothing
}



long	InterpolatorLerp::do_get_impulse_len () const
{
	return (IMPULSE_LEN);
}



fstb::FixedPoint	InterpolatorLerp::do_get_group_delay () const
{
	return (fstb::FixedPoint (1));
}



void	InterpolatorLerp::do_start (int nbr_chn)
{
	_nbr_chn = nbr_chn;
}



long	InterpolatorLerp::do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	const fstb::FixedPoint  pos_src_old (pos_src);
	const fstb::FixedPoint  rate_old (rate);
	const long		pos_dest_old = pos_dest;

	const long		src_limit   = end_src - IMPULSE_LEN + 1;
	long				pos_src_int = pos_src.get_int_val ();

	{
		const float *	src_ptr  = src_ptr_arr [0];
		float *			dest_ptr = dest_ptr_arr [0];

		do
		{
			const float    q    = pos_src.get_frac_val_flt ();
			const float    in_0 = src_ptr [pos_src_int    ];
			const float    in_1 = src_ptr [pos_src_int + 1];
			const float    out  = in_0 + q * (in_1 - in_0);
			dest_ptr [pos_dest] = out;

			pos_src += rate;
			rate += rate_step;
			++ pos_dest;

			pos_src_int = pos_src.get_int_val ();
		}
		while (   pos_dest < end_dest
		       && pos_src_int < src_limit
		       && pos_src_int >= beg_src);
	}
	const long		stop_pos = pos_dest;

	for (int chn = 1; chn < _nbr_chn; ++chn)
	{
		pos_src  = pos_src_old;
		rate     = rate_old;
		pos_dest = pos_dest_old;
		const float *	src_ptr  = src_ptr_arr [chn];
		float *			dest_ptr = dest_ptr_arr [chn];

		do
		{
			pos_src_int = pos_src.get_int_val ();
			const float    q    = pos_src.get_frac_val_flt ();
			const float    in_0 = src_ptr [pos_src_int    ];
			const float    in_1 = src_ptr [pos_src_int + 1];
			const float    out  = in_0 + q * (in_1 - in_0);
			dest_ptr [pos_dest] = out;

			pos_src += rate;
			rate += rate_step;
			++ pos_dest;
		}
		while (pos_dest < stop_pos);
	}

	return (stop_pos - pos_dest_old);
}



float	InterpolatorLerp::do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate)
{
	const int32_t  pos_int = pos_src.get_int_val ();
	const float    q       = pos_src.get_frac_val_flt ();
	const float    in_0    = src_ptr [pos_int    ];
	const float    in_1    = src_ptr [pos_int + 1];
	const float    out     = in_0 + q * (in_1 - in_0);

	return out;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
