/*****************************************************************************

        InterpolatorLerpSimd.cpp
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
#include "mfx/dsp/rspl/InterpolatorLerpSimd.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InterpolatorLerpSimd::do_set_ovrspl_l2 (int ovrspl_l2)
{
	fstb::unused (ovrspl_l2);

	// Nothing
}



int	InterpolatorLerpSimd::do_get_impulse_len () const
{
	return IMPULSE_LEN;
}



fstb::FixedPoint	InterpolatorLerpSimd::do_get_group_delay () const
{
	return fstb::FixedPoint (0, 0);
}



void	InterpolatorLerpSimd::do_start (int nbr_chn)
{
	_nbr_chn = nbr_chn;
}



int	InterpolatorLerpSimd::do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	typedef	float	Buffer [4];
	alignas (16) Buffer  in_0_arr;
	alignas (16) Buffer  in_1_arr;
	alignas (16) Buffer  q_arr;
	int            pos_src_arr [4];

	const int      pos_dest_old = pos_dest;
	const int      src_limit = end_src - IMPULSE_LEN + 1;

	bool           cont_flag = true;
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

				int            chn_cnt = 0;
				do
				{
					const float *  src_ptr = src_ptr_arr [chn_cnt];
					float *        dest_ptr = dest_ptr_arr [chn_cnt];

					const float    in_0 = src_ptr [pos_src_int    ];
					const float    in_1 = src_ptr [pos_src_int + 1];
					const float    out  = in_0 + q * (in_1 - in_0);
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
			const auto     q       = fstb::ToolsSimd::load_f32 (q_arr);

			int            chn_cnt = 0;
			do
			{
				const float *  src_ptr  = src_ptr_arr [chn_cnt];
				float *        dest_ptr = dest_ptr_arr [chn_cnt];

				pos_src_int  = pos_src_arr [0];
				in_0_arr [0] = src_ptr [pos_src_int    ];
				in_1_arr [0] = src_ptr [pos_src_int + 1];

				pos_src_int  = pos_src_arr [1];
				in_0_arr [1] = src_ptr [pos_src_int    ];
				in_1_arr [1] = src_ptr [pos_src_int + 1];

				pos_src_int  = pos_src_arr [2];
				in_0_arr [2] = src_ptr [pos_src_int    ];
				in_1_arr [2] = src_ptr [pos_src_int + 1];

				pos_src_int  = pos_src_arr [3];
				in_0_arr [3] = src_ptr [pos_src_int    ];
				in_1_arr [3] = src_ptr [pos_src_int + 1];

				const auto     in_0 = fstb::ToolsSimd::load_f32 (in_0_arr);
				const auto     in_1 = fstb::ToolsSimd::load_f32 (in_1_arr);
				const auto     dif  = in_1 - in_0;
				auto           out  = in_0;
				fstb::ToolsSimd::mac (out, dif, q);

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



float	InterpolatorLerpSimd::do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate)
{
	fstb::unused (rate);

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
