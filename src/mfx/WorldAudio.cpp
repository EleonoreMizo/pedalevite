/*****************************************************************************

        WorldAudio.cpp
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

#include "fstb/BitFieldSparseIterator.h"
#include "fstb/DataAlign.h"
#include "mfx/cmd/DelayInterface.h"
#include "mfx/piapi/BypassState.h"
#include "mfx/piapi/ProcInfo.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/Cst.h"
#include "mfx/Dir.h"
#include "mfx/FileOpWav.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"
#include "mfx/WaMsg.h"
#include "mfx/WorldAudio.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



WorldAudio::WorldAudio (PluginPool &plugin_pool, WaMsgQueue &queue_from_cmd, WaMsgQueue &queue_to_cmd, ui::UserInputInterface::MsgQueue &queue_from_input, ui::UserInputInterface &input_device, conc::CellPool <WaMsg> &msg_pool_cmd)
:	_pi_pool (plugin_pool)
,	_queue_from_cmd (queue_from_cmd)
,	_queue_to_cmd (queue_to_cmd)
,	_queue_from_input (queue_from_input)
,	_input_device (input_device)
,	_msg_pool_cmd (msg_pool_cmd)
,	_max_block_size (0)
,	_sample_freq (0)
,	_ctx_ptr (0)
,	_lvl_meter ()
,	_meter_result ()
,	_period_now (1)
,	_rate_expected (44100.f / (64 * 1000000))
,	_evt_arr ()
,	_evt_ptr_arr ()
,	_tempo_new (0)
,	_tempo_cur (float (Cst::_tempo_ref))
,	_proc_date_beg (0)
,	_proc_date_end (0)
,	_proc_analyser ()
,	_sig_res_arr ()
,	_reset_flag (false)
,	_fade_chnmap (0)
#if defined (mfx_WorldAudio_BUF_REC)
,	_data_rec_flag (true)
,	_data_rec_arr ()
,	_data_rec_cur_buf (0)
,	_data_rec_pos (0)
,	_data_rec_len (0)
#endif
{
	_evt_arr.reserve (_max_nbr_evt);
	_evt_ptr_arr.reserve (_max_nbr_evt);
	_proc_analyser.set_release_time_s (1.0);
}



WorldAudio::~WorldAudio ()
{
	// Flushes the queues
	collect_msg_cmd (false);
	collect_msg_ui (false);
}



// Does not reset() plug-ins.
void	WorldAudio::set_process_info (double sample_freq, int max_block_size)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);

	_max_block_size = max_block_size;
	_sample_freq    = float (sample_freq);

	const int      multiple = int (
		  AlignedZone::allocator_type::ALIGNMENT
		/ sizeof (AlignedZone::value_type)
	);
	assert (fstb::is_pow_2 (multiple));
	const int      block_align = (max_block_size + multiple - 1) & -multiple;
	_buf_zone.resize (block_align * Cst::_max_nbr_buf);
	for (int buf_index = 0; buf_index < Cst::_max_nbr_buf; ++buf_index)
	{
		_buf_arr [buf_index] = &_buf_zone [buf_index * block_align];
	}

#if defined (mfx_WorldAudio_BUF_REC)
	// Preallocates a few recording buffers
	_data_rec_arr.resize (16);
	_data_rec_len = size_t (sample_freq * _max_rec_duration);
	for (auto &zone : _data_rec_arr)
	{
		zone.reserve (_data_rec_len);
	}
#endif

	_lvl_meter->set_sample_freq (_sample_freq);
	_proc_analyser.set_sample_freq (_sample_freq / max_block_size);

	for (auto &x : _sig_res_arr) { x = 0; }

	_meter_result.reset ();
	const std::chrono::microseconds  date_cur (_input_device.get_cur_date ());
	_proc_date_end = date_cur;
	_proc_date_beg = date_cur;
	_period_now    = 1;
	_rate_expected = float (sample_freq / (int64_t (max_block_size) * 1000000));

	_fade_chnmap   = 0;
}



MeterResultSet &	WorldAudio::use_meters ()
{
	return _meter_result;
}



float	WorldAudio::get_audio_period_ratio () const
{
	return _period_now;
}



void	WorldAudio::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	assert (dst_arr != 0);
	assert (dst_arr [0] != 0);
	assert (src_arr != 0);
	assert (src_arr [0] != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	// Time measurement
	const std::chrono::microseconds  date_beg (_input_device.get_cur_date ());
	const std::chrono::microseconds  dur_tot  (      date_beg - _proc_date_beg);
	const std::chrono::microseconds  dur_proc (_proc_date_end - _proc_date_beg);
	if (dur_tot.count () > 0)
	{
		const float    ratio =
			float (dur_proc.count ()) / float (dur_tot.count ());
		_proc_analyser.process_sample (ratio);
		_meter_result._dsp_use._peak = float (_proc_analyser.get_peak_hold ());
		_meter_result._dsp_use._rms  = float (_proc_analyser.get_rms ());
		_period_now = float (dur_tot.count ()) * _rate_expected;
	}
	_proc_date_beg = date_beg;

	// A problem occured...
	if (_reset_flag)
	{
		reset_everything ();
		_reset_flag = false;
	}

	// Collects messages from the command thread
	collect_msg_cmd (true);

	// Collects messages from the user input thread
	collect_msg_ui (true);

	// Audio processing
	if (_ctx_ptr != 0)
	{
#if defined (mfx_WorldAudio_BUF_REC)
		if (_data_rec_flag)
		{
			_data_rec_cur_buf = 0;
		}
#endif

		copy_input (src_arr, nbr_spl);

		const ProcessingContext::PluginCtxArray & pi_ctx_arr =
			_ctx_ptr->_context_arr;
		for (const auto & pi_ctx : pi_ctx_arr)
		{
			process_plugin_bundle (pi_ctx, nbr_spl);
		}

		copy_output (dst_arr, nbr_spl);

		check_signal_level (dst_arr, src_arr, nbr_spl);

#if defined (mfx_WorldAudio_BUF_REC)
		if (_data_rec_flag)
		{
			_data_rec_pos += nbr_spl;
			if (_data_rec_pos >= _data_rec_len)
			{
				_data_rec_flag = false;
			}
		}
#endif
	}

	if (_tempo_new > 0)
	{
		_tempo_cur = _tempo_new;
		_tempo_new = 0;
	}

	_proc_date_end = _input_device.get_cur_date ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	WorldAudio::reset_everything ()
{
	const ProcessingContext::PluginCtxArray & pi_ctx_arr =
		_ctx_ptr->_context_arr;
	for (const auto & pi_ctx : pi_ctx_arr)
	{
		reset_plugin (pi_ctx._node_arr [PiType_MAIN]._pi_id);
		if (pi_ctx._mixer_flag)
		{
			reset_plugin (pi_ctx._node_arr [PiType_MIX]._pi_id);
		}
	}

	_lvl_meter->clear_peak ();
	_lvl_meter->clear_buffers ();
}



void	WorldAudio::reset_plugin (int pi_id)
{
	PluginPool::PluginDetails &  details = _pi_pool.use_plugin (pi_id);
	int            dummy_lat = 0;
	details._pi_uptr->reset (_sample_freq, _max_block_size, dummy_lat);
}



void	WorldAudio::collect_msg_cmd (bool proc_flag)
{
	bool           ctx_update_flag = false;

	conc::LockFreeCell <WaMsg> * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_from_cmd.dequeue ();
		if (cell_ptr != 0)
		{
			// Reply to our own messages
			if (cell_ptr->_val._sender == WaMsg::Sender_AUDIO)
			{
				// Nothing more here
				_msg_pool_cmd.return_cell (*cell_ptr);
			}

			// Message from the command thread
			else
			{
				bool           ret_flag = true;

				switch (cell_ptr->_val._type)
				{
				case WaMsg::Type_CTX:
					if (proc_flag)
					{
						handle_msg_ctx (cell_ptr->_val._content._ctx);
						ctx_update_flag = true;
					}
					break;
				case WaMsg::Type_PARAM:
					if (proc_flag)
					{
						handle_msg_param (cell_ptr->_val._content._param);
					}
					ret_flag = false;
					break;
				case WaMsg::Type_TEMPO:
					if (proc_flag)
					{
						handle_msg_tempo (cell_ptr->_val._content._tempo);
					}
					ret_flag = false;
					break;

				default:
					assert (false);
					break;
				}

				// Returns the message to the sender so they know it has been
				// taken into account (if there are resources to free for ex.)
				if (ret_flag)
				{
					_queue_to_cmd.enqueue (*cell_ptr);
				}
				else
				{
					_msg_pool_cmd.return_cell (*cell_ptr);
				}
			}
		}
	}
	while (cell_ptr != 0);

	if (ctx_update_flag)
	{
		update_aux_param ();
	}

#if defined (mfx_WorldAudio_BUF_REC)
	if (_data_rec_pos >= _data_rec_len)
	{
		while (! _data_rec_arr.empty () && _data_rec_arr.back ().empty ())
		{
			_data_rec_arr.pop_back ();
		}
		if (! _data_rec_arr.empty ())
		{
			std::vector <const float *> buf_arr;
			for (auto &buf_vec : _data_rec_arr)
			{
				buf_arr.push_back (buf_vec.data ());
			}

			FileOpWav::save (
				"audiodump.wav",
				buf_arr.data (),
				_data_rec_len,
				int (buf_arr.size ()),
				_sample_freq,
				1
			);
		}
		_data_rec_pos = 0;
	}
#endif
}



void	WorldAudio::collect_msg_ui (bool proc_flag)
{
	ui::UserInputInterface::MsgCell * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_from_input.dequeue ();
		if (cell_ptr != 0)
		{
			ControlSource  controller;
			controller._type       = ControllerType (cell_ptr->_val.get_type ());
			controller._index      = cell_ptr->_val.get_index ();
			const float    val_raw = cell_ptr->_val.get_val ();

			if (_ctx_ptr != 0 && proc_flag)
			{
				handle_controller (controller, val_raw);
			}

			_input_device.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);
}



void	WorldAudio::update_aux_param ()
{
	for (const auto &pi_ctx : _ctx_ptr->_context_arr)
	{
		update_aux_param_pi (pi_ctx._node_arr [PiType_MAIN]);
		if (pi_ctx._mixer_flag)
		{
			update_aux_param_pi (pi_ctx._node_arr [PiType_MIX]);
		}
	}
}



void	WorldAudio::update_aux_param_pi (const ProcessingContextNode &node)
{
	if (node._aux_param_flag)
	{
		PluginPool::PluginDetails &  details = _pi_pool.use_plugin (node._pi_id);
		cmd::DelayInterface *	delay_ptr =
			dynamic_cast <cmd::DelayInterface *> (details._pi_uptr.get ());
		if (delay_ptr == 0)
		{
			assert (false);
		}
		else
		{
			delay_ptr->set_aux_param (node._comp_delay, node._pin_mult);
		}
	}
}



void	WorldAudio::handle_controller (const ControlSource &controller, float val_raw)
{
	assert (_ctx_ptr != 0);

	// Updates controller unit values
	const ProcessingContext::MapSourceUnit &  map_src_unit =
		_ctx_ptr->_map_src_unit;
	auto           it_unit = map_src_unit.find (controller);
	while (it_unit != map_src_unit.end () && controller == it_unit->first)
	{
		CtrlUnit &     unit = *(it_unit->second);
		assert (unit._source == controller);

		unit.update_abs_val (val_raw);

		++ it_unit;
	}

	// Notifies changed parameters
	const ProcessingContext::MapSourceParam & map_src_param =
		_ctx_ptr->_map_src_param;
	auto           it_param = map_src_param.find (controller);
	while (it_param != map_src_param.end () && controller == it_param->first)
	{
		ControlledParam &    param = (*it_param->second);
		const ParamCoord &   coord = param.use_coord ();
		const int            pi_id = coord._plugin_id;
		const int            index = coord._param_index;

		PluginPool::PluginDetails &   details = _pi_pool.use_plugin (pi_id);
		details._param_update.fill_bit (index);

		// If the parameter is directly linked to the controller,
		// we update the value immediately and send a message
		// to the command thread.
		mfx::ControlledParam::CtrlUnitList &   unit_list =
			param.use_unit_list ();
		if (! unit_list.empty ())
		{
			mfx::CtrlUnit &   unit_first = *(unit_list [0]);
			if (   unit_first._source == controller
			    && unit_first._abs_flag)
			{
				float          base_val = unit_first.evaluate (0);
				base_val = fstb::limit (base_val, 0.f, 1.f);
				details._param_arr [index] = base_val;
				details._param_update_from_audio [index] = true;

				conc::LockFreeCell <WaMsg> * cell_ptr =
					_msg_pool_cmd.take_cell (true);
				if (cell_ptr != 0)
				{
					cell_ptr->_val._sender                    = WaMsg::Sender_AUDIO;
					cell_ptr->_val._type                      = WaMsg::Type_PARAM;
					cell_ptr->_val._content._param._plugin_id = pi_id;
					cell_ptr->_val._content._param._index     = index;
					cell_ptr->_val._content._param._val       = base_val;

					_queue_to_cmd.enqueue (*cell_ptr);
				}
			}
		}

		++ it_param;
	}
}



void	WorldAudio::copy_input (const float * const * src_arr, int nbr_spl)
{
	typedef dsp::mix::Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <false>
	> MixUnalignToAlign;
	typedef dsp::mix::Align MixAlign;

	const ProcessingContextNode::Side & side =
		_ctx_ptr->_interface_ctx._side_arr [Dir_IN];

	if (side._nbr_chn_tot == 2)
	{
		const int      buf_0_index = side._buf_arr [0];
		const int      buf_1_index = side._buf_arr [1];
		assert (buf_0_index >= 0);
		assert (buf_0_index < int (_buf_arr.size ()));
		assert (buf_1_index >= 0);
		assert (buf_1_index < int (_buf_arr.size ()));
		float *        buf_0_ptr   = _buf_arr [buf_0_index];
		float *        buf_1_ptr   = _buf_arr [buf_1_index];
		MixUnalignToAlign::copy_2_2 (
			buf_0_ptr, buf_1_ptr,
			src_arr [0], src_arr [1],
			nbr_spl
		);
	}
	else
	{
		for (int chn = 0; chn < side._nbr_chn_tot; ++chn)
		{
			const int      buf_index = side._buf_arr [chn];
			assert (buf_index >= 0);
			assert (buf_index < int (_buf_arr.size ()));
			float *        buf_ptr   = _buf_arr [buf_index];
			MixUnalignToAlign::copy_1_1 (buf_ptr, src_arr [chn], nbr_spl);
		}
	}

	if (_fade_chnmap != 0)
	{

/*** To do:
Fading the input is not enough, we should also mix the output with a
fade out version of the extrapolation of the previous output.
We can use LPC: build a filter (16 coef?) from the last known output
samples (buffer them first), filter with LPC the previous buffer, use
the filter output + a time-reverted version of it to feed the inverse
LPC filter and keep the second part. Fade it out and mix.
LPC code:
https://www.dsprelated.com/showthread/comp.dsp/119064-1.php
https://www.dsprelated.com/showthread/comp.dsp/134663-1.php
***/

		const int      fade_len = std::min (
			fstb::floor_int (_sample_freq * 0.0015f),
			nbr_spl
		);
		for (int chn = 0; chn < side._nbr_chn_tot; ++chn)
		{
			if (((_fade_chnmap >> chn) & 1) != 0)
			{
				const int      buf_index = side._buf_arr [chn];
				float *        buf_ptr   = _buf_arr [buf_index];
				MixAlign::scale_1_vlr (buf_ptr, fade_len, 0, 1);
			}
		}
		_fade_chnmap = 0;
	}

