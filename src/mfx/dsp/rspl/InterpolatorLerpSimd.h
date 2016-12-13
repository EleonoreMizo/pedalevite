/*****************************************************************************

        InterpolatorLerpSimd.h
        Author: Laurent de Soras, 2016

Looks slower than the FPU version.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpolatorLerpSimd_HEADER_INCLUDED)
#define	mfx_dsp_rspl_InterpolatorLerpSimd_HEADER_INCLUDED
#pragma once

#if defined (_MSC_VER)
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/rspl/InterpolatorInterface.h"



namespace mfx
{
namespace dsp
{
namespace rspl
{



class InterpolatorLerpSimd
:	public InterpolatorInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						InterpolatorLerpSimd ();
	virtual			~InterpolatorLerpSimd () {}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// InterpolatorInterface
	virtual void	do_set_ovrspl_l2 (int ovrspl_l2);
	virtual long	do_get_impulse_len () const;
	virtual fstb::FixedPoint
						do_get_group_delay () const;

	virtual void	do_start (int nbr_chn);
	virtual long	do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step);
	virtual float  do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  IMPULSE_LEN = 2;

	int				_nbr_chn;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						InterpolatorLerpSimd (const InterpolatorLerpSimd &other);
	InterpolatorLerpSimd &
						operator = (const InterpolatorLerpSimd &other);
	bool				operator == (const InterpolatorLerpSimd &other) const;
	bool				operator != (const InterpolatorLerpSimd &other) const;

};	// class InterpolatorLerpSimd



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



//#include	"mfx/dsp/rspl/InterpolatorLerpSimd.hpp"



#endif	// mfx_dsp_rspl_InterpolatorLerpSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
