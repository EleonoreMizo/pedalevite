/*****************************************************************************

        InterpolatorInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpolatorInterface_HEADER_INCLUDED)
#define	mfx_dsp_rspl_InterpolatorInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/FixedPoint.h"



namespace mfx
{
namespace dsp
{
namespace rspl
{



class InterpolatorInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual			~InterpolatorInterface () {}

	inline void		set_ovrspl_l2 (int ovrspl_l2);
	inline long		get_impulse_len () const;
	inline fstb::FixedPoint
						get_group_delay () const;

	inline void		start (int nbr_chn);
	inline long		process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step);

	// Utility functions
	static inline fstb::FixedPoint
						integrate_rate (long len, const fstb::FixedPoint &rate, const fstb::FixedPoint &rate_step);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void	do_set_ovrspl_l2 (int ovrspl_l2) = 0;
	virtual long	do_get_impulse_len () const = 0;
	virtual fstb::FixedPoint
						do_get_group_delay () const = 0;

	virtual void	do_start (int nbr_chn) = 0;
	virtual long	do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step) = 0;

	// Utility functions
	static bool		test_and_process_trivial_cases (long &nbr_spl_gen, int nbr_chn, float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step, fstb::FixedPoint group_delay);

};	// class InterpolatorInterface



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



#include "mfx/dsp/rspl/InterpolatorInterface.hpp"



#endif	// mfx_dsp_rspl_InterpolatorInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
