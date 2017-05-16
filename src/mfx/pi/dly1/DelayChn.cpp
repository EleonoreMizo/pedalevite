/*****************************************************************************

        DelayChn.cpp
        Author: Laurent de Soras, 2016

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

#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/pi/dly1/Cst.h"
#include "mfx/pi/dly1/DelayChn.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DelayChn::DelayChn ()
:	_sample_freq (44100)
,	_dly_line ()
,	_dly_reader ()
,	_filter ()
,	_f_lo (5)
,	_f_hi (20000)
,	_delay (1)
,	_tmp_ptr (0)
,	_tmp_len (0)
,	_max_proc_len (0)
{
	_dly_line.set_max_delay_time (Cst::_max_delay / 1000.0);

	_dly_reader.set_delay_line (_dly_line);
	_dly_reader.set_crossfade (256, 0);
	_dly_reader.set_resampling_range (-4.0, +4.0);
}



void	DelayChn::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	_dly_line.set_sample_freq (_sample_freq, 0);
}



void	DelayChn::init (dsp::rspl::InterpolatorInterface &interp, float *buf_ptr, int buf_len)
{
	assert (&interp != 0);
	assert (buf_ptr != 0);
	assert (buf_len > 0);

	_tmp_ptr = buf_ptr;
	_tmp_len = buf_len;
	_dly_line.set_interpolator (interp);
	_dly_reader.set_tmp_buf (buf_ptr, buf_len);

	update_max_proc_len ();
}



void	DelayChn::set_delay_time (double delay_time, int transition_time)
{
	_delay = delay_time;

	_dly_reader.set_delay_time (_delay, transition_time);
	update_max_proc_len ();
}



double	DelayChn::get_delay_time () const
{
	return (_delay);
}



void	DelayChn::restore ()
{
	_tmp_ptr = 0;
	_tmp_len = 0;
}



void	DelayChn::set_filter_freq (double f_lo, double f_hi)
{
	assert (f_lo > 0);
	assert (f_hi > f_lo);
	assert (f_hi < _sample_freq * 0.5);

	_f_lo = f_lo;
	_f_hi = f_hi;

	update_filter ();
}



int	DelayChn::get_max_proc_len () const
{
	return (_max_proc_len);
}



void	DelayChn::process_block_read (float dst_ptr [], int nbr_spl)
{
	assert (_tmp_ptr != 0);
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_proc_len);

	_dly_reader.read_data (dst_ptr, nbr_spl, 0);
	_filter.process_block (dst_ptr, dst_ptr, nbr_spl);
}



void	DelayChn::process_block_write (const float src_ptr [], const float fdbk_ptr [], float lvl_beg, float lvl_end, int nbr_spl)
{
	assert (_tmp_ptr != 0);
	assert (src_ptr != 0);
	assert (fdbk_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_proc_len);

	dsp::mix::Generic::copy_1_1 (_tmp_ptr, src_ptr, nbr_spl);
	dsp::mix::Generic::mix_1_1_vlrauto (_tmp_ptr, fdbk_ptr, nbr_spl, lvl_beg, lvl_end);
	_dly_line.push_block (_tmp_ptr, nbr_spl);
}



void	DelayChn::clear_buffers ()
{
	_dly_line.clear_buffers ();
	_dly_reader.clear_buffers ();
	_filter.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayChn::update_filter ()
{
	const double   f_ref = sqrt (_f_lo * _f_hi);
	const float    f1    = float (
		dsp::iir::TransSZBilin::prewarp_freq_rel_1 (_f_lo, f_ref, _sample_freq)
	);
	const float    f2    = float (
		dsp::iir::TransSZBilin::prewarp_freq_rel_1 (_f_hi, f_ref, _sample_freq)
	);

	//             Low-pass              High-pass
	// f (s) = (1 / (1 + s/f2))  *  ((s/f1) / (1 + s/f1))
	//       = (s/f1) / ((1 + s/f2) * (1 + s/f1))
	//       = (s / f1) / (1 + s * (f1+f2)/(f1*f2) + s^2 / (f1*f2))
	const float    if1f2  = 1 / (f1 * f2);
	const float    bs [3] = { 0,            1 / f1,     0 };
	const float    as [3] = { 1, (f1 + f2) * if1f2, if1f2 };

	float          bz [3];
	float          az [3];

	const float    k = dsp::iir::TransSZBilin::compute_k_approx (float (
		f_ref / _sample_freq
	));
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
	_filter.set_z_eq (bz, az);
}



void	DelayChn::update_max_proc_len ()
{
	_max_proc_len = _dly_line.estimate_max_one_shot_proc_w_feedback (_delay);
	assert (_max_proc_len > 0);
	_max_proc_len = std::min (_max_proc_len, _tmp_len);
}



}  // namespace dly1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
