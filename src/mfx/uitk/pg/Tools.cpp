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
#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/uitk/pg/CtrlSrcNamed.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NText.h"
#include "mfx/Model.h"
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

	const View::SlotInfoList & sil = view.use_slot_info_list ();

	size_t         rem_pix_fx_name = width;
	char           txt_0 [255+1];

	if (sil.empty () || sil [slot_index] [type].get () == 0)
	{
		if (fx_name_ptr != 0)
		{
			fx_name_ptr->set_text ("");
		}

		if (param_name_ptr != 0)
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "%-4d",
				index
			);
			param_name_ptr->set_text (txt_0);
		}

		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "%.4f",
			val
		);
		txt_val = txt_0;
	}

	else
	{
		const ModelObserverInterface::PluginInfo & pi_info =
			*(sil [slot_index] [type]);
		const piapi::ParamDescInterface & desc =
			pi_info._pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

		size_t         pos_utf8;
		size_t         len_utf8;
		size_t         len_pix;

		// Get value & unit
		std::string    val_s;
		std::string    unit;
		print_param_with_pres (
			val_s, unit,
			preset, slot_index, type, index, val, desc
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
		const doc::Slot &  slot = *(preset._slot_list [slot_index]);

		const piapi::PluginDescInterface &  desc =
			model.get_model_desc (slot._pi_model);
		std::string    pi_type_name = desc.get_name ();

		pi_type_name = pi::param::Tools::print_name_bestfit (
			rem_pix_fx_name, pi_type_name.c_str (),
			*fx_name_ptr, &NText::get_char_width
		);
		fx_name_ptr->set_text (pi_type_name);
	}
}



MsgHandlerInterface::EvtProp	Tools::change_param (Model &model, const View &view, int slot_index, PiType type, int index, float step, int dir)
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
		val_nrm, view, slot_index, type, index, step, dir
	));

	model.set_param (slot_index, type, index, val_nrm);

	return MsgHandlerInterface::EvtProp_CATCH;
}



double	Tools::change_param (double val_nrm, const View &view, int slot_index, PiType type, int index, float step, int dir)
{
	assert (slot_index >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (dir != 0);

	const doc::Preset &  preset = view.use_preset_cur ();
	bool           done_flag = false;
	const View::SlotInfoList & sil = view.use_slot_info_list ();
	if (! sil.empty () && sil [slot_index] [type].get () != 0)
	{
		const ModelObserverInterface::PluginInfo & pi_info =
			*(sil [slot_index] [type]);
		const piapi::ParamDescInterface & desc =
			pi_info._pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
		const piapi::ParamDescInterface::Range range = desc.get_range ();
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



// Returns a set of notches for time pots, in beats
std::set <float>	Tools::create_beat_notches ()
{
	std::set <float>  notches;

	for (float base = 1.0f / 256; base < 256; base *= 2)
	{
		notches.insert (base);
		notches.insert (base * (4.0f / 3));
		notches.insert (base * (3.0f / 2));
		notches.insert (base * (8.0f / 5));
	}

	return notches;
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tools::print_param_with_pres (std::string &val_s, std::string &unit, const doc::Preset &preset, int slot_index, PiType type, int index, float val, const piapi::ParamDescInterface &desc)
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



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
