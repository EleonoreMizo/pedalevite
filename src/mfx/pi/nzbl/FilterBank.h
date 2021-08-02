/*****************************************************************************

        FilterBank.h
        Author: Laurent de Soras, 2017

Multi-band noise gate. Monophonic.

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
#include "mfx/dsp/iir/SplitMultiband.h"
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
	static constexpr int _max_blk_size = 64; // Samples
	static constexpr int _order_ap0    = 2;
	static constexpr int _order_ap1    = 3;

	void           process_band (int band_idx, int nbr_spl, int sub_block_len);

	typedef std::array <float, _max_blk_size> Buf;

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
	float          _ka          = 0.5f; // Filter parameters
	float          _kb          = 1.0f;
	dsp::iir::SplitMultiband <float, _order_ap0, _order_ap1>
	               _splitter;
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
