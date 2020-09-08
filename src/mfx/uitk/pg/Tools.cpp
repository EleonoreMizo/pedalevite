/*****************************************************************************

        Tools.cpp
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

#include "fstb/fnc.h"
#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/ActionSettings.h"
#include "mfx/doc/ActionTempoSet.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/ui/Font.h"
#include "mfx/uitk/pg/CtrlSrcNamed.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NText.h"
#include "mfx/Model.h"
#include "mfx/ToolsParam.h"
#include "mfx/View.h"

#include <memory>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// val is a normalized value
// val = -1: use the settings content
// width in pixels
void	Tools::set_param_text (const Model &model, const View &view, int width, int index, float val, int slot_id, PiType type, NText *param_name_ptr, NText &param_val, NText *param_unit_ptr, NText *fx_name_ptr, bool group_unit_val_flag)
{
	assert (val <= 1);
	assert (width > 0);
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	if (param_unit_ptr == nullptr && group_unit_val_flag)
	{
		param_unit_ptr = &param_val;
	}

	std::string    txt_val; 
	std::string    txt_unit; 

	const doc::Preset &   preset = view.use_preset_cur ();
	if (val < 0)
	{
		val = view.get_param_val (preset, slot_id, type, index);
	}

	const doc::Slot &    slot     = preset.use_slot (slot_id);
	const std::string &  pi_model =
		(type == PiType_MIX) ? Cst::_plugin_dwm : slot._pi_model;
	const piapi::PluginDescInterface &  desc_pi =
		model.get_model_desc (pi_model);
	const piapi::ParamDescInterface &   desc    =
		desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

	size_t         pos_utf8        = 0;
	size_t         len_utf8        = 0;
	size_t         len_pix         = 0;
	size_t         rem_pix_fx_name = width;

	// Get value & unit
	std::string    val_s;
	std::string    unit;
	const double   tempo = view.get_tempo ();
	print_param_with_pres (
		val_s, unit,
		preset, slot_id, type, index, val, desc, tempo
	);

	// Value
	if (! group_unit_val_flag)
	{
		// Reformat: erases all leading whitespaces then adds a single one
		while (! val_s.empty () && val_s [0] == ' ')
		{
			val_s = val_s.substr (1);
		}
		size_t         pos = 0;
		do
		{
			pos = val_s.find ("\n ", pos);
			if (pos != std::string::npos)
			{
				++ pos;
				val_s.erase (val_s.begin () + pos);
			}
		}
		while (pos != std::string::npos);
		val_s = pi::param::Tools::join_strings_multi (
			val_s.c_str (), '\n', " ", ""
		);
	}
	pi::param::Tools::cut_str_bestfit (
		pos_utf8, len_utf8, len_pix,
		width, val_s.c_str (), '\n',
		param_val, &NText::get_char_width
	);
	txt_val = val_s.substr (pos_utf8, len_utf8);

	// Name
	if (param_name_ptr != nullptr)
	{
		std::string    name = desc.get_name (0);
		name = pi::param::Tools::print_name_bestfit (
			width - len_pix, name.c_str (),
			*param_name_ptr, &NText::get_char_width
		);
		param_name_ptr->set_text (name);
	}

	// Unit
	if (param_unit_ptr != nullptr)
	{
		int            w_unit = width;
		if (group_unit_val_flag)
		{
			w_unit -= int (len_pix);
			if (txt_val.back () != ' ' && ! unit.empty () && unit [0] != ' ')
			{
				unit = pi::param::Tools::join_strings_multi (
					unit.c_str (), '\n', " ", ""
				);
			}
		}
		pi::param::Tools::cut_str_bestfit (
			pos_utf8, len_utf8, len_pix,
			w_unit, unit.c_str (), '\n',
			*param_unit_ptr, &NText::get_char_width
		);
		txt_unit = unit.substr (pos_utf8, len_utf8);
		if (! group_unit_val_flag)
		{
			rem_pix_fx_name -= len_pix;
		}
	}

	if (group_unit_val_flag)
	{
		param_val.set_text (txt_val + txt_unit);
	}
	else
	{
		param_val.set_text (txt_val);
		if (param_unit_ptr != nullptr)
		{
			param_unit_ptr->set_text (txt_unit);
		}
	}

	if (fx_name_ptr != nullptr)
	{
		// Displays the main plug-in name, never the dry/wet mixer
		const piapi::PluginDescInterface &  desc_pi_main =
			model.get_model_desc (slot._pi_model);
		const piapi::PluginInfo pi_info { desc_pi_main.get_info () };
		std::string    pi_type_name = pi_info._name;

		pi_type_name = pi::param::Tools::print_name_bestfit (
			rem_pix_fx_name, pi_type_name.c_str (),
			*fx_name_ptr, &NText::get_char_width
		);
		fx_name_ptr->set_text (pi_type_name);
	}
}



void	Tools::print_param_with_pres (std::string &val_s, std::string &unit, const Model &model, const View &view, const doc::Preset &preset, int slot_id, PiType type, int index, float val)
{
	const doc::Slot &    slot     = preset.use_slot (slot_id);
	const std::string &  pi_model =
		(type == PiType_MIX) ? Cst::_plugin_dwm : slot._pi_model;
	const piapi::PluginDescInterface &  desc_pi =
		model.get_model_desc (pi_model);
	const piapi::ParamDescInterface &   desc    =
		desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

	const double   tempo = view.get_tempo ();
	print_param_with_pres (
		val_s, unit,
		preset, slot_id, type, index, val, desc, tempo
	);
}



MsgHandlerInterface::EvtProp	Tools::change_param (Model &model, const View &view, int slot_id, PiType type, int index, double step, int step_index, int dir)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (dir != 0);

	const doc::Preset &  preset = view.use_preset_cur ();
	double         val_nrm =
		view.get_param_val (preset, slot_id, type, index);

	val_nrm = change_param (
		val_nrm, model, view, slot_id, type, index, step, step_index, dir
	);

	// Check the beat case
	bool           set_flag = false;
	if (type == PiType_MAIN)
	{
		const doc::Slot &    slot = preset.use_slot (slot_id);
		const doc::PluginSettings& settings = slot.use_settings (PiType_MAIN);
		const doc::ParamPresentation *   pres_ptr =
			settings.use_pres_if_tempo_ctrl (index);

		// Uses beats
		if (pres_ptr != nullptr)
		{
			const double   tempo = view.get_tempo ();
			const piapi::PluginDescInterface &  desc_pi =
				model.get_model_desc (slot._pi_model);
			const piapi::ParamDescInterface &   desc    =
				desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
			const float    val_beats = float (
				ToolsParam::conv_nrm_to_beats (val_nrm, desc, tempo)
			);
			model.set_param_beats (slot_id, index, val_beats);
			set_flag = true;
		}
	}

	if (! set_flag)
	{
		model.set_param (slot_id, type, index, float (val_nrm));
	}

	return MsgHandlerInterface::EvtProp_CATCH;
}



double	Tools::change_param (double val_nrm, const Model &model, const View &view, int slot_id, PiType type, int index, double step, int step_index, int dir)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (dir != 0);

	const doc::Preset &  preset = view.use_preset_cur ();
	const doc::Slot &    slot   = preset.use_slot (slot_id);
	const doc::PluginSettings & settings = slot.use_settings (type);

	const std::string &  pi_model =
		(type == PiType_MIX) ? Cst::_plugin_dwm : slot._pi_model;
	const piapi::PluginDescInterface &  desc_pi =
		model.get_model_desc (pi_model);

	return change_param (
		val_nrm, view, desc_pi, settings, index, step, step_index, dir
	);
}



double	Tools::change_param (double val_nrm, const View &view, const piapi::PluginDescInterface &desc_pi, const doc::PluginSettings &settings, int index, double step, int step_index, int dir)
{
	assert (index >= 0);
	assert (dir != 0);

	const piapi::ParamDescInterface &   desc    =
		desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

	bool           done_flag = false;
	if (step_index == 0)
	{
		// Uses a notch list
		const std::set <float> *   notch_list_ptr =
			settings.find_notch_list (index);
		if (notch_list_ptr != nullptr)
		{
			const auto     it_notch = ToolsParam::advance_to_notch (
				float (val_nrm), *notch_list_ptr, dir
			);
			val_nrm   = *it_notch;
			done_flag = true;
		}

		else  // else if (type != PiType_MIX)
		{
			// Check if we can use the beat mode
			const doc::ParamPresentation *   pres_ptr =
				settings.use_pres_if_tempo_ctrl (index);
			if (pres_ptr != nullptr && desc.get_nat_min () >= 0)
			{
				const double   tempo     = view.get_tempo ();
				double         val_beats = ToolsParam::conv_nrm_to_beats (
					val_nrm, desc, tempo
				);

				const auto     it_notch = ToolsParam::advance_to_notch (
					float (val_beats), ToolsParam::_beat_notch_list, dir
				);
				val_beats = *it_notch;

				val_nrm   = float (ToolsParam::conv_beats_to_nrm (
					val_beats, desc, tempo
				));
				done_flag = true;
			}
		}
	}

	if (done_flag)
	{
		val_nrm = fstb::limit (val_nrm, 0.0 ,1.0);
	}
	else
	{
		val_nrm = change_param (val_nrm, desc_pi, index, step, dir);
	}

	return val_nrm;
}



double	Tools::change_param (double val_nrm, const piapi::PluginDescInterface &desc_pi, int index, double step, int dir)
{
	assert (index >= 0);
	assert (dir != 0);

	bool           done_flag = false;

	const piapi::ParamDescInterface &   desc =
		desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

	const piapi::ParamDescInterface::Range range = desc.get_range ();

	// Uses a discrete range
	if (range == piapi::ParamDescInterface::Range_DISCRETE)
	{
		double         val_nat = desc.conv_nrm_to_nat (val_nrm);
		val_nat += dir;
		const double   nat_min = desc.get_nat_min ();
		const double   nat_max = desc.get_nat_max ();
		val_nat   = fstb::limit (val_nat, nat_min, nat_max);
		val_nrm   = desc.conv_nat_to_nrm (val_nat);
		done_flag = true;
	}

	if (! done_flag)
	{
		val_nrm += step * dir;
	}

	val_nrm = fstb::limit (val_nrm, 0.0 ,1.0);

	return val_nrm;
}



// Returns -1 if not found
int	Tools::find_ctrl_index (const ControlSource &src, const std::vector <CtrlSrcNamed> &ctrl_list)
{
	const int      nbr_src    = int (ctrl_list.size ());
	int            found_pos  = -1;
	for (int pos = 0; pos < nbr_src && found_pos < 0; ++pos)
	{
		if (ctrl_list [pos]._src == src)
		{
			found_pos = pos;
		}
	}

	return found_pos;
}



std::string	Tools::find_ctrl_name (const ControlSource &src, const std::vector <CtrlSrcNamed> &ctrl_list)
{
	std::string    name = "\?\?\?";
	const int      pos  = find_ctrl_index (src, ctrl_list);
	if (pos >= 0)
	{
		name = ctrl_list [pos]._name;
	}

	return name;
}



std::vector <CtrlSrcNamed>	Tools::make_port_list (const Model &model, const View &view)
{
	const doc::Preset &  preset = view.use_preset_cur ();
	std::vector <CtrlSrcNamed> port_list;
	port_list.reserve (preset._port_map.size ());

	std::vector <NodeEntry>   entry_list;
	Tools::extract_slot_list (entry_list, preset, model);

	for (const auto &node_port : preset._port_map)
	{
		CtrlSrcNamed   csn;
		csn._src._type  = ControllerType_FX_SIG;
		csn._src._index = node_port.first;
		char           txt_0 [127+1];

		const int      slot_id = node_port.second._slot_id;
		const auto     it_slot = preset._slot_map.find (slot_id);

		if (it_slot == preset._slot_map.end () || preset.is_slot_empty (it_slot))
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"<Unknown %d-%d>", slot_id, node_port.second._sig_index + 1
			);
			csn._name = txt_0;
		}

		else
		{
			const doc::Slot & slot = *(it_slot->second);
			const piapi::PluginDescInterface &  desc =
				model.get_model_desc (slot._pi_model);
			int            nbr_i = 1;
			int            nbr_o = 1;
			int            nbr_s = 0;
			desc.get_nbr_io (nbr_i, nbr_o, nbr_s);

			const piapi::PluginInfo pi_info { desc.get_info () };
			std::string    multilabel = pi_info._name;
			std::string    txt_post;

			// Checks if there are multiple instance of this model in the preset
			const auto     it_entry = std::find_if (
				entry_list.begin (),
				entry_list.end (),
				[slot_id] (const NodeEntry &x) { return x._slot_id == slot_id; }
			);
			assert (it_entry != entry_list.end ());
			if (it_entry->_instance_nbr >= 0)
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), " %d", it_entry->_instance_nbr + 1
				);
				txt_post += txt_0;
			}

			// Checks if there are multiple signal pins on this plug-in
			if (nbr_s > 1)
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "-s%d", node_port.second._sig_index + 1
				);
				txt_post += txt_0;
			}

			if (! txt_post.empty ())
			{
				multilabel = pi::param::Tools::join_strings_multi (
					multilabel.c_str (), '\n', "", txt_post
				);
			}

			csn._name = multilabel;
		}

		port_list.push_back (csn);
	}

	std::sort (
		port_list.begin (),
		port_list.end (),
		[] (const CtrlSrcNamed &lhs, const CtrlSrcNamed &rhs)
		{
			return (lhs._name < rhs._name);
		}
	);

	return port_list;
}



// If slot_id < 0, a new slot is created
// Returns the slot_id resulting of the operation
int	Tools::change_plugin (Model &model, const View &view, int slot_id, int dir, const std::vector <std::string> &fx_list, bool chain_flag)
{
	assert (dir != 0);

	const int      nbr_types = int (fx_list.size ());

	// Index within the official plug-in list. end = empty
	int            pi_index  = nbr_types;
	if (slot_id >= 0)
	{
		const doc::Preset &  preset = view.use_preset_cur ();
		if (! preset.is_slot_empty (slot_id))
		{
			const doc::Slot & slot = preset.use_slot (slot_id);
			const std::string type = slot._pi_model;
			auto          it_type =
				std::find (fx_list.begin (), fx_list.end (), type);
			if (it_type == fx_list.end ())
			{
				// Shouldn't happen, but not harmful anyway
				assert (false);
			}
			else
			{
				pi_index = int (it_type - fx_list.begin ());
			}
		}
	}

	const int      mod_len = nbr_types + 1;
	pi_index += dir;
	pi_index = (pi_index + mod_len) % mod_len;

	std::string    model_id;
	if (pi_index < nbr_types)
	{
		model_id = fx_list [pi_index];
	}

	return change_plugin (model, view, slot_id, model_id, chain_flag);
}



// model_id.empty (): delete plug-in (and possibly the slot if this is the
// last one from the chain)
int	Tools::change_plugin (Model &model, const View &view, int slot_id, std::string model_id, bool chain_flag)
{
	int            audio_size = int (view.use_slot_list_aud ().size ());

	int            chain_pos = -1;
	if (chain_flag)
	{
		if (slot_id >= 0)
		{
			chain_pos = find_linear_index_audio_graph (view, slot_id);
			assert (chain_pos >= 0);
			assert (chain_pos <= audio_size);
		}
		else
		{
			chain_pos = audio_size;
		}
	}

	const doc::Preset &  preset = view.use_preset_cur ();

	// We need to add a slot at the end?
	if (! model_id.empty ())
	{
		if (chain_flag && chain_pos == audio_size)
		{
			slot_id = model.add_slot ();

			doc::Routing   routing = preset.use_routing (); // Makes a copy
			ToolsRouting::insert_slot_before (
				routing._cnx_audio_set,
				slot_id,
				-1
			);
			model.set_routing (routing);
		}
		else if (! chain_flag && slot_id < 0)
		{
			slot_id = model.add_slot ();
		}
	}

	if (model_id.empty ())
	{
		model.remove_plugin (slot_id);
	}
	else
	{
		model.set_plugin (slot_id, model_id);
		create_missing_signal_ports (model, view, slot_id);
	}

	// Last slot needs to be removed?
	if (model_id.empty ())
	{
		if (chain_flag && chain_pos == audio_size - 1)
		{
			/*
			We remove the last slot(s) only if all these conditions are met:
				- Their output is connected to the audio output
				- There is only one input on pin #0
			So the slot can be assumed as neutral in the audio graph.
			*/
			bool           del_flag = false;
			do
			{
				const ToolsRouting::NodeMap & graph = view.use_graph ();
				del_flag = (
					   preset.is_slot_empty (slot_id)
					&& ToolsRouting::is_slot_last_and_neutral (graph, slot_id)
				);
				if (del_flag)
				{
					doc::Routing   routing = preset.use_routing (); // Copy
					ToolsRouting::disconnect_slot (routing._cnx_audio_set, slot_id);
					model.set_routing (routing);
					model.remove_slot (slot_id);
					const std::vector <int> &  slot_list_aud =
						view.use_slot_list_aud ();
					audio_size = int (slot_list_aud.size ());
					if (audio_size == 0)
					{
						slot_id = -1;
					}
					else
					{
						slot_id = slot_list_aud.back ();
					}
				}
			}
			while (del_flag && slot_id >= 0);
			slot_id = -1;
		}
		else if (! chain_flag)
		{
			model.remove_slot (slot_id);
			slot_id = -1;
		}
	}

	return slot_id;
}



