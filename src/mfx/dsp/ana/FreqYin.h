/*****************************************************************************

        FreqYin.h
        Author: Laurent de Soras, 2016

Based on:
Alain de Chevigne, Hideki Kawahara
YIN, a fundamental frequency estimator for speech and music,
Acoustical Society of America, 2002
Implemented up to step 5.

Note: this implementation has significant latency in the detection.
The frequencies are tested one after each other, one per input sample, so
it may take one full cycle (depending on freq_bot) to get the frequency
update.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_FreqYin_HEADER_INCLUDED)
#define mfx_dsp_ana_FreqYin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/ValSmooth.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace ana
{



class FreqYin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_freq_bot (float f);
	void           set_freq_top (float f);
	void           set_smoothing (float responsiveness, float thr);
	void           clear_buffers ();
	float          process_block (const float spl_ptr [], int nbr_spl);
	float          process_sample (float x);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Delta
	{
	public:
		float          _sum_u = 0; // Difference function = _sum_u + _sum_d
		float          _sum_d = 0;
		float          _cmndf = 0;
	};

	void           update_freq_bot_param ();
	void           update_freq_top_param ();
	void           update_difference_functions ();
	void           analyse ();

	const float    _min_freq    = 20.0f; // Hz
	const float    _threshold   = 0.1f; // From step 4: absolute theshold

	std::vector <float>
	               _buffer;
	int            _buf_pos_w   = 0;    // Writing position within the buffer
	int            _buf_mask    = 0;
	float          _sample_freq = 0;    // Hz. 0 = not set
	int            _win_len     = 0;    // Samples
	int            _min_delta   = 0;    // >= 2
	int            _ana_per     = 64;   // Period between two analysis, in samples. >= 1
	int            _ana_pos     = 0;    // Position within the analysis period, [0 ; _ana_per[
	int            _sum_pos     = 0;    // Counter for the cumulated sum. Reset when reaching _win_len

	float          _freq_bot    = _min_freq; // Hz, > 0
	float          _freq_top    = 1000; // Hz, > _freq_bot
	ValSmooth <float, 0>                // Value in Hz. 0 = not found (yet)
	               _freq_smooth;
	std::vector <Delta>
	               _delta_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FreqYin &other) const = delete;
	bool           operator != (const FreqYin &other) const = delete;

}; // class FreqYin



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/FreqYin.hpp"



#endif   // mfx_dsp_ana_FreqYin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
