/*****************************************************************************

        FreqFast.h
        Author: Laurent de Soras, 2018

Algorithm inspired by:
Joel de Guzman, Fast and Efficient Pitch Detection, 2017-10-02
http://www.cycfi.com/2017/10/fast-and-efficient-pitch-detection/
http://www.cycfi.com/2017/11/fast-and-efficient-pitch-detection-double-trouble/

Doesn't work very well for now. Must probably be hand-tuned. We'll see later.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_FreqFast_HEADER_INCLUDED)
#define mfx_dsp_ana_FreqFast_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/dsp/dyn/EnvFollowerPeak.h"
#include "mfx/dsp/iir/DcKiller1p.h"

#include <array>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace ana
{



class FreqFast
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_freq_bot (float f) noexcept;
	void           set_freq_top (float f) noexcept;
	void           clear_buffers () noexcept;
	float          process_block (const float spl_ptr [], int nbr_spl) noexcept;
	float          process_sample (float x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_buf        = 4;
	static const int  _max_block_size = 256;  // Samples
	static_assert (
		(_max_block_size & 3) == 0,
		"_max_block_size must be multiple of 4 because of SIMD"
	);
	const float    _lvl_gate  = 1e-4f;
	const float    _deadzone  = 0.3f;
	const float    _peak_thr  = 0.9f;
	const float    _smoothing = 0.5f;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef std::array <BufAlign, _nbr_buf> BufArray;

	void           proc_autogain (int nbr_spl) noexcept;
	void           proc_peaks (int nbr_spl) noexcept;
	void           find_freq (int nbr_spl) noexcept;
	void           process_freq (int ts_rel) noexcept;
	void           validate_period (float per) noexcept;
	void           fix_timestamps () noexcept;

	static bool    is_period_similar (float p1, float p2) noexcept;

	float          _sample_freq = 0;    // Sampling rate, Hz, > 0. 0 = not set
	float          _inv_fs      = 0;
	float          _freq        = 0;    // Hz. 0 = not found (yet)
	float          _freq_bot    = 20.0f;// Hz, > 0
	float          _freq_top    = 1500; // Hz, > _freq_bot
	bool           _pp_flag     = false;// false = negative peak detected, true = positive peak detected
	float          _last_pp     = 0;
	float          _last_pn     = 0;
	int32_t        _time_ref    = 0;    // Current timestamp. Updated after each subblock
	int32_t        _time_p1     = 0;    // Timestamp of the previous positive peak
	float          _per_prev    = 0;    // 0 = nothing saved
	float          _per_avg     = 0;    // Averaged. 0 = not set
	iir::DcKiller1p
	               _dc_killer;
	dyn::EnvFollowerPeak
	               _env_autogain;
	dyn::EnvFollowerPeak
	               _env_peak_pos;
	dyn::EnvFollowerPeak
	               _env_peak_neg;
	BufArray       _buf_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class FreqFast



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/FreqFast.hpp"



#endif   // mfx_dsp_ana_FreqFast_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