#if defined (mfx_WorldAudio_BUF_REC)
	if (_data_rec_flag)
	{
		for (int chn = 0; chn < side._nbr_chn_tot; ++chn)
		{
			const int      buf_index = side._buf_arr [chn];
			store_data (_buf_arr [buf_index], nbr_spl);
		}
	}
#endif
}



void	WorldAudio::check_signal_level (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	std::array <const float *, 4> data_ptr_arr = {{
		src_arr [0],
		src_arr [1],
		dst_arr [0],
		dst_arr [1]
	}};

	_lvl_meter->process_block (&data_ptr_arr [0], nbr_spl);

	const auto     peak  = _lvl_meter->get_peak ();
	const auto     hold  = _lvl_meter->get_peak_hold ();
	const auto     rms   = _lvl_meter->get_rms ();
	_lvl_meter->clear_peak ();

	_meter_result._audio_io [Dir_IN ]._chn_arr [0]._peak = fstb::ToolsSimd::Shift <0>::extract (hold);
	_meter_result._audio_io [Dir_IN ]._chn_arr [0]._rms  = fstb::ToolsSimd::Shift <0>::extract (rms );
	_meter_result._audio_io [Dir_IN ]._chn_arr [1]._peak = fstb::ToolsSimd::Shift <1>::extract (hold);
	_meter_result._audio_io [Dir_IN ]._chn_arr [1]._rms  = fstb::ToolsSimd::Shift <1>::extract (rms );
	_meter_result._audio_io [Dir_OUT]._chn_arr [0]._peak = fstb::ToolsSimd::Shift <2>::extract (hold);
	_meter_result._audio_io [Dir_OUT]._chn_arr [0]._rms  = fstb::ToolsSimd::Shift <2>::extract (rms );
	_meter_result._audio_io [Dir_OUT]._chn_arr [1]._peak = fstb::ToolsSimd::Shift <3>::extract (hold);
	_meter_result._audio_io [Dir_OUT]._chn_arr [1]._rms  = fstb::ToolsSimd::Shift <3>::extract (rms );

	const float    p_i_0 = fstb::ToolsSimd::Shift <0>::extract (peak);
	const float    p_i_1 = fstb::ToolsSimd::Shift <1>::extract (peak);
	const float    p_o_0 = fstb::ToolsSimd::Shift <2>::extract (peak);
	const float    p_o_1 = fstb::ToolsSimd::Shift <3>::extract (peak);

	const float    p_i   = std::max (p_i_0, p_i_1);
	if (p_i > Cst::_clip_lvl)
	{
		_meter_result._audio_io [Dir_IN ]._clip_flag.exchange (true);
	}

	float          p_o   = p_o_0;
	if (_ctx_ptr->_nbr_chn_out > 1)
	{
		p_o = std::max (p_o_0, p_o_1);
	}
	if (p_o > Cst::_clip_lvl)
	{
		_meter_result._audio_io [Dir_OUT]._clip_flag.exchange (true);
	}
}



