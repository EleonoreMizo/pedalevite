/*****************************************************************************

        CurProg.cpp
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
#include "mfx/uitk/pg/CurProg.h"
#include "mfx/uitk/ContainerInterface.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/ui/Font.h"
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



/*
Layout:
NNN
NNN         bank nbr
NNN        bank name
   Program name

Effect
Parameter      value

    IP address
*/
CurProg::CurProg (std::string ip_addr)
:	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_ip_addr (ip_addr)
,	_prog_nbr_sptr (  new NText (0))
,	_prog_name_sptr ( new NText (1))
,	_bank_nbr_sptr (  new NText (2))
,	_bank_name_sptr ( new NText (3))
,	_fx_name_sptr (   new NText (4))
,	_param_unit_sptr (new NText (5))
,	_param_name_sptr (new NText (6))
,	_param_val_sptr ( new NText (7))
,	_ip_sptr (        new NText (8))
,	_size_max_bank_name (0)
,	_bank_index (0)
,	_preset_index (0)
{
	_prog_nbr_sptr->set_mag (_mag_prog_nbr, _mag_prog_nbr);
	_prog_nbr_sptr->set_bold (true, true);
	_prog_name_sptr->set_justification (0.5f, 0, false);
	_bank_nbr_sptr->set_justification (1.0f, 0, true);
	_bank_name_sptr->set_justification (1.0f, 0, true);
	_param_unit_sptr->set_justification (1.0f, 0, false);
	_param_val_sptr->set_justification (1.0f, 0, false);
	_ip_sptr->set_justification (0.5f, 1.0f, false);
	_ip_sptr->set_text (ip_addr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CurProg::do_connect (Model &model, const View &view, ContainerInterface &page, Vec2d page_size, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	_prog_nbr_sptr->set_font (fnt_m);
	_prog_name_sptr->set_font (fnt_l);
	_bank_nbr_sptr->set_font (fnt_s);
	_bank_name_sptr->set_font (fnt_s);
	_fx_name_sptr->set_font (fnt_s);
	_param_unit_sptr->set_font (fnt_s);
	_param_name_sptr->set_font (fnt_s);
	_param_val_sptr->set_font (fnt_s);
	_ip_sptr->set_font (fnt_m);

	const int     bl_s  = fnt_s.get_baseline ();
	const int     bl_m  = fnt_m.get_baseline ();
	const int     h_s   = fnt_s.get_char_h ();
	const int     h_m   = fnt_m.get_char_h ();
	const int     h_l   = fnt_l.get_char_h ();

	const int     x_mid = _page_size [0] >> 1;
	const int     y_prg = std::max (bl_m * _mag_prog_nbr, h_s + bl_s );
	_size_max_bank_name =
		_page_size [0] - _prog_nbr_sptr->get_char_width ('0') * 2;
	const int     y_pna = y_prg + ( h_l      >> 1);
	const int     y_fx  = y_pna + ((h_l * 3) >> 1);

	_prog_nbr_sptr->set_coord (Vec2d (0, 0));
	_prog_name_sptr->set_coord (Vec2d (x_mid, y_pna));
	_bank_nbr_sptr->set_coord (Vec2d (_page_size [0], y_prg - h_s));
	_bank_name_sptr->set_coord (Vec2d (_page_size [0], y_prg));
	_fx_name_sptr->set_coord (Vec2d (0, y_fx + h_s));
	_param_name_sptr->set_coord (Vec2d (0, y_fx));
	_param_unit_sptr->set_coord (Vec2d (_page_size [0], y_fx + h_s));
	_param_val_sptr->set_coord (Vec2d (_page_size [0], y_fx));
	_ip_sptr->set_coord (Vec2d (x_mid, _page_size [1]));

	_page_ptr->push_back (_bank_nbr_sptr);
	_page_ptr->push_back (_bank_name_sptr);
	_page_ptr->push_back (_prog_nbr_sptr);
	_page_ptr->push_back (_prog_name_sptr);
	_page_ptr->push_back (_fx_name_sptr);
	_page_ptr->push_back (_param_unit_sptr);
	_page_ptr->push_back (_param_name_sptr);
	_page_ptr->push_back (_param_val_sptr);
	_page_ptr->push_back (_ip_sptr);

	const int      bank_index   = _view_ptr->get_bank_index ();
	const int      preset_index = _view_ptr->get_preset_index ();
	const doc::Setup &   setup  = _view_ptr->use_setup ();
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	i_set_bank_nbr (bank_index);
	i_set_prog_nbr (preset_index);
	i_set_bank_name (setup._bank_arr [bank_index]._name);
	i_set_prog_name (preset._name);
	i_set_param (-1, 0, 0, 0, PiType (0));
}



void	CurProg::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	CurProg::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		int            preset_index;
		switch (but)
		{
		case Button_U:
			preset_index =
				  (_preset_index + Cst::_nbr_presets_per_bank - 1)
				% Cst::_nbr_presets_per_bank;
			_model_ptr->activate_preset (preset_index);
			ret_val = EvtProp_CATCH;
			break;
		case Button_D:
			preset_index = (_preset_index + 1) % Cst::_nbr_presets_per_bank;
			_model_ptr->activate_preset (preset_index);
			ret_val = EvtProp_CATCH;
			break;
		}
	}

	return ret_val;
}



