/*****************************************************************************

        StageTaps.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_StageTaps_HEADER_INCLUDED)
#define mfx_pi_dly2_StageTaps_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/dly/DelayLineReaderPitch.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/dsp/wnd/XFadeEqAmpPoly3.h"
#include "mfx/dsp/wnd/XFadeShape.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Eq.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace dly2
{



class StageTaps
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_chn = 2;

	enum Buf
	{
		Buf_RAW_L = 0,
		Buf_RAW_R,
		Buf_OUT_L,
		Buf_OUT_R,
		Buf_READER,

		Buf_NBR_ELT
	};


	               StageTaps ();
	               StageTaps (const StageTaps &other)  = default;
	               StageTaps (StageTaps &&other)       = default;

	               ~StageTaps ()                       = default;

	StageTaps &    operator = (const StageTaps &other) = default;
	StageTaps &    operator = (StageTaps &&other)      = default;

	void           reset (double sample_freq, int max_block_size, float tmp_ptr [], int buf_size);
	void           clear_buffers ();

	void           set_ramp_time (int nbr_spl);

	void           set_level_tap_input (float lvl);
	void           set_level_predelay (float lvl);
	void           set_level_dry (float lvl);

	// For all taps including the "zero"
	void           set_tap_pan (int index, float pan);
	void           set_tap_vol (int index, float vol);
	void           set_tap_gain (int index, float gain);
	void           set_tap_spread (int index, float spread);

	// For delayed taps only
	void           set_tap_delay_time (int index, float dly);
	void           set_tap_pitch_rate (int index, float rate);
	void           set_tap_freq_lo (int index, float f);
	void           set_tap_freq_hi (int index, float f);

	void           process_block (float * const line_ptr_arr [Cst::_nbr_lines], float * const dst_ptr_arr [_nbr_chn], const float * const src_ptr_arr [_nbr_chn], int nbr_spl, int nbr_chn_src, int nbr_chn_dst);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Tap
	{
	public:
		class Channel
		{
		public:
			Eq             _eq;
			dsp::dly::DelayLineReaderPitch <float>
			               _reader;
		};

		dsp::ctrl::Ramp
		               _pan    = dsp::ctrl::Ramp (0);
		dsp::ctrl::Ramp                  // For the mix output
		               _vol    = dsp::ctrl::Ramp (1);
		dsp::ctrl::Ramp                  // For feeding the lines
		               _gain   = dsp::ctrl::Ramp (1);
		dsp::ctrl::Ramp
		               _spread = dsp::ctrl::Ramp (0);
		dsp::ctrl::Ramp                  // In seconds
		               _delay_time = dsp::ctrl::Ramp (0);
		std::array <Channel, _nbr_chn>
		               _chn_arr;
	};
	typedef std::array <Tap, Cst::_nbr_taps + 1> TapArray;

	inline float * use_buf (Buf index);
	void           mix_tap (float dst_0_ptr [], float dst_1_ptr [], const float src_0_ptr [], const float src_1_ptr [], int nbr_spl, const dsp::ctrl::Ramp &vol_1, const dsp::ctrl::Ramp &vol_2, const dsp::ctrl::Ramp &pan, bool mix_flag, bool mono_out_flag);

	static void    vol_pan_to_lvl (dsp::StereoLevel &sl, float pan, float vol);

	float          _sample_freq;
	int            _max_block_size;
	int            _ramp_time;
	float *        _tmp_buf_ptr;
	int            _buf_size;
	dsp::ctrl::Ramp                     // Before entering the delay, for delayed taps only
	               _level_tap_input;
	dsp::ctrl::Ramp                     // Global delayed tap output volume
	               _level_predelay;
	dsp::ctrl::Ramp                     // Global dry volume
	               _level_dry;
	TapArray       _tap_arr;            // [Cst::_nbr_taps] -> no delay
	std::array <dsp::dly::DelayLine, _nbr_chn>
	               _delay_arr;
	mfx::dsp::rspl::InterpolatorHermite43
	               _interp;             // Common for all reading heads because stateless
	dsp::wnd::XFadeShape <dsp::wnd::XFadeEqAmpPoly3>
	               _xfade_shape_normal;
	dsp::wnd::XFadeShape <dsp::wnd::XFadeEqAmpPoly3>
	               _xfade_shape_pitchshift;
	float          _min_dly_time;       // s, > 0. 0 = not initialized

	static const dsp::ctrl::Ramp _ramp_one;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const StageTaps &other) const = delete;
	bool           operator != (const StageTaps &other) const = delete;

}; // class StageTaps



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/StageTaps.hpp"



#endif   // mfx_pi_dly2_StageTaps_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
