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

#include "fstb/AllocAlign.h"
#include "mfx/cmd/BufAlloc.h"
#include "mfx/cmd/Central.h"
#include "mfx/cmd/CentralCbInterface.h"
#include "mfx/doc/CtrlLinkSet.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/PluginInterface.h"

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

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
,	_d2d_rec ()
,	_d2d_buf_arr ()
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
	conc::LockFreeCell <WaMsg> * cell_ptr = 0;
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
#if ! defined (NDEBUG)
		int            ret_val =
#endif
			details._pi_uptr->reset (sample_freq, max_block_size, latency);
#if ! defined (NDEBUG)
		assert (ret_val == piapi::PluginInterface::Err_OK);
#endif
	}
	_audio.set_process_info (sample_freq, max_block_size);
	for (auto &buf : _d2d_buf_arr)
	{
		buf.resize (max_block_size);
	}
}



void	Central::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	_audio.process_block (dst_arr, src_arr, nbr_spl);

	if (_d2d_rec.is_recording ())
	{
		for (int chn = 0; chn < Cst::_nbr_chn_in; ++chn)
		{
			_d2d_buf_ptr_arr [chn] = src_arr [chn];
		}
		for (int chn = 0; chn < Cst::_nbr_chn_out; ++chn)
		{
			_d2d_buf_ptr_arr [Cst::_nbr_chn_in + chn] = dst_arr [chn];
		}
		_d2d_rec.write_data (_d2d_buf_ptr_arr.data (), nbr_spl);
	}
}



MeterResultSet &	Central::use_meters ()
{
	return _audio.use_meters ();
}



float	Central::get_audio_period_ratio () const
{
	return _audio.get_audio_period_ratio ();
}



