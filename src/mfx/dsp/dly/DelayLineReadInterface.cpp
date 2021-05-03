/*****************************************************************************

        DelayLineReadInterface.cpp
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

#include "mfx/dsp/dly/DelayLineReadInterface.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	DelayLineReadInterface::get_sample_freq () const noexcept
{
	const double   sample_freq = do_get_sample_freq ();
	assert (sample_freq > 0);

	return sample_freq;
}



int	DelayLineReadInterface::get_ovrspl_l2 () const noexcept
{
	const int      ovrspl_l2 = do_get_ovrspl_l2 ();
	assert (ovrspl_l2 >= 0);

	return ovrspl_l2;
}



double	DelayLineReadInterface::get_min_delay_time () const noexcept
{
	const double   dly = do_get_min_delay_time ();
	assert (dly > 0);

	return dly;
}



double	DelayLineReadInterface::get_max_delay_time () const noexcept
{
	const double   dly = do_get_max_delay_time ();
	assert (dly > 0);

	return dly;
}



int	DelayLineReadInterface::estimate_max_one_shot_proc_w_feedback (double min_dly_time) const noexcept
{
	assert (min_dly_time > 0);

	const int      nbr_spl =
		do_estimate_max_one_shot_proc_w_feedback (min_dly_time);
	assert (nbr_spl >= 0);

	return nbr_spl;
}



// pos_in_block (in samples) is subtracted to both dly_beg and dly_end
// (in seconds). Use positive values to offset the read position if you
// read the line in multiple times between two calls to push_block().
// Use negative values if you push_block() first before reading the line.
// Important: read data is oversampled, if oversampling has been set.
// Therefore pos_in_block is related to oversampled data, too.
void	DelayLineReadInterface::read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const noexcept
{
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (dly_beg > 0);
	assert (dly_end > 0);

	do_read_block (dst_ptr, nbr_spl, dly_beg, dly_end, pos_in_block);
}



float	DelayLineReadInterface::read_sample (float dly) const noexcept
{
	assert (dly > 0);

	return do_read_sample (dly);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