void	WorldAudio::copy_output (float * const * dst_arr, int nbr_spl)
{
	typedef dsp::mix::Simd <
		fstb::DataAlign <false>,
		fstb::DataAlign <true>
	> MixAlignToUnalign;

	if (! _ctx_ptr->_interface_mix.empty ())
	{
		mix_source_channels (
			_ctx_ptr->_interface_ctx._side_arr [Dir_OUT],
			_ctx_ptr->_interface_mix,
			nbr_spl
		);
	}

	const ProcessingContextNode::Side & side =
		_ctx_ptr->_interface_ctx._side_arr [Dir_OUT];

	const int      buf_0_index = side._buf_arr [0];
	const float *  buf_0_ptr   = _buf_arr [buf_0_index];

	if (side._nbr_chn == 2)
	{
		const int      buf_1_index = side._buf_arr [1];
		assert (buf_0_index >= 0);
		assert (buf_0_index < int (_buf_arr.size ()));
		assert (buf_1_index >= 0);
		assert (buf_1_index < int (_buf_arr.size ()));
		const float *  buf_1_ptr   = _buf_arr [buf_1_index];
		if (_ctx_ptr->_nbr_chn_out == 2)
		{
			MixAlignToUnalign::copy_2_2_v (
				dst_arr [0], dst_arr [1],
				buf_0_ptr, buf_1_ptr,
				nbr_spl,
				_ctx_ptr->_master_vol
			);
		}
		else
		{
			assert (_ctx_ptr->_nbr_chn_out == 1);
			MixAlignToUnalign::copy_2_1_v (
				dst_arr [0],
				buf_0_ptr, buf_1_ptr,
				nbr_spl,
				_ctx_ptr->_master_vol * 0.5f
			);
		}
	}
	else
	{
		if (_ctx_ptr->_nbr_chn_out == 2)
		{
			MixAlignToUnalign::copy_1_2_v (
				dst_arr [0], dst_arr [1],
				buf_0_ptr,
				nbr_spl,
				_ctx_ptr->_master_vol
			);
		}
		else if (_ctx_ptr->_nbr_chn_out == 1)
		{
			MixAlignToUnalign::copy_1_1_v (
				dst_arr [0],
				buf_0_ptr,
				nbr_spl,
				_ctx_ptr->_master_vol
			);
		}
		else
		{
			for (int chn = 0; chn < side._nbr_chn; ++chn)
			{
				const int      buf_index = side._buf_arr [chn];
				assert (buf_index >= 0);
				assert (buf_index < int (_buf_arr.size ()));
				const float *  buf_ptr   = _buf_arr [buf_index];
				MixAlignToUnalign::copy_1_1_v (
					dst_arr [chn],
					buf_ptr,
					nbr_spl,
					_ctx_ptr->_master_vol
				);
			}
		}
	}

	if (_ctx_ptr->_nbr_chn_out == 1)
	{
		MixAlignToUnalign::copy_1_1 (dst_arr [1], dst_arr [0], nbr_spl);
	}
}