void	Central::commit ()
{
	if (_new_sptr.get () != 0)
	{
		Document &     doc = *_new_sptr;
		doc._ctx_sptr = ContextSPtr (new ProcessingContext);

		create_routing ();
		create_mod_maps ();
		_new_sptr->_ctx_sptr->_master_vol = _new_sptr->_master_vol;

		std::vector <conc::LockFreeCell <WaMsg> *>  msg_list (1);
		msg_list [0] = _msg_pool.take_cell (true);
		msg_list [0]->_val._sender = WaMsg::Sender_CMD;
		msg_list [0]->_val._type   = WaMsg::Type_CTX;
		msg_list [0]->_val._content._ctx._ctx_ptr = _new_sptr->_ctx_sptr.get ();
		if (doc._smooth_transition_flag)
		{
			msg_list [0]->_val._content._ctx._fade_chnmap = ~uint64_t (0);
		}

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
int	Central::set_plugin (int pos, std::string model, bool force_reset_flag, bool gen_audio_flag)
{
	int            pi_id =
		set_plugin (pos, model, PiType_MAIN, force_reset_flag, gen_audio_flag);
	if (! gen_audio_flag)
	{
		remove_mixer (pos);
	}

	return pi_id;
}



// state_ptr can be null to keep the the default state.
void	Central::preinstantiate_plugins (std::string model, int count, const piapi::PluginState *state_ptr)
{
	Document &     doc = modify ();

	// Counts the plug-in already existing
	int            nbr_found   = 0;
	auto           it_inst_map = doc._map_model_id.find (model);
	if (it_inst_map != doc._map_model_id.end ())
	{
		Document::InstanceMap & inst_map = it_inst_map->second;
		nbr_found = int (inst_map.size ());
	}

	// Creates the remaining plug-ins, if required
	const int         nbr_inst = count - nbr_found;
	std::vector <piapi::EventTs> evt_list;
	std::vector <const piapi::EventTs *> evt_ptr_list;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	std::array <BufAlign, 6>  buf_arr;
	std::array <const float *, 2> src_ptr_arr = { 0, 0 };
	std::array <      float *, 2> dst_ptr_arr = { 0, 0 };
	std::array <      float *, 2> sig_ptr_arr = { 0, 0 };
	if (_sample_freq > 0)
	{
		const int      mbs_alig = (_max_block_size + 3) & ~3;
		for (auto &buf : buf_arr)
		{
			buf.resize (mbs_alig, 0);
		}
		src_ptr_arr [0] = &buf_arr [0] [0];
		src_ptr_arr [1] = &buf_arr [1] [0];
		dst_ptr_arr [0] = &buf_arr [2] [0];
		dst_ptr_arr [1] = &buf_arr [3] [0];
		sig_ptr_arr [0] = &buf_arr [4] [0];
		sig_ptr_arr [1] = &buf_arr [5] [0];
	}

	for (int pi_cnt = 0; pi_cnt < nbr_inst; ++ pi_cnt)
	{
		const int      pi_id = _plugin_pool.create (model);
		PluginPool::PluginDetails &   details =
			_plugin_pool.use_plugin (pi_id);
		if (_sample_freq > 0)
		{
			int            latency = 0;
#if ! defined (NDEBUG)
			int            ret_val =
#endif
				details._pi_uptr->reset (
					_sample_freq,
					_max_block_size,
					latency
				);
#if ! defined (NDEBUG)
			assert (ret_val == piapi::PluginInterface::Err_OK);
#endif
		}
		check_and_get_default_settings (*details._pi_uptr, *details._desc_ptr, model);

		// Not used, actually
		doc._map_model_id [model] [pi_id] = false;

		// Sets the requested plug-in state by processing a sample buffer
		if (state_ptr != 0 && _sample_freq > 0)
		{
			const int      nbr_param = int (state_ptr->_param_list.size ());
			evt_list.resize (nbr_param);
			evt_ptr_list.resize (nbr_param);
			for (int index = 0; index < nbr_param; ++index)
			{
				piapi::EventTs &  evt = evt_list [index];
				evt_ptr_list [index]  = &evt;

				evt._type                = piapi::EventType_PARAM;
				evt._timestamp           = 0;

				evt._evt._param._categ   = piapi::ParamCateg_GLOBAL;
				evt._evt._param._index   = index;
				evt._evt._param._note_id = -1;
				evt._evt._param._val     = float (state_ptr->_param_list [index]);
			}

			piapi::PluginInterface::ProcInfo proc_info;
			proc_info._byp_arr   = 0;
			proc_info._byp_state = piapi::PluginInterface::BypassState_IGNORE;
			proc_info._dst_arr   = &dst_ptr_arr [0];
			proc_info._evt_arr   = (nbr_param > 0) ? &evt_ptr_list [0] : 0;
			proc_info._nbr_chn_arr [piapi::PluginInterface::Dir_IN ] = 2;
			proc_info._nbr_chn_arr [piapi::PluginInterface::Dir_OUT] = 2;
			proc_info._nbr_evt   = int (evt_ptr_list.size ());
			proc_info._nbr_spl   = _max_block_size;
			proc_info._sig_arr   = &sig_ptr_arr [0];
			proc_info._src_arr   = &src_ptr_arr [0];

			details._pi_uptr->process_block (proc_info);
		}
	}
}



void	Central::remove_plugin (int pos)
{
	remove_plugin (pos, PiType_MAIN);
}



// Returns the plug-in Id
int	Central::set_mixer (int pos)
{
	return set_plugin (pos, Cst::_plugin_mix, PiType_MIX, false, true);
}



void	Central::remove_mixer (int pos)
{
	remove_plugin (pos, PiType_MIX);
}



void	Central::force_mono (int pos, bool flag)
{
#if defined (NDEBUG)
	modify ();
#else
	Document &     doc = modify ();
	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));
#endif

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

	plug._ctrl_map [index] = std::shared_ptr <doc::CtrlLinkSet> (
		new doc::CtrlLinkSet (cls)
	);
}



void	Central::set_sig_source (int pi_id, int sig_pin, int port_id)
{
	Document &     doc = modify ();

	assert (sig_pin >= 0);

	Plugin &       plug = find_plugin (doc, pi_id);

	if (sig_pin >= int (plug._sig_port_list.size ()))
	{
		plug._sig_port_list.resize (sig_pin + 1, -1);
	}
	plug._sig_port_list [sig_pin] = port_id;
}



void	Central::set_chn_mode (ChnMode mode)
{
	modify ();

	assert (mode >= 0);
	assert (mode < ChnMode_NBR_ELT);

	_new_sptr->_chn_mode = mode;
}



void	Central::set_master_vol (double vol)
{
	modify ();

	assert (vol > 0);

	_new_sptr->_master_vol = float (vol);
}



void	Central::set_transition (bool smooth_flag)
{
	modify ();

	_new_sptr->_smooth_transition_flag = smooth_flag;
}



void	Central::set_param (int pi_id, int index, float val)
{
	assert (pi_id >= 0);
	assert (index >= 0);
	assert (index < _plugin_pool.use_plugin (pi_id)._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL));
	assert (val >= 0);
	assert (val <= 1);

	conc::LockFreeCell <WaMsg> *  cell_ptr = make_param_msg (pi_id, index, val);
	_queue_cmd_to_audio.enqueue (*cell_ptr);
}



