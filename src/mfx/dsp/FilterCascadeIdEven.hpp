/*****************************************************************************

        FilterCascadeIdEven.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_FilterCascadeIdEven_CODEHEADER_INCLUDED)
#define mfx_dsp_FilterCascadeIdEven_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename FLT, int NBR>
FLT &	FilterCascadeIdEven <FLT, NBR>::use_filter (int unit)
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	return _filter_arr [unit];
}



template <typename FLT, int NBR>
const FLT &	FilterCascadeIdEven <FLT, NBR>::use_filter (int unit) const
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	return _filter_arr [unit];
}



template <typename FLT, int NBR>
float	FilterCascadeIdEven <FLT, NBR>::process_sample (float x)
{
	int				pos = 0;
	do
	{
		x = _filter_arr [pos].process_sample (x);
		++ pos;
	}
	while (pos < _nbr_units);

	return (x);
}



template <typename FLT, int NBR>
void	FilterCascadeIdEven <FLT, NBR>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	_filter_arr [0].process_block (dst_ptr, src_ptr, nbr_spl);
	for (int unit = 1; unit < _nbr_units; ++unit)
	{
		_filter_arr [unit].process_block (dst_ptr, dst_ptr, nbr_spl);
	}
}



template <typename FLT, int NBR>
void	FilterCascadeIdEven <FLT, NBR>::clear_buffers ()
{
	for (int unit = 0; unit < _nbr_units; ++unit)
	{
		_filter_arr [unit].clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_FilterCascadeIdEven_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