void	WorldAudio::process_plugin_bundle (const ProcessingContext::PluginContext &pi_ctx, int nbr_spl)
{
	// Source mix
	if (! pi_ctx._mix_in_arr.empty ())
	{
		mix_source_channels (
			pi_ctx._node_arr [PiType_MAIN]._side_arr [Dir_IN],
			pi_ctx._mix_in_arr,
			nbr_spl
		);
	}

	// Plug-in processing
	if (pi_ctx._node_arr [PiType_MAIN]._pi_id >= 0)
	{
		piapi::ProcInfo   proc_info;
		BufSrcArr      src_arr;
		BufDstArr      dst_arr;
		BufDstArr      byp_arr;
		BufSigArr      sig_arr;

		proc_info._src_arr = &src_arr [0];
		proc_info._dst_arr = &dst_arr [0];
		proc_info._byp_arr = &byp_arr [0];
		proc_info._sig_arr = &sig_arr [0];
		proc_info._nbr_spl = nbr_spl;
		proc_info._nbr_evt = 0;
		proc_info._evt_arr = 0;

		// Main plug-in
		proc_info._byp_state = (pi_ctx._mixer_flag)
			? piapi::BypassState_ASK
			: piapi::BypassState_IGNORE;
		prepare_buffers (
			proc_info,
			pi_ctx._node_arr [PiType_MAIN],
			pi_ctx._bypass_buf_arr,
			false
		);

		process_single_plugin (pi_ctx._node_arr [PiType_MAIN]._pi_id, proc_info);

#if defined (mfx_WorldAudio_BUF_REC)
		if (_data_rec_flag)
		{
			// Records only the first output pin
			const int      nbr_chn = proc_info._nbr_chn_arr [Dir_OUT];
			if (nbr_chn > 0)
			{
				for (int chn = 0; chn < nbr_chn; ++chn)
				{
					store_data (proc_info._dst_arr [chn], proc_info._nbr_spl);
				}
			}
		}
#endif

		handle_signals (proc_info, pi_ctx._node_arr [PiType_MAIN]);

		const bool       bypass_produced_flag =
			(proc_info._byp_state == piapi::BypassState_PRODUCED);

		// Bypass/Mix/Gain
		/*** To do: Is it the right test? or just pi_ctx._mixer_flag? ***/
		if (proc_info._byp_state)
		{
			proc_info._byp_state = piapi::BypassState_IGNORE;
			prepare_buffers (
				proc_info,
				pi_ctx._node_arr [PiType_MIX],
				pi_ctx._bypass_buf_arr,
				bypass_produced_flag
			);

			process_single_plugin (pi_ctx._node_arr [PiType_MIX]._pi_id, proc_info);
		}
	}
}



