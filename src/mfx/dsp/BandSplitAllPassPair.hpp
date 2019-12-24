/*****************************************************************************

        BandSplitAllPassPair.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_BandSplitAllPassPair_CODEHEADER_INCLUDED)
#define mfx_dsp_BandSplitAllPassPair_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/mix/Generic.h"

#include <cassert>



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename AP0, typename AP1, bool VFF>
BandSplitAllPassPair <AP0, AP1, VFF>::BandSplitAllPassPair ()
:	_filter_0 ()
,	_filter_1 ()
,	_filter_fix ()
,	_add_sub_ptr (0)
,	_vol_ptr (0)
{
	mix::Generic::setup ();

	_add_sub_ptr = mix::Generic::add_sub_ip_2_2;
	_vol_ptr     = mix::Generic::copy_1_1_v;
}



template <typename AP0, typename AP1, bool VFF>
typename BandSplitAllPassPair <AP0, AP1, VFF>::ApFilter0 &	BandSplitAllPassPair <AP0, AP1, VFF>::use_filter_0 ()
{
	return _filter_0;
}



template <typename AP0, typename AP1, bool VFF>
const typename BandSplitAllPassPair <AP0, AP1, VFF>::ApFilter0 &	BandSplitAllPassPair <AP0, AP1, VFF>::use_filter_0 () const
{
	return _filter_0;
}



template <typename AP0, typename AP1, bool VFF>
typename BandSplitAllPassPair <AP0, AP1, VFF>::ApFilter1 &	BandSplitAllPassPair <AP0, AP1, VFF>::use_filter_1 ()
{
	return _filter_1;
}



template <typename AP0, typename AP1, bool VFF>
const typename BandSplitAllPassPair <AP0, AP1, VFF>::ApFilter1 &	BandSplitAllPassPair <AP0, AP1, VFF>::use_filter_1 () const
{
	return _filter_1;
}



template <typename AP0, typename AP1, bool VFF>
typename BandSplitAllPassPair <AP0, AP1, VFF>::ApFilter0 &	BandSplitAllPassPair <AP0, AP1, VFF>::use_fix_filter ()
{
	return _filter_fix;
}



template <typename AP0, typename AP1, bool VFF>
const typename BandSplitAllPassPair <AP0, AP1, VFF>::ApFilter0 &	BandSplitAllPassPair <AP0, AP1, VFF>::use_fix_filter () const
{
	return _filter_fix;
}



template <typename AP0, typename AP1, bool VFF>
void	BandSplitAllPassPair <AP0, AP1, VFF>::set_add_sub_function (AddSubProto fnc_ptr)
{
	assert (fnc_ptr != 0);

	_add_sub_ptr = fnc_ptr;
}



template <typename AP0, typename AP1, bool VFF>
void	BandSplitAllPassPair <AP0, AP1, VFF>::set_vol_function (VolumeProto vol_ptr)
{
	assert (vol_ptr != 0);

	_vol_ptr = vol_ptr;
}



template <typename AP0, typename AP1, bool VFF>
void	BandSplitAllPassPair <AP0, AP1, VFF>::split_sample (float &out_1, float &out_2, float sample)
{
	if (VFF)
	{
		sample *= 0.5f;
	}

	const float    tmp_1 = _filter_0.process_sample (sample);
	const float    tmp_2 = _filter_1.process_sample (sample);

	out_1 = tmp_1 + tmp_2;
	out_2 = tmp_1 - tmp_2;
}



// Can work in-place (1 or 2)
template <typename AP0, typename AP1, bool VFF>
void	BandSplitAllPassPair <AP0, AP1, VFF>::split_block (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	if (VFF)
	{
		_vol_ptr (out_1_ptr, in_ptr, nbr_spl, 0.5f);
		in_ptr = out_1_ptr;
	}
	
	if (in_ptr == out_1_ptr)
	{
		_filter_1.process_block (out_2_ptr, out_1_ptr, nbr_spl);
		_filter_0.process_block (out_1_ptr, out_1_ptr, nbr_spl);
	}
	else // Possibly in_ptr == out_2_ptr
	{
		_filter_0.process_block (out_1_ptr, in_ptr, nbr_spl);
		_filter_1.process_block (out_2_ptr, in_ptr, nbr_spl);
	}

	_add_sub_ptr (out_1_ptr, out_2_ptr, nbr_spl);
}



template <typename AP0, typename AP1, bool VFF>
float	BandSplitAllPassPair <AP0, AP1, VFF>::compensate_sample (float sample)
{
	return (_filter_fix.process_sample (sample));
}



template <typename AP0, typename AP1, bool VFF>
void	BandSplitAllPassPair <AP0, AP1, VFF>::compensate_block (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	_filter_fix.process_block (out_ptr, in_ptr, nbr_spl);
}



template <typename AP0, typename AP1, bool VFF>
void	BandSplitAllPassPair <AP0, AP1, VFF>::clear_buffers ()
{
	_filter_0.clear_buffers ();
	_filter_1.clear_buffers ();
	_filter_fix.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_BandSplitAllPassPair_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
