/*****************************************************************************

        DistoSimple.cpp
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

#include "fstb/ToolsSimd.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/piapi/EventTs.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoSimple::DistoSimple ()
:	_state (State_CREATED)
,	_param_state_gain ()
,	_param_desc_gain (
		float (_gain_min), float (_gain_max),
		"Distortion Gain\nGain",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%5.1f"
	)
,	_gain (float (_gain_min))
{
	_param_state_gain.set_desc (_param_desc_gain);
	_param_state_gain.set_ramp_time (0.02);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	DistoSimple::do_get_unique_id () const
{
	return "dist1";
}



std::string	DistoSimple::do_get_name () const
{
	return "Simple distortion\nDisto S";
}



void	DistoSimple::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	DistoSimple::do_prefer_stereo () const
{
	return false;
}



int	DistoSimple::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return (categ == piapi::ParamCateg_GLOBAL) ? 1 : 0;
}



const piapi::ParamDescInterface &	DistoSimple::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _param_desc_gain;
}



piapi::PluginInterface::State	DistoSimple::do_get_state () const
{
	return _state;
}



double	DistoSimple::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	return _param_state_gain.get_val_tgt ();
}



int	DistoSimple::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	for (int chn = 0; chn < _max_nbr_chn; ++chn)
	{
		_buf_arr [chn].resize (max_buf_len);
	}

	_state = State_ACTIVE;

	return Err_OK;
}



#define mfx_pi_dist1_DistoSimple_USE_MIXALIGN

// x -> { x - x^9/9 if x >  0
//      { x + x^2/2 if x <= 0
// x * (1 - x^8/9)
void	DistoSimple::do_process_block (ProcInfo &proc)
{
	const int      nbr_evt = proc._nbr_evt;
	for (int index = 0; index < nbr_evt; ++index)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [index]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const double   val_nrm = evt._evt._param._val;
			_param_state_gain.set_val (val_nrm);
		}
	}

	const int      nbr_spl = proc._nbr_spl;

	_param_state_gain.tick (nbr_spl);

	// Gain (ramp)
	if (_param_state_gain.is_ramping ())
	{
		const float    gain_beg = float (
			_param_desc_gain.conv_nrm_to_nat (_param_state_gain.get_val_beg ())
		);
		const float    gain_end = float (
			_param_desc_gain.conv_nrm_to_nat (_param_state_gain.get_val_end ())
		);

#if defined (mfx_pi_dist1_DistoSimple_USE_MIXALIGN)

		if (proc._nbr_chn_arr [Dir_IN] == 1)
		{
			mfx::dsp::mix::Align::copy_1_1_vlr (
				&_buf_arr [0] [0],
				&proc._src_arr [0] [0],
				nbr_spl,
				gain_beg,
				gain_end
			);
		}
		else
		{
			static_assert (_max_nbr_chn == 2, "Multichannel not supported");
			mfx::dsp::mix::Align::copy_2_2_vlr (
				&_buf_arr [0] [0],
				&_buf_arr [1] [0],
				&proc._src_arr [0] [0],
				&proc._src_arr [1] [0],
				nbr_spl,
				gain_beg,
				gain_end
			);
		}

#else

		const float    step    = (gain_end - gain_beg) / nbr_spl;
		auto           g       = fstb::ToolsSimd::set1_f32 (gain_beg);
		const auto     c0123   = fstb::ToolsSimd::set_f32 (0, 1, 2, 3);
		fstb::ToolsSimd::mac (g, fstb::ToolsSimd::set1_f32 (step), c0123);
		const auto     g_step  = fstb::ToolsSimd::set1_f32 (step * 4);

		for (int chn = 0; chn < proc._nbr_chn_arr [Dir_IN]; ++chn)
		{
			const float *  src_ptr = proc._src_arr [chn];
			float *        dst_ptr = &_buf_arr [chn] [0];
			for (int pos = 0; pos < nbr_spl; pos += 4)
			{
				auto           x = fstb::ToolsSimd::load_f32 (src_ptr + pos);
				x *= g;
				fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
				g += g_step;
			}
		}

#endif

		_gain = gain_end;
	}

	// Gain (constant)
	else
	{
#if defined (mfx_pi_dist1_DistoSimple_USE_MIXALIGN)

		if (proc._nbr_chn_arr [Dir_IN] == 1)
		{
			mfx::dsp::mix::Align::copy_1_1_v (
				&_buf_arr [0] [0],
				&proc._src_arr [0] [0],
				nbr_spl,
				_gain
			);
		}
		else
		{
			static_assert (_max_nbr_chn == 2, "Multichannel not supported");
			mfx::dsp::mix::Align::copy_2_2_v (
				&_buf_arr [0] [0],
				&_buf_arr [1] [0],
				&proc._src_arr [0] [0],
				&proc._src_arr [1] [0],
				nbr_spl,
				_gain
			);
		}

#else

		const auto     gain = fstb::ToolsSimd::set1_f32 (_gain);

		for (int chn = 0; chn < proc._nbr_chn_arr [Dir_OUT]; ++chn)
		{
			const float *  src_ptr = proc._src_arr [chn];
			float *        dst_ptr = &_buf_arr [chn] [0];
			for (int pos = 0; pos < nbr_spl; pos += 4)
			{
				auto           x = fstb::ToolsSimd::load_f32 (src_ptr + pos);
				x *= gain;
				fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
			}
		}

#endif
	}

	const auto     mi    = fstb::ToolsSimd::set1_f32 (-1);
	const auto     ma    = fstb::ToolsSimd::set1_f32 ( 1);
	const auto     zero  = fstb::ToolsSimd::set_f32_zero ();
	const auto     c_1_9 = fstb::ToolsSimd::set1_f32 ( 1.0f / 9);
	const auto     c_1_2 = fstb::ToolsSimd::set1_f32 ( 1.0f / 2);
	const auto     bias  = fstb::ToolsSimd::set1_f32 ( 0.2f);

	int            chn_src_step = 1;
	if (proc._nbr_chn_arr [Dir_IN] == 1 && proc._nbr_chn_arr [Dir_OUT] > 1)
	{
		chn_src_step = 0;
	}

	int            chn_src = 0;
	for (int chn_dst = 0; chn_dst < proc._nbr_chn_arr [Dir_OUT]; ++chn_dst)
	{
		const float *  src_ptr = &_buf_arr [chn_src] [0];
		float *        dst_ptr = &proc._dst_arr [chn_dst] [0];
		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			auto           x = fstb::ToolsSimd::load_f32 (src_ptr + pos);

			x += bias;

			x = fstb::ToolsSimd::min_f32 (x, ma);
			x = fstb::ToolsSimd::max_f32 (x, mi);

			const auto     x2  = x * x;
			const auto     x4  = x2 * x2;
			const auto     x8  = x4 * x4;
			const auto     x9  = x8 * x;
			const auto     x_n = x + x2 * c_1_2;
			const auto     x_p = x - x9 * c_1_9;
			const auto     t_0 = fstb::ToolsSimd::cmp_gt_f32 (x, zero);
			x = fstb::ToolsSimd::select (t_0, x_p, x_n);

			x -= bias;

			fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
		}

		chn_src += chn_src_step;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
