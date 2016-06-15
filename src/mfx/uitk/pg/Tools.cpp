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
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tools::set_param_text (const View &view, int width, int index, float val, int slot_index, PiType type, NText &param_name, NText &param_val, NText *param_unit_ptr, NText *fx_name_ptr)
{
	char           txt_0 [255+1];

	const doc::Preset &   preset = view.use_preset_cur ();
	const doc::Slot &  slot = *(preset._slot_list [slot_index]);
	const doc::PluginSettings * settings_ptr = &slot._settings_mixer;
	if (type == PiType_MAIN)
	{
		const auto     it = slot._settings_all.find (slot._pi_model);
		if (it == slot._settings_all.end ())
		{
			assert (false);
		}
		else
		{
			settings_ptr = &it->second;
		}
	}

	const View::SlotInfoList & sil = view.use_slot_info_list ();

	size_t         rem_pix_fx_name = width;

	if (sil.empty () || sil [slot_index] [type].get () == 0)
	{
		if (fx_name_ptr != 0)
		{
			fx_name_ptr->set_text ("");
		}
		if (param_unit_ptr != 0)
		{
			param_unit_ptr->set_text ("");
		}

		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "%-4d",
			index
		);
		param_name.set_text (txt_0);

		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "%.4f",
			settings_ptr->_param_list [index]
		);
		param_val.set_text (txt_0);
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
		while (! val_s.empty () && val_s [0] == ' ')
		{
			val_s = val_s.substr (1);
		}
		pi::param::Tools::cut_str_bestfit (
			pos_utf8, len_utf8, len_pix,
			width, val_s.c_str (), '\n',
			param_val, &NText::get_char_width
		);
		val_s = val_s.substr (pos_utf8, len_utf8);
		param_val.set_text (val_s);

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
			std::string    unit = desc.get_unit (0);
			pi::param::Tools::cut_str_bestfit (
				pos_utf8, len_utf8, len_pix,
				width, unit.c_str (), '\n',
				*param_unit_ptr, &NText::get_char_width
			);
			param_unit_ptr->set_text (unit);
			rem_pix_fx_name -= len_pix;
		}
	}

	if (fx_name_ptr != 0)
	{
		std::string    pi_type_name =
			mfx::pi::PluginModel_get_name (slot._pi_model);
		pi_type_name = pi::param::Tools::print_name_bestfit (
			rem_pix_fx_name, pi_type_name.c_str (),
			*fx_name_ptr, &NText::get_char_width
		);
		fx_name_ptr->set_text (pi_type_name);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
