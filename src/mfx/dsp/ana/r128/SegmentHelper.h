/*****************************************************************************

        SegmentHelper.h
        Author: Laurent de Soras, 2022

Helper to split block processing of arbitrary length at fixed frame boundaries

Usage:

// s is an object of this class
for (s.start (block_len); s.is_rem_spl (); s.iterate ())
{
	const int seg_len = s.get_seg_len ();
	// Placeholder: process a segment of seg_len input samples
	if (s.is_frame_ready ())
	{
		// Placeholder: handle frame
	}
}

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_SegmentHelper_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_SegmentHelper_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/FilterK.h"
#include "mfx/dsp/ana/r128/MeanSq.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class SegmentHelper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_period (double per_s);

	void           start (int nbr_spl) noexcept;
	bool           is_rem_elt () const noexcept;
	void           iterate () noexcept;

	int            get_seg_len () const noexcept;
	bool           is_frame_ready () const noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline bool    is_setup () const noexcept;
	inline bool    is_started () const noexcept;

	void           update_period () noexcept;
	inline void    compute_seg_len () noexcept;
	inline void    step_frame () noexcept;

	// Sample frequency, Hz. > 0. 0 = not set yet (invalid)
	double         _sample_freq = 0;

	// Frame period, seconds. > 0. 0 = not set yet (invalid)
	double         _per_s       = 0;

	// Frame period, samples. > 0. 0 = not set yet (invalid)
	int            _per_spl     = 0;

	// Current position within the frame, in samples.
	// 0 = beginning of a frame or end of the previous one.
	int            _pos_spl     = 0;

	// Length of the block in samples
	int            _blk_len     = 0;

	// Position within the block in samples
	int            _blk_pos     = 0;

	// Segment length in samples after a call to start () or iterate ().
	// > 0, or 0 if there are no segment left.
	int            _seg_len     = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SegmentHelper &other) const = delete;
	bool           operator != (const SegmentHelper &other) const = delete;

}; // class SegmentHelper



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/SegmentHelper.hpp"



#endif   // mfx_dsp_ana_r128_SegmentHelper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