// Fills the event-related members in proc_info.
void	WorldAudio::process_single_plugin (int plugin_id, piapi::ProcInfo &proc_info)
{
	assert (plugin_id >= 0);

	_evt_arr.clear ();
	_evt_ptr_arr.clear ();

	// Checks the tempo
	if (_tempo_new > 0)
	{
		piapi::EventTs    evt;
		evt._timestamp = 0;
		evt._type      = piapi::EventType_TRANSPORT;
		evt._evt._transport._tempo = _tempo_new;
		evt._evt._transport._flags = piapi::EventTransport::Flag_TEMPO;
		_evt_arr.push_back (evt);
	}

	// Handles modulations and automations
	PluginPool::PluginDetails &  details = _pi_pool.use_plugin (plugin_id);
	if (details._param_update.has_a_bit_set ())
	{
		fstb::BitFieldSparseIterator param_it (details._param_update);
		for (param_it.start (); param_it.is_rem_elt (); param_it.iterate ())
		{
			const int      index = param_it.get_bit_index ();

			// Updates the final value, checks if there is a modulation
			float          val   = 0;
			const ProcessingContext::MapParamCtrl &   map_param_ctrl =
				_ctx_ptr->_map_param_ctrl;
			const ParamCoord  coord (plugin_id, index);
			const auto     ctrl_it = map_param_ctrl.find (coord);
			if (ctrl_it == map_param_ctrl.end ())
			{
				val = details._param_arr [index];
				details._param_mod_arr [index] = -1;
			}
			else
			{
				ControlledParam & ctrl = *(ctrl_it->second);
				val = ctrl.compute_final_val (_pi_pool);
				details._param_mod_arr [index] = val;
			}

			// Adds a parameter event
			piapi::EventTs    evt;
			evt._timestamp = 0;
			evt._type      = piapi::EventType_PARAM;
			evt._evt._param._categ = piapi::ParamCateg_GLOBAL;
			evt._evt._param._index = index;
			evt._evt._param._val   = val;
			_evt_arr.push_back (evt);
		}

		details._param_update.clear ();
	}

	// Finalizes the event list
	proc_info._nbr_evt = int (_evt_arr.size ());
	if (_evt_arr.empty ())
	{
		proc_info._evt_arr = 0;
	}
	else
	{
		for (auto &evt : _evt_arr)
		{
			_evt_ptr_arr.push_back (&evt);
		}
		proc_info._evt_arr = &_evt_ptr_arr [0];
	}

	// Audio processing now
	piapi::PluginInterface &  plugin = *(details._pi_uptr);
	plugin.process_block (proc_info);

	// Checks the output
	if (proc_info._dir_arr [Dir_OUT]._nbr_chn > 0)
	{
		const float          val = proc_info._dst_arr [0] [0];
		static const float   thr = 1e9f;
		if (! std::isfinite (val) || val < -thr || val > thr)
		{
			_reset_flag = true;
		}
	}
}