void	Tools::assign_default_rotenc_mapping (Model &model, const View &view, int slot_id, int page)
{
	std::vector <Model::RotEncOverride> ovr_arr;

	const doc::Preset &  preset = view.use_preset_cur ();

	// Makes sure the slot really exists. Such a case could happen
	// when we clear the last slot (by changing the plug-in type), and
	// the previous slot was also empty. So both slots are removed
	// automatically, therefore _loc_edit._slot_id points to an
	// inexisting slot during the various model notifications sends
	// during the process.
	const auto     it_slot = preset._slot_map.find (slot_id);
	if (it_slot != preset._slot_map.end () && ! preset.is_slot_empty (it_slot))
	{
		const doc::Slot & slot = *(it_slot->second);
		const auto        it_s = slot._settings_all.find (slot._pi_model);
		if (it_s != slot._settings_all.end ())
		{
			const int      nbr_param = int (it_s->second._param_list.size ());
			ovr_arr.reserve (Cst::RotEnc_GEN);
			for (int pos = 0; pos < Cst::RotEnc_NBR_GEN; ++pos)
			{
				const int      index = Cst::RotEnc_NBR_GEN * page + pos;
				if (index < nbr_param)
				{
					const int      rotenc_index = Cst::RotEnc_GEN + pos;
					Model::RotEncOverride   reo;
					reo._slot_id      = slot_id;
					reo._pi_type      = PiType_MAIN;
					reo._index        = index;
					reo._rotenc_index = rotenc_index;
					ovr_arr.push_back (reo);
				}
			}
		}
	}

	model.reset_and_override_param_ctrl_multi (ovr_arr);
}



