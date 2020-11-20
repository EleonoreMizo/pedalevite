/*****************************************************************************

        FreeverbCore.cpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/spat/fv/FreeverbCore.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FreeverbCore::FreeverbCore ()
:	_sample_freq (0)
,	_chn_arr ()
{
	dsp::mix::Align::setup ();

	for (auto &chn : _chn_arr)
	{
		for (auto &ap : chn._ap_arr)
		{
			ap.set_feedback (0.5f);
		}
	}
}



void	FreeverbCore::reset (double sample_freq, int max_buf_len)
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);

	_sample_freq = sample_freq;

	const int      mult =
		std::max (fstb::round_int (sample_freq / 44100), 1);
	for (int chn_cnt = 0; chn_cnt < int (_chn_arr.size ()); ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_cnt];
		for (int comb_cnt = 0; comb_cnt < int (chn._comb_arr.size ()); ++comb_cnt)
		{
			DelayComb &    comb = chn._comb_arr [comb_cnt];
			int            len  = _comb_len_arr [comb_cnt];
			len += chn_cnt * _stereospread;
			len *= mult;
			comb.set_delay (len);
			comb.clear_buffers ();
		}
		for (int ap_cnt = 0; ap_cnt < int (chn._ap_arr.size ()); ++ap_cnt)
		{
			DelayAllPassSimd &   ap  = chn._ap_arr [ap_cnt];
			const int            len = _ap_len_arr [ap_cnt];
			ap.set_delay (len * mult);
			ap.clear_buffers ();
		}
	}

	_buf.resize (max_buf_len);
}



void	FreeverbCore::set_reflectivity (float fdbk)
{
	assert (fdbk > -1);
	assert (fdbk <= 1);

	for (int chn_index = 0; chn_index < _max_nbr_chn; ++chn_index)
	{
		set_reflectivity (fdbk, chn_index);
	}
}



void	FreeverbCore::set_reflectivity (float fdbk, int chn_index)
{
	assert (fdbk > -1);
	assert (fdbk <= 1);
	assert (chn_index >= 0);
	assert (chn_index < _max_nbr_chn);

	Channel &      chn = _chn_arr [chn_index];
	for (auto &comb : chn._comb_arr)
	{
		comb.set_feedback (fdbk);
	}
}



void	FreeverbCore::set_damp (float damp)
{
	assert (damp >= 0);
	assert (damp <  1);

	for (int chn_index = 0; chn_index < _max_nbr_chn; ++chn_index)
	{
		set_damp (damp, chn_index);
	}
}



void	FreeverbCore::set_damp (float damp, int chn_index)
{
	assert (damp >= 0);
	assert (damp <  1);
	assert (chn_index >= 0);
	assert (chn_index < _max_nbr_chn);

	Channel &      chn = _chn_arr [chn_index];
	for (auto &comb : chn._comb_arr)
	{
		comb.set_damp (damp);
	}
}



void	FreeverbCore::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, int chn_index)
{
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr));
	assert (fstb::DataAlign <true>::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert (chn_index >= 0);
	assert (chn_index < _max_nbr_chn);

	Channel &      chn = _chn_arr [chn_index];

	// Comb filters in parallel
	bool           full_flag = false;
	for (auto &comb : chn._comb_arr)
	{
		float *        dst2_ptr = (full_flag) ? &_buf [0] : dst_ptr;
		comb.process_block (dst2_ptr, src_ptr, nbr_spl);
		if (full_flag)
		{
			dsp::mix::Align::mix_1_1 (dst_ptr, &_buf [0], nbr_spl);
		}
		full_flag = true;
	}

	// Allpasses in series
	for (auto &ap : chn._ap_arr)
	{
		ap.process_block (dst_ptr, dst_ptr, nbr_spl);
	}
}



void	FreeverbCore::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		for (auto &comb : chn._comb_arr)
		{
			comb.clear_buffers ();

		}
		for (auto &ap : chn._ap_arr)
		{
			ap.clear_buffers ();
		}
	}
}



const float	FreeverbCore::_scalewet = 3.0f / 2.0f;

// Original freeverb had 0.015 but we're not summing both channel
// together as it did, so we have to double the input gain.
const float	FreeverbCore::_scalein  = 0.030f;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const std::array <int, FreeverbCore::_nbr_comb>	FreeverbCore::_comb_len_arr =
{{
	1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617
}};
const std::array <int, FreeverbCore::_nbr_ap>	FreeverbCore::_ap_len_arr =
{{
	556, 441, 341, 225
}};



}  // namespace fv
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
