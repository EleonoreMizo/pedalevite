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
#include "mfx/dsp/mix/Simd.h"
#include "mfx/Msg.h"
#include "mfx/MsgQueue.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"
#include "mfx/WorldAudio.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



WorldAudio::WorldAudio (PluginPool &plugin_pool, MsgQueue &queue_from_cmd, MsgQueue &queue_to_cmd, ui::UserInputInterface::MsgQueue &queue_from_input, ui::UserInputInterface &input_device, conc::CellPool <Msg> &msg_pool_cmd)
:	_pi_pool (plugin_pool)
,	_queue_from_cmd (queue_from_cmd)
,	_queue_to_cmd (queue_to_cmd)
,	_queue_from_input (queue_from_input)
,	_input_device (input_device)
,	_msg_pool_cmd (msg_pool_cmd)
,	_max_block_size (0)
,	_sample_freq (0)
,	_ctx_ptr (0)
,	_evt_arr ()
,	_evt_ptr_arr ()
{
	_evt_arr.reserve (_max_nbr_evt);
	_evt_ptr_arr.reserve (_max_nbr_evt);
}



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
}



void	WorldAudio::set_context (const ProcessingContext &ctx)
{
	_ctx_ptr = &ctx;
}



void	WorldAudio::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	assert (dst_arr != 0);
	assert (dst_arr [0] != 0);
	assert (src_arr != 0);
	assert (src_arr [0] != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl < _max_block_size);

	// Collects messages from the command thread
	collect_msg_cmd ();

	// Collects messages from the user input thread
	collect_msg_ui ();

	// Audio processing
	assert (_ctx_ptr != 0);

	copy_input (src_arr, nbr_spl);

	const ProcessingContext::PluginCtxArray & pi_ctx_arr =
		_ctx_ptr->_context_arr;
	for (const auto & pi_ctx : pi_ctx_arr)
	{
		process_plugin_bundle (pi_ctx, nbr_spl);
	}

	copy_output (dst_arr, nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	WorldAudio::collect_msg_cmd ()
{
	conc::LockFreeCell <Msg> * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_from_cmd.dequeue ();
		if (cell_ptr != 0)
		{
			// Reply to our own messages
			if (cell_ptr->_val._sender == Msg::Sender_AUDIO)
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
				case Msg::Type_CTX:
					handle_msg_ctx (cell_ptr->_val._content._ctx);
					break;
				case Msg::Type_PARAM:
					handle_msg_param (cell_ptr->_val._content._param);
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
}



void	WorldAudio::collect_msg_ui ()
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

			handle_controller (controller, val_raw);

			_input_device.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);
}



void	WorldAudio::handle_controller (const ControlSource &controller, float val_raw)
{
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
		// updates the value immediately and send a message to the
		// command thread.
		mfx::ControlledParam::CtrlUnitList &   unit_list =
			param.use_unit_list ();
		if (unit_list.empty ())
		{
			mfx::CtrlUnit &   unit_first = *(unit_list [0]);
			if (   unit_first._source == controller
			    && unit_first._abs_flag)
			{
				const float    base_val = unit_first.evaluate (0);
				details._param_arr [index] =
					fstb::limit (base_val, 0.f, 1.f);
				details._param_update_from_audio [index] = true;

				conc::LockFreeCell <Msg> * cell_ptr =
					_msg_pool_cmd.take_cell (true);
				if (cell_ptr != 0)
				{
					cell_ptr->_val._sender                    = Msg::Sender_AUDIO;
					cell_ptr->_val._type                      = Msg::Type_PARAM;
					cell_ptr->_val._content._param._plugin_id = pi_id;
					cell_ptr->_val._content._param._index     = index;
					cell_ptr->_val._content._param._val       = base_val;

					_queue_to_cmd.enqueue (*cell_ptr);
				}
			}
		}
	}
}



void	WorldAudio::copy_input (const float * const * src_arr, int nbr_spl)
{
	typedef dsp::mix::Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <false>
	> MixUnalignToAlign;

	const ProcessingContextNode::Side & side =
		_ctx_ptr->_interface_ctx._side_arr [piapi::PluginInterface::Dir_IN];

	if (side._nbr_chn == 2)
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
		for (int chn = 0; chn < side._nbr_chn; ++chn)
		{
			const int      buf_index = side._buf_arr [chn];
			assert (buf_index >= 0);
			assert (buf_index < int (_buf_arr.size ()));
			float *        buf_ptr   = _buf_arr [buf_index];
			MixUnalignToAlign::copy_1_1 (buf_ptr, src_arr [chn], nbr_spl);
		}
	}
}



void	WorldAudio::copy_output (float * const * dst_arr, int nbr_spl)
{
	typedef dsp::mix::Simd <
		fstb::DataAlign <false>,
		fstb::DataAlign <true>
	> MixAlignToUnalign;

	const ProcessingContextNode::Side & side =
		_ctx_ptr->_interface_ctx._side_arr [piapi::PluginInterface::Dir_OUT];

	if (side._nbr_chn == 2)
	{
		const int      buf_0_index = side._buf_arr [0];
		const int      buf_1_index = side._buf_arr [1];
		assert (buf_0_index >= 0);
		assert (buf_0_index < int (_buf_arr.size ()));
		assert (buf_1_index >= 0);
		assert (buf_1_index < int (_buf_arr.size ()));
		float *        buf_0_ptr   = _buf_arr [buf_0_index];
		float *        buf_1_ptr   = _buf_arr [buf_1_index];
		MixAlignToUnalign::copy_2_2 (
			dst_arr [0], dst_arr [1],
			buf_0_ptr, buf_1_ptr,
			nbr_spl
		);
	}
	else
	{
		for (int chn = 0; chn < side._nbr_chn; ++chn)
		{
			const int      buf_index = side._buf_arr [chn];
			assert (buf_index >= 0);
			assert (buf_index < int (_buf_arr.size ()));
			float *        buf_ptr   = _buf_arr [buf_index];
			MixAlignToUnalign::copy_1_1 (dst_arr [chn], buf_ptr, nbr_spl);
		}
	}
}



