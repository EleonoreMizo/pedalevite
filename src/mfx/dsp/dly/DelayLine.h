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



class DelayLine
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DelayLine ()  = default;
	virtual        ~DelayLine () = default;

	void           set_interpolator (rspl::InterpolatorInterface &interp);
	const rspl::InterpolatorInterface &
	               use_interpolator () const;

	void           set_sample_freq (double sample_freq, int ovrspl_l2);
	double         get_sample_freq () const;
	int            get_ovrspl_l2 () const;

	void           set_max_delay_time (double max_time);

	bool           is_ready () const;

	void           clear_buffers ();

	// Real-time functions
	double         get_min_delay_time () const;
	double         get_max_delay_time () const;
	int            estimate_max_one_shot_proc_w_feedback (double min_delay_time) const;
	void           read_line (float dest_ptr [], int nbr_spl, double delay_beg, double delay_end, int pos_in_block);
	void           push_data (const float src_ptr [], int nbr_spl);
	void           push_sample (float src);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            compute_margin () const;

	rspl::InterpolatorInterface *          // 0: interpolator not set.
	               _interp_ptr   = 0;
	int            _imp_len      = 1;      // Impulse length, samples. > 0
	fstb::FixedPoint                       // Group delay, samples. [0 ; _imp_len - 1]
	               _group_dly    = fstb::FixedPoint (0);

	DelayLineData <float>
	               _line_data;
	int            _write_pos    = 0;
	int            _ovrspl_l2    = 0; 		// Base-2 logarithm of the oversampling. >= 0.
	double         _sample_freq  = 44100;  // Output (possibly oversampled) sample frequency, Hz, > 0.

	double         _min_dly_time = 0;      // Seconds, > 0. 0 = not initialized
	double         _max_dly_time = 0;      // Seconds, > 0. 0 = not initialized



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelayLine (const DelayLine &other)         = delete;
	DelayLine &    operator = (const DelayLine &other)        = delete;
	bool           operator == (const DelayLine &other) const = delete;
	bool           operator != (const DelayLine &other) const = delete;

}; // class DelayLine



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dly/DelayLine.hpp"



#endif   // mfx_dsp_dly_DelayLine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
