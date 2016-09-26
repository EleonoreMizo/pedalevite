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
#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/uitk/pg/CtrlSrcNamed.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NText.h"
#include "mfx/Model.h"
#include "mfx/ToolsParam.h"
#include "mfx/View.h"

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
void	Tools::set_param_text (const Model &model, const View &view, int width, int index, float val, int slot_index, PiType type, NText *param_name_ptr, NText &param_val, NText *param_unit_ptr, NText *fx_name_ptr, bool group_unit_val_flag)
{
	assert (val <= 1);
	assert (width > 0);
	assert (slot_index >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	if (param_unit_ptr == 0 && group_unit_val_flag)
	{
		param_unit_ptr = &param_val;
	}

	std::string    txt_val; 
	std::string    txt_unit; 

	const doc::Preset &   preset = view.use_preset_cur ();
	if (val < 0)
	{
		val = view.get_param_val (preset, slot_index, type, index);
	}

	assert (! preset.is_slot_empty (slot_index));
	const doc::Slot &    slot     = *(preset._slot_list [slot_index]);
	const std::string &  pi_model =
		(type == PiType_MIX) ? Cst::_plugin_mix : slot._pi_model;
	const piapi::PluginDescInterface &  desc_pi =
		model.get_model_desc (pi_model);
	const piapi::ParamDescInterface &   desc    =
		desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

	size_t         pos_utf8;
	size_t         len_utf8;
	size_t         len_pix;
	size_t         rem_pix_fx_name = width;

	// Get value & unit
	std::string    val_s;
	std::string    unit;
	const double   tempo = view.get_tempo ();
	print_param_with_pres (
		val_s, unit,
		preset, slot_index, type, index, val, desc, tempo
	);

	// Value
	if (! group_unit_val_flag)
	{
		while (! val_s.empty () && val_s [0] == ' ')
		{
			val_s = val_s.substr (1);
		}
	}
	pi::param::Tools::cut_str_bestfit (
		pos_utf8, len_utf8, len_pix,
		width, val_s.c_str (), '\n',
		param_val, &NText::get_char_width
	);
	txt_val = val_s.substr (pos_utf8, len_utf8);

	// Name
	if (param_name_ptr != 0)
	{
		std::string    name = desc.get_name (0);
		name = pi::param::Tools::print_name_bestfit (
			width - len_pix, name.c_str (),
			*param_name_ptr, &NText::get_char_width
		);
		param_name_ptr->set_text (name);
	}

	// Unit
	if (param_unit_ptr != 0)
	{
		int            w_unit = width;
		if (group_unit_val_flag)
		{
			w_unit -= len_pix;
			if (! unit.empty ())
			{
				unit = std::string (" ") + unit;
				// We should do this on all the labels of the unit, but :efforts:
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
		if (param_unit_ptr != 0)
		{
			param_unit_ptr->set_text (txt_unit);
		}
	}

	if (fx_name_ptr != 0)
	{
		std::string    pi_type_name = desc_pi.get_name ();

		pi_type_name = pi::param::Tools::print_name_bestfit (
			rem_pix_fx_name, pi_type_name.c_str (),
			*fx_name_ptr, &NText::get_char_width
		);
		fx_name_ptr->set_text (pi_type_name);
	}
}



MsgHandlerInterface::EvtProp	Tools::change_param (Model &model, const View &view, int slot_index, PiType type, int index, float step, int step_index, int dir)
{
	assert (slot_index >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (dir != 0);

	const doc::Preset &  preset = view.use_preset_cur ();
	float          val_nrm =
		view.get_param_val (preset, slot_index, type, index);

	val_nrm = float (change_param (
		val_nrm, model, view, slot_index, type, index, step, step_index, dir
	));

	// Check the beat case
	bool           set_flag = false;
	if (type == PiType_MAIN)
	{
		const doc::Slot &    slot = *(preset._slot_list [slot_index]);
		const doc::PluginSettings& settings = slot.use_settings (PiType_MAIN);
		const doc::ParamPresentation *   pres_ptr =
			settings.use_pres_if_tempo_ctrl (index);

		// Uses beats
		if (pres_ptr != 0)
		{
			const double   tempo = view.get_tempo ();
			const piapi::PluginDescInterface &  desc_pi =
				model.get_model_desc (slot._pi_model);
			const piapi::ParamDescInterface &   desc    =
				desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
			const float    val_beats = float (
				ToolsParam::conv_nrm_to_beats (val_nrm, desc, tempo)
			);
			model.set_param_beats (slot_index, index, val_beats);
			set_flag = true;
		}
	}

	if (! set_flag)
	{
		model.set_param (slot_index, type, index, val_nrm);
	}

	return MsgHandlerInterface::EvtProp_CATCH;
}



double	Tools::change_param (double val_nrm, const Model &model, const View &view, int slot_index, PiType type, int index, float step, int step_index, int dir)
{
	assert (slot_index >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (dir != 0);

	const doc::Preset &  preset = view.use_preset_cur ();
	assert (! preset.is_slot_empty (slot_index));
	const doc::Slot &    slot   = *(preset._slot_list [slot_index]);
	const doc::PluginSettings & settings = slot.use_settings (type);

	const std::string &  pi_model =
		(type == PiType_MIX) ? Cst::_plugin_mix : slot._pi_model;
	const piapi::PluginDescInterface &  desc_pi =
		model.get_model_desc (pi_model);
	const piapi::ParamDescInterface &   desc    =
		desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

	bool           done_flag = false;
	if (step_index == 0)
	{
		// Uses a notch list
		const std::set <float> *   notch_list_ptr =
			settings.find_notch_list (index);
		if (notch_list_ptr != 0)
		{
			const auto     it_notch = ToolsParam::advance_to_notch (
				float (val_nrm), *notch_list_ptr, dir
			);
			val_nrm   = *it_notch;
			done_flag = true;
		}

		else if (type != PiType_MIX)
		{
			// Check if we can use the beat mode
			const doc::ParamPresentation *   pres_ptr =
				settings.use_pres_if_tempo_ctrl (index);
			if (pres_ptr != 0 && desc.get_nat_min () >= 0)
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

	if (! done_flag)
	{
		const piapi::ParamDescInterface::Range range = desc.get_range ();

		// Uses a discrete range
		if (range == piapi::ParamDescInterface::Range_DISCRETE)
		{
			double         val_nat = desc.conv_nrm_to_nat (val_nrm);
			val_nat += dir;
			const double   nat_min = desc.get_nat_min ();
			const double   nat_max = desc.get_nat_max ();
			val_nat   = fstb::limit (val_nat, nat_min, nat_max);
			val_nrm   = float (desc.conv_nat_to_nrm (val_nat));
			done_flag = true;
		}
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



void	Tools::change_plugin (Model &model, const View &view, int slot_index, int dir, const std::vector <std::string> &fx_list)
{
	assert (slot_index >= 0);
	assert (dir != 0);

	const doc::Preset &  preset = view.use_preset_cur ();
	const int      nbr_slots = preset._slot_list.size ();
	assert (slot_index <= nbr_slots);

	const int      nbr_types = int (fx_list.size ());

	// Index within the official plug-in list. end = empty
	int            pi_index  = nbr_types;
	if (slot_index < nbr_slots)
	{
		if (! preset.is_slot_empty (slot_index))
		{
			const doc::Slot & slot = *(preset._slot_list [slot_index]);
			const std::string type = slot._pi_model;
			auto          type_it =
				std::find (fx_list.begin (), fx_list.end (), type);
			assert (type_it != fx_list.end ());
			pi_index = type_it - fx_list.begin ();
		}
	}

	const int      mod_len = nbr_types + 1;
	pi_index += dir;
	pi_index = (pi_index + mod_len) % mod_len;

	// We need to add a slot at the end?
	if (slot_index == nbr_slots && pi_index != nbr_types)
	{
		model.set_nbr_slots (nbr_slots + 1);
	}

	if (pi_index == nbr_types)
	{
		model.remove_plugin (slot_index);
	}
	else
	{
		model.set_plugin (slot_index, fx_list [pi_index]);
	}

	// Last slot needs to be removed?
	if (slot_index == nbr_slots - 1 && pi_index == nbr_types)
	{
		int         nbr_slots_new = nbr_slots - 1;
		while (nbr_slots_new > 0 && preset.is_slot_empty (nbr_slots_new - 1))
		{
			-- nbr_slots_new;
		}
		model.set_nbr_slots (nbr_slots_new);
	}
}



void	Tools::assign_default_rotenc_mapping (Model &model, const View &view, int slot_index, int page)
{
	model.reset_all_overridden_param_ctrl ();

	const doc::Preset &  preset = view.use_preset_cur ();
	if (! preset.is_slot_empty (slot_index))
	{
		const doc::Slot &    slot = *(preset._slot_list [slot_index]);
		const auto     it_s       = slot._settings_all.find (slot._pi_model);
		if (it_s != slot._settings_all.end ())
		{
			const int      nbr_param = int (it_s->second._param_list.size ());

			for (int pos = 0; pos < Cst::RotEnc_NBR_GEN; ++pos)
			{
				const int      index = Cst::RotEnc_NBR_GEN * page + pos;
				if (index < nbr_param)
				{
					const int      rotenc_index = Cst::RotEnc_GEN + pos;
					model.override_param_ctrl (
						slot_index,
						PiType_MAIN,
						index,
						rotenc_index
					);
				}
			}
		}
	}
}



std::string	Tools::conv_pedal_conf_to_short_txt (PedalConf &conf, const doc::PedalboardLayout &layout, int index, const Model &model, const View &view)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_pedals);

	std::string    txt = "<Complex>";
	conf = PedalConf_OTHER;

	const doc::PedalActionGroup & group = layout._pedal_arr [index];
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tools::print_param_with_pres (std::string &val_s, std::string &unit, const doc::Preset &preset, int slot_index, PiType type, int index, float val, const piapi::ParamDescInterface &desc, double tempo)
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
			assert (! preset.is_slot_empty (slot_index));
			const doc::Slot & slot = *(preset._slot_list [slot_index]);
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
	const doc::PedalActionSingleInterface *   action_ptr = 0;
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
	if (action_ptr != 0 && ! other_flag)
	{
		ok_flag = true;
		char           txt_0 [127+1];

		const doc::ActionType   type = action_ptr->get_type ();
		switch (type)
		{
		case doc::ActionType_BANK:
			{
				const doc::ActionBank & bank =
					dynamic_cast <const doc::ActionBank &> (*action_ptr);
				if (bank._relative_flag)
				{
					fstb::snprintf4all (
						txt_0, sizeof (txt_0),
						"%s", (bank._val < 0) ? "-" : "+"
					);
				}
				else
				{
					fstb::snprintf4all (
						txt_0, sizeof (txt_0),
						"%d", bank._val
					);
				}
				name = "Bank ";
				name += txt_0;
			}
			break;
		case doc::ActionType_PRESET:
			{
				const doc::ActionPreset &  preset =
					dynamic_cast <const doc::ActionPreset &> (*action_ptr);
				if (preset._relative_flag)
				{
					fstb::snprintf4all (
						txt_0, sizeof (txt_0), "%s",
						(preset._val < 0) ? "-" : "+"
					);
				}
				else
				{
					fstb::snprintf4all (
						txt_0, sizeof (txt_0),
						"%d", preset._val
					);
				}
				name = "Preset ";
				name += txt_0;
			}
			break;
		case doc::ActionType_TOGGLE_TUNER:
			name = "Tuner";
			break;
		case doc::ActionType_TOGGLE_FX:
			name = "FX" /*** To do ***/;
			break;
		case doc::ActionType_LOOP_REC:
			name = "Loop Rec";
			break;
		case doc::ActionType_LOOP_PLAY_STOP:
			name = "Loop P/S";
			break;
		case doc::ActionType_LOOP_UNDO:
			name = "Loop Undo";
			break;
		case doc::ActionType_PARAM:
			{
				const doc::ActionParam &  param =
					dynamic_cast <const doc::ActionParam &> (*action_ptr);
				name = print_param_action (param, model, view);
				name = "Set " + name;
			}
			break;
		case doc::ActionType_TEMPO:
			name = "Tempo";
			break;
		case doc::ActionType_EVENT:
			name = "Event\?";
			break;
		default:
			ok_flag = false;
			break;
		}
	}

	return (ok_flag);
}



bool	Tools::is_pedal_momentary_button (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name)
{
	name.clear ();

	// Finds candidate actions
	bool           other_flag = false;
	// Press, release
	std::array <const doc::PedalActionSingleInterface *, 2>  action_ptr_arr =
	{{ 0, 0 }};
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
	if (   action_ptr_arr [0] != 0
	    && action_ptr_arr [1] != 0
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
				name += print_param_action (p_0, model, view);
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
	{{ 0, 0 }};
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
	if (   action_ptr_arr [0] != 0
	    && action_ptr_arr [1] != 0
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
				name += print_param_action (p_0, model, view);
			}
		}
	}

	return ok_flag;
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
		bool           found_flag = false;
		const doc::Setup &   setup = view.use_setup ();
		for (auto & bank : setup._bank_arr)
		{
			for (auto & preset : bank._preset_arr)
			{
				const int      nbr_slots = int (preset._slot_list.size ());
				for (int slot_index = 0
				;	slot_index < nbr_slots && ! found_flag
				;	++slot_index)
				{
					if (! preset.is_slot_empty (slot_index))
					{
						const doc::Slot & slot = *(preset._slot_list [slot_index]);
						if (slot._label == fx_id._label_or_model)
						{
							type = slot._pi_model;
							found_flag = true;
						}
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

	return type;
}



std::string	Tools::print_param_action (const doc::ActionParam &param, const Model &model, const View &view)
{
	const std::string model_id = find_fx_type (param._fx_id, view);
	const piapi::PluginDescInterface & desc_main =
		model.get_model_desc (model_id);
	const std::string model_name_multi = desc_main.get_name ();
	const std::string model_name =
			(param._fx_id._location_type == doc::FxId::LocType_CATEGORY)
		? pi::param::Tools::print_name_bestfit (8, model_name_multi.c_str ())
		: param._fx_id._label_or_model;
	std::string    name = model_name + " ";
	const piapi::PluginDescInterface & desc = model.get_model_desc (
		(param._fx_id._type == PiType_MAIN) ? model_id : Cst::_plugin_mix
	);
	const int      nbr_param =
		desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
	if (param._index < nbr_param)
	{
		const piapi::ParamDescInterface & param_desc =
			desc.get_param_info (piapi::ParamCateg_GLOBAL, param._index);
		const std::string param_name = param_desc.get_name (0);
		name += param_name;
	}
	else
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "%d", param._index);
		name += txt_0;
	}

	return name;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
