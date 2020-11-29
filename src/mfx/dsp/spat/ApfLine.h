/*****************************************************************************

        ApfLine.h
        Author: Laurent de Soras, 2020

Multiple all-pass filter delay chain, building block for a reverberator.
Contains:
- Several all-pass delays
– A simple delay without feedback
- A 2-pole state-variable filter

Template parameters:

- NAPD: number of allpass delays. > 0

- DF: indicates if the standard delay is used.

- FF: indicates if the filter is used.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_ApfLine_HEADER_INCLUDED)
#define mfx_dsp_spat_ApfLine_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/iir/SvfCore.h"
#include "mfx/dsp/spat/DelayAllPass.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace spat
{



template <int NAPD, bool DF, bool FF>
class ApfLine
{
	static_assert (NAPD > 0, "Number of all-pass delays should be > 0");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_apd      = NAPD;

	// Maximum processing block size
	static constexpr int _max_blk_size = 64;

	typedef DelayAllPass <float, 6> ApdType;

	void           set_delay_max (int len);
	void           set_apd_max_delay (int idx, int len);

	fstb_FORCEINLINE void
	               set_delay (int len);

	fstb_FORCEINLINE void
	               set_apd_delay_flt (int idx, float len_spl);
	fstb_FORCEINLINE void
	               set_apd_coef (int idx, float coef);

	fstb_FORCEINLINE void
	               set_filter_coefs (float g0, float g1, float g2);
	fstb_FORCEINLINE void
	               set_filter_mix (float v0m, float v1m, float v2m);

	fstb_FORCEINLINE float
	               process_sample (float x);
	fstb_FORCEINLINE float
	               read_apd (int idx, int delay) const;
	fstb_FORCEINLINE float
	               read_delay (int delay) const;

	fstb_FORCEINLINE int
	               compute_max_block_len () const;
	inline void    process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	inline void    read_apd_block (float dst_ptr [], int idx, int delay, int nbr_spl) const;
	inline void    read_delay_block (float dst_ptr [], int delay, int nbr_spl) const;

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <ApdType, _nbr_apd> ApdArray;

	ApdArray       _apd_arr;
	dly::DelaySimple
	               _delay;
	iir::SvfCore<> _filter;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ApfLine &other) const = delete;
	bool           operator != (const ApfLine &other) const = delete;

}; // class ApfLine



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/ApfLine.hpp"



#endif   // mfx_dsp_spat_ApfLine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