void	Central::set_tempo (float bpm)
{
	assert (bpm > 0);

	conc::LockFreeCell <WaMsg> * cell_ptr =
		_msg_pool.take_cell (true);
	cell_ptr->_val._sender              = WaMsg::Sender_CMD;
	cell_ptr->_val._type                = WaMsg::Type_TEMPO;
	cell_ptr->_val._content._tempo._bpm = bpm;

	_queue_cmd_to_audio.enqueue (*cell_ptr);
}



void	Central::process_queue_audio_to_cmd ()
{
	const std::chrono::microseconds  timeout (150 * 1000);
	std::chrono::microseconds        t_beg = get_cur_date ();

	int            a2c_cnt          = 0;
	bool           discard_a2c_flag = false;
	conc::LockFreeCell <WaMsg> * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_audio_to_cmd.dequeue ();
		if (cell_ptr != 0)
		{
			if (cell_ptr->_val._sender == WaMsg::Sender_CMD)
			{
				if (cell_ptr->_val._type == WaMsg::Type_CTX)
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
					if (! discard_a2c_flag)
					{
						_cb_ptr->process_msg_audio_to_cmd (cell_ptr->_val);

						// Every 64 events, we check if we are not overwhelmed
						++ a2c_cnt;
						if ((a2c_cnt & 63) == 0)
						{
							const std::chrono::microseconds   t_now = get_cur_date ();
							if (t_now - t_beg > timeout)
							{
								discard_a2c_flag = true;
							}
						}
					}
				}
			}

			_msg_pool.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);

	_d2d_rec.process_messages ();
}



PluginPool &	Central::use_pi_pool ()
{
	return (_plugin_pool);
}



const PluginPool &	Central::use_pi_pool () const
{
	return (_plugin_pool);
}



std::chrono::microseconds	Central::get_cur_date () const
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



int	Central::start_d2d_rec (const char pathname_0 [], size_t max_len)
{
	assert (! is_d2d_recording ());
	assert (pathname_0 != 0);

	return _d2d_rec.create_file (
		pathname_0, Cst::_nbr_chn_in + Cst::_nbr_chn_out, _sample_freq, max_len
	);
}



int	Central::stop_d2d_rec ()
{
	assert (is_d2d_recording ());

	return _d2d_rec.close_file ();
}



bool	Central::is_d2d_recording () const
{
	return _d2d_rec.is_recording ();
}



