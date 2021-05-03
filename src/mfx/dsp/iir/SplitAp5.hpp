/*****************************************************************************

        SplitAp5.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitAp5_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitAp5_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	SplitAp5::is_dirty () const noexcept
{
	return _dirty_flag;
}



std::array <float, 2>	SplitAp5::process_sample_split (float x) noexcept
{
	assert (! _dirty_flag);

	const float    xh = x * 0.5f;
	const float    p0 = _ap0.process_sample (xh);
	const float    p1 = _ap2.process_sample (_ap1.process_sample (xh));

	return std::array <float, 2> {{ p0 + p1, p0 - p1 }};
}



float	SplitAp5::process_sample_compensate (float x) noexcept
{
	assert (! _dirty_flag);

	return _comp.process_sample (x);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitAp5_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
