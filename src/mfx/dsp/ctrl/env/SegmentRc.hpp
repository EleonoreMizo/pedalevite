/*****************************************************************************

        SegmentRc.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_env_SegmentRc_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_env_SegmentRc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace env
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	SegmentRc::get_val () const noexcept
{
	return _raw_val + _offset;
}



float	SegmentRc::get_final_val () const noexcept
{
	return _final_val;
}



int	SegmentRc::get_nbr_rem_spl () const noexcept
{
	return _nbr_rem_spl;
}



float	SegmentRc::process_sample () noexcept
{
	const float    val = get_val ();

	if (_nbr_rem_spl > 0)
	{
		_raw_val *= _mult;
		-- _nbr_rem_spl;
	}

	return val;
}



bool	SegmentRc::is_finished () const noexcept
{
	return (fabsf (_raw_val) <= _end_thr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace env
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_env_SegmentRc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
