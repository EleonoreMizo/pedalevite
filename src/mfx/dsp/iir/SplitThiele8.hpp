/*****************************************************************************

        SplitThiele8.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitThiele8_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitThiele8_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	SplitThiele8::is_dirty () const
{
	return _dirty_flag;
}



// Returns { low, high }
std::array <float, 2>	SplitThiele8::process_sample_split (float x)
{
	assert (! _dirty_flag);

	const auto     res4 = compute_ap4_lp4 (x, _split_1, _split_2);
	const float    ap4  = res4 [0];
	const float    lp4  = res4 [1];

	const float    lp6  = _split_3.process_sample (lp4);
	const float    lp8  = _split_4.process_sample (lp6);

	const float    hp8  = ap4 - lp8;

	return std::array <float, 2> {{ lp8, hp8 }};
}



float	SplitThiele8::process_sample_compensate (float x)
{
	assert (! _dirty_flag);

	return compute_ap4_lp4 (x, _comp_1, _comp_2) [0];
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::array <float, 2>	SplitThiele8::compute_ap4_lp4 (float x, SvfCore <> &filt_1, SvfCore <> &filt_2)
{
	const auto     s1     = filt_1.process_sample_mm (x);
	const float    lp2    = s1 [1];
	const float    bp2    = s1 [0];

	const auto     s2     = filt_2.process_sample_mm (lp2);
	const float    lp4    = s2 [1];
	const float    lp2bp2 = s2 [0];

	const float    ap4    = x + _n1 * bp2 + _n4 * (lp4 - lp2) + _n3 * lp2bp2;

	return std::array <float, 2> {{ ap4, lp4 }};
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitThiele8_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
