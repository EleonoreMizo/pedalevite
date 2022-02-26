/*****************************************************************************

        ChnMerger.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/r128/ChnMerger.h"

#include <algorithm>
#include <array>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ChnMerger::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_time_param ();
}



void	ChnMerger::set_nbr_chn (int nbr_chn)
{
	assert (nbr_chn > 0);

	_chn_arr.resize (nbr_chn);
	update_time_param ();
}



void	ChnMerger::set_win_len (double t)
{
	assert (t > 0);

	_win_dur = t;
	update_time_param ();
}



void	ChnMerger::set_chn_weights (const float weight_arr []) noexcept
{
	assert (weight_arr != nullptr);

	for (int chn_cnt = 0; chn_cnt < int (_chn_arr.size ()); ++chn_cnt)
	{
		const auto     w = weight_arr [chn_cnt];
		assert (w > 0);
		_chn_arr [chn_cnt]._weight = w;
	}
}



void	ChnMerger::set_chn_buffers (const float *src_ptr_arr []) noexcept
{
	assert (src_ptr_arr != nullptr);

	for (int chn_cnt = 0; chn_cnt < int (_chn_arr.size ()); ++chn_cnt)
	{
		const auto     src_ptr = src_ptr_arr [chn_cnt];
		assert (src_ptr != nullptr);
		_chn_arr [chn_cnt]._src_ptr = src_ptr;
	}
}



void	ChnMerger::analyse_sample (int pos) noexcept
{
	assert (is_ready ());

	for (auto &chn : _chn_arr)
	{
		auto           x = chn._src_ptr [pos];
		x = chn._flt_k.process_sample (x);
		chn._flt_msq.analyse_sample (x);
	}
}



void	ChnMerger::analyse_block (int pos_beg, int pos_end) noexcept
{
	assert (is_ready ());

	const int      nbr_spl = pos_end - pos_beg;
	assert (nbr_spl > 0);

	std::array <float, _buf_size> buf;
	for (auto &chn : _chn_arr)
	{
		auto           src_ptr = chn._src_ptr + pos_beg;
		int            pos = 0;
		do
		{
			const auto     work_len = std::min (nbr_spl - pos, int (_buf_size));
			chn._flt_k.process_block (buf.data (), src_ptr + pos, work_len);
			chn._flt_msq.analyse_block (buf.data (), work_len);
			pos += work_len;
		}
		while (pos < nbr_spl);
	}
}



float	ChnMerger::compute_msq () const noexcept
{
	assert (is_ready ());

	float          sum = 0;
	for (auto &chn : _chn_arr)
	{
		const auto     msq = chn._flt_msq.get_val ();
		sum += msq * chn._weight;
	}

	return sum;
}



void	ChnMerger::clear_buffers () noexcept
{
	for (auto &chn : _chn_arr)
	{
		chn._flt_k.clear_buffers ();
		chn._flt_msq.clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr int	ChnMerger::_buf_size;



bool	ChnMerger::is_chn_count_set () const noexcept
{
	return (! _chn_arr.empty ());
}



bool	ChnMerger::is_setup () const noexcept
{
	return (_sample_freq > 0 && _win_dur > 0 && is_chn_count_set ());
}



bool	ChnMerger::is_ready () const noexcept
{
	return (
	   is_setup () && std::all_of (
			_chn_arr.begin (), _chn_arr.end (),
			[] (const Channel &chn) { return (chn._src_ptr != nullptr); }
		)
	);
}



void	ChnMerger::update_time_param ()
{
	if (is_setup ())
	{
		for (auto &chn : _chn_arr)
		{
			chn._flt_k.set_sample_freq (_sample_freq);
			chn._flt_msq.set_sample_freq (_sample_freq);
			chn._flt_msq.set_win_len (_win_dur);
		}

		clear_buffers ();
	}
}



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
