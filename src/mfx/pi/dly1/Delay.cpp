/*****************************************************************************

        Delay.cpp
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

#include "fstb/Approx.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/StereoLevel.h"
#include "mfx/pi/dly1/Cst.h"
#include "mfx/pi/dly1/Delay.h"
#include "mfx/pi/dly1/Param.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Delay::Delay ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_filter_flag ()
,	_param_change_vol_flag ()
,	_param_change_time_flag ()
,	_param_change_link_flag ()
,	_interp ()
,	_chn_arr ()
,	_tmp_chn_buf ()
,	_tmp_buf_arr ()
,	_gain_fdbk_arr ()
,	_cross_fdbk ()
,	_lvl_in ()
,	_lvl_out ()
,	_delay_time_arr ()
,	_link_flag (false)
,	_quick_clean_req_flag (false)
,	_nbr_chn_in (0)
,	_nbr_chn_out (0)
{
	dsp::mix::Generic::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_LVL_IN  , 1);
	_state_set.set_val_nat (desc_set, Param_LVL_OUT , 0.5);
	_state_set.set_val_nat (desc_set, Param_L_TIME  , 0.375);
	_state_set.set_val_nat (desc_set, Param_L_FDBK  , 0.5);
	_state_set.set_val_nat (desc_set, Param_L_FILTER, 0);
	_state_set.set_val_nat (desc_set, Param_LINK    , 1);
	_state_set.set_val_nat (desc_set, Param_R_TIME  , 0.375);
	_state_set.set_val_nat (desc_set, Param_R_FDBK  , 0.5);
	_state_set.set_val_nat (desc_set, Param_R_FILTER, 0);
	_state_set.set_val_nat (desc_set, Param_X_FDBK  , 0);

	_state_set.add_observer (Param_LVL_IN  , _param_change_vol_flag);
	_state_set.add_observer (Param_LVL_OUT , _param_change_vol_flag);
	_state_set.add_observer (Param_L_TIME  , _param_change_time_flag);
	_state_set.add_observer (Param_L_FDBK  , _param_change_vol_flag);
	_state_set.add_observer (Param_L_FILTER, _param_change_filter_flag);
	_state_set.add_observer (Param_LINK    , _param_change_link_flag);
	_state_set.add_observer (Param_R_TIME  , _param_change_time_flag);
	_state_set.add_observer (Param_R_FDBK  , _param_change_vol_flag);
	_state_set.add_observer (Param_R_FILTER, _param_change_filter_flag);
	_state_set.add_observer (Param_X_FDBK  , _param_change_vol_flag);

	_param_change_filter_flag.add_observer (_param_change_flag);
	_param_change_vol_flag   .add_observer (_param_change_flag);
	_param_change_time_flag  .add_observer (_param_change_flag);
	_param_change_link_flag  .add_observer (_param_change_flag);

	for (auto &gf : _gain_fdbk_arr)
	{
		gf.set_inertia_time (0.00125);
	}
	_cross_fdbk.set_inertia_time (0.00125);
	_lvl_in.set_inertia_time (0.00125);
	_lvl_out.set_inertia_time (0.00125);

	_tmp_chn_buf.resize (_tmp_zone_len);
	for (auto &buf : _tmp_buf_arr)
	{
		buf.resize (_tmp_zone_len);
	}

	for (auto &chn_sptr : _chn_arr)
	{
		chn_sptr = ChnSPtr (new DelayChn);
		chn_sptr->init (
			_interp,
			&_tmp_chn_buf [0],
			int (_tmp_chn_buf.size ())
		);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Delay::do_get_state () const
{
	return _state;
}



double	Delay::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Delay::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);

	for (auto &gf : _gain_fdbk_arr)
	{
		gf.set_sample_freq (_sample_freq);
	}
	for (auto &chn_sptr : _chn_arr)
	{
		chn_sptr->set_sample_freq (_sample_freq);
	}
	_cross_fdbk.set_sample_freq (_sample_freq);
	_lvl_in.set_sample_freq (_sample_freq);
	_lvl_out.set_sample_freq (_sample_freq);

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	Delay::do_clean_quick ()
{
	_quick_clean_req_flag = true;
}



void	Delay::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_in =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_out =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	if (nbr_chn_out != _nbr_chn_out)
	{
		_nbr_chn_out = nbr_chn_out;
		update_param (true);
		clear_buffers ();
	}
	_nbr_chn_in  = nbr_chn_in;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Events
	for (int evt_cnt = 0; evt_cnt < proc._nbr_evt; ++evt_cnt)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [evt_cnt]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	if (_quick_clean_req_flag)
	{


		/*** To do: something quicker ***/
		clear_buffers ();


		_quick_clean_req_flag = false;
	}

	int            block_pos = 0;
	do
	{
		int            work_len = proc._nbr_spl - block_pos;

		// Parameter update
		if (_param_change_flag (true))
		{
			// We need this intermediate varaible because for some reason GCC
			// fails to link when _update_resol is directly used in std::min.
			const int      max_len  = _update_resol;
			work_len = std::min (work_len, max_len);

			_state_set.process_block (work_len);
			update_param ();
		}

		// Processing
		process_block_part (
			proc._dst_arr,
			proc._src_arr,
			block_pos,
			block_pos + work_len
		);

		block_pos += work_len;
	}
	while (block_pos < proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Delay::clear_buffers ()
{
	_gain_fdbk_arr [0].clear_buffers ();
	_gain_fdbk_arr [1].clear_buffers ();
	_cross_fdbk.clear_buffers ();
	_lvl_in.clear_buffers ();
	_lvl_out.clear_buffers ();

	for (auto &chn_sptr : _chn_arr)
	{
		chn_sptr->clear_buffers ();
	}

	_quick_clean_req_flag = false;
}



void	Delay::update_param (bool force_flag)
{
	if (_param_change_link_flag (true) || force_flag)
	{
		const double	link_val = _state_set.get_val_tgt_nat (Param_LINK);
		_link_flag = (link_val >= 0.5);

		_param_change_filter_flag.set ();
		_param_change_vol_flag.set ();
		_param_change_time_flag.set ();
	}

	if (_param_change_filter_flag (true) || force_flag)
	{
		update_param_filter ();
	}

	if (_param_change_vol_flag (true) || force_flag)
	{
		update_param_vol ();
	}

	if (_param_change_time_flag (true) || force_flag)
	{
		update_param_time ();
	}
}



void	Delay::update_param_filter ()
{
	for (int chn = 0; chn < Cst::_nbr_lines; ++chn)
	{
		static const std::array <Param, Cst::_nbr_lines> par_arr =
		{{
			Param_L_FILTER,
			Param_R_FILTER
		}};

		float          val = float (_state_set.get_val_end_nat (
			par_arr [chn]
		));
		const float    val2 = val * val;
		val = val * 0.5f * (1 + val2 * val2);

		const float    freq_base = 20.0f;   // Hz.
		const float    freq_min  =  5.0f;   // Hz.
		const float    nyq_ratio =  0.48f;  // Maximum filter range as ratio of the sampling frequency
		const float    margin    =  0.10f;  // Size of the parameter range where the cutoff slides from a high (or low) value to the maximum (or minimum) value.
		const int      range_oct = 10;      // Octaves

		// min < base <<<<< top < max
		const float    freq_max  = float (_sample_freq) * nyq_ratio;
		const float    freq_top  = freq_base * (1 << range_oct);

		float          freq_lo   = freq_min;
		float          freq_hi   = freq_max;

		// Low-pass
		if (val < -margin)
		{
			const float    lerp = (val + 1) / (1 - margin);
			freq_hi = freq_base * fstb::Approx::exp2 (range_oct * lerp);
		}
		else if (val < 0)
		{
			const float    lerp = (val + margin) / margin;
			freq_hi = freq_top + lerp * (freq_max - freq_top);
		}

		// High-pass
		else if (val < margin)
		{
			const float    lerp = val / margin;
			freq_lo = freq_min + lerp * (freq_base - freq_min);
		}
		else
		{
			const float    lerp = (val - margin) / (1 - margin);
			freq_lo = freq_base * fstb::Approx::exp2 (range_oct * lerp);
		}

		if (! _link_flag)
		{
			_chn_arr [chn]->set_filter_freq (freq_lo, freq_hi);
		}
		else if (chn == 0)
		{
			for (auto &chn_sptr : _chn_arr)
			{
				chn_sptr->set_filter_freq (freq_lo, freq_hi);
			}
			break;
		}
	}
}



void	Delay::update_param_vol ()
{
	// Volume
	const float    lvl_in = float (_state_set.get_val_end_nat (Param_LVL_IN));
	_lvl_in.set_val (lvl_in);
	const float    lvl_out = float (_state_set.get_val_end_nat (Param_LVL_OUT));
	_lvl_out.set_val (lvl_out);

	// Feedback
	const float    fdbk_l = float (_state_set.get_val_end_nat (Param_L_FDBK));
	_gain_fdbk_arr [0].set_val (fdbk_l);

	const float    fdbk_r = float (_state_set.get_val_end_nat (Param_R_FDBK));
	_gain_fdbk_arr [1].set_val ((_link_flag) ? fdbk_l : fdbk_r);

	const float    fdbk_x = float (_state_set.get_val_end_nat (Param_X_FDBK));
	_cross_fdbk.set_val (fdbk_x);
}



void	Delay::update_param_time ()
{
	_delay_time_arr [0] = float (_state_set.get_val_end_nat (Param_L_TIME));
	_delay_time_arr [1] = float (_state_set.get_val_end_nat (Param_R_TIME));

	update_times (0);
}



void	Delay::process_block_part (float * const out_ptr_arr [], const float * const in_ptr_arr [], int pos_beg, int pos_end)
{
	assert (out_ptr_arr != 0);
	assert (in_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_beg < pos_end);

	do
	{
		const int      max_len  = _tmp_zone_len;
		int            work_len = std::min (pos_end - pos_beg, max_len);

		for (int chn_cnt = 0; chn_cnt < Cst::_nbr_lines; ++chn_cnt)
		{
			const int      max_len_chn = _chn_arr [chn_cnt]->get_max_proc_len ();
			work_len = std::min (work_len, max_len_chn);
		}

		// Special case for mono + link
		if (_link_flag && _nbr_chn_out == 1)
		{
			process_block_part_mono_link (
				out_ptr_arr,
				in_ptr_arr,
				pos_beg,
				pos_beg + work_len
			);
		}

		else
		{
			process_block_part_standard (
				out_ptr_arr,
				in_ptr_arr,
				pos_beg,
				pos_beg + work_len
			);
		}

		pos_beg += work_len;
	}
	while (pos_beg < pos_end);
}



void	Delay::process_block_part_standard (float * const out_ptr_arr [], const float * const in_ptr_arr [], int pos_beg, int pos_end)
{
	assert (out_ptr_arr != 0);
	assert (in_ptr_arr != 0);
	assert (pos_beg >= 0);

	const int      nbr_spl = pos_end - pos_beg;
	assert (nbr_spl <= _tmp_zone_len);

	const int      chn_in_r    = (_nbr_chn_in > 1) ? 1 : 0;

	std::array <float, Cst::_nbr_lines> fdbk_beg_arr;
	std::array <float, Cst::_nbr_lines> fdbk_end_arr;

	for (int chn_cnt = 0; chn_cnt < Cst::_nbr_lines; ++chn_cnt)
	{
		fdbk_beg_arr [chn_cnt] = float (_gain_fdbk_arr [chn_cnt].get_val ());
		_gain_fdbk_arr [chn_cnt].tick (nbr_spl);
		fdbk_end_arr [chn_cnt] = float (_gain_fdbk_arr [chn_cnt].get_val ());
	}

	const float    xf_beg      = float (_cross_fdbk.get_val ());
	_cross_fdbk.tick (nbr_spl);
	const float    xf_end      = float (_cross_fdbk.get_val ());

	const float    lvl_in_beg  = float (_lvl_in.get_val ());
	const float    lvl_out_beg = float (_lvl_out.get_val ());
	_lvl_in.tick (nbr_spl);
	_lvl_out.tick (nbr_spl);
	const float    lvl_in_end  = float (_lvl_in.get_val ());
	const float    lvl_out_end = float (_lvl_out.get_val ());

	DelayChn &		chn_l = *(_chn_arr [0]);
	DelayChn &		chn_r = *(_chn_arr [1]);

	chn_l.process_block_read (&_tmp_buf_arr [0] [0], nbr_spl);
	chn_r.process_block_read (&_tmp_buf_arr [1] [0], nbr_spl);

	const dsp::StereoLevel  s_vol (1 - xf_beg, xf_beg, xf_beg, 1 - xf_beg);
	const dsp::StereoLevel  e_vol (1 - xf_end, xf_end, xf_end, 1 - xf_end);
	dsp::mix::Generic::copy_mat_2_2_vlrauto (
		&_tmp_buf_arr [0] [0],
		&_tmp_buf_arr [1] [0],
		&_tmp_buf_arr [0] [0],
		&_tmp_buf_arr [1] [0],
		nbr_spl,
		s_vol,
		e_vol
	);

	chn_l.process_block_write (
		in_ptr_arr [0] + pos_beg,
		&_tmp_buf_arr [0] [0],
		fdbk_beg_arr [0],
		fdbk_end_arr [0],
		nbr_spl
	);
	chn_r.process_block_write (
		in_ptr_arr [chn_in_r] + pos_beg,
		&_tmp_buf_arr [1] [0],
		fdbk_beg_arr [1],
		fdbk_end_arr [1],
		nbr_spl
	);

	if (_nbr_chn_out == 2)
	{
		dsp::mix::Generic::copy_2_2_vlrauto (
			out_ptr_arr [0] + pos_beg,
			out_ptr_arr [1] + pos_beg,
			in_ptr_arr [0       ] + pos_beg,
			in_ptr_arr [chn_in_r] + pos_beg,
			nbr_spl,
			lvl_in_beg,
			lvl_in_end
		);

		dsp::mix::Generic::mix_2_2_vlrauto (
			out_ptr_arr [0] + pos_beg,
			out_ptr_arr [1] + pos_beg,
			&_tmp_buf_arr [0] [0],
			&_tmp_buf_arr [1] [0],
			nbr_spl,
			lvl_out_beg,
			lvl_out_end
		);
	}
	else
	{
		const int      nbr_lines    = Cst::_nbr_lines;
		const int      nbr_chn_proc = std::max (_nbr_chn_out, nbr_lines);
		int            chn_in_idx   = 0;
		const int      chn_in_inc   = (_nbr_chn_in  >= Cst::_nbr_lines) ? 1 : 0;
		int            chn_out_idx  = 0;
		const int      chn_out_inc  = (_nbr_chn_out >= Cst::_nbr_lines) ? 1 : 0;
		for (int chn_cnt = 0; chn_cnt < nbr_chn_proc; ++chn_cnt)
		{
			if (chn_cnt < _nbr_chn_out)
			{
				dsp::mix::Generic::copy_1_1_vlrauto (
					out_ptr_arr [chn_cnt]   + pos_beg,
					in_ptr_arr [chn_in_idx] + pos_beg,
					nbr_spl,
					lvl_in_beg,
					lvl_in_end
				);
			}

			if (chn_cnt < Cst::_nbr_lines)
			{
				dsp::mix::Generic::mix_1_1_vlrauto (
					out_ptr_arr [chn_out_idx] + pos_beg,
					&_tmp_buf_arr [chn_cnt] [0],
					nbr_spl,
					lvl_out_beg,
					lvl_out_end
				);
			}

			chn_in_idx  += chn_in_inc;
			chn_out_idx += chn_out_inc;
		}
	}
}



void	Delay::process_block_part_mono_link (float * const out_ptr_arr [], const float * const in_ptr_arr [], int pos_beg, int pos_end)
{
	assert (out_ptr_arr != 0);
	assert (in_ptr_arr != 0);
	assert (pos_beg >= 0);

	const int      nbr_spl = pos_end - pos_beg;
	assert (nbr_spl <= _tmp_zone_len);

	const float    fdbk_beg    = float (_gain_fdbk_arr [0].get_val ());
	_gain_fdbk_arr [0].tick (nbr_spl);
	const float    fdbk_end    = float (_gain_fdbk_arr [0].get_val ());

	const float    lvl_in_beg  = float (_lvl_in.get_val ());
	float          lvl_out_beg = float (_lvl_out.get_val ());
	_lvl_in.tick (nbr_spl);
	_lvl_out.tick (nbr_spl);
	const float    lvl_in_end  = float (_lvl_in.get_val ());
	float          lvl_out_end =  float (_lvl_out.get_val ());

	if (_nbr_chn_out < Cst::_nbr_lines)
	{
		const float    gain = 1.0f / Cst::_nbr_lines;
		lvl_out_beg *= gain;
		lvl_out_end *= gain;
	}

	dsp::mix::Generic::copy_1_1_vlrauto (
		out_ptr_arr [0] + pos_beg,
		in_ptr_arr [0]  + pos_beg,
		nbr_spl,
		lvl_in_beg,
		lvl_in_end
	);

	DelayChn &		chn = *(_chn_arr [0]);

	chn.process_block_read (&_tmp_buf_arr [0] [0], nbr_spl);

	chn.process_block_write (
		in_ptr_arr [0] + pos_beg,
		&_tmp_buf_arr [0] [0],
		fdbk_beg,
		fdbk_end,
		nbr_spl
	);

	dsp::mix::Generic::mix_1_1_vlrauto (
		out_ptr_arr [0] + pos_beg,
		&_tmp_buf_arr [0] [0],
		nbr_spl,
		lvl_out_beg,
		lvl_out_end
	);
}



void	Delay::update_times (int nbr_spl)
{
	assert (nbr_spl >= 0);

	float          time_s = 0;
	for (int chn_cnt = 0; chn_cnt < Cst::_nbr_lines; ++chn_cnt)
	{
		if (chn_cnt == 0 || ! _link_flag)
		{
			time_s = compute_delay_time (chn_cnt);
		}
		_chn_arr [chn_cnt]->set_delay_time (time_s, nbr_spl);
	}
}



float	Delay::compute_delay_time (int chn) const
{
	assert (chn >= 0);
	assert (chn < Cst::_nbr_lines);

	return _delay_time_arr [chn];
}



}  // namespace dly1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
