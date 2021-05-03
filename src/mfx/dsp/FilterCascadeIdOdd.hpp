/*****************************************************************************

        FilterCascadeIdOdd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_FilterCascadeIdOdd_CODEHEADER_INCLUDED)
#define mfx_dsp_FilterCascadeIdOdd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename F1P, typename F2P, int NBR>
F1P &	FilterCascadeIdOdd <F1P, F2P, NBR>::use_filter () noexcept
{
	return _odd;
}



template <typename F1P, typename F2P, int NBR>
const F1P &	FilterCascadeIdOdd <F1P, F2P, NBR>::use_filter () const noexcept
{
	return _odd;
}



template <typename F1P, typename F2P, int NBR>
F2P &	FilterCascadeIdOdd <F1P, F2P, NBR>::use_filter (int unit) noexcept
{
	return _even.use_filter (unit);
}



template <typename F1P, typename F2P, int NBR>
const F2P &	FilterCascadeIdOdd <F1P, F2P, NBR>::use_filter (int unit) const noexcept
{
	return _even.use_filter (unit);
}



template <typename F1P, typename F2P, int NBR>
float	FilterCascadeIdOdd <F1P, F2P, NBR>::process_sample (float x) noexcept
{
	x = _even.process_sample (x);
	x = _odd.process_sample (x);

	return (x);
}



template <typename F1P, typename F2P, int NBR>
void	FilterCascadeIdOdd <F1P, F2P, NBR>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl != 0);

	_even.process_block (dst_ptr, src_ptr, nbr_spl);
	_odd.process_block (dst_ptr, dst_ptr, nbr_spl);
}



template <typename F1P, typename F2P, int NBR>
void	FilterCascadeIdOdd <F1P, F2P, NBR>::clear_buffers () noexcept
{
	_even.clear_buffers ();
	_odd.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_FilterCascadeIdOdd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
