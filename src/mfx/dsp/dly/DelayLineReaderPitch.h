/*****************************************************************************

        DelayLineReaderPitch.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelayLineReaderPitch_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineReaderPitch_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace mfx
{
namespace dsp
{
namespace dly
{



class DelayLineReadInterface;

template <typename TC>
class DelayLineReaderPitch
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_tmp_buf (float *buf_ptr, int len) noexcept;
	float *        get_tmp_buf_ptr () const noexcept;
	int            get_tmp_buf_len () const noexcept;

	void           set_delay_line (const DelayLineReadInterface &delay_line) noexcept;

	void           set_resampling_range (TC rate_inf, TC rate_sup) noexcept;
	void           set_crossfade_normal (int nbr_spl, const float shape_ptr []) noexcept;
	void           set_crossfade_pitchshift (int nbr_spl, const float shape_ptr []) noexcept;

	bool           is_ready () const noexcept;

	void           set_delay_time (TC delay_time, int transition_time) noexcept;
	void           set_grain_pitch (float ratio) noexcept;
	void           read_data (float dst_ptr [], int nbr_spl, int src_pos) noexcept;
	bool           is_time_change_programmed () const noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Grain
	{
	public:
		bool           is_ramping () const noexcept;
		int            clip_block_len (int len) const noexcept;

		TC             _dly_cur   =  0;  // Current delay time in seconds. Can be out of bound.
		TC             _dly_tgt   =  0;  // Target delay time in seconds, for transitions. Can be out of bound.
		TC             _dly_stp   =  0;  // Time step, for the transition. s/sample. Should keep its value after a transition, if we want to extend it.
		int            _trans_pos = -1;  // Transition position. Samples, in [0 ; _trans_len-1]. Negative = no transition set.
		int            _trans_len =  0;  // Transition duration. Samples, > 0 or 0 if no transition set.
	};
	typedef std::array <Grain, 2> GrainArray;

	void           check_and_start_transition () noexcept;
	bool           process_grain (Grain &g, float dest_ptr [], int src_pos, int nbr_spl) noexcept;
	int            get_xfade_len () const noexcept;
	const float *  use_xfase_shape () const noexcept;

	const DelayLineReadInterface *
	               _delay_line_ptr = nullptr; // 0 = not initialised.
	TC             _dly_min    = -1;    // Min and max delay times, seconds, cached. Negative = not set
	TC             _dly_max    = -1;


	const float *  _xfade_sn_ptr  = nullptr; // 0 = not set.
	const float *  _xfade_sp_ptr  = nullptr; // 0 = not set.

	float *        _tmp_buf_ptr   = nullptr; // 0: not set.
	int            _tmp_buf_len   = 0; // samples, > 0

	GrainArray     _grain_arr;

	TC             _rate_grain =  1;    // Reading speed for the pitch shifting (2 = octave up)
	TC             _rate_inf   = -2;    // When the actual reading rate is out of range,
	TC             _rate_sup   = +2;    // we crossfade grains instead of resampling data.
	TC             _time_cur   =  0;    // Current delay time (steady state and time ramp). s.
	TC             _prog_time  =  0;    // Programmed delay time, if the user set a new delay time during a transition. s.
	int            _prog_trans = -1;    // Number of samples before the programmed new delay time (sample). Negative = no programmed time change.
	int            _xfade_dn   = 64;    // Crossfade duration, samples (normal)
	int            _xfade_dp   = 4096;  // Crossfade duration, samples (pitch shift)
	int            _xfade_pos  = -1;    // Crossfading position, samples. Negative: no crossfading
	int            _grain_cur  =  0;    // Current grain index (or fading in).
	bool           _ps_flag    = false; // Indicates if the _rate_grain value requires pitch shifting (!= 1)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayLineReaderPitch <TC> &other) const = delete;
	bool           operator != (const DelayLineReaderPitch <TC> &other) const = delete;

}; // class DelayLineReaderPitch



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/DelayLineReaderPitch.hpp"



#endif   // mfx_dsp_dly_DelayLineReaderPitch_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
