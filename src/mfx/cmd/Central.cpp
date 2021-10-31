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
#include "mfx/cmd/Central.h"
#include "mfx/cmd/CentralCbInterface.h"
#include "mfx/cmd/PluginCb.h"
#include "mfx/doc/CtrlLinkSet.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/BypassState.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/FactoryInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/PluginInterface.h"
#include "mfx/piapi/ProcInfo.h"
#include "mfx/FileIOInterface.h"

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
:	_pi_aud_type_list ()
,	_pi_sig_type_list ()
,	_msg_pool ()
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
,	_cb_ptr (nullptr)
,	_router ()
,	_cur_sptr ()
,	_new_sptr ()
,	_ctx_trash ()
,	_dummy_mix_id (-1)
,	_d2d_rec ()
,	_d2d_buf_arr ()
,	_d2d_buf_ptr_arr ({{ nullptr }})
{
	_msg_pool.expand_to (1024);

	_dummy_mix_id = _plugin_pool.create (
		Cst::_plugin_dwm, std::make_unique <PluginCb> (*this)
	);
}



Central::~Central ()
{
	rollback ();

	// Flushes the audio -> cmd queue
	_cb_ptr = nullptr;
	process_queue_audio_to_cmd	();

	// Deinstantiate the plug-ins
	if (_cur_sptr.get () != nullptr)
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
	conc::LockFreeCell <WaMsg> * cell_ptr = nullptr;
	do
	{
		cell_ptr = _queue_cmd_to_audio.dequeue ();
		if (cell_ptr != nullptr)
		{
			_msg_pool.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != nullptr);
}



void	Central::create_plugin_lists ()
{
	_pi_aud_type_list.clear ();
	_pi_sig_type_list.clear ();
	std::vector <std::string> pi_list = use_pi_pool ().list_models ();
	std::map <std::string, std::string> pi_aud_map;
	std::map <std::string, std::string> pi_sig_map;
	for (std::string model_id : pi_list)
	{
		if (model_id [0] != '\?')
		{
			const mfx::piapi::PluginDescInterface &   desc =
				use_pi_pool ().get_model_desc (model_id);

			int            nbr_i = 1;
			int            nbr_o = 1;
			int            nbr_s = 0;
			desc.get_nbr_io (nbr_i, nbr_o, nbr_s);

			const piapi::PluginInfo pi_info { desc.get_info () };
			std::string    name     = pi::param::Tools::extract_longest_str (
				pi_info._name.c_str (), '\n'
			);

			if (nbr_i > 0 || nbr_o > 0)
			{
				pi_aud_map [name] = model_id;
			}
			else if (nbr_s > 0)
			{
				pi_sig_map [name] = model_id;
			}
		}
	}
	for (auto &node : pi_aud_map)
	{
		_pi_aud_type_list.push_back (node.second);
	}
	for (auto &node : pi_sig_map)
	{
		_pi_sig_type_list.push_back (node.second);
	}
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
		details._latency = 0;
#if ! defined (NDEBUG)
		int            ret_val =
#endif
			details._pi_uptr->reset (
				sample_freq, max_block_size, details._latency
			);
#if ! defined (NDEBUG)
		assert (ret_val == piapi::Err_OK);
#endif
	}
	_audio.set_process_info (sample_freq, max_block_size);
	_router.set_process_info (sample_freq, max_block_size);
	for (auto &buf : _d2d_buf_arr)
	{
		buf.resize (max_block_size);
	}
}



void	Central::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl) noexcept
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



MeterResultSet &	Central::use_meters () noexcept
{
	return _audio.use_meters ();
}



float	Central::get_audio_period_ratio () const noexcept
{
	return _audio.get_audio_period_ratio ();
}



