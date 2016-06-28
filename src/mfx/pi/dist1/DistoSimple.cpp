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
#include "mfx/pi/dist1/Param.h"
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
,	_desc ()
,	_state_set ()
,	_gain (float (DistoSimpleDesc::_gain_min))
,	_buf_arr ()
{
	_state_set.init (piapi::ParamCateg_GLOBAL, _desc.use_desc_set ());

	_state_set.set_val (Param_GAIN, 0); // 0 dB

	for (int index = 0; index < Param_NBR_ELT; ++index)
	{
		_state_set.add_observer (index, _param_change_flag);
	}

	_state_set.set_ramp_time (Param_GAIN, 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	DistoSimple::do_get_unique_id () const
{
	return _desc.get_unique_id ();
}



std::string	DistoSimple::do_get_name () const
{
	return _desc.get_name ();
}



void	DistoSimple::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	_desc.get_nbr_io (nbr_i, nbr_o);
}



bool	DistoSimple::do_prefer_stereo () const
{
	return _desc.prefer_stereo ();
}



int	DistoSimple::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DistoSimple::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc.get_param_info (categ, index);
}



piapi::PluginInterface::State	DistoSimple::do_get_state () const
{
	return _state;
}



double	DistoSimple::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistoSimple::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

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
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	const int      nbr_spl = proc._nbr_spl;

	_state_set.process_block (nbr_spl);

	// Gain (ramp)
	if (_param_change_flag (true))
	{
		const float    gain_beg =
			float (_state_set.get_val_beg_nat (Param_GAIN));
		const float    gain_end =
			float (_state_set.get_val_end_nat (Param_GAIN));

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