// Returns true if data is available (slot not empty)
// Otherwise data is left as it is
bool	Tools::get_physical_io (int &nbr_i, int &nbr_o, int &nbr_s, int slot_id, const doc::Preset &prog, const Model &model)
{
	const bool     exist_flag = ! prog.is_slot_empty (slot_id);
	if (exist_flag)
	{
		const doc::Slot & slot     = prog.use_slot (slot_id);
		const std::string pi_model = slot._pi_model;
		const piapi::PluginDescInterface &  pi_desc =
			model.get_model_desc (pi_model);
		nbr_i = 1;
		nbr_o = 1;
		nbr_s = 0;
		pi_desc.get_nbr_io (nbr_i, nbr_o, nbr_s);
	}

	return exist_flag;
}



std::string	Tools::conv_pedal_conf_to_short_txt (PedalConf &conf, const doc::PedalActionGroup &group, const Model &model, const View &view)
{
	std::string    txt = "<Complex>";
	conf = PedalConf_OTHER;

	std::string    name;
	if (is_pedal_empty (group))
	{
		txt  = "<Empty>";
		conf = PedalConf_EMPTY;
	}
	else if (is_pedal_simple_action (group, model, view, name))
	{
		txt  = name;
		conf = PedalConf_PUSH;
	}
	else if (is_pedal_momentary_button (group, model, view, name))
	{
		txt  = name;
		conf = PedalConf_MOMENTARY;
	}
	else if (is_pedal_toggle (group, model, view, name))
	{
		txt  = name;
		conf = PedalConf_TOGGLE;
	}

	return txt;
}