void	WorldAudio::mix_source_channels (const ProcessingContextNode::Side &side, const ProcessingContext::PluginContext::MixInputArray &mix_in_arr, int nbr_spl)
{
	assert (nbr_spl > 0);
	assert (int (mix_in_arr.size ()) == side._nbr_chn_tot);

	for (int chn_dst_cnt = 0; chn_dst_cnt < side._nbr_chn_tot; ++chn_dst_cnt)
	{
		const ProcessingContext::PluginContext::MixInChn & mix_info =
			mix_in_arr [chn_dst_cnt];
		if (! mix_info.empty ())
		{
			const int      nbr_chn_src = int (mix_info.size ());
			assert (nbr_chn_src >= 2);

			const int      buf_dst   = side._buf_arr [chn_dst_cnt];
			float * const  dst_ptr   = _buf_arr [buf_dst];

			// Mixes two inputs at a time
			int            buf_src_1 = mix_info [0];
			int            buf_src_2 = mix_info [1];
			const float *  src_1_ptr = _buf_arr [buf_src_1];
			const float *  src_2_ptr = _buf_arr [buf_src_2];
			dsp::mix::Align::copy_2_1 (dst_ptr, src_1_ptr, src_2_ptr, nbr_spl);

			const int      ncs2 = nbr_chn_src & ~1;
			for (int chn_src_cnt = 2; chn_src_cnt < ncs2; chn_src_cnt += 2)
			{
				buf_src_1 = mix_info [mix_info [chn_src_cnt    ]];
				buf_src_2 = mix_info [mix_info [chn_src_cnt + 1]];
				src_1_ptr = _buf_arr [buf_src_1];
				src_2_ptr = _buf_arr [buf_src_2];
				dsp::mix::Align::mix_2_1 (dst_ptr, src_1_ptr, src_2_ptr, nbr_spl);
			}

			if (ncs2 < nbr_chn_src)
			{
				assert (ncs2 == nbr_chn_src - 1);
				src_1_ptr = _buf_arr [mix_info [ncs2]];
				dsp::mix::Align::mix_1_1 (dst_ptr, src_1_ptr, nbr_spl);
			}
		}
	}
}



