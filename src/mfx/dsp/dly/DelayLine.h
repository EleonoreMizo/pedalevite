/*****************************************************************************

        DelayLine.h
        Author: Laurent de Soras, 2016

Requires mfx::dsm::mix::Generic to be initialised.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelayLine_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLine_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/FixedPoint.h"
#include "mfx/dsp/dly/DelayLineData.h"
#include "mfx/dsp/dly/DelayLineReadInterface.h"



namespace mfx
{
namespace dsp
{

namespace rspl
{
	class InterpolatorInterface;
}

namespace dly
{



class DelayLine final
:	public DelayLineReadInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_interpolator (rspl::InterpolatorInterface &interp);
	const rspl::InterpolatorInterface &
	               use_interpolator () const noexcept;

	void           set_sample_freq (double sample_freq, int ovrspl_l2);
	void           set_max_delay_time (double max_time);
	bool           is_ready () const noexcept;
	void           clear_buffers () noexcept;

	// Real-time functions
	void           push_block (const float src_ptr [], int nbr_spl) noexcept;
	void           push_sample (float src) noexcept;
	void           move_write_head (int offset) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// DelayLineReadInterface
	double         do_get_sample_freq () const noexcept final;
	int            do_get_ovrspl_l2 () const noexcept final;
	double         do_get_min_delay_time () const noexcept final;
	double         do_get_max_delay_time () const noexcept final;
	int            do_estimate_max_one_shot_proc_w_feedback (double min_delay_time) const noexcept final;
	void           do_read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const noexcept final;
	float          do_read_sample (float delay) const noexcept final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	rspl::InterpolatorInterface *          // 0: interpolator not set.
	               _interp_ptr   = nullptr;
	int            _imp_len      = 1;      // Impulse length, samples. > 0
	fstb::FixedPoint                       // Group delay, samples. [0 ; _imp_len - 1]
	               _group_dly    = fstb::FixedPoint (0, 0);

	DelayLineData <float>
	               _line_data;
	int            _write_pos    = 0;
	int            _ovrspl_l2    = 0; 		// Base-2 logarithm of the oversampling. >= 0.
	double         _sample_freq  = 44100;  // Output (possibly oversampled) sample frequency, Hz, > 0.

	double         _min_dly_time = 0;      // Seconds, > 0. 0 = not initialized
	double         _max_dly_time = 0;      // Seconds, > 0. 0 = not initialized



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayLine &other) const = delete;
	bool           operator != (const DelayLine &other) const = delete;

}; // class DelayLine



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dly/DelayLine.hpp"



#endif   // mfx_dsp_dly_DelayLine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
