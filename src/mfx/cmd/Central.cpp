/*****************************************************************************

        Central.cpp
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

#include "mfx/cmd/BufAlloc.h"
#include "mfx/cmd/Central.h"
#include "mfx/cmd/CentralCbInterface.h"
#include "mfx/doc/CtrlLinkSet.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dwm/DryWet.h"
#include "mfx/pi/freqsh/FrequencyShifter.h"
#include "mfx/pi/trem1/Tremolo.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/pi/wha1/Wha.h"
#include "mfx/Cst.h"

#include <algorithm>
#include <utility>

#include <cassert>



namespace mfx
{
namespace cmd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Central::Central (ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device)
:	_msg_pool ()
,	_input_device (input_device)
,	_queue_input_to_audio (queue_input_to_audio)
,	_queue_cmd_to_audio ()
,	_queue_audio_to_cmd ()
,	_plugin_pool ()
,	_audio (
		_plugin_pool,
		_queue_cmd_to_audio,
		_queue_audio_to_cmd,
		queue_input_to_audio,
		input_device,
		_msg_pool
	)
,	_default_map ()
,	_sample_freq (0)
,	_max_block_size (0)
,	_cb_ptr (0)
,	_cur_sptr ()
,	_new_sptr ()
,	_ctx_trash ()
,	_dummy_mix_id (-1)
{
	_msg_pool.expand_to (1024);

	_dummy_mix_id = _plugin_pool.create (Cst::_plugin_mix);
}



Central::~Central ()
{
	rollback ();

	// Flushes the audio -> cmd queue
	_cb_ptr = 0;
	process_queue_audio_to_cmd	();

	// Deinstantiate the plug-ins
	if (_cur_sptr.get () != 0)
	{
		std::vector <int> pi_list;
		
		pi_list = _plugin_pool.list_plugins (mfx::SharedRscState_INUSE);
		for (int index : pi_list)
		{
			_plugin_pool.schedule_for_release (index);
		}

		pi_list = _plugin_pool.list_plugins (mfx::SharedRscState_RECYCLING);
		for (int index : pi_list)
		{
			_plugin_pool.release (index);
		}
	}

	// Flushes the cmd -> audio queue
	conc::LockFreeCell <Msg> * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_cmd_to_audio.dequeue ();
		if (cell_ptr != 0)
		{
			_msg_pool.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);
}



// 0 to cancel the callback
void	Central::set_callback (CentralCbInterface *cb_ptr)
{
	_cb_ptr = cb_ptr;
}



void	Central::set_process_info (double sample_freq, int max_block_size)
{
	_sample_freq    = sample_freq;
	_max_block_size = max_block_size;
	const std::vector <int> id_list =
		_plugin_pool.list_plugins (SharedRscState_INUSE);
	for (int pi_id : id_list)
	{
		PluginPool::PluginDetails &         details  =
			_plugin_pool.use_plugin (pi_id);
		int            latency = 0;
		int            ret_val =
			details._pi_uptr->reset (sample_freq, max_block_size, latency);
		assert (ret_val == piapi::PluginInterface::Err_OK);
	}
	_audio.set_process_info (sample_freq, max_block_size);
}



void	Central::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	_audio.process_block (dst_arr, src_arr, nbr_spl);
}



bool	Central::check_signal_clipping ()
{
	return _audio.check_signal_clipping ();
}



void	Central::commit ()
{
	if (_new_sptr.get () != 0)
	{
		Document &     doc = *_new_sptr;
		doc._ctx_sptr = ContextSPtr (new ProcessingContext);

		create_routing ();
		create_mod_maps ();

		std::vector <conc::LockFreeCell <Msg> *>  msg_list (1);
		msg_list [0] = _msg_pool.take_cell (true);
		msg_list [0]->_val._sender = Msg::Sender_CMD;
		msg_list [0]->_val._type   = Msg::Type_CTX;
		msg_list [0]->_val._content._ctx._ctx_ptr = _new_sptr->_ctx_sptr.get ();

		create_param_msg (msg_list);

		for (auto cell_ptr : msg_list)
		{
			_queue_cmd_to_audio.enqueue (*cell_ptr);
		}

		if (_cur_sptr.get () != 0)
		{
			_ctx_trash.insert (_cur_sptr->_ctx_sptr);
		}
		_cur_sptr = _new_sptr;
		_new_sptr.reset ();
	}
}



void	Central::rollback ()
{
	_new_sptr.reset ();
}



void	Central::clear ()
{
	Document &     doc = modify ();

	const int      nbr_slots = int (doc._slot_list.size ());
	for (int pos = 0; pos < nbr_slots; ++pos)
	{
		remove_plugin (pos, PiType_MAIN);
		remove_plugin (pos, PiType_MIX);
	}

	doc._slot_list.clear ();
}



// Insertion before the given position
// The new slot is totally empty
void	Central::insert_slot (int pos)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos <= int (doc._slot_list.size ()));

	doc._slot_list.insert (
		_new_sptr->_slot_list.begin () + pos,
		Slot ()
	);
}



void	Central::delete_slot (int pos)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));

	remove_plugin (pos, PiType_MAIN);
	remove_plugin (pos, PiType_MIX);

	doc._slot_list.erase (_new_sptr->_slot_list.begin () + pos);
}



void	Central::clear_slot (int pos)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));

	remove_plugin (pos, PiType_MAIN);
	remove_plugin (pos, PiType_MIX);

	doc._slot_list [pos] = Slot ();
}



// Returns the plug-in Id
int	Central::set_plugin (int pos, std::string model, bool force_reset_flag)
{
	return set_plugin (pos, model, PiType_MAIN, force_reset_flag);
}



void	Central::remove_plugin (int pos)
{
	return remove_plugin (pos, PiType_MAIN);
}



// Returns the plug-in Id
int	Central::set_mixer (int pos)
{
	return set_plugin (pos, Cst::_plugin_mix, PiType_MIX, false);
}



void	Central::remove_mixer (int pos)
{
	return remove_plugin (pos, PiType_MIX);
}



void	Central::force_mono (int pos, bool flag)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));

	_new_sptr->_slot_list [pos]._force_mono_flag = flag;
}



// Returns -1 if not found.
int	Central::find_pi (int pi_id)
{
	int            pos     = -1;

	Document *     doc_ptr = _new_sptr.get ();
	if (doc_ptr == 0)
	{
		doc_ptr = _cur_sptr.get ();
	}

	if (doc_ptr != 0)
	{
		auto           it_loc = doc_ptr->_map_id_loc.find (pi_id);
		if (it_loc != doc_ptr->_map_id_loc.end ())
		{
			pos = it_loc->second._slot_pos;
		}
	}

	return pos;
}



void	Central::set_pi_state (int pi_id, const std::vector <float> &param_list)
{
	Document &     doc = modify ();

	assert (pi_id >= 0);
	assert (int (param_list.size ()) == _plugin_pool.use_plugin (pi_id)._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL));

	Plugin &       plug = find_plugin (doc, pi_id);

	plug._param_list = param_list;
}



void	Central::clear_mod (int pi_id)
{
	Document &     doc = modify ();

	assert (pi_id >= 0);

	Plugin &       plug = find_plugin (doc, pi_id);
	plug._ctrl_map.clear ();
}



void	Central::set_mod (int pi_id, int index, const doc::CtrlLinkSet &cls)
{
	Document &     doc = modify ();

	assert (pi_id >= 0);
	assert (index >= 0);
	assert (index < _plugin_pool.use_plugin (pi_id)._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL));

	Plugin &       plug = find_plugin (doc, pi_id);

	plug._ctrl_map [index] = cls.duplicate ();
}



void	Central::set_param (int pi_id, int index, float val)
{
	assert (pi_id >= 0);
	assert (index >= 0);
	assert (index < _plugin_pool.use_plugin (pi_id)._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL));
	assert (val >= 0);
	assert (val <= 1);

	conc::LockFreeCell <Msg> * cell_ptr = make_param_msg (pi_id, index, val);
	_queue_cmd_to_audio.enqueue (*cell_ptr);
}



void	Central::process_queue_audio_to_cmd ()
{
	conc::LockFreeCell <Msg> * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_audio_to_cmd.dequeue ();
		if (cell_ptr != 0)
		{
			if (cell_ptr->_val._sender == Msg::Sender_CMD)
			{
				if (cell_ptr->_val._type == Msg::Type_CTX)
				{
					const ProcessingContext *  ctx_ptr =
						cell_ptr->_val._content._ctx._ctx_ptr;
					if (ctx_ptr != 0)
					{
						auto           it = std::find_if (
							_ctx_trash.begin (),
							_ctx_trash.end (),
							[=] (const ContextSPtr &ctx_sptr)
							{
								return (ctx_sptr.get () == ctx_ptr);
							}
						);
						if (it == _ctx_trash.end ())
						{
							assert (false);
						}
						else
						{
							_ctx_trash.erase (it);
						}
					}
				}
			}

			else
			{
				if (_cb_ptr != 0)
				{
					_cb_ptr->process_msg_audio_to_cmd (cell_ptr->_val);
				}
			}

			_msg_pool.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);
}



PluginPool &	Central::use_pi_pool ()
{
	return (_plugin_pool);
}



const PluginPool &	Central::use_pi_pool () const
{
	return (_plugin_pool);
}



int64_t	Central::get_cur_date () const
{
	return _input_device.get_cur_date ();
}



int	Central::get_dummy_mix_id () const
{
	return _dummy_mix_id;
}



const piapi::PluginState &	Central::use_default_settings (std::string model) const
{
	assert (! model.empty ());

	auto           it = _default_map.find (model);
	assert (it != _default_map.end ());

	return it->second;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Central::Document &	Central::modify ()
{
	if (_new_sptr.get () == 0)
	{
		if (_cur_sptr.get () == 0)
		{
			_new_sptr = DocumentSPtr (new Document);
		}
		else
		{
			_new_sptr = DocumentSPtr (new Document (*_cur_sptr));
		}
		_new_sptr->_ctx_sptr.reset (); // Will be set during commit.
	}

	return *_new_sptr;
}



Plugin &	Central::find_plugin (Document &doc, int pi_id)
{
	auto           it_loc = doc._map_id_loc.find (pi_id);
	assert (it_loc != doc._map_id_loc.end ());

	const int      pos  = it_loc->second._slot_pos;
	const PiType   type = it_loc->second._type;

	Slot &         slot = doc._slot_list [pos];
	Plugin &       plug = slot._component_arr [type];

	return plug;
}



int	Central::set_plugin (int pos, std::string model, PiType type, bool force_reset_flag)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));
	assert (! model.empty ());

	Plugin &       plugin = doc._slot_list [pos]._component_arr [type];
	int            pi_id = -1;

	// If we replace a plug-in with one of the same type,
	// reuse the previous one.
	if (plugin._model == model && plugin._pi_id >= 0)
	{
		pi_id = plugin._pi_id;
		clear_mod (pi_id);
	}

	else
	{
		if (plugin._pi_id >= 0)
		{
			remove_plugin (pos);
		}

		// Check if there is a unused plug-in
		auto           it_inst_map = doc._map_model_id.find (model);
		if (it_inst_map != doc._map_model_id.end ())
		{
			Document::InstanceMap & inst_map = it_inst_map->second;
			for (auto &&inst_node : inst_map)
			{
				if (! inst_node.second)
				{
					pi_id = inst_node.first;
					inst_node.second = true;
					if (force_reset_flag)
					{
						PluginPool::PluginDetails &   details =
							_plugin_pool.use_plugin (pi_id);
						int         latency = 0;
						int         ret_val = details._pi_uptr->reset (
							_sample_freq, _max_block_size, latency
						);
						assert (ret_val == piapi::PluginInterface::Err_OK);
					}
					break;
				}
			}
		}

		// Not found? Create one
		if (pi_id < 0)
		{
			pi_id = _plugin_pool.create (model);
			PluginPool::PluginDetails &   details =
				_plugin_pool.use_plugin (pi_id);
			if (_sample_freq > 0)
			{
				int         latency = 0;
				int         ret_val = details._pi_uptr->reset (
					_sample_freq,
					_max_block_size,
					latency
				);
				assert (ret_val == piapi::PluginInterface::Err_OK);
			}
			check_and_get_default_settings (*details._pi_uptr, *details._desc_ptr, model);

			doc._map_model_id [model] [pi_id] = true;
		}

		plugin._pi_id = pi_id;
		plugin._model = model;
		doc._map_id_loc [pi_id] = { pos, type };
	}

	return pi_id;
}



void	Central::remove_plugin (int pos, PiType type)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));

	Plugin &       plugin = doc._slot_list [pos]._component_arr [type];
	auto           it_inst_map = doc._map_model_id.find (plugin._model);

	if (it_inst_map != doc._map_model_id.end ())
	{
		Document::InstanceMap & inst_map = it_inst_map->second;
		auto           it_inst  = inst_map.find (plugin._pi_id);
		if (it_inst != inst_map.end ())
		{
			it_inst->second = false;
		}
	}

	auto           it_loc = doc._map_id_loc.find (plugin._pi_id);
	if (it_loc != doc._map_id_loc.end ())
	{
		doc._map_id_loc.erase (it_loc);
	}

	plugin._model.clear ();
	plugin._pi_id = -1;
	plugin._ctrl_map.clear ();
	doc._slot_list [pos]._force_mono_flag = false;
}



void	Central::create_routing ()
{
	assert (_new_sptr.get () != 0);
	assert (_new_sptr->_ctx_sptr.get () != 0);

	Document &           doc = *_new_sptr;
	ProcessingContext &  ctx = *doc._ctx_sptr;

	BufAlloc       buf_alloc (Cst::BufSpecial_NBR_ELT);

	std::array <int, piapi::PluginInterface::_max_nbr_chn>   cur_buf_arr;
	int            nbr_chn_cur = 1;
	for (auto &b : cur_buf_arr)
	{
		b = -1;
	}

	// Input
	ProcessingContextNode::Side & audio_i =
		ctx._interface_ctx._side_arr [piapi::PluginInterface::Dir_IN ];
	audio_i._nbr_chn     = Cst::_nbr_chn_in;
	audio_i._nbr_chn_tot = audio_i._nbr_chn;
	for (int i = 0; i < audio_i._nbr_chn; ++i)
	{
		const int      buf = buf_alloc.alloc ();
		audio_i._buf_arr [i] = buf;
	}
	cur_buf_arr [0] = audio_i._buf_arr [0];

	// Plug-ins
	for (Slot & slot : doc._slot_list)
	{
		const int      pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
		if (pi_id_main >= 0)
		{
			int            nbr_chn_in  = nbr_chn_cur;
			const piapi::PluginDescInterface &   desc_main =
				*_plugin_pool.use_plugin (pi_id_main)._desc_ptr;
			const bool     out_st_flag = desc_main.prefer_stereo ();
			int            nbr_chn_out =
				(out_st_flag && ! slot._force_mono_flag) ? 2 : nbr_chn_in;

			const int      pi_id_mix = slot._component_arr [PiType_MIX]._pi_id;

			// Processing context
			slot._ctx_index = int (ctx._context_arr.size ());
			ctx._context_arr.resize (slot._ctx_index + 1);
			ProcessingContext::PluginContext &  pi_ctx = ctx._context_arr.back ();
			pi_ctx._mixer_flag  = (pi_id_mix >= 0);

			// Main plug-in
			pi_ctx._node_arr [PiType_MAIN]._pi_id = pi_id_main;
			ProcessingContextNode::Side & main_side_i =
				pi_ctx._node_arr [PiType_MAIN]._side_arr [piapi::PluginInterface::Dir_IN ];
			ProcessingContextNode::Side & main_side_o =
				pi_ctx._node_arr [PiType_MAIN]._side_arr [piapi::PluginInterface::Dir_OUT];
			int            main_nbr_i = 1;
			int            main_nbr_o = 1;

			// Input
			desc_main.get_nbr_io (main_nbr_i, main_nbr_o);
			main_side_i._nbr_chn     = nbr_chn_in;
			main_side_i._nbr_chn_tot = nbr_chn_in * main_nbr_i;
			for (int chn = 0; chn < main_side_i._nbr_chn_tot; ++chn)
			{
				if (chn < nbr_chn_in)
				{
					main_side_i._buf_arr [chn] = cur_buf_arr [chn];
				}
				else
				{
					main_side_i._buf_arr [chn] = Cst::BufSpecial_SILENCE;
				}
			}

			// Output
			std::array <int, piapi::PluginInterface::_max_nbr_chn>   nxt_buf_arr;
			main_side_o._nbr_chn     = nbr_chn_out;
			main_side_o._nbr_chn_tot = nbr_chn_out * main_nbr_o;
			for (int chn = 0; chn < main_side_o._nbr_chn_tot; ++chn)
			{
				if (chn < nbr_chn_out)
				{
					const int      buf = buf_alloc.alloc ();
					nxt_buf_arr [chn]          = buf;
					main_side_o._buf_arr [chn] = buf;
				}
				else
				{
					main_side_o._buf_arr [chn] = Cst::BufSpecial_TRASH;
				}
			}

			// With dry/wet mixer
			if (pi_id_mix >= 0)
			{
				pi_ctx._node_arr [PiType_MIX]._pi_id = pi_id_mix;
				ProcessingContextNode::Side & mix_side_i =
					pi_ctx._node_arr [PiType_MIX]._side_arr [piapi::PluginInterface::Dir_IN ];
				ProcessingContextNode::Side & mix_side_o =
					pi_ctx._node_arr [PiType_MIX]._side_arr [piapi::PluginInterface::Dir_OUT];

				// Bypass output for the main plug-in
				for (int chn = 0; chn < nbr_chn_out * main_nbr_o; ++chn)
				{
					pi_ctx._node_arr [PiType_MAIN]._bypass_buf_arr [chn] =
						buf_alloc.alloc ();
				}

				// Dry/wet input
				mix_side_i._nbr_chn     = nbr_chn_in;
				mix_side_i._nbr_chn_tot = nbr_chn_in * 2;
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					// 1st pin: main output
					mix_side_i._buf_arr [chn] = nxt_buf_arr [chn];

					// 2nd pin: main input as default bypass
					const int       chn_in = std::min (chn, nbr_chn_in - 1);
					const int       buf    = cur_buf_arr [chn_in];
					mix_side_i._buf_arr [nbr_chn_in + chn] = buf;
				}

				// Dry/wet output
				std::array <int, piapi::PluginInterface::_max_nbr_chn>   mix_buf_arr;
				mix_side_o._nbr_chn     = nbr_chn_out;
				mix_side_o._nbr_chn_tot = nbr_chn_out;
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					const int      buf = buf_alloc.alloc ();
					mix_buf_arr [chn]         = buf;
					mix_side_o._buf_arr [chn] = buf;
				}

				// Shift buffers
				for (int chn = 0; chn < nbr_chn_out * main_nbr_o; ++chn)
				{
					buf_alloc.ret (
						pi_ctx._node_arr [PiType_MAIN]._bypass_buf_arr [chn]
					);
				}
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					buf_alloc.ret (nxt_buf_arr [chn]);
					nxt_buf_arr [chn] = mix_buf_arr [chn];
				}
			}

			// Next buffers
			for (int chn = 0; chn < nbr_chn_out; ++chn)
			{
				if (chn < nbr_chn_in)
				{
					buf_alloc.ret (cur_buf_arr [chn]);
				}
				cur_buf_arr [chn] = nxt_buf_arr [chn];
			}
			nbr_chn_cur = nbr_chn_out;
		}
	}

	// Output
	ProcessingContextNode::Side & audio_o =
		ctx._interface_ctx._side_arr [piapi::PluginInterface::Dir_OUT];
	audio_o._nbr_chn     = Cst::_nbr_chn_out;
	audio_o._nbr_chn_tot = audio_o._nbr_chn;
	for (int i = 0; i < audio_o._nbr_chn; ++i)
	{
		const int      chn_src = std::min (i, nbr_chn_cur - 1);
		audio_o._buf_arr [i] = cur_buf_arr [chn_src];
	}

	for (int chn = 0; chn < nbr_chn_cur; ++chn)
	{
		buf_alloc.ret (cur_buf_arr [chn]);
	}
}



void	Central::create_mod_maps ()
{
	assert (_new_sptr.get () != 0);
	assert (_new_sptr->_ctx_sptr.get () != 0);

	Document &           doc = *_new_sptr;
	ProcessingContext &  ctx = *doc._ctx_sptr;

	for (const Slot & slot : doc._slot_list)
	{
		ProcessingContext::PluginContext & ctx_slot =
			ctx._context_arr [slot._ctx_index];

		for (int component_index = 0
		;	component_index < int (slot._component_arr.size ())
		;	++ component_index)
		{
			const Plugin & plug = slot._component_arr [component_index];

			if (plug._pi_id < 0)
			{
				break;
			}

			for (const auto & ctrl_node : plug._ctrl_map)
			{
				const int                  index =  ctrl_node.first;
				const doc::CtrlLinkSet &   cls   = *ctrl_node.second;

				const ParamCoord           coord (plug._pi_id, index);
				std::shared_ptr <ControlledParam>   ctrl_param_sptr (
					new ControlledParam (coord)
				);

				if (cls._bind_sptr.get () != 0)
				{
					const doc::CtrlLink &   link = *cls._bind_sptr;
					add_controller (ctx, link, ctrl_param_sptr, true);
				}

				for (const auto & link_sptr : cls._mod_arr)
				{
					const doc::CtrlLink &   link = *link_sptr;
					add_controller (ctx, link, ctrl_param_sptr, false);
				} // link_sptr
			} // ctrl_node
		} // plug
	} // slot
}



void	Central::add_controller (ProcessingContext &ctx, const doc::CtrlLink &link, std::shared_ptr <ControlledParam> &ctrl_param_sptr, bool abs_flag)
{
	const ParamCoord &   coord = ctrl_param_sptr->use_coord ();
	std::shared_ptr <CtrlUnit> unit_sptr (new CtrlUnit (link, abs_flag));
	ctrl_param_sptr->use_unit_list ().push_back (unit_sptr);
	ctx._map_param_ctrl.insert (std::make_pair (
		coord, ctrl_param_sptr
	));
	ctx._map_src_param.insert (std::make_pair (
		link._source, ctrl_param_sptr
	));
	ctx._map_src_unit.insert (std::make_pair (
		link._source, unit_sptr
	));
}



// Appends messages to msg_list
void	Central::create_param_msg (std::vector <conc::LockFreeCell <Msg> *> &msg_list)
{
	assert (_new_sptr.get () != 0);
	assert (_new_sptr->_ctx_sptr.get () != 0);

	Document &           doc = *_new_sptr;
	ProcessingContext &  ctx = *doc._ctx_sptr;

	for (Slot & slot : doc._slot_list)
	{
		for (Plugin & plug : slot._component_arr)
		{
			if (plug._pi_id < 0)
			{
				break;
			}

			const int      nbr_param = int (plug._param_list.size ());
			for (int index = 0; index < nbr_param; ++index)
			{
				conc::LockFreeCell <Msg> * msg_ptr =
					make_param_msg (plug._pi_id, index, plug._param_list [index]);
				msg_list.push_back (msg_ptr);
			}

			plug._param_list.clear ();
		}
	}
}



conc::LockFreeCell <Msg> *	Central::make_param_msg (int pi_id, int index, float val)
{
	conc::LockFreeCell <Msg> * cell_ptr =
		_msg_pool.take_cell (true);
	cell_ptr->_val._sender = Msg::Sender_CMD;
	cell_ptr->_val._type   = Msg::Type_PARAM;
	cell_ptr->_val._content._param._plugin_id = pi_id;
	cell_ptr->_val._content._param._index     = index;
	cell_ptr->_val._content._param._val       = val;

	return cell_ptr;
}



void	Central::check_and_get_default_settings (piapi::PluginInterface &plug, const piapi::PluginDescInterface &desc, std::string model)
{
	auto           it = _default_map.find (model);
	if (it == _default_map.end ())
	{
		piapi::PluginState & def = _default_map [model];
		const int      nbr_param = desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
		def._param_list.resize (nbr_param);
		for (int index = 0; index < nbr_param; ++index)
		{
			def._param_list [index] = float (
				plug.get_param_val (piapi::ParamCateg_GLOBAL, index, 0)
			);
		}
	}
}



}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
