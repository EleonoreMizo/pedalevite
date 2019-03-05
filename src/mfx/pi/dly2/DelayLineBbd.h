/*****************************************************************************

        DelayLineBbd.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_DelayLineBbd_HEADER_INCLUDED)
#define mfx_pi_dly2_DelayLineBbd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/dly/BbdLine.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/pi/dly2/Eq.h"
#include "mfx/pi/dly2/FilterType.h"
#include "mfx/pi/dly2/FxSection.h"
#include "mfx/dsp/wnd/XFadeShape.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace dly2
{



class DelayLineBbd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Buf
	{
		Buf_TMP_0 = 0,
		Buf_TMP_1,

		Buf_NBR_ELT
	};

	               DelayLineBbd ();
	virtual        ~DelayLineBbd () = default;

	void           init (int line_index, const ParamDescSet &desc_set, ParamStateSet &state_set);
	void           reset (double sample_freq, int max_block_size, float buf_zone_ptr [], int buf_len);
	void           clear_buffers ();

	void           set_ramp_time (int ramp_time);

	void           set_input_gain (float g);
	void           set_delay_time (float t);
	void           set_bbd_speed (float spd);
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::mix::Simd <
		fstb::DataAlign <false>,
		fstb::DataAlign <false>
	>              MixUnalign;

	float *        use_buf (Buf buf) const;
	float          find_min_delay_time () const;

	int            _line_index;

	float          _sample_freq;        // Hz, > 0. 0 = not initialised
	float          _inv_fs;             // 1 / _sample_freq, > 0. 0 = not initialised
	float *        _buf_zone_ptr;       // 0 = not set
	int            _buf_len;

	mfx::dsp::rspl::InterpolatorHermite43
	               _interp;
	dsp::dly::BbdLine
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
	float          _delay_time_lb;      // Lower bound for the delay time (depending on the frame)
	int            _xfade_len;
	int            _xfade_pos;          // > 0: cross-fading between _xfade_dly_old et _xfade_dly_new
	float          _xfade_dly_old;      // Samples

	dsp::wnd::XFadeShape
	               _xfade_shape;

	static const float
	               _speed_limit;        // Absolute reading speed threshold for resampling/crossfading



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelayLineBbd (const DelayLineBbd &other)      = delete;
	DelayLineBbd & operator = (const DelayLineBbd &other)        = delete;
	bool           operator == (const DelayLineBbd &other) const = delete;
	bool           operator != (const DelayLineBbd &other) const = delete;

}; // class DelayLineBbd



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/DelayLineBbd.hpp"



#endif   // mfx_pi_dly2_DelayLineBbd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
