/*****************************************************************************

        FreqShift.cpp
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

#include "fstb/def.h"
#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/cdsp/FreqShift.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace cdsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FreqShift::FreqShift ()
:	_sample_freq (0)
,	_ali ()
,	_inv_fs (0)
,	_freq (0)
,	_step_angle (0)
,	_buf_arr ()
,	_coef_list ()
{
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		_coef_list.data (), _nbr_coef, 1 / 1000.0
	);
	for (auto &chn : _ali->_chn_arr)
	{
		chn._ssb.set_coefs (_coef_list.data ());
	}
}



void	FreqShift::reset (double sample_freq, int max_buf_len, double &latency)
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);

	_sample_freq = float (sample_freq);

	_inv_fs = float (1.0 / _sample_freq);
	for (auto &buf : _buf_arr)
	{
		buf.resize (max_buf_len);
	}

	latency = hiir::PolyphaseIir2Designer::compute_group_delay (
		_coef_list.data (), _nbr_coef, 1000 * _inv_fs, true
	);

	clear_buffers ();
}



void	FreqShift::set_freq (float f)
{
	_freq = f;
	update_step ();
}



bool	FreqShift::is_active () const
{
	return (fabs (_freq) >= 1e-3f);
}



void	FreqShift::process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn)
{
	assert (dst_ptr_arr != nullptr);
	assert (src_ptr_arr != nullptr);
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr_arr [0]));
	assert (fstb::DataAlign <true>::check_ptr (src_ptr_arr [0]));
	assert (nbr_spl > 0);
	assert (nbr_chn > 0);

	_ali->_osc.process_block <fstb::DataAlign <true> > (
		&_buf_arr [Buf_COS] [0],
		&_buf_arr [Buf_SIN] [0],
		nbr_spl
	);

	for (int c = 0; c < nbr_chn; ++c)
	{
		_ali->_chn_arr [c]._aa.process_block (
			&_buf_arr [Buf_AAF] [0],
			src_ptr_arr [c],
			nbr_spl
		);

		_ali->_chn_arr [c]._ssb.process_block (
			&_buf_arr [Buf_PHS] [0],
			&_buf_arr [Buf_PHC] [0],
			&_buf_arr [Buf_AAF] [0],
			nbr_spl
		);

		float *        dst_ptr = dst_ptr_arr [c];

#if 1
		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			const auto     co  = fstb::ToolsSimd::load_f32 (&_buf_arr [Buf_COS] [pos]);
			const auto     si  = fstb::ToolsSimd::load_f32 (&_buf_arr [Buf_SIN] [pos]);
			const auto     x   = fstb::ToolsSimd::load_f32 (&_buf_arr [Buf_PHC] [pos]);
			const auto     y   = fstb::ToolsSimd::load_f32 (&_buf_arr [Buf_PHS] [pos]);
			const auto     val = co * x + si * y;
			fstb::ToolsSimd::store_f32 (dst_ptr + pos, val);
		}

#else // Reference implementation
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const float   co  = _buf_arr [Buf_COS] [pos];
			const float   si  = _buf_arr [Buf_SIN] [pos];
			const float   x   = _buf_arr [Buf_PHC] [pos];
			const float   y   = _buf_arr [Buf_PHS] [pos];
			const float   val = co * x + si * y;
			dst_ptr [pos] = val;
		}

#endif
	}
}



void	FreqShift::clear_buffers ()
{
	for (auto &chn : _ali->_chn_arr)
	{
		chn._aa.clear_buffers ();
		chn._ssb.clear_buffers ();
	}
	_ali->_osc.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqShift::update_step ()
{
	_step_angle = float ((fstb::PI * 2) * _freq / _sample_freq);
	_ali->_osc.set_step (_step_angle);

	const float    bs [3]  = { 0,                   0, 1 };
	const float    as [3]  = { 1, float (fstb::SQRT2), 1 };
	float          bz [3];
	float          az [3];
	const float    freq_aa = std::max (-_freq, 20.0f);
	const float    k       =
		dsp::iir::TransSZBilin::compute_k_approx (freq_aa * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
	for (auto &chn : _ali->_chn_arr)
	{
		chn._aa.set_z_eq (bz, az);
	}
}



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
