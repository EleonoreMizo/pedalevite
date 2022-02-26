/*****************************************************************************

        PeakDetectStd.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_PeakDetectStd_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_PeakDetectStd_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/Upsampler.h"



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class PeakDetectStd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	float          process_sample (float x) noexcept;
	float          process_block (const float src_ptr [], int nbr_spl) noexcept;
	float          get_peak () const noexcept;
	void           clear_peak () noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _ovrspl = 4;

	static inline float
	               compute_peak4 (float peak_lvl, const float data_ptr [4]);

	Upsampler      _upspl;
	float          _peak_lvl = 0; // Linear, > 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PeakDetectStd &other) const = delete;
	bool           operator != (const PeakDetectStd &other) const = delete;

}; // class PeakDetectStd



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/PeakDetectStd.hpp"



#endif   // mfx_dsp_ana_r128_PeakDetectStd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
