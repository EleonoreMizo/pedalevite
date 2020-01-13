/*****************************************************************************

        DelayLineReader.h
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
#if ! defined (mfx_dsp_dly_DelayLineReader_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineReader_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dly
{



class DelayLineReadInterface;

class DelayLineReader
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_tmp_buf (float *buf_ptr, int len);
	float *        get_tmp_buf_ptr () const;
	int            get_tmp_buf_len () const;

	void           set_delay_line (const DelayLineReadInterface &delay_line);

	void           set_resampling_range (double rate_inf, double rate_sup);
	void           set_crossfade (int nbr_spl, const float shape_ptr []);

	bool           is_ready () const;

	void           set_delay_time (double delay_time, int transition_time);
	void           clip_times ();
	void           read_data (float dest_ptr [], int nbr_spl, int src_pos);
	bool           is_time_ramping () const;
	bool           is_time_change_programmed () const;
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           setup_immediate_transition (double delay_time, int transition_time);
	void           apply_crossfade (float dest_ptr [], int nbr_spl, double lerp_pos_end, int src_pos);

	const DelayLineReadInterface *
	               _delay_line_ptr  =  nullptr; // 0 = not initialised.

	const float *  _xfade_shape_ptr =  nullptr; // 0 = not set.

	float *        _tmp_buf_ptr     =  nullptr; // 0: not set.
	int            _tmp_buf_len     =  0;  // samples, > 0

	double         _rate_inf   = -2;    // Out of the rate range, we crossfade instead
	double         _rate_sup   = +2;    // of resampling data.
	double         _time_cur   =  0;    // Current delay time (steady state and time ramp)
	double         _time_beg   =  0;    // Delay time of the ramp beginning, or delay time for the fading out part. s.
	double         _time_end   =  0;    // Target delay time, or delay time for the fading in part. s.
	double         _time_prog  =  0;    // Programmed delay time, if the user set a new delay time during a transition. s.
	int            _trans_dur  =  0;    // Transition duration between two delay values (samples).
	int            _trans_pos  = -1;    // Position within the transition (samples). Negative = no transition.
	int            _trans_prog = -1;    // Number of samples before the programmed new delay time (sample). Negative = no programmed time change.
	int            _xfade_dur  = 64;    // Crossfade duration, samples
	bool           _xfade_flag = false; // Current transition is a crossfade.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayLineReader &other) const = delete;
	bool           operator != (const DelayLineReader &other) const = delete;

}; // class DelayLineReader



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dly/DelayLineReader.hpp"



#endif   // mfx_dsp_dly_DelayLineReader_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
