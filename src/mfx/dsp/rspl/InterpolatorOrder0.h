/*****************************************************************************

        InterpolatorOrder0.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_rspl_InterpolatorOrder0_HEADER_INCLUDED)
#define mfx_dsp_rspl_InterpolatorOrder0_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/rspl/InterpolatorInterface.h"



namespace mfx
{
namespace dsp
{
namespace rspl
{



class InterpolatorOrder0
:	public InterpolatorInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               InterpolatorOrder0 ()  = default;
	virtual        ~InterpolatorOrder0 () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// InterpolatorInterface
	inline virtual void
	               do_set_ovrspl_l2 (int ovrspl_l2);
	inline virtual long
	               do_get_impulse_len () const;
	inline virtual fstb::FixedPoint
	               do_get_group_delay () const;

	inline virtual void
	               do_start (int nbr_chn);
	inline virtual long
	               do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _impulse_len = 1;

	int            _nbr_chn = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               InterpolatorOrder0 (const InterpolatorOrder0 &other) = delete;
	InterpolatorOrder0 &
	               operator = (const InterpolatorOrder0 &other)         = delete;
	bool           operator == (const InterpolatorOrder0 &other) const  = delete;
	bool           operator != (const InterpolatorOrder0 &other) const  = delete;

}; // class InterpolatorOrder0



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/rspl/InterpolatorOrder0.hpp"



#endif   // mfx_dsp_rspl_InterpolatorOrder0_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
