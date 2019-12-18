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

#include "fstb/def.h"
#if fstb_IS (SYS, WIN)
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
#endif

#include "fstb/fnc.h"
#include "mfx/adrv/DriverInterface.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/uitk/pg/CurProg.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/ui/UserInputType.h"
#include "mfx/ControlSource.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#if fstb_IS (SYS, LINUX)
	#include <arpa/inet.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#elif fstb_IS (SYS, WIN)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#error Unsupported operating system
#endif

#include <vector>

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
CurProg::CurProg (PageSwitcher &page_switcher, adrv::DriverInterface &snd_drv)
:	_page_switcher (page_switcher)
,	_snd_drv (snd_drv)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_ip_addr ()
,	_prog_nbr_sptr (  new NText (Entry_PROG_IDX))
,	_prog_name_sptr ( new NText (Entry_PROG_NAME))
,	_bank_nbr_sptr (  new NText (Entry_BANK_IDX))
,	_bank_name_sptr ( new NText (Entry_BANK_NAME))
,	_fx_name_sptr (   new NText (Entry_FX_NAME))
,	_param_unit_sptr (new NText (Entry_PARAM_UNIT))
,	_param_name_sptr (new NText (Entry_PARAM_NAME))
,	_param_val_sptr ( new NText (Entry_PARAM_VAL))
,	_modlist_sptr (   new NText (Entry_MOD_LIST))
,	_ip_sptr (        new NText (Entry_IP))
,	_size_max_bank_name (0)
,	_bank_index (0)
,	_preset_index (0)
,	_tempo_date (INT64_MIN)
,	_esc_count (0)
{
	_prog_nbr_sptr->set_mag (_mag_prog_nbr, _mag_prog_nbr);
	_prog_nbr_sptr->set_bold (true, true);
	_prog_name_sptr->set_justification (0.5f, 0, false);
	_bank_nbr_sptr->set_justification (1.0f, 0, true);
	_bank_name_sptr->set_justification (1.0f, 0, true);
	_param_unit_sptr->set_justification (1.0f, 0, false);
	_param_val_sptr->set_justification (1.0f, 0, false);
	_ip_sptr->set_justification (0.5f, 1.0f, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CurProg::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	_tempo_date = _model_ptr->get_cur_date () - std::chrono::seconds (1);

	if (_view_ptr->use_setup ()._save_mode != doc::Setup::SaveMode_AUTO)
	{
		_model_ptr->set_edit_mode (false);
	}

	if (_ip_addr.empty ())
	{
		_ip_addr = get_ip_address ();
		_ip_sptr->set_text (_ip_addr);
	}

	_prog_nbr_sptr->set_font (fnt._m);
	_prog_name_sptr->set_font (fnt._l);
	_bank_nbr_sptr->set_font (fnt._s);
	_bank_name_sptr->set_font (fnt._s);
	_fx_name_sptr->set_font (fnt._s);
	_param_unit_sptr->set_font (fnt._s);
	_param_name_sptr->set_font (fnt._s);
	_param_val_sptr->set_font (fnt._s);
	_modlist_sptr->set_font (fnt._s);
	_ip_sptr->set_font (fnt._m);

	const int      bl_s  = fnt._s.get_baseline ();
	const int      bl_m  = fnt._m.get_baseline ();
	const int      h_s   = fnt._s.get_char_h ();
	const int      h_l   = fnt._l.get_char_h ();

	const int      x_mid = _page_size [0] >> 1;
	const int      y_prg = std::max (bl_m * _mag_prog_nbr, h_s + bl_s );
	_size_max_bank_name =
		_page_size [0] - _prog_nbr_sptr->get_char_width ('0') * 2;
	const int      y_pna = y_prg + ( h_l      >> 1);
	const int      y_fx  = y_pna + ((h_l * 3) >> 1);

	_prog_nbr_sptr->set_coord (Vec2d (0, 0));
	_prog_name_sptr->set_coord (Vec2d (x_mid, y_pna));
	_bank_nbr_sptr->set_coord (Vec2d (_page_size [0], y_prg - h_s));
	_bank_name_sptr->set_coord (Vec2d (_page_size [0], y_prg));
	_fx_name_sptr->set_coord (Vec2d (0, y_fx + h_s));
	_param_name_sptr->set_coord (Vec2d (0, y_fx));
	_param_unit_sptr->set_coord (Vec2d (_page_size [0], y_fx + h_s));
	_param_val_sptr->set_coord (Vec2d (_page_size [0], y_fx));
	_modlist_sptr->set_coord (Vec2d (0, y_fx + h_s));
	_ip_sptr->set_coord (Vec2d (x_mid, _page_size [1]));

	_page_ptr->push_back (_bank_nbr_sptr);
	_page_ptr->push_back (_bank_name_sptr);
	_page_ptr->push_back (_prog_nbr_sptr);
	_page_ptr->push_back (_prog_name_sptr);
	_page_ptr->push_back (_fx_name_sptr);
	_page_ptr->push_back (_param_unit_sptr);
	_page_ptr->push_back (_param_name_sptr);
	_page_ptr->push_back (_param_val_sptr);
	_page_ptr->push_back (_modlist_sptr);
	_page_ptr->push_back (_ip_sptr);

	const int      bank_index   = _view_ptr->get_bank_index ();
	const int      preset_index = _view_ptr->get_preset_index ();
	const doc::Setup &   setup  = _view_ptr->use_setup ();
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	i_set_bank_nbr (bank_index);
	i_set_prog_nbr (preset_index);
	i_set_bank_name (setup._bank_arr [bank_index]._name);
	i_set_prog_name (preset._name);
	i_set_param (false, 0, 0, 0, PiType (0));
	i_show_mod_list ();

	_esc_count = 0;
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
		case Button_S:
			_page_switcher.switch_to (pg::PageType_MENU_MAIN, 0);
			ret_val = EvtProp_CATCH;
			break;
		case Button_E:
			_ip_addr = get_ip_address ();
			_ip_sptr->set_text (_ip_addr);
			++ _esc_count;
			if (_esc_count == 2)
			{
				_page_ptr->reset_display ();
			}
			else if (_esc_count == 3)
			{
				_snd_drv.restart ();
				_esc_count = 0;
			}
			_page_ptr->invalidate (Rect (Vec2d (), _page_size));
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	CurProg::do_set_tempo (double bpm)
{
	_tempo_date = _model_ptr->get_cur_date ();

	_modlist_sptr->set_text ("");
	_fx_name_sptr->set_text ("");
	_param_unit_sptr->set_text ("BPM");
	_param_name_sptr->set_text ("Tempo");

	char           val_0 [127+1];
	fstb::snprintf4all (val_0, sizeof (val_0), "%7.3f", bpm);
	_param_val_sptr->set_text (val_0);
}



void	CurProg::do_select_bank (int index)
{
	i_set_bank_nbr (index);
	const doc::Setup &   setup = _view_ptr->use_setup ();
	i_set_bank_name (setup._bank_arr [index]._name);
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
	i_set_param (false, 0, 0, 0, PiType (0));
	i_show_mod_list ();
}



void	CurProg::do_set_param (int slot_id, int index, float val, PiType type)
{
	const std::chrono::microseconds  cur_date (_model_ptr->get_cur_date ());
	const std::chrono::microseconds  dist     (cur_date - _tempo_date);
	if (dist >= std::chrono::milliseconds (100))
	{
		i_set_param (true, slot_id, index, val, type);
	}
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



void	CurProg::i_set_param (bool show_flag, int slot_id, int index, float val, PiType type)
{
	if (! show_flag || _view_ptr == 0)
	{
		_fx_name_sptr->set_text ("");
		_param_unit_sptr->set_text ("");
		_param_name_sptr->set_text ("");
		_param_val_sptr->set_text ("");
	}
	else
	{
		_modlist_sptr->set_text ("");
		Tools::set_param_text (
			*_model_ptr, *_view_ptr, _page_size [0], index, val, slot_id, type,
			_param_name_sptr.get (), *_param_val_sptr,
			_param_unit_sptr.get (), _fx_name_sptr.get (),
			false
		);
	}
}



void	CurProg::i_show_mod_list ()
{
	if (_view_ptr != 0)
	{
		const mfx::doc::Preset &   cur = _view_ptr->use_preset_cur ();

		std::set <ControlSource>   src_list;

		// Retrieves all unique modulation sources
		std::vector <int> slot_list (cur.build_ordered_node_list (true));
		for (int slot_id : slot_list)
		{
			const doc::Slot &   slot = cur.use_slot (slot_id);
			for (int type = 0; type < PiType_NBR_ELT; ++type)
			{
				const doc::PluginSettings & settings =
					slot.use_settings (static_cast <PiType> (type));
				for (auto &cls : settings._map_param_ctrl)
				{
					if (! cls.second.is_empty ())
					{
						if (cls.second._bind_sptr.get () != 0)
						{
							add_mod_source (src_list, cls.second._bind_sptr->_source);
						}
						for (auto &cl_sptr : cls.second._mod_arr)
						{
							if (cl_sptr.get () != 0)
							{
								add_mod_source (src_list, cl_sptr->_source);
							}
						}
					}
				}
			}
		}

		// Prints the list
		ui::UserInputType type_cur = ui::UserInputType_UNDEFINED;
		std::string    txt;
		char           txt_0 [255];
		for (auto &src : src_list)
		{
			ui::UserInputType    type = ui::UserInputType (src._type);
			if (type != type_cur)
			{
				type_cur = type;
				if (! txt.empty ())
				{
					txt += " ";
				}
				switch (type)
				{
				case ui::UserInputType_SW:
					txt += "FSw";
					break;
				case ui::UserInputType_POT:
					txt += "Pdl";
					break;
				case ui::UserInputType_ROTENC:
					txt += "Pot";
					break;
				default:
					txt += "\?\?\?";
					assert (false);
					break;
				}
			}

			fstb::snprintf4all (txt_0, sizeof (txt_0), " %d", src._index);
			txt += txt_0;
		}
		
		_modlist_sptr->set_text (txt);

		_fx_name_sptr->set_text ("");
		_param_unit_sptr->set_text ("");
		_param_name_sptr->set_text ("");
		_param_val_sptr->set_text ("");
	}
}



void CurProg::add_mod_source (std::set <ControlSource> &src_list, const ControlSource &src)
{
	assert (src.is_valid ());

	if (src.is_physical ())
	{
		src_list.insert (src);
	}
}



std::string CurProg::get_ip_address ()
{
	std::string    ip_addr;

#if fstb_IS (SYS, LINUX)

	// Source:
	// http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
	int            fd = socket (AF_INET, SOCK_DGRAM, 0);
	struct ifreq   ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	fstb::snprintf4all (ifr.ifr_name, IFNAMSIZ, "%s", "eth0");
	int            ret_val = ioctl (fd, SIOCGIFADDR, &ifr);
	if (ret_val == 0)
	{
		ip_addr = inet_ntoa (((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr);
	}
	close (fd);

#else

	::WSADATA      wsa_data;
	::WSAStartup (2, &wsa_data);
	char           name_0 [255+1];
	int            ret_val = gethostname (name_0, sizeof (name_0));
	if (ret_val == 0)
	{
		::PHOSTENT     hostinfo = gethostbyname (name_0);
		if (hostinfo != 0)
		{
			ip_addr = inet_ntoa (*(struct in_addr *)(*hostinfo->h_addr_list));
		}
	}
	::WSACleanup ();

#endif

	return ip_addr;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