// When use_byp_as_src_flag is set, the content of byp_arr is used for
// the odd input pins (the bypass pins from the mixer plug-in).
// By default, for the mixer plug-in, the bypass channels are filled
// with the input buffers from the main plug-in. However we can
// explicitely request to use the bypass output, if it was generated.
void	WorldAudio::prepare_buffers (piapi::ProcInfo &proc_info, const ProcessingContextNode &node, const ProcessingContext::PluginContext::BypBufArray &bypass_buf_arr, bool use_byp_as_src_flag)
{
	const float ** src_arr = const_cast <const float **> (proc_info._src_arr);
	float **       dst_arr = const_cast <      float **> (proc_info._dst_arr);
	float **       byp_arr = const_cast <      float **> (proc_info._byp_arr);
	float **       sig_arr = const_cast <      float **> (proc_info._sig_arr);

	const ProcessingContextNode::Side & side_i = node._side_arr [Dir_IN ];
	const ProcessingContextNode::Side & side_o = node._side_arr [Dir_OUT];

	// Sets the input buffers
	if (use_byp_as_src_flag)
	{
		// Make sure we're operating on a dry/wet mix plug-in
		assert (side_i._nbr_chn_tot == side_o._nbr_chn_tot * 2);
		assert (side_i._nbr_chn     == side_o._nbr_chn);

		// Uses the bypass buffers for every odd pin
		const int      nbr_chn_tot = side_o._nbr_chn_tot;
		for (int chn_ofs = 0; chn_ofs < nbr_chn_tot; chn_ofs += side_i._nbr_chn)
		{
			for (int chn = 0; chn < side_i._nbr_chn; ++ chn)
			{
				const int      buf_src = side_i._buf_arr [chn_ofs + chn];
				assert (buf_src >= 0);
				assert (buf_src < int (_buf_arr.size ()));
				src_arr [chn_ofs * 2 + chn] =
					_buf_arr [buf_src];

				const int      buf_byp = bypass_buf_arr [chn_ofs + chn];
				assert (buf_byp >= 0);
				assert (buf_byp < int (_buf_arr.size ()));
				src_arr [chn_ofs * 2 + side_i._nbr_chn + chn] = _buf_arr [buf_byp];
			}
		}
	}
	else
	{
		// Standard input buffers
		for (int chn = 0; chn < side_i._nbr_chn_tot; ++ chn)
		{
			const int      buf_index = side_i._buf_arr [chn];
			assert (buf_index >= 0);
			assert (buf_index < int (_buf_arr.size ()));
			src_arr [chn] = _buf_arr [buf_index];
		}
	}

	// Sets the output buffers
	for (int chn = 0; chn < side_o._nbr_chn_tot; ++ chn)
	{
		const int      buf_index = side_o._buf_arr [chn];
		assert (buf_index >= 0);
		assert (buf_index < int (_buf_arr.size ()));
		dst_arr [chn] = _buf_arr [buf_index];
	}

	// Sets the bypass buffers (for the D/W mix plug-in)
	if (bypass_buf_arr [0] >= 0)
	{
		for (int chn = 0; chn < side_o._nbr_chn_tot; ++ chn)
		{
			const int      buf_index = bypass_buf_arr [chn];
			assert (buf_index >= 0);
			assert (buf_index < int (_buf_arr.size ()));
			byp_arr [chn] = _buf_arr [buf_index];
		}
	}

	// Sets the signal buffers
	for (int sig = 0; sig < node._nbr_sig; ++sig)
	{
		const int      buf_index = node._sig_buf_arr [sig]._buf_index;
		assert (buf_index >= 0);
		assert (buf_index < int (_buf_arr.size ()));
		sig_arr [sig] = _buf_arr [buf_index];
	}

	// Sets the number of channels
	for (int dir = 0; dir < Dir_NBR_ELT; ++dir)
	{
		proc_info._dir_arr [dir]._nbr_chn = node._side_arr [dir]._nbr_chn;
	}
}