int64_t	Central::get_d2d_size_frames () const
{
	return _d2d_rec.get_size_frames ();
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



// Checks in the current config
bool	Central::is_plugin_used_in_audio_thread (int pi_id) const
{
	// Current
	const auto     it         = _cur_sptr->_map_id_loc.find (pi_id);
	bool           found_flag = (it != _cur_sptr->_map_id_loc.end ());

// Not necessary.
// Trash content shouldn't be active in the audio thread any more.
#if 0
	// Trash
	if (! found_flag)
	{
		for (const auto &ctx_sptr : _ctx_trash)
		{
			for (const auto &pi_ctx : ctx_sptr->_context_arr)
			{
				for (const auto &node : pi_ctx._node_arr)
				{
					if (node._pi_id == pi_id)
					{
						found_flag = true;
						break;
					}
				}
				if (found_flag)
				{
					break;
				}
			}
			if (found_flag)
			{
				break;
			}
		}
	}
#endif

	return found_flag;
}



int	Central::set_plugin (int pos, std::string model, PiType type, bool force_reset_flag, bool gen_audio_flag)
{
	Document &     doc = modify ();

	assert (pos >= 0);
	assert (pos < int (doc._slot_list.size ()));
	assert (! model.empty ());

	Slot &         slot   = doc._slot_list [pos];
	Plugin &       plugin = slot._component_arr [type];
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
			for (auto &inst_node : inst_map)
			{
				if (! inst_node.second)
				{
					// If we need to do a reset, we don't want a plug-in that is
					// potentially used in the audio thread at the moment.
					if (! (   force_reset_flag
					       && is_plugin_used_in_audio_thread (inst_node.first)))
					{
						pi_id = inst_node.first;
						inst_node.second = true;
						PluginPool::PluginDetails &   details =
							_plugin_pool.use_plugin (pi_id);
						const auto     state = details._pi_uptr->get_state ();
						if (    _sample_freq > 0
						    && (   force_reset_flag
						        || state != piapi::PluginInterface::State_ACTIVE))
						{
							int            latency = 0;
#if ! defined (NDEBUG)
							int            ret_val =
#endif
								details._pi_uptr->reset (
									_sample_freq, _max_block_size, latency
								);
#if ! defined (NDEBUG)
							assert (ret_val == piapi::PluginInterface::Err_OK);
#endif
						}
						break;
					}
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
				int            latency = 0;
#if ! defined (NDEBUG)
				int            ret_val =
#endif
					details._pi_uptr->reset (
						_sample_freq,
						_max_block_size,
						latency
					);
#if ! defined (NDEBUG)
				assert (ret_val == piapi::PluginInterface::Err_OK);
#endif
			}
			check_and_get_default_settings (*details._pi_uptr, *details._desc_ptr, model);

			doc._map_model_id [model] [pi_id] = true;
		}

		plugin._pi_id = pi_id;
		plugin._model = model;
		doc._map_id_loc [pi_id] = { pos, type };
	}

	if (type == PiType_MAIN)
	{
		slot._gen_audio_flag = gen_audio_flag;
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
	plugin._sig_port_list.clear ();
	doc._slot_list [pos]._force_mono_flag = false;
}



void	Central::create_routing ()
{
	assert (_new_sptr.get () != 0);
	assert (_new_sptr->_ctx_sptr.get () != 0);

	Document &           doc = *_new_sptr;
	ProcessingContext &  ctx = *doc._ctx_sptr;

	// Final number of channels
	int            nbr_chn_cur   = 1;
	int            nbr_chn_final = 1;
	switch (doc._chn_mode)
	{
	case ChnMode_1M_1M:
		nbr_chn_final = 1;
		break;
	case ChnMode_1M_1S:
		nbr_chn_final = 2;
		break;
	case ChnMode_1S_1S:
		nbr_chn_cur   = 2;
		nbr_chn_final = 2;
		break;
	default:
		assert (false);
		break;
	}
	ctx._nbr_chn_out = nbr_chn_final;

	// Buffers
	BufAlloc       buf_alloc (Cst::BufSpecial_NBR_ELT);

	std::array <int, piapi::PluginInterface::_max_nbr_chn>   cur_buf_arr;
	for (auto &b : cur_buf_arr)
	{
		b = -1;
	}

	// Input
	ProcessingContextNode::Side & audio_i =
		ctx._interface_ctx._side_arr [Dir_IN ];
	audio_i._nbr_chn     = Cst::_nbr_chn_in;
	audio_i._nbr_chn_tot = audio_i._nbr_chn;
	for (int i = 0; i < audio_i._nbr_chn; ++i)
	{
		const int      buf = buf_alloc.alloc ();
		audio_i._buf_arr [i] = buf;
	}
	assert (nbr_chn_cur <= audio_i._nbr_chn);
	for (int i = 0; i < nbr_chn_cur; ++i)
	{
		cur_buf_arr [i] = audio_i._buf_arr [i];
	}

	// Plug-ins
	for (Slot & slot : doc._slot_list)
	{
		const int      pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
		if (pi_id_main >= 0)
		{
			int            nbr_chn_in      = nbr_chn_cur;
			const piapi::PluginDescInterface &   desc_main =
				*_plugin_pool.use_plugin (pi_id_main)._desc_ptr;
			const bool     out_st_flag     = desc_main.prefer_stereo ();
			const bool     final_mono_flag = (nbr_chn_final == 1);
			int            nbr_chn_out     =
				  (out_st_flag && ! (slot._force_mono_flag || final_mono_flag))
				? 2
				: nbr_chn_in;

			const int      pi_id_mix = slot._component_arr [PiType_MIX]._pi_id;

			// Processing context
			slot._ctx_index = int (ctx._context_arr.size ());
			ctx._context_arr.resize (slot._ctx_index + 1);
			ProcessingContext::PluginContext &  pi_ctx = ctx._context_arr.back ();
			pi_ctx._mixer_flag  = (pi_id_mix >= 0);

			// Main plug-in
			pi_ctx._node_arr [PiType_MAIN]._pi_id = pi_id_main;
			ProcessingContextNode::Side & main_side_i =
				pi_ctx._node_arr [PiType_MAIN]._side_arr [Dir_IN ];
			ProcessingContextNode::Side & main_side_o =
				pi_ctx._node_arr [PiType_MAIN]._side_arr [Dir_OUT];
			int            main_nbr_i = 1;
			int            main_nbr_o = 1;
			int            main_nbr_s = 0;

			// Input
			desc_main.get_nbr_io (main_nbr_i, main_nbr_o, main_nbr_s);
			main_side_i._nbr_chn     = (main_nbr_i > 0) ? nbr_chn_in : 0;
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
			main_side_o._nbr_chn     = (main_nbr_o > 0) ? nbr_chn_out : 0;
			main_side_o._nbr_chn_tot = nbr_chn_out * main_nbr_o;
			for (int chn = 0; chn < main_side_o._nbr_chn_tot; ++chn)
			{
				if (chn < nbr_chn_out && slot._gen_audio_flag)
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

			// Signals
			pi_ctx._node_arr [PiType_MAIN]._nbr_sig = main_nbr_s;
			const int      nbr_reg_sig =
				int (slot._component_arr [PiType_MAIN]._sig_port_list.size ());
			for (int sig = 0; sig < main_nbr_s; ++sig)
			{
				ProcessingContextNode::SigInfo & sig_info =
					pi_ctx._node_arr [PiType_MAIN]._sig_buf_arr [sig];
				sig_info._buf_index  = Cst::BufSpecial_TRASH;
				sig_info._port_index = -1;

				if (sig < nbr_reg_sig)
				{
					const int      port_index =
						slot._component_arr [PiType_MAIN]._sig_port_list [sig];
					if (port_index >= 0)
					{
						sig_info._buf_index  = buf_alloc.alloc ();
						sig_info._port_index = port_index;
					}
				}
			}

			// With dry/wet mixer
			if (pi_id_mix >= 0)
			{
				assert (slot._gen_audio_flag);

				pi_ctx._node_arr [PiType_MIX]._pi_id = pi_id_mix;
				ProcessingContextNode::Side & mix_side_i =
					pi_ctx._node_arr [PiType_MIX]._side_arr [Dir_IN ];
				ProcessingContextNode::Side & mix_side_o =
					pi_ctx._node_arr [PiType_MIX]._side_arr [Dir_OUT];

				pi_ctx._node_arr [PiType_MIX]._nbr_sig = 0;

				// Bypass output for the main plug-in
				for (int chn = 0; chn < nbr_chn_out * main_nbr_o; ++chn)
				{
					pi_ctx._node_arr [PiType_MAIN]._bypass_buf_arr [chn] =
						buf_alloc.alloc ();
				}

				// Dry/wet input
				mix_side_i._nbr_chn     = nbr_chn_out;
				mix_side_i._nbr_chn_tot = nbr_chn_out * 2;
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					// 1st pin: main output
					mix_side_i._buf_arr [              chn] = nxt_buf_arr [chn];

					// 2nd pin: main input as default bypass
					const int       chn_in = std::min (chn, nbr_chn_in - 1);
					const int       buf    = cur_buf_arr [chn_in];
					mix_side_i._buf_arr [nbr_chn_out + chn] = buf;
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

			// Output buffers become the next input buffers
			if (slot._gen_audio_flag)
			{
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
	}

	// Output
	ProcessingContextNode::Side & audio_o =
		ctx._interface_ctx._side_arr [Dir_OUT];
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

	// For direct-link initialises the internal value from
	// the current public value.
	if (abs_flag)
	{
		PluginPool::PluginDetails &   details =
			_plugin_pool.use_plugin (coord._plugin_id);
		const float    val_nrm = details._param_arr [coord._param_index];
		unit_sptr->update_internal_val (val_nrm);
	}

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
void	Central::create_param_msg (std::vector <conc::LockFreeCell <WaMsg> *> &msg_list)
{
	assert (_new_sptr.get () != 0);
	assert (_new_sptr->_ctx_sptr.get () != 0);

	Document &           doc = *_new_sptr;

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
				conc::LockFreeCell <WaMsg> *  msg_ptr =
					make_param_msg (plug._pi_id, index, plug._param_list [index]);
				msg_list.push_back (msg_ptr);
			}

			plug._param_list.clear ();
		}
	}
}



conc::LockFreeCell <WaMsg> *	Central::make_param_msg (int pi_id, int index, float val)
{
	conc::LockFreeCell <WaMsg> * cell_ptr =
		_msg_pool.take_cell (true);
	cell_ptr->_val._sender = WaMsg::Sender_CMD;
	cell_ptr->_val._type   = WaMsg::Type_PARAM;
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