std::string	Tools::conv_pedal_conf_to_short_txt (PedalConf &conf, const doc::PedalboardLayout &layout, int index, const Model &model, const View &view)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_pedals);

	const doc::PedalActionGroup & group = layout._pedal_arr [index];

	return conv_pedal_conf_to_short_txt (conf, group, model, view);
}



std::string	Tools::conv_pedal_action_to_short_txt (const doc::PedalActionSingleInterface &action, const Model &model, const View &view)
{
	std::string    name;
	char           txt_0 [127+1];
	const doc::ActionType   type = action.get_type ();

	switch (type)
	{
	case doc::ActionType_BANK:
		{
			const doc::ActionBank & bank =
				dynamic_cast <const doc::ActionBank &> (action);
			if (bank._relative_flag)
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%s", (bank._val < 0) ? "-" : "+"
				);
			}
			else
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%d", bank._val
				);
			}
			name = "Bank ";
			name += txt_0;
		}
		break;

	case doc::ActionType_PRESET:
		{
			const doc::ActionPreset &  preset =
				dynamic_cast <const doc::ActionPreset &> (action);
			if (preset._relative_flag)
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%s", (preset._val < 0) ? "-" : "+"
				);
			}
			else
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%d", preset._val
				);
			}
			name = "Prog ";
			name += txt_0;
		}
		break;

	case doc::ActionType_TOGGLE_TUNER:
		name = "Tuner";
		break;

	case doc::ActionType_TOGGLE_FX:
		{
			const doc::ActionToggleFx &  param =
				dynamic_cast <const doc::ActionToggleFx &> (action);
			name = print_action_toggle_fx (param, model, view);
			name = "Toggle FX " + name;
		}
		break;

	case doc::ActionType_LOOP_REC:
		name = "Loop rec";
		break;

	case doc::ActionType_LOOP_PLAY_STOP:
		name = "Loop P/S";
		break;

	case doc::ActionType_LOOP_UNDO:
		name = "Loop undo";
		break;

	case doc::ActionType_PARAM:
		{
			const doc::ActionParam &  param =
				dynamic_cast <const doc::ActionParam &> (action);
			name = print_action_param (param, model, view);
			name = "Set " + name;
		}
		break;

	case doc::ActionType_TEMPO:
		name = "Tempo tap";
		break;

	case doc::ActionType_SETTINGS:
		{
			const doc::ActionSettings &   settings =
				dynamic_cast <const doc::ActionSettings &> (action);
			if (settings._relative_flag)
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%s", (settings._val < 0) ? "-" : "+"
				);
			}
			else
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%d", settings._val
				);
			}
			name = "Preset ";
			name += txt_0;
		}
		break;

	case doc::ActionType_EVENT:
		name = "Event\?";
		break;

	case doc::ActionType_TEMPO_SET:
		{
			const doc::ActionTempoSet & tempo =
				dynamic_cast <const doc::ActionTempoSet &> (action);
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "BPM %.0f", tempo._tempo_bpm
			);
			name = txt_0;
		}
		break;

	case doc::ActionType_CLICK:
		name = "Click";
		break;

	default:
		fstb::snprintf4all (txt_0, sizeof (txt_0), "type %d", type);
		name = txt_0;
		break;
	}

	return name;
}