void	CurProg::do_select_bank (int index)
{
	i_set_bank_nbr (index);
}



void	CurProg::do_set_bank_name (std::string name)
{
	i_set_bank_name (name);
}



void	CurProg::do_set_preset_name (std::string name)
{
	i_set_prog_name (name);
}



void	CurProg::do_activate_preset (int index)
{
	i_set_prog_nbr (index);
	if (_view_ptr != 0)
	{
		i_set_prog_name (_view_ptr->use_preset_cur ()._name);
	}
	i_set_param (-1, 0, 0, 0, PiType (0));
}



void	CurProg::do_set_param (int pi_id, int index, float val, int slot_index, PiType type)
{
	i_set_param (pi_id, index, val, slot_index, type);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CurProg::i_set_bank_nbr (int index)
{
	_bank_index = index;
	char           txt_0 [255+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "Bank %02d", index);
	_bank_nbr_sptr->set_text (txt_0);
}



void	CurProg::i_set_prog_nbr (int index)
{
	_preset_index = index;
	char           txt_0 [255+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%02d", index);
	_prog_nbr_sptr->set_text (txt_0);
}



void	CurProg::i_set_bank_name (std::string name)
{
	name = pi::param::Tools::print_name_bestfit (
		_size_max_bank_name, name.c_str (),
		*_bank_name_sptr, &NText::get_char_width
	);
	_bank_name_sptr->set_text (name);
}



void	CurProg::i_set_prog_name (std::string name)
{
	name = pi::param::Tools::print_name_bestfit (
		_page_size [0], name.c_str (),
		*_prog_name_sptr, &NText::get_char_width
	);
	_prog_name_sptr->set_text (name);
}



void	CurProg::i_set_param (int pi_id, int index, float val, int slot_index, PiType type)
{
	char           txt_0 [255+1];

	if (pi_id < 0 || _view_ptr == 0)
	{
		_fx_name_sptr->set_text ("");
		_param_unit_sptr->set_text ("");
		_param_name_sptr->set_text ("");
		_param_val_sptr->set_text ("");
	}
	else
	{
		const doc::Setup &    setup  = _view_ptr->use_setup ();
		const doc::Preset &   preset =
			setup._bank_arr [_bank_index]._preset_arr [_preset_index];
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

		const SlotInfoList & sil = _view_ptr->use_slot_info_list ();

		size_t         rem_pix_fx_name = _page_size [0];

		if (sil.empty () || sil [slot_index] [type].get () == 0)
		{
			_fx_name_sptr->set_text ("");
			_param_unit_sptr->set_text ("");

			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "%-4d",
				index
			);
			_param_name_sptr->set_text (txt_0);

			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "%.4f",
				settings_ptr->_param_list [index]
			);
			_param_val_sptr->set_text (txt_0);

			_param_unit_sptr->set_text ("");
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
			const double   nat = desc.conv_nrm_to_nat (pi_info._param_arr [index]);
			std::string    val_s = desc.conv_nat_to_str (nat, 0);
			pi::param::Tools::cut_str_bestfit (
				pos_utf8, len_utf8, len_pix,
				_page_size [0], val_s.c_str (), '\n',
				*_param_val_sptr, &NText::get_char_width
			);
			val_s = val_s.substr (pos_utf8, len_utf8);
			_param_val_sptr->set_text (val_s);

			// Name
			std::string    name = desc.get_name (0);
			name = pi::param::Tools::print_name_bestfit (
				_page_size [0] - len_pix, name.c_str (),
				*_param_name_sptr, &NText::get_char_width
			);
			_param_name_sptr->set_text (name);

			// Unit
			std::string    unit = desc.get_unit (0);
			pi::param::Tools::cut_str_bestfit (
				pos_utf8, len_utf8, len_pix,
				_page_size [0], unit.c_str (), '\n',
				*_param_unit_sptr, &NText::get_char_width
			);
			_param_unit_sptr->set_text (unit);
			rem_pix_fx_name -= len_pix;
		}

		std::string    pi_type_name =
			mfx::pi::PluginModel_get_name (slot._pi_model);
		pi_type_name = pi::param::Tools::print_name_bestfit (
			rem_pix_fx_name, pi_type_name.c_str (),
			*_fx_name_sptr, &NText::get_char_width
		);
		_fx_name_sptr->set_text (pi_type_name);
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
