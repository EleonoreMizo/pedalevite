/*****************************************************************************

        HardclipBl.h
        Author: Laurent de Soras, 2020

Band-limited hard-clipping.
Latency: 3 samples

When the slope of the input signal is > L units/spl in absolute value (L being
the clipping threshold), the filters starts to generate some broadband noise
and becomes ineffective. The algorithm has been modified to fix this issue,
but the fix is of limited efficiency. This is most likely a combination of
approximation errors, too few BLAMP taps and lack of C2 (and above) fix.

Therefore when the signal amplitude is much higher than the clipping
threshold, it is recommended to oversample at least by a factor 4.

Base algorithm from:
Fabian Esqueda, Vesa Valimaki, Stefan Bilbao,
Rounding Corners With BLAMP,
Proceedings of the 19th International Conference on Digital Audio Effects (DAFx)
September 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_HardclipBl_HEADER_INCLUDED)
#define mfx_dsp_shape_HardclipBl_HEADER_INCLUDED



// 0 = original algorithm
// 1 = modified algorithm. Slower but higher quality when |slope| > _lvl
#define mfx_dsp_shape_HardclipBl_ALGO 1



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>
#include <utility>



namespace mfx
{
namespace dsp
{
namespace shape
{



class HardclipBl
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_clip_lvl (float lvl) noexcept;
	float          process_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if mfx_dsp_shape_HardclipBl_ALGO == 0

	static const int  _max_nbr_it = 100; // Maximum number of N-R iterations

#else

	static const int  _buf_len    = 4; // Power of 2, >= 3
	static const int  _buf_mask   = _buf_len - 1;

	class Crossing
	{
	public:
		float          _xrel;   // In [0 ; 1[
		float          _y;      // Crossing point
		float          _yd;     // Slope
	};

	class CrossingList
	{
	public:
		// Worst case senario: 2 crossings per boundary
		std::array <Crossing, 4>
		               _arr;
		int            _nbr_cx = 0;
	};

	void           find_crossings (CrossingList &cxm, CrossingList &cxp, float xm1, float x0, float xp1) const noexcept;

#endif

	float          _lvl = 1;   // Clipping level, > 0

	// States
	std::array <float, 4>      // Input memory
	               _x_arr = { 0, 0, 0, 0 };
	std::array <float, 4>      // Output memory
	               _y_arr = { 0, 0, 0, 0 };

#if mfx_dsp_shape_HardclipBl_ALGO == 0

	bool           _req_clip_flag = false;
	std::array <bool, 2>
	               _clip_flag_arr = {{ false, false }};
#else

	std::array <CrossingList, _buf_len>
	               _buf_cxl;
	int            _buf_pos = 0;

#endif



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const HardclipBl &other) const = delete;
	bool           operator != (const HardclipBl &other) const = delete;

}; // class HardclipBl



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/HardclipBl.hpp"



#endif   // mfx_dsp_shape_HardclipBl_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

