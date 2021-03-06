/*****************************************************************************

        FilterBankSimd.h
        Author: Laurent de Soras, 2017

Multi-band noise gate. Monophonic. SIMD version.

The signal is split into spectrum bands that can be summed with a gain of 1 on
the full spectrum and some phasing. Each band is processed individually.

The splitter works with pairs of low-pass and high-pass filters that can be
summed with these conditions (ex: Linkwitz-Riley). To keep everything in
phase, all bands are filtered at all boundary frequencies, by the low- or
high-pass filters depending on the band position, according to the flowgraph
described by Christofer Bustad on the Music-DSP mailing list:
http://music.columbia.edu/pipermail/music-dsp/2004-March/059520.html

In -+-->HP1----+--->HP2----+--> ... ---+--->HP6----+--->HP7--.
    |          |           |           |           |         |
    v          v           v           v           v         |
   LP1        LP2         LP3         LP6         LP7        |
    |          |           |           |           |         |
    v          v           v           v           v         v
    P1         P2          P3          P6          P7        P8
    |          |           |           |           |         |
    |          v           v           v           v         v
    `-->LP2-->(+)-->LP3-->(+)-> ... ->(+)-->LP7-->(+)------>(+)-> Out

HPi/LPi are the filters at the i-th frequency between band i and i + 1.
Pi is the processing for the i-th band.

Because the bands are filtered multiple times, the actual filter slopes
become steeper accross the bands. Therefore the need for an asymptotically
steep slope is much reduced for the core filters. It's possible to focus on
the direct neighbourhood of the cutoff frequencies despite lower asymptotical
performances. We use here 4th order Thiele filters (modified L-R filters) with
a coefficient of 0.65. With such a setting, their single performances are
poorly balanced but these filters become interesting when grouped in the bank.

Ref:
Neville Thiele, Loudspeaker Crossovers with Notched Response,
Journal of the Audio Engineering Society, vol. 48, no. 9, pp. 786-799,
2000-09

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
#include "fstb/DataAlign.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/iir/Biquad4Simd.h"
#include "mfx/pi/nzbl/Cst.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace nzbl
{



class FilterBankSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_bands = Cst::_nbr_bands;
	static constexpr int _nbr_split = _nbr_bands - 1;

	               FilterBankSimd ();
	               FilterBankSimd (const FilterBankSimd &other) = default;
	               FilterBankSimd (FilterBankSimd &&other)      = default;

	               ~FilterBankSimd ()                           = default;

	FilterBankSimd &   operator = (const FilterBankSimd &other) = default;
	FilterBankSimd &   operator = (FilterBankSimd &&other)      = default;

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

	static constexpr float
	               compute_split_freq (int split_idx);

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	typedef dsp::iir::Biquad4Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> Biq4;

	class Split
	{
	public:
		Biq4           _main;      // 2x2 processing: left = LP, right = HP
		Biq4           _fix;       // 1x2 serial processing
	};
	typedef std::vector <Split, fstb::AllocAlign <Split, 16> > SplitArray;

	class Band
	{
	public:
		float          _lvl = 0;
		dsp::dyn::EnvFollowerRms
		               _env;
		BufAlign       _buf;       // Band content
	};
	typedef std::vector <Band, fstb::AllocAlign <Band, 16> > BandArray;

	float          _sample_freq;  // Sample frequency, Hz. > 0. 0 = not set
	float          _inv_fs;       // 1 / _sample_freq. 0 = not set
	int            _max_block_size;
	float          _rel_thr;      // Threshold (relative to _lvl) above which the notch has no effect
	SplitArray     _split_arr;
	BandArray      _band_arr;
	BufAlign       _buf;          // Stereo content (size 2x)



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
