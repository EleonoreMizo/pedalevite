/*****************************************************************************

        DelayChn.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly1_DelayChn_HEADER_INCLUDED)
#define mfx_pi_dly1_DelayChn_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/dly/DelayLineReader.h"
#include "mfx/dsp/iir/Biquad.h"



namespace mfx
{
namespace pi
{
namespace dly1
{



class DelayChn
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DelayChn ();
	               DelayChn (const DelayChn &other)   = default;
	               DelayChn (DelayChn &&other)        = default;

	               ~DelayChn ()                       = default;

	DelayChn &     operator = (const DelayChn &other) = default;
	DelayChn &     operator = (DelayChn &&other)      = default;

	void           set_sample_freq (double sample_freq);

	void           init (dsp::rspl::InterpolatorInterface &interp, float *buf_ptr, int buf_len);
	void           restore ();

	void           set_delay_time (double delay_time, int transition_time);
	double         get_delay_time () const;
	void           set_filter_freq (double f_lo, double f_hi);
	int            get_max_proc_len () const;
	void           process_block_read (float dst_ptr [], int nbr_spl);
	void           process_block_write (const float src_ptr [], const float fdbk_ptr [], float lvl_beg, float lvl_end, int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_filter ();
	void           update_max_proc_len ();
	void           update_tail_duration ();

	double         _sample_freq;     // Hz, >= 44100
	dsp::dly::DelayLine
	               _dly_line;
	dsp::dly::DelayLineReader
	               _dly_reader;
	dsp::iir::Biquad
	               _filter;
	double         _f_lo;            // Hz
	double         _f_hi;            // Hz
	double         _delay;           // s
	float *        _tmp_ptr;
	int            _tmp_len;         // Samples. 0 = not initialized.
	int            _max_proc_len;    // Maximum length. Depends on the delay.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayChn &other) const = delete;
	bool           operator != (const DelayChn &other) const = delete;

}; // class DelayChn



}  // namespace dly1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly1/DelayChn.hpp"



#endif   // mfx_pi_dly1_DelayChn_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