int	Tools::extract_slot_list (std::vector <NodeEntry> &slot_list, const doc::Preset &preset, const Model &model)
{
	slot_list.clear ();

	std::vector <int> slot_id_list;
	const int     audio_len = ToolsRouting::build_ordered_node_list (
		slot_id_list, true, preset, model.use_aud_pi_list ()
	);
	std::map <std::string, int>   type_map;      // [type   ] = count
	std::map <int, NodeEntry>     instance_map;  // [slot_id] = data

	for (auto it_slot = preset._slot_map.begin ()
	;	it_slot != preset._slot_map.end ()
	;	++ it_slot)
	{
		NodeEntry      entry;

		const int      slot_id = it_slot->first;
		entry._slot_id = slot_id;

		if (! preset.is_slot_empty (it_slot))
		{
			const doc::Slot & slot = *(it_slot->second);
			entry._type = slot._pi_model;

			const auto &   desc = model.get_model_desc (slot._pi_model);
			const piapi::PluginInfo pi_info { desc.get_info () };
			entry._name_multilabel = pi_info._name;
		}

		auto           it_count = type_map.emplace (std::make_pair (
			entry._type, 0
		)).first;
		entry._instance_nbr = it_count->second;
		++ it_count->second;

		instance_map [slot_id] = entry;
	}

	for (auto slot_id : slot_id_list)
	{
		NodeEntry &    entry = instance_map [slot_id];
		if (type_map [entry._type] <= 1)
		{
			entry._instance_nbr = -1;
		}
		slot_list.push_back (entry);
	}

	return audio_len;
}



std::string	Tools::build_slot_name_with_index (const NodeEntry &entry)
{
	std::string    multilabel = entry._name_multilabel;

	if (entry._instance_nbr >= 0)
	{
		char        txt_0 [127+1];
		fstb::snprintf4all (
			txt_0, sizeof (txt_0), " %d", entry._instance_nbr + 1
		);
		multilabel = pi::param::Tools::join_strings_multi (
			multilabel.c_str (), '\n', "", txt_0
		);
	}

	return multilabel;
}



// Returns -1 if not found
int	Tools::find_linear_index_audio_graph (const View &view, int slot_id)
{
	assert (slot_id >= 0);

	const auto &   slot_list_aud = view.use_slot_list_aud ();
	return ToolsRouting::find_linear_index_audio_graph (slot_list_aud, slot_id);
}



// Gives only the main plug-in type
// Returns an empty string if not found
std::string	Tools::find_fx_type (const doc::FxId &fx_id, const View &view)
{
	std::string    type;

	if (fx_id._location_type == doc::FxId::LocType_CATEGORY)
	{
		type = fx_id._label_or_model;
	}
	else
	{
		const doc::Setup &   setup = view.use_setup ();

		// First, search in the current preset
		{
			const int      preset_index = view.get_preset_index ();
			const int      bank_index   = view.get_bank_index ();
			const doc::Preset &  preset =
				setup._bank_arr [bank_index]._preset_arr [preset_index];
			type = find_fx_type_in_preset (fx_id._label_or_model, preset);
		}

		if (type.empty ())
		{
			bool           found_flag = false;
			for (auto & bank : setup._bank_arr)
			{
				for (auto & preset : bank._preset_arr)
				{
					type = find_fx_type_in_preset (fx_id._label_or_model, preset);
					if (! type.empty ())
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
		}
	}

	return type;
}



// Gives only the main plug-in type
// Returns an empty string if not found
std::string	Tools::find_fx_type_in_preset (const std::string &label, const doc::Preset &preset)
{
	std::string    type;

	bool           found_flag = false;
	for (auto it_slot = preset._slot_map.begin ()
	;	it_slot != preset._slot_map.end () && ! found_flag
	;	++ it_slot)
	{
		if (! preset.is_slot_empty (it_slot))
		{
			const doc::Slot & slot = *(it_slot->second);
			if (slot._label == label)
			{
				type       = slot._pi_model;
				found_flag = true;
			}
		}
	}

	return type;
}



// Returns model_name as a multi-label string
// Returns empty strings if the effect cannot be located.
void	Tools::print_action_toggle_fx (std::string &model_name, const doc::ActionToggleFx &action, const Model &model, const View &view)
{
	model_name.clear ();
	const std::string model_id = find_fx_type (action._fx_id, view);
	if (! model_id.empty ())
	{
		const piapi::PluginDescInterface & desc_main =
			model.get_model_desc (model_id);
		if (action._fx_id._location_type == doc::FxId::LocType_CATEGORY)
		{
			const piapi::PluginInfo pi_info { desc_main.get_info () };
			model_name = pi_info._name;
		}
		else
		{
			model_name  = "\xE2\x80\x9C"; // U+201C LEFT DOUBLE QUOTATION MARK
			model_name += action._fx_id._label_or_model;
			model_name += "\xE2\x80\x9D"; // U+201D RIGHT DOUBLE QUOTATION MARK
		}
	}
}



// Returns model_name as a multi-label string
// Returns empty strings if the effect cannot be located.
void	Tools::print_action_param (std::string &model_name, std::string &param_name, const doc::ActionParam &action, const Model &model, const View &view)
{
	model_name.clear ();
	param_name.clear ();
	const std::string model_id = find_fx_type (action._fx_id, view);
	if (! model_id.empty ())
	{
		const piapi::PluginDescInterface & desc_main =
			model.get_model_desc (model_id);
		if (action._fx_id._location_type == doc::FxId::LocType_CATEGORY)
		{
			const piapi::PluginInfo pi_info { desc_main.get_info () };
			model_name = pi_info._name;
		}
		else
		{
			model_name  = "\xE2\x80\x9C"; // U+201C LEFT DOUBLE QUOTATION MARK
			model_name += action._fx_id._label_or_model;
			model_name += "\xE2\x80\x9D"; // U+201D RIGHT DOUBLE QUOTATION MARK
		}

		const piapi::PluginDescInterface & desc = model.get_model_desc (
			(action._fx_id._type == PiType_MAIN) ? model_id : Cst::_plugin_dwm
		);
		const int      nbr_param =
			desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
		if (action._index < nbr_param)
		{
			const piapi::ParamDescInterface & param_desc =
				desc.get_param_info (piapi::ParamCateg_GLOBAL, action._index);
			param_name = param_desc.get_name (0);
		}
		else
		{
			char           txt_0 [127+1];
			fstb::snprintf4all (txt_0, sizeof (txt_0), "%d", action._index);
			param_name = txt_0;
		}
	}
}



// nbr_pins == 1: doesn't print the pin index
// If not known, nbr_pins can be 0.
void	Tools::print_cnx_name (NText &txtbox, int width, const std::vector <Tools::NodeEntry> &entry_list, piapi::Dir dir, const doc::CnxEnd &cnx_end, const char prefix_0 [], int nbr_pins)
{
	assert (width > 0);
	assert (   cnx_end.get_type () == doc::CnxEnd::Type_IO
	        || ! entry_list.empty ());
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);
	assert (prefix_0 != nullptr);
	assert (nbr_pins >= 0);
	assert (nbr_pins == 0 || cnx_end.get_pin () < nbr_pins);

	char           txt_0 [255+1];

	const doc::CnxEnd::Type end_type = cnx_end.get_type ();
	std::string    multilabel;
	if (end_type == doc::CnxEnd::Type_IO)
	{
		multilabel = "Audio ";
		multilabel += _dir_txt_arr [dir];
	}
	else
	{
		// Retrieves plug-in name
		const int      end_slot_id = cnx_end.get_slot_id ();
		const auto     it_entry    = std::find_if (
			entry_list.begin (),
			entry_list.end (),
			[end_slot_id] (const NodeEntry &entry)
			{
				return (entry._slot_id == end_slot_id);
			}
		);
		assert (it_entry != entry_list.end ());
		multilabel = it_entry->_name_multilabel;
		if (it_entry->_instance_nbr >= 0)
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), " %d", it_entry->_instance_nbr + 1
			);
			multilabel = pi::param::Tools::join_strings_multi (
				multilabel.c_str (), '\n', "", txt_0
			);
		}
	}

	if (nbr_pins == 1)
	{
		txt_0 [0] = '\0';
	}
	else
	{
		const int      end_pin_idx = cnx_end.get_pin ();
		fstb::snprintf4all (
			txt_0, sizeof (txt_0), " - %d", end_pin_idx + 1
		);
	}
	multilabel = pi::param::Tools::join_strings_multi (
		multilabel.c_str (), '\n', prefix_0, txt_0
	);

	std::string    txt = pi::param::Tools::print_name_bestfit (
		width, multilabel.c_str (),
		txtbox, &NText::get_char_width
	);
	txtbox.set_text (txt);
}



