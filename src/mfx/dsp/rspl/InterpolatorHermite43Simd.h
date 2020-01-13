/*****************************************************************************

        InterpolatorHermite43Simd.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpolatorHermite43Simd_HEADER_INCLUDED)
#define	mfx_dsp_rspl_InterpolatorHermite43Simd_HEADER_INCLUDED
#pragma once

#if defined (_MSC_VER)
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/rspl/InterpolatorInterface.h"



namespace mfx
{
namespace dsp
{
namespace rspl
{



class InterpolatorHermite43Simd
:	public InterpolatorInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// InterpolatorInterface
	void           do_set_ovrspl_l2 (int ovrspl_l2) override;
	int            do_get_impulse_len () const override;
	fstb::FixedPoint
	               do_get_group_delay () const override;

	void           do_start (int nbr_chn) override;
	int            do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step) override;
	float          do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int IMPULSE_LEN	= 4;

	int            _nbr_chn = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const InterpolatorHermite43Simd &other) const = delete;
	bool           operator != (const InterpolatorHermite43Simd &other) const = delete;

};	// class InterpolatorHermite43Simd



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



//#include "mfx/dsp/rspl/InterpolatorHermite43Simd.hpp"



#endif	// mfx_dsp_rspl_InterpolatorHermite43Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
