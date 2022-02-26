/*****************************************************************************

        MeanSq.h
        Author: Laurent de Soras, 2022

Performs a mean square analysis of a sample stream, using a rectangular
window, for a single channel.

Reference:
ITU-R BS.1770-4, formula (1) p. 5.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_MeanSq_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_MeanSq_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/fir/MovingSum.h"



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class MeanSq
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_win_len (double t);

	float          process_sample (float x) noexcept;
	void           analyse_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           analyse_block (const float src_ptr [], int nbr_spl) noexcept;
	float          get_val () const noexcept;

	void           clear_buffers () noexcept;

	static float   conv_mean_sq_to_lufs (float msq) noexcept;
	static float   conv_lufs_to_mean_sq (float lufs) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline double  push_sample_and_get_sum (float x) noexcept;
	inline float   compute_mean_from_sum (double sum) const noexcept;

	double         _sample_freq = 0; // Hz, > 0. 0 = not set
	double         _win_len     = 0; // s, > 0. 0 = not set
	double         _scale       = 0; // Multiplier to get the average. Equiv to Hz, > 0. 0 = not set

	fir::MovingSum <float, double>
	               _avg;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MeanSq &other) const = delete;
	bool           operator != (const MeanSq &other) const = delete;

}; // class MeanSq



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/MeanSq.hpp"



#endif   // mfx_dsp_ana_r128_MeanSq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
