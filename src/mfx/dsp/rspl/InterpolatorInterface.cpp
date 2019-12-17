/*****************************************************************************

        InterpolatorInterface.cpp
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

#include "mfx/dsp/rspl/InterpolatorInterface.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Suitable for interpolators having an impulse response of ... 0 0 1 0 0...
// at exact sample positons (group delay taken into account).
bool	InterpolatorInterface::test_and_process_trivial_cases (int &nbr_spl_gen, int nbr_chn, float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step, fstb::FixedPoint group_delay)
{
	bool				proc_flag = false;

	if (   rate.get_frac_val () == 0
	    && rate_step.get_val_int64 () == 0)
	{
		pos_src += group_delay;

		if (pos_src.get_frac_val () == 0)
		{
			const int      nbr_spl_req = end_dest - pos_dest;
			const int      pos_src_int = pos_src.get_int_val ();
			const int      rate_int    = rate.get_int_val ();

			// Copy
			if (rate_int == 1)
			{
				const int      nbr_available = end_src - pos_src_int;
				nbr_spl_gen = std::min (nbr_spl_req, nbr_available);

				for (int chn = 0; chn < nbr_chn; ++chn)
				{
					memcpy (
						dest_ptr_arr [chn] + pos_dest,
						src_ptr_arr [chn] + pos_src_int,
						nbr_spl_gen * sizeof (dest_ptr_arr [chn] [0])
					);
				}
			}

			// Fill
			else if (rate_int == 0)
			{
				nbr_spl_gen = nbr_spl_req;

				for (int chn = 0; chn < nbr_chn; ++chn)
				{
					const float		val = src_ptr_arr [chn] [pos_src_int];
					float *			dest_ptr = dest_ptr_arr [chn];
					for (int pos = pos_dest; pos < end_dest; ++pos)
					{
						dest_ptr [pos] = val;
					}
				}
			}

			// Any other integer rate
			else
			{
				int            nbr_available_src;
				if (rate_int < 0)
				{
					nbr_available_src = pos_src_int - beg_src + 1;
				}
				else
				{
					nbr_available_src = end_src - pos_src_int;
				}
				const int      nbr_available_dest = nbr_available_src / rate_int;
				nbr_spl_gen = std::min (nbr_spl_req, nbr_available_dest);

				const int      pos_dest_stop = pos_dest + nbr_spl_gen;
				for (int chn = 0; chn < nbr_chn; ++chn)
				{
					const float *	src_ptr = src_ptr_arr [chn] + pos_src_int;
					float *			dest_ptr = dest_ptr_arr [chn];
					for (int pos = pos_dest; pos < pos_dest_stop; ++pos)
					{
						dest_ptr [pos] = *src_ptr;
						src_ptr += rate_int;
					}
				}
			}

			proc_flag = true;
		}
	}

	return proc_flag;
}



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
