/*****************************************************************************

        FreqAmGha.h
        Author: Laurent de Soras, 2019

Based on an algorithm by Amanda Ghassaei
https://www.instructables.com/id/Arduino-Frequency-Detection/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_FreqAmGha_HEADER_INCLUDED)
#define mfx_dsp_ana_FreqAmGha_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/ValSmooth.h"
#include "mfx/dsp/iir/Biquad.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace ana
{



class FreqAmGha
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_freq_bot (float f) noexcept;
	void           set_freq_top (float f) noexcept;
	void           set_smoothing (float responsiveness, float thr) noexcept;
	void           clear_buffers () noexcept;
	float          process_block (const float spl_ptr [], int nbr_spl) noexcept;
	float          process_sample (float x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _data_size = 10;
	static const int  _max_fails = 10;

	class ZeroCross
	{
	public:
		float          _timer = 0;       // Samples, may be negative
		float          _slope = 0;
	};

	void           reset () noexcept;
	void           update_lpf () noexcept;

	static float   compute_zc_time (float cur, float old) noexcept;

	float          _sample_freq = 0;    // Sampling rate, Hz, > 0. 0 = not set
	float          _inv_fs      = 0;
	ValSmooth <float, 0>                // Hz. 0 = not found (yet)
	               _freq;
	float          _freq_bot    = 20.0f;// Hz, > 0
	float          _freq_top    = 1500; // Hz, > _freq_bot
	float          _val_cur     = 0;
	float          _val_old     = 0;
	float          _time_cur    = 0;
	std::array <ZeroCross, _data_size>
	               _zc_arr;
	int            _index       = 0;
	float          _slope_max   = 0;
	float          _slope_cur   = 0;
	const float    _slope_tol   = 3 / 128.0f;
	const float    _timer_tol   = 10;
	int            _nbr_match_fail = 0;
	iir::Biquad    _lpf_in;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FreqAmGha &other) const = delete;
	bool           operator != (const FreqAmGha &other) const = delete;

}; // class FreqAmGha



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/FreqAmGha.hpp"



#endif   // mfx_dsp_ana_FreqAmGha_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
