/*****************************************************************************

        FilterK.h
        Author: Laurent de Soras, 2022

Implements a K-weighting filter, as specified in ITU-R BS.1770-4.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_FilterK_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_FilterK_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/Biquad.h"



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class FilterK
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr float _ofs_997 = -0.691f; // See note 1 p. 6

	void           set_sample_freq (double sample_freq);

	float          process_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Sampling rate, Hz. 0 = not set (invalid state)
	double         _sample_freq = 0;

	// High-shelf, +4 dB, transition between 1 and 3 kHz
	iir::Biquad    _shelf;

	// High-pass filter, -3 dB around 60 Hz
	iir::Biquad    _hpf;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FilterK &other) const = delete;
	bool           operator != (const FilterK &other) const = delete;

}; // class FilterK



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/FilterK.hpp"



#endif   // mfx_dsp_ana_r128_FilterK_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
