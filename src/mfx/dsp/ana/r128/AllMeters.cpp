/*****************************************************************************

        AllMeters.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/r128/AllMeters.h"

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



void	AllMeters::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_sample_freq ();
	_seg.set_period (0.1);
}



void	AllMeters::set_nbr_chn (int nbr_chn)
{
	assert (nbr_chn > 0);

	_chn_arr.resize (nbr_chn);
	for (auto &meter : _meter_arr)
	{
		meter._merger.set_nbr_chn (nbr_chn);
	}
	update_sample_freq ();

	_meter_arr [int (Win::M)]._merger.set_win_len (0.4);
	_meter_arr [int (Win::S)]._merger.set_win_len (3.0);
}



// Call is optional
void	AllMeters::set_chn_weights (const float weight_arr []) noexcept
{
	assert (weight_arr != nullptr);

	for (auto &meter : _meter_arr)
	{
		meter._merger.set_chn_weights (weight_arr);
	}
}



void	AllMeters::set_chn_buffers (const float *src_ptr_arr []) noexcept
{
	assert (src_ptr_arr != nullptr);

	for (auto &meter : _meter_arr)
	{
		meter._merger.set_chn_buffers (src_ptr_arr);
	}
	for (int chn_cnt = 0; chn_cnt < int (_chn_arr.size ()); ++chn_cnt)
	{
		const auto     src_ptr = src_ptr_arr [chn_cnt];
		assert (src_ptr != nullptr);
		_chn_arr [chn_cnt]._src_ptr = src_ptr;
	}
}



void	AllMeters::analyse_sample (int pos) noexcept
{
	for (auto &chn : _chn_arr)
	{
		const auto     x = chn._src_ptr [pos];
		chn._peak_detect.process_sample (x);
	}

	_seg.start (1);
	assert (_seg.is_rem_elt ());
	assert (_seg.get_seg_len () == 1);
	for (auto &meter : _meter_arr)
	{
		meter._merger.analyse_sample (pos);
		if (_seg.is_frame_ready ())
		{
			const auto     msq = meter._merger.compute_msq ();
			meter._histo.add_block (msq);
		}
	}
	_seg.iterate ();
	assert (! _seg.is_rem_elt ());
}



void	AllMeters::analyse_block (int pos_beg, int pos_end) noexcept
{
	const int      nbr_spl = pos_end - pos_beg;
	assert (nbr_spl > 0);

	for (auto &chn : _chn_arr)
	{
		chn._peak_detect.process_block (chn._src_ptr + pos_beg, nbr_spl);
	}

	int            pos = pos_beg;
	for (_seg.start (nbr_spl); _seg.is_rem_elt (); _seg.iterate ())
	{
		const int      seg_len = _seg.get_seg_len ();
		for (auto &meter : _meter_arr)
		{
			meter._merger.analyse_block (pos, pos + seg_len);
			if (_seg.is_frame_ready ())
			{
				const auto     msq = meter._merger.compute_msq ();
				meter._histo.add_block (msq);
			}
		}
		pos += seg_len;
	}
}



float	AllMeters::get_loudness_m () const noexcept
{
	return get_loudness (Win::M);
}



float	AllMeters::get_loudness_s () const noexcept
{
	return get_loudness (Win::S);
}



float	AllMeters::compute_loudness_r () const noexcept
{
	const auto     lra =
		_meter_arr [int (Win::S)]._histo.compute_loudness_range ();

	return lra;
}



float	AllMeters::compute_loudness_i () const noexcept
{
	const auto     lufs =
		_meter_arr [int (Win::M)]._histo.compute_loudness_integrated ();

	return lufs;
}



float	AllMeters::get_peak (int chn_idx) const noexcept
{
	assert (chn_idx >= 0);
	assert (chn_idx < int (_chn_arr.size ()));

	return _chn_arr [chn_idx]._peak_detect.get_peak ();
}



void	AllMeters::clear_peaks () noexcept
{
	for (auto &chn : _chn_arr)
	{
		chn._peak_detect.clear_peak ();
	}
}



void	AllMeters::clear_buffers () noexcept
{
	for (auto &chn : _chn_arr)
	{
		chn._peak_detect.clear_buffers ();
	}
	for (auto &meter : _meter_arr)
	{
		meter._merger.clear_buffers ();
		meter._histo.clear_buffers ();
	}
	_seg.clear_buffers ();
	_loud_m = _clear_peak_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AllMeters::update_sample_freq ()
{
	if (_sample_freq > 0)
	{
		for (auto &chn : _chn_arr)
		{
			chn._peak_detect.set_sample_freq (_sample_freq);
		}
		for (auto &meter : _meter_arr)
		{
			meter._merger.set_sample_freq (_sample_freq);
		}
		_seg.set_sample_freq (_sample_freq);
	}
}



float	AllMeters::get_loudness (Win type) const noexcept
{
	const int      idx  = int (type);
	assert (idx >= 0 && idx < int (Win::NBR_ELT));

	const auto     msq  = _meter_arr [idx]._merger.compute_msq ();
	const auto     lufs = MeanSq::conv_mean_sq_to_lufs (msq);

	return lufs;
}



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