void	WorldAudio::process_plugin_bundle (const ProcessingContext::PluginContext &pi_ctx, int nbr_spl)
{
	piapi::PluginInterface::ProcInfo proc_info;
	BufSrcArr      src_arr;
	BufDstArr      dst_arr;
	BufDstArr      byp_arr;

	proc_info._src_arr = &src_arr [0];
	proc_info._dst_arr = &dst_arr [0];
	proc_info._byp_arr = &byp_arr [0];
	proc_info._nbr_spl = nbr_spl;
	proc_info._nbr_evt = 0;
	proc_info._evt_arr = 0;

	// Main plug-in
	proc_info._byp_state = piapi::PluginInterface::BypassState_ASK;
	prepare_buffers (proc_info, pi_ctx._main_pi, false);

	process_single_plugin (pi_ctx._main_pi._pi_id, proc_info);

	const bool       bypass_produced_flag =
		(proc_info._byp_state == piapi::PluginInterface::BypassState_PRODUCED);

	// Bypass/Mix/Gain
	proc_info._byp_state = piapi::PluginInterface::BypassState_IGNORE;
	prepare_buffers (proc_info, pi_ctx._mixer, bypass_produced_flag);

	process_single_plugin (pi_ctx._mixer._pi_id, proc_info);
}



// Fills the event-related members in proc_info.
void	WorldAudio::process_single_plugin (int plugin_id, piapi::PluginInterface::ProcInfo &proc_info)
{
	_evt_arr.clear ();
	_evt_ptr_arr.clear ();

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
			}
			else
			{
				ControlledParam & ctrl = *(ctrl_it->second);
				val = ctrl.compute_final_val (_pi_pool);
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
	proc_info._nbr_evt = int (_evt_ptr_arr.size ());
	if (_evt_ptr_arr.empty ())
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
}



// Content of byp_arr is used for the second half of the source channels
// if use_byp_as_src_flag is set.
void	WorldAudio::prepare_buffers (piapi::PluginInterface::ProcInfo &proc_info, const ProcessingContextNode &node, bool use_byp_as_src_flag)
{
	const float ** src_arr = const_cast <const float **> (proc_info._src_arr);
	float **       dst_arr = const_cast <float **      > (proc_info._dst_arr);
	float **       byp_arr = const_cast <float **      > (proc_info._byp_arr);

	const ProcessingContextNode::Side & side_i =
		node._side_arr [piapi::PluginInterface::Dir_IN ];
	const ProcessingContextNode::Side & side_o =
		node._side_arr [piapi::PluginInterface::Dir_OUT];

	// By default, for the mixer plug-in, the bypass channels are filled
	// with the input buffers from the main plug-in. However we can
	// explicitely request to use the bypass output.
	int           src_end = side_i._nbr_chn_tot;
	int           nbr_byp = 0;
	if (use_byp_as_src_flag)
	{
		assert ((src_end & 1) == 0);
		src_end >>= 1;
		nbr_byp   = src_end;
	}

	for (int chn = 0; chn < src_end; ++ chn)
	{
		const int      buf_index = side_i._buf_arr [chn];
		assert (buf_index >= 0);
		assert (buf_index < int (_buf_arr.size ()));
		src_arr [chn] = _buf_arr [buf_index];
	}
	for (int chn = 0; chn < nbr_byp; ++ chn)
	{
		src_arr [src_end + chn] = byp_arr [chn];
	}

	for (int chn = 0; chn < side_o._nbr_chn_tot; ++ chn)
	{
		const int      buf_index = side_o._buf_arr [chn];
		assert (buf_index >= 0);
		assert (buf_index < int (_buf_arr.size ()));
		dst_arr [chn] = _buf_arr [buf_index];
	}

	if (node._bypass_buf_arr [0] >= 0)
	{
		for (int chn = 0; chn < side_o._nbr_chn_tot; ++ chn)
		{
			const int      buf_index = node._bypass_buf_arr [chn];
			assert (buf_index >= 0);
			assert (buf_index < int (_buf_arr.size ()));
			byp_arr [chn] = _buf_arr [buf_index];
		}
	}

	for (int dir = 0; dir < piapi::PluginInterface::Dir_NBR_ELT; ++dir)
	{
		proc_info._nbr_chn_arr [dir] = node._side_arr [dir]._nbr_chn;
	}
}



void	WorldAudio::handle_msg_ctx (Msg::Ctx &msg)
{
	_ctx_ptr = msg._ctx_ptr;
}



void	WorldAudio::handle_msg_param (Msg::Param &msg)
{
	const int      index = msg._index;
	PluginPool::PluginDetails &   details =
		_pi_pool.use_plugin (msg._plugin_id);
	details._param_arr [index] = msg._val;
	details._param_update.fill_bit (index);
	details._param_update_from_audio [index] = false;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
