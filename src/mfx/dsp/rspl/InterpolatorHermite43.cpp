/*****************************************************************************

        InterpolatorHermite43.cpp
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

#include	"mfx/dsp/rspl/InterpolatorHermite43.h"

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



InterpolatorHermite43::InterpolatorHermite43 ()
:	_nbr_chn (1)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InterpolatorHermite43::do_set_ovrspl_l2 (int ovrspl_l2)
{
	// Nothing
}



int	InterpolatorHermite43::do_get_impulse_len () const
{
	return IMPULSE_LEN;
}



fstb::FixedPoint	InterpolatorHermite43::do_get_group_delay () const
{
	return fstb::FixedPoint (1);
}



void	InterpolatorHermite43::do_start (int nbr_chn)
{
	_nbr_chn = nbr_chn;
}



int	InterpolatorHermite43::do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	const int      pos_dest_old = pos_dest;
	const int      src_limit    = end_src - IMPULSE_LEN + 1;
	int            pos_src_int  = pos_src.get_int_val ();

	do
	{
		const float		q       = pos_src.get_frac_val_flt ();
		int            chn_cnt = 0;
		do
		{
			const float *  src_ptr  = src_ptr_arr [chn_cnt];
			float *        dest_ptr = dest_ptr_arr [chn_cnt];

			const float    in_0 = src_ptr [pos_src_int    ];
			const float    in_1 = src_ptr [pos_src_int + 1];
			const float    in_2 = src_ptr [pos_src_int + 2];
			const float    in_3 = src_ptr [pos_src_int + 3];

#if 0		// Less operations, more dependencies (difficult to parallelize)
			const float    c = (in_2 - in_0) * 0.5f;
			const float    v = in_1 - in_2;
			const float    w = c + v;
			const float    a = w + v + (in_3 - in_1) * 0.5f;
			const float    b = -w -a;

#else		// Modified version by James McCartney <asynth@io.com>. Seems faster.
			const float    c = 0.5f * (in_2 - in_0);
			const float    a = 1.5f * (in_1 - in_2) + 0.5f * (in_3 - in_0);
			const float    b = in_0 - in_1 + c - a;

#endif

			const float    out = (((a * q) + b) * q + c) * q + in_1;
			dest_ptr [pos_dest] = out;

			++ chn_cnt;
		}
		while (chn_cnt < _nbr_chn);

		pos_src += rate;
		rate    += rate_step;
		++ pos_dest;

		pos_src_int = pos_src.get_int_val ();
	}
	while (   pos_dest < end_dest
	       && pos_src_int < src_limit
	       && pos_src_int >= beg_src);

	return pos_dest - pos_dest_old;
}



float	InterpolatorHermite43::do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate)
{
	const int32_t  pos_int = pos_src.get_int_val ();
	const float    q       = pos_src.get_frac_val_flt ();
	const float    in_0    = src_ptr [pos_int    ];
	const float    in_1    = src_ptr [pos_int + 1];
	const float    in_2    = src_ptr [pos_int + 2];
	const float    in_3    = src_ptr [pos_int + 3];
	const float		c       = 0.5f * (in_2 - in_0);
	const float		a       = 1.5f * (in_1 - in_2) + 0.5f * (in_3 - in_0);
	const float		b       = in_0 - in_1 + c - a;
	const float		out     = (((a * q) + b) * q + c) * q + in_1;

	return out;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