void	Central::commit ()
{
	if (_new_sptr.get () != nullptr)
	{
		Document &     doc = *_new_sptr;
		doc._ctx_sptr = std::make_shared <ProcessingContext> ();

		create_routing ();
		create_mod_maps ();
		_new_sptr->_ctx_sptr->_master_vol = _new_sptr->_master_vol;

		std::vector <conc::LockFreeCell <WaMsg> *>  msg_list (1);
		msg_list [0] = _msg_pool.take_cell (true);
		msg_list [0]->_val._sender = WaMsg::Sender_CMD;
		msg_list [0]->_val._type   = WaMsg::Type_CTX;
		msg_list [0]->_val._content._ctx._ctx_ptr = _new_sptr->_ctx_sptr.get ();
		msg_list [0]->_val._content._ctx._prog_switch_mode = doc._prog_switch_mode;

		create_param_msg (msg_list);

		for (auto cell_ptr : msg_list)
		{
			_queue_cmd_to_audio.enqueue (*cell_ptr);
		}

#if 0 // Context text dump, for debugging
		std::string    ctx_txt = doc._ctx_sptr->dump_as_str (_plugin_pool);
		const std::string pathname = Cst::_log_dir + "/audio-proc-ctx.txt";
		FileIOInterface::write_txt_file_direct (pathname, ctx_txt);
#endif

		if (_cur_sptr.get () != nullptr)
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

	constexpr int     nbr_chn_tot =
		  piapi::PluginInterface::_max_nbr_chn
		* piapi::PluginInterface::_max_nbr_pins;
	constexpr int     nbr_sig_tot = piapi::PluginInterface::_max_nbr_sig;
	constexpr int     nbr_buf_tot = nbr_chn_tot * 2 + nbr_sig_tot;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	BufAlign          buf;
	std::array <const float *, nbr_chn_tot> src_ptr_arr {};
	std::array <      float *, nbr_chn_tot> dst_ptr_arr {};
	std::array <      float *, nbr_sig_tot> sig_ptr_arr {};
	if (_sample_freq > 0)
	{
		const int      mbs_align = (_max_block_size + 3) & ~3;
		buf.resize (mbs_align * nbr_buf_tot, 0);
		int            buf_idx = 0;
		for (auto &ptr : src_ptr_arr)
		{
			ptr = &buf [buf_idx * mbs_align];
			++ buf_idx;
		}
		for (auto &ptr : dst_ptr_arr)
		{
			ptr = &buf [buf_idx * mbs_align];
			++ buf_idx;
		}
		for (auto &ptr : sig_ptr_arr)
		{
			ptr = &buf [buf_idx * mbs_align];
			++ buf_idx;
		}
	}

	for (int pi_cnt = 0; pi_cnt < nbr_inst; ++ pi_cnt)
	{
		const int      pi_id = _plugin_pool.create (
			model, std::make_unique <PluginCb> (*this)
		);
		PluginPool::PluginDetails &   details =
			_plugin_pool.use_plugin (pi_id);
		if (_sample_freq > 0)
		{
			details._latency = 0;
#if ! defined (NDEBUG)
			int            ret_val =
#endif
				details._pi_uptr->reset (
					_sample_freq,
					_max_block_size,
					details._latency
				);
#if ! defined (NDEBUG)
			assert (ret_val == piapi::Err_OK);
#endif
		}
		check_and_get_default_settings (*details._pi_uptr, *details._desc_ptr, model);

		// Not used, actually
		doc._map_model_id [model] [pi_id] = false;

		// Sets the requested plug-in state by processing a sample buffer
		if (state_ptr != nullptr && _sample_freq > 0)
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

			const int         nbr_chn_i = 2;
			int               nbr_chn_o = 2;
			piapi::PluginInfo info { details._desc_ptr->get_info () };
			if (info._chn_pref == piapi::ChnPref::MONO)
			{
				nbr_chn_o = 1;
			}

			piapi::ProcInfo   proc_info;
			proc_info._byp_arr   = nullptr;
			proc_info._byp_state = piapi::BypassState_IGNORE;
			proc_info._dst_arr   = &dst_ptr_arr [0];
			proc_info._evt_arr   = (nbr_param > 0) ? &evt_ptr_list [0] : nullptr;
			proc_info._dir_arr [piapi::Dir_IN ]._nbr_chn = nbr_chn_i;
			proc_info._dir_arr [piapi::Dir_OUT]._nbr_chn = nbr_chn_o;
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
	return set_plugin (pos, Cst::_plugin_dwm, PiType_MIX, false, true);
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
	if (doc_ptr == nullptr)
	{
		doc_ptr = _cur_sptr.get ();
	}

	if (doc_ptr != nullptr)
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

	Plugin &       plug = doc.find_plugin (pi_id);

	plug._param_list = param_list;
}



void	Central::clear_mod (int pi_id)
{
	Document &     doc = modify ();

	assert (pi_id >= 0);

	Plugin &       plug = doc.find_plugin (pi_id);
	plug._ctrl_map.clear ();
}



void	Central::set_mod (int pi_id, int index, const doc::CtrlLinkSet &cls)
{
	Document &     doc = modify ();

	assert (pi_id >= 0);
	assert (index >= 0);
	assert (index < _plugin_pool.use_plugin (pi_id)._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL));

	Plugin &       plug = doc.find_plugin (pi_id);

	plug._ctrl_map [index] = std::make_shared <doc::CtrlLinkSet> (cls);
}



void	Central::set_sig_source (int pi_id, int sig_pin, int port_id)
{
	Document &     doc = modify ();

	assert (sig_pin >= 0);

	Plugin &       plug = doc.find_plugin (pi_id);

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



void	Central::set_prog_switch_mode (doc::ProgSwitchMode prog_switch_mode)
{
	assert (int (prog_switch_mode) >= 0);
	assert (prog_switch_mode < doc::ProgSwitchMode::NBR_ELT);

	modify ();

	_new_sptr->_prog_switch_mode = prog_switch_mode;
}



void	Central::clear_routing ()
{
	modify ();

	_new_sptr->_cnx_list.clear ();
}



void	Central::add_cnx (const Cnx &cnx)
{
	assert (   cnx.is_valid ());
	assert (   cnx._src._slot_type != CnxEnd::SlotType_DLY);
	assert (   cnx._src._slot_type != CnxEnd::SlotType_NORMAL
	        || cnx._src._slot_pos < int (_new_sptr->_slot_list.size ()));
	assert (   cnx._dst._slot_type != CnxEnd::SlotType_DLY);
	assert (   cnx._dst._slot_type != CnxEnd::SlotType_NORMAL
	        || cnx._dst._slot_pos < int (_new_sptr->_slot_list.size ()));

	modify ();

	_new_sptr->_cnx_list.push_back (cnx);
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
	if (cell_ptr != nullptr)
	{
		cell_ptr->_val._sender              = WaMsg::Sender_CMD;
		cell_ptr->_val._type                = WaMsg::Type_TEMPO;
		cell_ptr->_val._content._tempo._bpm = bpm;

		_queue_cmd_to_audio.enqueue (*cell_ptr);
	}
}



// Returns true if at least one message has been processed.
bool	Central::process_queue_audio_to_cmd ()
{
	bool           proc_flag = false;

	const std::chrono::microseconds  timeout (150 * 1000);
	std::chrono::microseconds        t_beg = get_cur_date ();

	int            a2c_cnt          = 0;
	bool           discard_a2c_flag = false;
	conc::LockFreeCell <WaMsg> * cell_ptr = nullptr;
	do
	{
		cell_ptr = _queue_audio_to_cmd.dequeue ();
		if (cell_ptr != nullptr)
		{
			proc_flag = true;
			if (cell_ptr->_val._sender == WaMsg::Sender_CMD)
			{
				if (cell_ptr->_val._type == WaMsg::Type_CTX)
				{
					const ProcessingContext *  ctx_ptr =
						cell_ptr->_val._content._ctx._ctx_ptr;
					if (ctx_ptr != nullptr)
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
				if (_cb_ptr != nullptr)
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
	while (cell_ptr != nullptr);

	proc_flag |= _d2d_rec.process_messages ().first;

	return proc_flag;
}



PluginPool &	Central::use_pi_pool ()
{
	return (_plugin_pool);
}



const PluginPool &	Central::use_pi_pool () const
{
	return (_plugin_pool);
}



const std::vector <std::string> &	Central::use_aud_pi_list () const
{
	return _pi_aud_type_list;
}



const std::vector <std::string> &	Central::use_sig_pi_list () const
{
	return _pi_sig_type_list;
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
	assert (pathname_0 != nullptr);

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



Document &	Central::modify ()
{
	if (_new_sptr.get () == nullptr)
	{
		if (_cur_sptr.get () == nullptr)
		{
			_new_sptr = std::make_shared <Document> ();
		}
		else
		{
			_new_sptr = std::make_shared <Document> (*_cur_sptr);
		}
		_new_sptr->_ctx_sptr.reset (); // Will be set during commit.
	}

	return *_new_sptr;
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

		int            latency = 0;

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
						if (_sample_freq > 0)
						{
							if (   force_reset_flag
							    || state != piapi::PluginInterface::State_ACTIVE)
							{
#if ! defined (NDEBUG)
								int            ret_val =
#endif
									details._pi_uptr->reset (
										_sample_freq, _max_block_size, latency
									);
								details._latency = latency;
#if ! defined (NDEBUG)
								assert (ret_val == piapi::Err_OK);
#endif
							}
							else
							{
								latency = details._latency;
							}
						}
						break;
					}
				}
			}
		}

		// Not found? Create one
		if (pi_id < 0)
		{
			pi_id = _plugin_pool.create (
				model, std::make_unique <PluginCb> (*this)
			);
			PluginPool::PluginDetails &   details =
				_plugin_pool.use_plugin (pi_id);
			if (_sample_freq > 0)
			{
#if ! defined (NDEBUG)
				int            ret_val =
#endif
					details._pi_uptr->reset (
						_sample_freq,
						_max_block_size,
						latency
					);
#if ! defined (NDEBUG)
				assert (ret_val == piapi::Err_OK);
#endif
				details._latency = latency;
			}
			check_and_get_default_settings (*details._pi_uptr, *details._desc_ptr, model);

			doc._map_model_id [model] [pi_id] = true;
		}

		plugin._pi_id   = pi_id;
		plugin._model   = model;
		plugin._latency = latency;
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

	plugin.clear ();
	doc._slot_list [pos]._force_mono_flag = false;
}



void	Central::create_routing ()
{
	assert (_new_sptr.get ()            != nullptr);
	assert (_new_sptr->_ctx_sptr.get () != nullptr);

	Document &           doc = *_new_sptr;
	_router.create_routing (doc, _plugin_pool);
}



void	Central::create_mod_maps ()
{
	assert (_new_sptr.get ()            != nullptr);
	assert (_new_sptr->_ctx_sptr.get () != nullptr);

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
				auto                       ctrl_param_sptr {
					std::make_shared <ControlledParam> (coord) };

				if (cls._bind_sptr.get () != nullptr)
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



void	Central::add_controller (ProcessingContext &ctx, const doc::CtrlLink &link, std::shared_ptr <ControlledParam> ctrl_param_sptr, bool abs_flag)
{
	const ParamCoord &   coord = ctrl_param_sptr->use_coord ();
	auto           unit_sptr { std::make_shared <CtrlUnit> (link, abs_flag) };

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
	assert (_new_sptr.get ()            != nullptr);
	assert (_new_sptr->_ctx_sptr.get () != nullptr);

	Document &           doc = *_new_sptr;

	for (Slot & slot : doc._slot_list)
	{
		for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
		{
			Plugin &          plug = slot._component_arr [type_cnt];
			if (plug._pi_id < 0)
			{
				break;
			}

			// Parameters
			const int      nbr_param = int (plug._param_list.size ());
			for (int index = 0; index < nbr_param; ++index)
			{
				conc::LockFreeCell <WaMsg> *  msg_ptr = make_param_msg (
					plug._pi_id,
					index,
					plug._param_list [index]
				);
				if (msg_ptr != nullptr)
				{
					msg_list.push_back (msg_ptr);
				}
			}

			plug._param_list.clear ();

			// Reset
			if (doc._ctx_sptr->_graph_changed_flag)
			{
				conc::LockFreeCell <WaMsg> *  msg_ptr = make_reset_msg (
					plug._pi_id,
					true,
					(type_cnt == PiType_MIX)
				);
				if (msg_ptr != nullptr)
				{
					msg_list.push_back (msg_ptr);
				}
			}
		}
	}
}



// May return nullptr (unexpected failure)
conc::LockFreeCell <WaMsg> *	Central::make_param_msg (int pi_id, int index, float val)
{
	conc::LockFreeCell <WaMsg> * cell_ptr =
		_msg_pool.take_cell (true);
	if (cell_ptr != nullptr)
	{
		cell_ptr->_val._sender = WaMsg::Sender_CMD;
		cell_ptr->_val._type   = WaMsg::Type_PARAM;
		cell_ptr->_val._content._param._plugin_id = pi_id;
		cell_ptr->_val._content._param._index     = index;
		cell_ptr->_val._content._param._val       = val;
	}

	return cell_ptr;
}



// May return nullptr (unexpected failure)
conc::LockFreeCell <WaMsg> *	Central::make_reset_msg (int pi_id, bool steady_flag, bool full_flag)
{
	conc::LockFreeCell <WaMsg> * cell_ptr =
		_msg_pool.take_cell (true);
	if (cell_ptr != nullptr)
	{
		cell_ptr->_val._sender = WaMsg::Sender_CMD;
		cell_ptr->_val._type   = WaMsg::Type_RESET;
		cell_ptr->_val._content._reset._plugin_id   = pi_id;
		cell_ptr->_val._content._reset._steady_flag = steady_flag;
		cell_ptr->_val._content._reset._full_flag   = full_flag;
	}

	return cell_ptr;
}



void	Central::check_and_get_default_settings (const piapi::PluginInterface &plug, const piapi::PluginDescInterface &desc, std::string model)
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