void	Tools::create_bank_list (TxtArray &bank_list, ContainerInterface &menu, PageMgrInterface::NavLocList &nav_list, const View &view, const ui::Font &fnt, int y, int w, bool chk_cur_flag)
{
	const int      h_m   = fnt.get_char_h ();
	const doc::Setup &   setup = view.use_setup ();
	const int      bank_index_cur = view.get_bank_index ();
	for (int bank_index = 0; bank_index < Cst::_nbr_banks; ++bank_index)
	{
		const int      node_id   = bank_index;
		TxtSPtr        name_sptr = std::make_shared <NText> (node_id);

		char           txt_0 [255+1];
		const doc::Bank & bank = setup._bank_arr [bank_index];

		if (chk_cur_flag)
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"%s %02d %s",
				(bank_index == bank_index_cur) ? "\xE2\x9C\x93" : " ",   // U+2713 CHECK MARK
				bank_index,
				bank._name.c_str ()
			);
		}
		else
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "%02d %s", bank_index, bank._name.c_str ()
			);
		}

		name_sptr->set_font (fnt);
		name_sptr->set_text (txt_0);
		name_sptr->set_coord (Vec2d (0, y + bank_index * h_m));
		name_sptr->set_frame (Vec2d (w, 0), Vec2d ());

		bank_list.push_back (name_sptr);
		menu.push_back (name_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);
	}
}



void	Tools::create_prog_list (TxtArray &prog_list, ContainerInterface &menu, PageMgrInterface::NavLocList &nav_list, const View &view, const ui::Font &fnt, int y, int w)
{
	const int      h_m   = fnt.get_char_h ();
	const doc::Setup &   setup = view.use_setup ();
	const int      bank_index  = view.get_bank_index ();
	const doc::Bank &    bank  = setup._bank_arr [bank_index];

	for (int prog_index = 0; prog_index < Cst::_nbr_presets_per_bank; ++prog_index)
	{
		const int      node_id   = prog_index;
		TxtSPtr        name_sptr = std::make_shared <NText> (node_id);

		char           txt_0 [255+1];
		const doc::Preset &  preset = bank._preset_arr [prog_index];

		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "%02d %s", prog_index, preset._name.c_str ()
		);

		name_sptr->set_font (fnt);
		name_sptr->set_text (txt_0);
		name_sptr->set_coord (Vec2d (0, y + prog_index * h_m));
		name_sptr->set_frame (Vec2d (w, 0), Vec2d ());

		prog_list.push_back (name_sptr);
		menu.push_back (name_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);
	}
}



void	Tools::draw_curve (std::vector <int32_t> y_arr, uint8_t *disp_ptr, int height, int stride, uint8_t c)
{
	const int      width = int (y_arr.size ());
	for (int x = 0; x < width; ++x)
	{
		int            y = y_arr [x];
		if (y >= 0 && y < height)
		{
			disp_ptr [y * stride + x] = c;
		}
		if (x > 0)
		{
			complete_v_seg (disp_ptr, x, y, y_arr [x - 1], height, stride, c);
		}
		if (x < width - 1)
		{
			complete_v_seg (disp_ptr, x, y, y_arr [x + 1], height, stride, c);
		}
	}
}



void	Tools::complete_v_seg (uint8_t *disp_ptr, int x, int y, int yn, int height, int stride, uint8_t c)
{
	if (std::abs (yn - y) > 1)
	{
		int            r_cst = (y > yn) ? 1 : -1;
		int            y_mid = (yn + y + r_cst) / 2;
		if (y_mid < y)
		{
			std::swap (y, y_mid);
		}
		if (y_mid >= 0 && y < height)
		{
			y     = fstb::limit (y    , 0, height - 1);
			y_mid = fstb::limit (y_mid, 0, height - 1);
			for (int yy = y; yy <= y_mid; ++yy)
			{
				disp_ptr [yy * stride + x] = c;
			}
		}
	}
}



