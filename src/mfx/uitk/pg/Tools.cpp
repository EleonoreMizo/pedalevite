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
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/ParamDescInterface.h"
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



// val = -1: use the settings content
void	Tools::set_param_text (const View &view, int width, int index, float val, int slot_index, PiType type, NText &param_name, NText &param_val, NText *param_unit_ptr, NText *fx_name_ptr, bool group_unit_val_flag)
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

		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "%-4d",
			index
		);
		param_name.set_text (txt_0);

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

		// Value
		const double   nat   = desc.conv_nrm_to_nat (val);
		std::string    val_s = desc.conv_nat_to_str (nat, 0);
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
		std::string    name = desc.get_name (0);
		name = pi::param::Tools::print_name_bestfit (
			width - len_pix, name.c_str (),
			param_name, &NText::get_char_width
		);
		param_name.set_text (name);

		// Unit
		if (param_unit_ptr != 0)
		{
			int            w_unit = width;
			std::string    unit = desc.get_unit (0);
			if (group_unit_val_flag)
			{
				w_unit -= len_pix;
				unit = std::string (" ") + unit;
				// We should do this on all the labels of the unit, but :efforts:
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
		std::string    pi_type_name =
			mfx::pi::PluginModel_get_name (slot._pi_model);
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

	MsgHandlerInterface::EvtProp  ret_val = MsgHandlerInterface::EvtProp_PASS;

	const doc::Preset &  preset = view.use_preset_cur ();
	float          val_nrm =
		view.get_param_val (preset, slot_index, type, index);

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

	val_nrm = fstb::limit (val_nrm, 0.0f ,1.0f);

	model.set_param (slot_index, type, index, val_nrm);

	ret_val = MsgHandlerInterface::EvtProp_CATCH;

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
