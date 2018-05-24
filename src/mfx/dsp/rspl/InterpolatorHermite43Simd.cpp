/*****************************************************************************

        InterpolatorHermite43Simd.cpp
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

#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
#include	"mfx/dsp/rspl/InterpolatorHermite43Simd.h"

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



InterpolatorHermite43Simd::InterpolatorHermite43Simd ()
:	_nbr_chn (1)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InterpolatorHermite43Simd::do_set_ovrspl_l2 (int ovrspl_l2)
{
	// Nothing
}



int	InterpolatorHermite43Simd::do_get_impulse_len () const
{
	return IMPULSE_LEN;
}



fstb::FixedPoint	InterpolatorHermite43Simd::do_get_group_delay () const
{
	return fstb::FixedPoint (1);
}



void	InterpolatorHermite43Simd::do_start (int nbr_chn)
{
	_nbr_chn = nbr_chn;
}



int	InterpolatorHermite43Simd::do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	const auto     half = fstb::ToolsSimd::set1_f32 (0.5);

	typedef	float	Buffer [4];
	fstb_TYPEDEF_ALIGN (16, Buffer, BufAlign);

	BufAlign       q_arr;
	int            pos_src_arr [4];

	const int      pos_dest_old = pos_dest;
	const int      src_limit    = end_src - IMPULSE_LEN + 1;

	bool           cont_flag    = true;
	do
	{
		q_arr [0]       = pos_src.get_frac_val_flt ();
		pos_src_arr [0] = pos_src.get_int_val ();
		pos_src += rate;
		rate    += rate_step;

		q_arr [1]       = pos_src.get_frac_val_flt ();
		pos_src_arr [1] = pos_src.get_int_val ();
		pos_src += rate;
		rate    += rate_step;

		q_arr [2]       = pos_src.get_frac_val_flt ();
		pos_src_arr [2] = pos_src.get_int_val ();
		pos_src += rate;
		rate    += rate_step;

		q_arr [3]       = pos_src.get_frac_val_flt ();
		pos_src_arr [3] = pos_src.get_int_val ();
		pos_src += rate;
		rate    += rate_step;

		int            pos_src_int = pos_src.get_int_val ();

		if (   pos_dest + 4 >= end_dest
		    || pos_src_int >= src_limit
		    || pos_src_int < beg_src)
		{
			cont_flag = false;

			int            last_valid = 3;
			while (   pos_dest + last_valid >= end_dest
			       || pos_src_arr [last_valid] >= src_limit
			       || pos_src_arr [last_valid] < beg_src)
			{
				-- last_valid;
				assert (last_valid >= 0);
			}

			int            valid_index = 0;
			do
			{
				const float    q = q_arr [valid_index];
				pos_src_int = pos_src_arr [valid_index];

				int				chn_cnt = 0;
				do
				{
					const float *  src_ptr = src_ptr_arr [chn_cnt];
					float *        dest_ptr = dest_ptr_arr [chn_cnt];

					const float    in_0 = src_ptr [pos_src_int    ];
					const float    in_1 = src_ptr [pos_src_int + 1];
					const float    in_2 = src_ptr [pos_src_int + 2];
					const float    in_3 = src_ptr [pos_src_int + 3];

#if 0				// Less operations, more dependencies (difficult to parallelize)
					const float    c = (in_2 - in_0) * 0.5f;
					const float    v = in_1 - in_2;
					const float    w = c + v;
					const float    a = w + v + (in_3 - in_1) * 0.5f;
					const float    b = -w -a;

#else				// Modified version by James McCartney <asynth@io.com>. Seems faster.
					const float    c = 0.5f * (in_2 - in_0);
					const float    a = 1.5f * (in_1 - in_2) + 0.5f * (in_3 - in_0);
					const float    b = in_0 - in_1 + c - a;

#endif

					const float    out = (((a * q) + b) * q + c) * q + in_1;
					dest_ptr [pos_dest] = out;

					++ chn_cnt;
				}
				while (chn_cnt < _nbr_chn);

				++ pos_dest;
				++ valid_index;
			}
			while (valid_index <= last_valid);
		}

		else
		{
			const auto     q = fstb::ToolsSimd::load_f32 (q_arr);

			int            chn_cnt = 0;
			do
			{
				const float *  src_ptr = src_ptr_arr [chn_cnt];
				float *        dest_ptr = dest_ptr_arr [chn_cnt];

				pos_src_int = pos_src_arr [0];
				auto           in_0 =
					fstb::ToolsSimd::loadu_f32 (src_ptr + pos_src_int);

				pos_src_int = pos_src_arr [1];
				auto           in_1 =
					fstb::ToolsSimd::loadu_f32 (src_ptr + pos_src_int);

				pos_src_int = pos_src_arr [2];
				auto           in_2 =
					fstb::ToolsSimd::loadu_f32 (src_ptr + pos_src_int);

				pos_src_int = pos_src_arr [3];
				auto           in_3 =
					fstb::ToolsSimd::loadu_f32 (src_ptr + pos_src_int);

				// Transposes [in_0 ... in_3]
				fstb::ToolsSimd::transpose_f32 (in_0, in_1, in_2, in_3);

				// Interpolation
				const auto     c = (in_2 - in_0) * half;
				const auto     v = in_1 - in_2;
				const auto     w = c + v;
				const auto     a = (w + v) + (in_3 - in_1) * half;
				const auto     b_neg = w + a;

				const auto     out = (((a * q) - b_neg) * q + c) * q + in_1;

				fstb::ToolsSimd::storeu_f32 (dest_ptr + pos_dest, out);

				++ chn_cnt;
			}
			while (chn_cnt < _nbr_chn);

			pos_dest += 4;
		}
	}
	while (cont_flag);

	return pos_dest - pos_dest_old;
}



float	InterpolatorHermite43Simd::do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate)
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