std::array <const char *, piapi::Dir_NBR_ELT>	Tools::_dir_txt_arr =
{{
	"In", "Out"
}};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tools::print_param_with_pres (std::string &val_s, std::string &unit, const doc::Preset &preset, int slot_id, PiType type, int index, float val, const piapi::ParamDescInterface &desc, double tempo)
{
	val_s.clear ();
	unit.clear ();

	const double   nat = desc.conv_nrm_to_nat (val);

	// Checks if the parameter has a forced display type
	if (type == PiType_MAIN)
	{
		const piapi::ParamDescInterface::Categ categ = desc.get_categ ();
		if (categ != piapi::ParamDescInterface::Categ_UNDEFINED)
		{
			const doc::Slot & slot = preset.use_slot (slot_id);
			const auto     it_settings = slot._settings_all.find (slot._pi_model);
			assert (it_settings != slot._settings_all.end ());
			const doc::PluginSettings &	settings = it_settings->second;
			const auto     it_pres = settings._map_param_pres.find (index);
			if (it_pres != settings._map_param_pres.end ())
			{
				const double   val_min  = desc.get_nat_min ();
				const double   val_max  = desc.get_nat_max ();

				if (val_min > 0)
				{
					double         val_hdn  = nat;
					bool           hdn_flag = false;
					pi::param::HelperDispNum   hdn;

					switch (it_pres->second._disp_mode)
					{
					case doc::ParamPresentation::DispMode_MS:
						hdn.set_preset (pi::param::HelperDispNum::Preset_FLOAT_MILLI);
						unit = "ms";
						switch (categ)
						{
						case piapi::ParamDescInterface::Categ_TIME_HZ:
						case piapi::ParamDescInterface::Categ_FREQ_HZ:
							hdn.set_print_format ("%.1f");
							hdn.set_range (1.0 / val_max, 1.0 / val_min);
							val_hdn = 1.0 / nat;
							hdn_flag = true;
							break;
						default:
							//Nothing
							break;
						}
						break;
					case doc::ParamPresentation::DispMode_HZ:
						hdn.set_preset (pi::param::HelperDispNum::Preset_FLOAT_STD);
						unit = "Hz";
						switch (categ)
						{
						case piapi::ParamDescInterface::Categ_TIME_S:
							hdn.set_print_format ("%.3f");
							hdn.set_range (1.0 / val_max, 1.0 / val_min);
							val_hdn = 1.0 / nat;
							hdn_flag = true;
							break;
						default:
							//Nothing
							break;
						}
						break;
					case doc::ParamPresentation::DispMode_NOTE:
						hdn.set_preset (pi::param::HelperDispNum::Preset_FREQ_NOTE);
						switch (categ)
						{
						case piapi::ParamDescInterface::Categ_TIME_S:
							hdn.set_range (1.0 / val_max, 1.0 / val_min);
							val_hdn = 1.0 / nat;
							hdn_flag = true;
							break;
						case piapi::ParamDescInterface::Categ_TIME_HZ:
						case piapi::ParamDescInterface::Categ_FREQ_HZ:
							hdn.set_range (val_min, val_max);
							hdn_flag = true;
							break;
						default:
							//Nothing
							break;
						}
						break;
					case doc::ParamPresentation::DispMode_BEATS:
						hdn.set_preset (pi::param::HelperDispNum::Preset_FRAC_STD);
						unit = "beats";
						switch (categ)
						{
						case piapi::ParamDescInterface::Categ_TIME_S:
							hdn.set_range (val_min * tempo / 60, val_max * tempo / 60);
							val_hdn = nat * tempo / 60;
							hdn_flag = true;
							break;
						case piapi::ParamDescInterface::Categ_TIME_HZ:
						case piapi::ParamDescInterface::Categ_FREQ_HZ:
							hdn.set_range (tempo / (val_max * 60), tempo / (val_min * 60));
							val_hdn = tempo / (nat * 60);
							hdn_flag = true;
							break;
						default:
							//Nothing
							break;
						}
					default:
						// Nothing
						break;
					}

					if (hdn_flag)
					{
						char           txt_0 [127+1];
						const int      ret_val =
							hdn.conv_to_str (val_hdn, txt_0, sizeof (txt_0));
						if (ret_val == pi::param::HelperDispNum::Err_OK)
						{
							val_s = txt_0;
						}
					}
				}
			}
		}
	}
	if (val_s.empty ())
	{
		val_s = desc.conv_nat_to_str (nat, 0);
		unit  = desc.get_unit (0);
	}
}



bool	Tools::is_pedal_empty (const doc::PedalActionGroup &group)
{
	bool           empty_flag = true;

	for (int trig_cnt = 0; trig_cnt < doc::ActionTrigger_NBR_ELT && empty_flag; ++trig_cnt)
	{
		const doc::PedalActionCycle & cycle = group._action_arr [trig_cnt];
		empty_flag = cycle.is_empty_default ();
	}

	return empty_flag;
}



bool	Tools::is_pedal_simple_action (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name)
{
	name.clear ();

	// Finds candidate actions
	bool           other_flag = false;
	const doc::PedalActionSingleInterface *   action_ptr = nullptr;
	for (int trig_cnt = 0; trig_cnt < doc::ActionTrigger_NBR_ELT; ++trig_cnt)
	{
		const doc::PedalActionCycle & cycle = group._action_arr [trig_cnt];
		if (! cycle.is_empty_default ())
		{
			if (trig_cnt == doc::ActionTrigger_PRESS)
			{
				if (cycle._cycle.size () == 1)
				{
					const doc::PedalActionCycle::ActionArray & action_arr =
						cycle._cycle [0];
					if (action_arr.size () == 1)
					{
						action_ptr = action_arr [0].get ();
					}
					else
					{
						other_flag = ! action_arr.empty ();
					}
				}
				else
				{
					other_flag = ! cycle._cycle.empty ();
				}
			}
			else
			{
				other_flag = true;
			}
		}
	}

	// Checks actions
	bool           ok_flag = false;
	if (action_ptr != nullptr && ! other_flag)
	{
		name    = conv_pedal_action_to_short_txt (*action_ptr, model, view);
		ok_flag = true;
	}

	return ok_flag;
}



