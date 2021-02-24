/*****************************************************************************

        FilterBank.h
        Author: Laurent de Soras, 2017

Multi-band noise gate. Monophonic.

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
#if ! defined (mfx_pi_nzbl_FilterBank_HEADER_INCLUDED)
#define mfx_pi_nzbl_FilterBank_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/nzbl/Cst.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace nzbl
{



class FilterBank
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_bands = Cst::_nbr_bands;
	static const int  _nbr_split = _nbr_bands - 1;

	void           reset (double sample_freq, int max_buf_len, double &latency);
	void           set_threshold (int band_idx, float thr);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _dspl_rate_l2 = 6;   // Must be > 0
	static constexpr int _dspl_rate    = 1 << _dspl_rate_l2;
	static constexpr int _nbr_stages   = 2;
	static constexpr int _max_blk_size = 64; // Samples

	void           process_band (int band_idx, int nbr_spl, int sub_block_len);

	static constexpr float
	               compute_split_freq (int split_idx);

	typedef std::array <float, _max_blk_size> Buf;

	typedef std::array <mfx::dsp::iir::Biquad, _nbr_stages> Biq2;

	class Split
	{
	public:
		Biq2           _lpf;
		Biq2           _hpf;
		Biq2           _fix;
	};
	typedef std::array <Split, _nbr_bands - 1> SplitArray;

	class Band
	{
	public:
		float          _thr   = 0; // >=0. 0 = inactive band.
		float          _g_old = 0; // Gain at the end of the previous block
		mfx::dsp::dyn::EnvFollowerRms
		               _env;
		Buf            _buf;       // Band content
	};
	typedef std::array <Band, _nbr_bands> BandArray;

	float          _sample_freq = 0; // Sample frequency, Hz. > 0. 0 = not set
	float          _inv_fs      = 0; // 1 / _sample_freq. 0 = not set
	float          _thr_hi_rel  = 20; // Threshold (relative to _thr) above which the notch has no effect
	float          _mul_thr_hi  = 1.0f / (_thr_hi_rel - 1); // Precomputed stuff
	SplitArray     _split_arr;
	BandArray      _band_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FilterBank &other) const = delete;
	bool           operator != (const FilterBank &other) const = delete;

}; // class FilterBank



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/nzbl/FilterBank.hpp"



#endif   // mfx_pi_nzbl_FilterBank_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
