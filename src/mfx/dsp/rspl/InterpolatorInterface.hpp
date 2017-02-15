/*****************************************************************************

        InterpolatorInterface.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (mfx_dsp_rspl_InterpolatorInterface_CURRENT_CODEHEADER)
	#error Recursive inclusion of InterpolatorInterface code header.
#endif
#define	mfx_dsp_rspl_InterpolatorInterface_CURRENT_CODEHEADER

#if ! defined (mfx_dsp_rspl_InterpolatorInterface_CODEHEADER_INCLUDED)
#define	mfx_dsp_rspl_InterpolatorInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/rspl/Cst.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InterpolatorInterface::set_ovrspl_l2 (int ovrspl_l2)
{
	assert (ovrspl_l2 >= 0);

	do_set_ovrspl_l2 (ovrspl_l2);
}



// Number of required samples, starting from the operational position (pos_src
// in process_block). The filter implementation is mostly anti-causal.
long	InterpolatorInterface::get_impulse_len () const
{
	const long		len = do_get_impulse_len ();
	assert (len >= 1);

	return (len);
}



// Not exactly the group delay. It is actually the location of the energy
// peak relative to the beginning of the impulse for integer pos_src (null
// fractional position).
// In other words, it is the value to subtract to the desired interpolation
// location to obtain the operational position:
// pos_src = desired interpolation location - offset
// For a FIR filter, it is more or less equivalent to:
// impulse length - 1 - filter group delay.
fstb::FixedPoint	InterpolatorInterface::get_group_delay () const
{
	const fstb::FixedPoint	group_delay = do_get_group_delay ();
	assert (group_delay.get_int_val () >= -1);

	return (group_delay);
}



void	InterpolatorInterface::start (int nbr_chn)
{
	assert (nbr_chn > 0);
	assert (nbr_chn <= Cst::MAX_NBR_CHN);

	do_start (nbr_chn);
}



// The valid sample source range is [beg_src ; end_src[
// Rate can be negative, or become negative during the processing.
// If there isn't enough source data:
// - positive rate: the last sample used at the next position will be at or
// right after end_src, meaning that the last used source position is always
// before end_src
// - negative rate: same principle. The next position will be right before
// beg_src.
// Returns the number of processed destination samples (> 0).
long	InterpolatorInterface::process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	assert (dest_ptr_arr != 0);
	assert (dest_ptr_arr [0] != 0);
	assert (src_ptr_arr != 0);
	assert (src_ptr_arr [0] != 0);
	assert (pos_src.get_int_val () >= beg_src);
	assert (pos_src.get_int_val () < end_src - get_impulse_len () + 1);
	assert (pos_dest < end_dest);

	const long		nbr_processed_spl = do_process_block (
		dest_ptr_arr,
		src_ptr_arr,
		pos_dest,
		pos_src,
		end_dest,
		beg_src,
		end_src,
		rate,
		rate_step
	);

	assert (nbr_processed_spl > 0);
	assert (nbr_processed_spl <= end_dest - pos_dest);
#if ! defined (NDEBUG)
	const fstb::FixedPoint	last_pos_src (
		pos_src + integrate_rate (nbr_processed_spl - 1, rate, rate_step)
	);
	assert (last_pos_src.get_int_val () >= beg_src);
	assert (last_pos_src.get_int_val () < end_src - get_impulse_len () + 1);
#endif

	return (nbr_processed_spl);
}



float	InterpolatorInterface::process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate)
{
	assert (src_ptr != 0);

	return do_process_sample (src_ptr, pos_src, rate);
}



fstb::FixedPoint	InterpolatorInterface::integrate_rate (long len, const fstb::FixedPoint &rate, const fstb::FixedPoint &rate_step)
{
	assert (len >= 0);

	fstb::FixedPoint	integ (rate_step);
	integ.mul_int (len - 1);
	integ.shift_right (1);
	integ.add (rate);
	integ.mul_int (len);

	return (integ);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



#endif	// mfx_dsp_rspl_InterpolatorInterface_CODEHEADER_INCLUDED

#undef mfx_dsp_rspl_InterpolatorInterface_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