bool	Tools::is_pedal_momentary_button (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name)
{
	name.clear ();

	// Finds candidate actions
	bool           other_flag = false;
	// Press, release
	std::array <const doc::PedalActionSingleInterface *, 2>  action_ptr_arr =
	{{ nullptr, nullptr }};
	for (int trig_cnt = 0; trig_cnt < doc::ActionTrigger_NBR_ELT; ++trig_cnt)
	{
		const doc::PedalActionCycle & cycle = group._action_arr [trig_cnt];
		if (! cycle.is_empty_default ())
		{
			if (   trig_cnt == doc::ActionTrigger_PRESS
			    || trig_cnt == doc::ActionTrigger_RELEASE)
			{
				if (cycle._cycle.size () == 1)
				{
					const doc::PedalActionCycle::ActionArray & action_arr =
						cycle._cycle [0];
					if (action_arr.size () == 1)
					{
						const int      index =
							(trig_cnt == doc::ActionTrigger_RELEASE) ? 1 : 0;
						action_ptr_arr [index] = action_arr [0].get ();
					}
					else
					{
						other_flag = ! action_arr.empty ();
					}
				}
				else
				{
					other_flag = ! cycle._cycle.empty ();
				}
			}
			else
			{
				other_flag = true;
			}
		}
	}

	// Checks actions
	bool           ok_flag = false;
	if (   action_ptr_arr [0] != nullptr
	    && action_ptr_arr [1] != nullptr
	    && ! other_flag)
	{
		const doc::ActionType   type_0 = action_ptr_arr [0]->get_type ();
		const doc::ActionType   type_1 = action_ptr_arr [1]->get_type ();
		ok_flag = (type_0 == type_1 && type_0 == doc::ActionType_PARAM);
		if (ok_flag)
		{
			const doc::ActionParam &  p_0 =
				dynamic_cast <const doc::ActionParam &> (*action_ptr_arr [0]);
			const doc::ActionParam &  p_1 =
				dynamic_cast <const doc::ActionParam &> (*action_ptr_arr [1]);
			ok_flag =
				(   p_0._fx_id == p_1._fx_id
				 && p_0._index == p_1._index);
			if (ok_flag)
			{
				name  = "PBSet ";
				name += print_action_param (p_0, model, view);
			}
		}
	}

	return ok_flag;
}



bool	Tools::is_pedal_toggle (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name)
{
	name.clear ();

	// Finds candidate actions
	bool           other_flag = false;
	// For each cycle
	std::array <const doc::PedalActionSingleInterface *, 2>  action_ptr_arr =
	{{ nullptr, nullptr }};
	for (int trig_cnt = 0; trig_cnt < doc::ActionTrigger_NBR_ELT; ++trig_cnt)
	{
		const doc::PedalActionCycle & cycle = group._action_arr [trig_cnt];
		if (! cycle.is_empty_default ())
		{
			if (trig_cnt == doc::ActionTrigger_PRESS)
			{
				if (cycle._cycle.size () == 2)
				{
					for (int cyc_cnt = 0; cyc_cnt < 2 && ! other_flag; ++cyc_cnt)
					{
						const doc::PedalActionCycle::ActionArray & action_arr =
							cycle._cycle [cyc_cnt];
						if (action_arr.size () == 1)
						{
							action_ptr_arr [cyc_cnt] = action_arr [0].get ();
						}
						else
						{
							other_flag = ! action_arr.empty ();
						}
					}
				}
				else
				{
					other_flag = ! cycle._cycle.empty ();
				}
			}
			else
			{
				other_flag = true;
			}
		}
	}

	// Checks actions
	bool           ok_flag = false;
	if (   action_ptr_arr [0] != nullptr
	    && action_ptr_arr [1] != nullptr
	    && ! other_flag)
	{
		const doc::ActionType   type_0 = action_ptr_arr [0]->get_type ();
		const doc::ActionType   type_1 = action_ptr_arr [1]->get_type ();
		ok_flag = (type_0 == type_1 && type_0 == doc::ActionType_PARAM);
		if (ok_flag)
		{
			const doc::ActionParam &  p_0 =
				dynamic_cast <const doc::ActionParam &> (*action_ptr_arr [0]);
			const doc::ActionParam &  p_1 =
				dynamic_cast <const doc::ActionParam &> (*action_ptr_arr [1]);
			ok_flag =
				(   p_0._fx_id == p_1._fx_id
				 && p_0._index == p_1._index);
			if (ok_flag)
			{
				name  = "Toggle ";
				name += print_action_param (p_0, model, view);
			}
		}
	}

	return ok_flag;
}



std::string	Tools::print_action_toggle_fx (const doc::ActionToggleFx &action, const Model &model, const View &view)
{
	std::string    model_name;
	print_action_toggle_fx (model_name, action, model, view);
	model_name = pi::param::Tools::print_name_bestfit (
		100, model_name.c_str ()
	);
	return model_name;
}



std::string	Tools::print_action_param (const doc::ActionParam &action, const Model &model, const View &view)
{
	std::string    model_name;
	std::string    param_name;
	print_action_param (model_name, param_name, action, model, view);

	model_name = pi::param::Tools::print_name_bestfit (
		8, model_name.c_str ()
	);
	param_name = pi::param::Tools::print_name_bestfit (
		100, param_name.c_str ()
	);

	return model_name + " " + param_name;
}



void	Tools::create_missing_signal_ports (Model &model, const View &view, int slot_id)
{
	const doc::Preset &  preset = view.use_preset_cur ();
	const doc::Slot &    slot   = preset.use_slot (slot_id);
	const piapi::PluginDescInterface &  desc =
		model.get_model_desc (slot._pi_model);
	int            nbr_i = 1;
	int            nbr_o = 1;
	int            nbr_s = 0;
	desc.get_nbr_io (nbr_i, nbr_o, nbr_s);
	if (nbr_s > 0)
	{
		// First, checks if there are existing ports
		uint64_t       exist_mask = 0;
		for (auto it_node : preset._port_map)
		{
			if (it_node.second._slot_id == slot_id)
			{
				exist_mask |= (uint64_t (1)) << it_node.second._sig_index;
			}
		}

		// Adds missing ports
		for (int sig_index = 0; sig_index < nbr_s; ++sig_index)
		{
			if ((exist_mask & (uint64_t (1)) << sig_index) == 0)
			{
				const int      port_index = preset.find_free_port ();
				doc::SignalPort   port;
				port._slot_id   = slot_id;
				port._sig_index = sig_index;
				model.set_signal_port (port_index, port);
			}
		}
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