void	WorldAudio::handle_signals (piapi::ProcInfo &proc_info, const ProcessingContextNode &node)
{
	for (int sig_pos = 0; sig_pos < node._nbr_sig; ++sig_pos)
	{
		const ProcessingContextNode::SigInfo & sig_info =
			node._sig_buf_arr [sig_pos];
		if (sig_info._buf_index >= 0 && sig_info._port_index >= 0)
		{
			const float *  buf_ptr = proc_info._sig_arr [sig_pos];
			assert (buf_ptr != 0);
			const float    val     = buf_ptr [0];
			_sig_res_arr [sig_info._port_index] = val;

			ControlSource  controller;
			controller._type       = ControllerType_FX_SIG;
			controller._index      = sig_info._port_index;
			handle_controller (controller, val);
		}
	}
}



void	WorldAudio::handle_msg_ctx (WaMsg::Ctx &msg)
{
	std::swap (_ctx_ptr, msg._ctx_ptr);
	_fade_chnmap = msg._fade_chnmap;

	// Automatic tempo refresh (for the new plug-ins)
	_tempo_new = _tempo_cur;
}



void	WorldAudio::handle_msg_param (WaMsg::Param &msg)
{
	const int      index = msg._index;
	PluginPool::PluginDetails &   details =
		_pi_pool.use_plugin (msg._plugin_id);
	details._param_arr [index] = msg._val;
	details._param_update.fill_bit (index);
	details._param_update_from_audio [index] = false;
}



void	WorldAudio::handle_msg_tempo (WaMsg::Tempo &msg)
{
	if (msg._bpm == 0)
	{
		_tempo_new = _tempo_cur;
	}
	else
	{
		_tempo_new = msg._bpm;
	}
}



#if defined (mfx_WorldAudio_BUF_REC)



void	WorldAudio::store_data (const float src_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	size_t         idx = _data_rec_cur_buf;
	if (idx >= _data_rec_arr.size ())
	{
		_data_rec_arr.resize (idx + 1);
	}
	AlignedZone &  zone = _data_rec_arr [idx];

	if (zone.empty ())
	{
		zone.resize (_data_rec_len, 0);
	}

	const size_t   rem_len  = zone.size () - _data_rec_pos;
	const size_t   work_len = std::min (size_t (nbr_spl), rem_len);
	dsp::mix::Align::copy_1_1 (&zone [_data_rec_pos], src_ptr, int (work_len));

	++ _data_rec_cur_buf;
}



#endif



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
