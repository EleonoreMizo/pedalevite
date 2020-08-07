/*****************************************************************************

        DelayLineBbdPitch.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_DelayLineBbdPitch_HEADER_INCLUDED)
#define mfx_pi_dly2_DelayLineBbdPitch_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/dly/BbdLine.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/dsp/wnd/XFadeShape.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Eq.h"
#include "mfx/pi/dly2/FilterType.h"
#include "mfx/pi/dly2/FxSection.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace dly2
{



class DelayLineBbdPitch
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Buf
	{
		Buf_TMP_0 = 0,
		Buf_TMP_1,

		Buf_NBR_ELT
	};

	               DelayLineBbdPitch ();

	void           init (int line_index, const ParamDescSet &desc_set, ParamStateSet &state_set);
	void           reset (double sample_freq, int max_block_size, float buf_zone_ptr [], int buf_len);
	void           clear_buffers ();

	void           set_ramp_time (int ramp_time);

	void           set_input_gain (float g);
	void           set_delay_time (float t);
	void           set_bbd_speed (float spd);
	void           set_grain_pitch (float ratio);
	void           set_feedback (float fdbk);
	void           set_freq_lo (float f);
	void           set_freq_hi (float f);
	void           set_vol (float v);
	void           set_pan (float p);
	void           set_duck_sensitivity (float s);
	void           set_duck_amount (float amt);

	int            start_and_compute_max_proc_len ();
	void           read_line (float dst_ptr [], int nbr_spl, float &f_beg, float &f_end);
	void           finish_processing (float * const out_ptr_arr [2], const float fx_ptr [], const float src_ptr [], const float fdbk_ptr [], const float duck_ptr [], int pos_out, bool stereo_flag, bool mix_flag);
	bool           is_time_change_programmed () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::mix::Simd <
		fstb::DataAlign <false>,
		fstb::DataAlign <false>
	>              MixUnalign;

	class Grain
	{
	public:
		bool           is_ramping () const;
		int            clip_block_len (int len) const;

		float          _dly_cur   =  0;  // Current delay time in seconds. Can be out of bound.
		float          _dly_tgt   =  0;  // Target delay time in seconds, for transitions. Can be out of bound.
		float          _dly_stp   =  0;  // Time step, for the transition. s/sample. Should keep its value after a transition, if we want to extend it.
		int            _trans_pos = -1;  // Transition position. Samples, in [0 ; _trans_len-1]. Negative = no transition set.
		int            _trans_len =  0;  // Transition duration. Samples, > 0 or 0 if no transition set.
	};
	typedef std::array <Grain, 2> GrainArray;

	float *        use_buf (Buf buf) const;
	float          find_min_delay_time () const;

	void           check_and_start_transition ();
	bool           process_grain (Grain &g, float dest_ptr [], int src_pos, int nbr_spl);
	int            get_xfade_len () const;
	const float *  use_xfade_shape () const;

	float          _sample_freq;        // Hz, > 0. 0 = not initialised
	float          _inv_fs;             // 1 / _sample_freq, > 0. 0 = not initialised
	float *        _buf_zone_ptr;       // 0 = not set
	int            _buf_len;

	mfx::dsp::rspl::InterpolatorHermite43
	               _interp;
	dsp::dly::BbdLine <Cst::_min_bbd_spd_l2>
	               _bbd;
	FxSection      _fx;
	Eq             _eq;

	int            _ramp_time;          // Samples, > 0. 0 = not set
	dsp::ctrl::Ramp                     // Samples
	               _input_gain;
	float          _bbd_speed;
	float          _pitch_oct;
	dsp::ctrl::Ramp
	               _fdbk;
	dsp::ctrl::Ramp
	               _pan;
	dsp::ctrl::Ramp
	               _vol;
	dsp::ctrl::Ramp
	               _duck_amt;
	float          _duck_sens;          // As a reference level
	float          _duck_sens_inv;

	int            _block_len;
	dsp::ctrl::Ramp                     // Samples
	               _delay_time;
	float          _dly_min;            // Lower and upper bounds for the delay time (depending on the frame), only for the BBD. samples, cached. Negative = not set
	float          _dly_max;
	float          _dly_min_assumed;    // Minimum delay for the frame, guessed but not exact. Clips the actual delay to keep the BBD happy.

	dsp::wnd::XFadeShape
	               _xfade_shape_sn;
	dsp::wnd::XFadeShape
	               _xfade_shape_sp;

	GrainArray     _grain_arr;

	float          _rate_grain;         // Reading speed for the pitch shifting (2 = octave up)
	float          _rate_inf;           // When the actual reading rate is out of range,
	float          _rate_sup;           // we crossfade grains instead of resampling data.
	float          _time_cur;           // Current delay time (steady state and time ramp). s.
	float          _prog_time;          // Programmed delay time, if the user set a new delay time during a transition. s.
	int            _prog_trans;         // Number of samples before the programmed new delay time (sample). Negative = no programmed time change.
	int            _xfade_dn;           // Crossfade duration, samples (normal)
	int            _xfade_dp;           // Crossfade duration, samples (pitch shift)
	int            _xfade_pos;          // Crossfading position, samples. Negative: no crossfading
	int            _grain_cur;          // Current grain index (or fading in).
	bool           _ps_flag;            // Indicates if the _rate_grain value requires pitch shifting (!= 1)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelayLineBbdPitch (const DelayLineBbdPitch &other)      = delete;
	DelayLineBbdPitch & operator = (const DelayLineBbdPitch &other)        = delete;
	bool           operator == (const DelayLineBbdPitch &other) const = delete;
	bool           operator != (const DelayLineBbdPitch &other) const = delete;

}; // class DelayLineBbdPitch



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/DelayLineBbdPitch.hpp"



#endif   // mfx_pi_dly2_DelayLineBbdPitch_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
