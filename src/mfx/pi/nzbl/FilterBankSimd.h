/*****************************************************************************

        FilterBankSimd.h
        Author: Laurent de Soras, 2017

Multi-band noise gate. Monophonic. SIMD version.
Instances of this class must be aligned on 16-byte boundaries.

The signal is split into spectrum bands that can be summed with a gain of 1 on
the full spectrum and some phasing. Each band is processed individually.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_nzbl_FilterBankSimd_HEADER_INCLUDED)
#define mfx_pi_nzbl_FilterBankSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/iir/Biquad4Simd.h"
#include "mfx/pi/nzbl/Cst.h"
#include "mfx/pi/nzbl/SplitterSimd.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace nzbl
{



class FilterBankSimd
{
	static_assert (SplitterSimd::_nbr_bands == Cst::_nbr_bands, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_bands = Cst::_nbr_bands;
	static constexpr int _nbr_split = _nbr_bands - 1;

	               FilterBankSimd ();
	               FilterBankSimd (const FilterBankSimd &other) = default;
	               FilterBankSimd (FilterBankSimd &&other)      = default;

	               ~FilterBankSimd ()                           = default;

	FilterBankSimd &
	               operator = (const FilterBankSimd &other)     = default;
	FilterBankSimd &
	               operator = (FilterBankSimd &&other)          = default;

	void           reset (double sample_freq, int max_buf_len, double &latency);
	void           set_level (int band_idx, float lvl);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _dspl_rate_l2 = 6;   // Must be > 2
	static constexpr int _dspl_rate    = 1 << _dspl_rate_l2;

	void           process_band (int band_idx, int nbr_spl);

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Band
	{
	public:
		float          _lvl = 0;
		dsp::dyn::EnvFollowerRms
		               _env;
		BufAlign       _buf;       // Band content
	};
	typedef std::vector <Band, fstb::AllocAlign <Band, 16> > BandArray;

	float          _sample_freq = 0;    // Sample frequency, Hz. > 0. 0 = not set
	float          _inv_fs      = 0;    // 1 / _sample_freq. 0 = not set
	int            _max_block_size = 0;
	float          _rel_thr     = 20;   // Threshold (linear, relative to _lvl) above which the notch has no effect
	float          _ka          = 0.5f; // Filter parameters
	float          _kb          = 1.0f;
	SplitterSimd   _splitter;
	BandArray      _band_arr { BandArray (_nbr_bands) };
	BufAlign       _buf;                // Stereo content (size 2x)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FilterBankSimd &other) const = delete;
	bool           operator != (const FilterBankSimd &other) const = delete;

}; // class FilterBankSimd



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/nzbl/FilterBankSimd.hpp"



#endif   // mfx_pi_nzbl_FilterBankSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
