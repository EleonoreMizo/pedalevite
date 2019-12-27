/*****************************************************************************

        PageSet.cpp
        Author: Laurent de Soras, 2019

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

#include "mfx/ui/FontDataDefault.h"
#include "mfx/Model.h"
#include "mfx/PageSet.h"

#include <stdexcept>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PageSet::PageSet (Model &model, View &view, ui::DisplayInterface &display, ui::UserInputInterface::MsgQueue &queue_input_to_gui, ui::UserInputInterface &input_device, ui::LedInterface &leds, const CmdLine &cmd_line, adrv::DriverInterface &snd_drv)
:	_model (model)
,	_fnt_8x12 ()
,	_fnt_6x8 ()
,	_fnt_6x6 ()
,	_fnt_4x6 ()
,	_pi_aud_type_list ()
,	_pi_sig_type_list ()
,	_csn_list ({
		{ ControllerType_POT   ,  0, "Expression 0" },
		{ ControllerType_POT   ,  1, "Expression 1" },
		{ ControllerType_POT   ,  2, "Expression 2" },
		{ ControllerType_ROTENC,  0, "Knob 0"       },
		{ ControllerType_ROTENC,  1, "Knob 1"       },
		{ ControllerType_ROTENC,  2, "Knob 2"       },
		{ ControllerType_ROTENC,  3, "Knob 3"       },
		{ ControllerType_ROTENC,  4, "Knob 4"       },
		{ ControllerType_SW    ,  2, "Footsw 0"     },
		{ ControllerType_SW    ,  3, "Footsw 1"     },
		{ ControllerType_SW    ,  4, "Footsw 2"     },
		{ ControllerType_SW    ,  5, "Footsw 3"     },
		{ ControllerType_SW    ,  6, "Footsw 4"     },
		{ ControllerType_SW    ,  7, "Footsw 5"     },
		{ ControllerType_SW    ,  8, "Footsw 6"     },
		{ ControllerType_SW    ,  9, "Footsw 7"     },
		{ ControllerType_SW    , 14, "Footsw 8"     },
		{ ControllerType_SW    , 15, "Footsw 9"     },
		{ ControllerType_SW    , 16, "Footsw 10"    },
		{ ControllerType_SW    , 17, "Footsw 11"    }
	})
,	_loc_edit ()
,	_loc_edit_pedal ()
,	_page_mgr (model, view, display, queue_input_to_gui, input_device, _fnt_4x6, _fnt_6x6, _fnt_6x8, _fnt_8x12)
,	_page_switcher (_page_mgr)
,	_page_cur_prog (_page_switcher, snd_drv)
,	_page_tuner (_page_switcher, leds)
,	_page_menu_main (_page_switcher, _loc_edit_pedal)
,	_page_edit_prog (_page_switcher, _loc_edit, _pi_aud_type_list, _pi_sig_type_list)
,	_page_param_list (_page_switcher, _loc_edit)
,	_page_param_edit (_page_switcher, _loc_edit)
,	_page_not_yet (_page_switcher)
,	_page_question (_page_switcher)
,	_page_param_controllers (_page_switcher, _loc_edit, _csn_list)
,	_page_ctrl_edit (_page_switcher, _loc_edit, _csn_list)
,	_page_menu_slot (_page_switcher, _loc_edit, _pi_aud_type_list, _pi_sig_type_list)
,	_page_edit_text (_page_switcher)
,	_page_save_prog (_page_switcher)
,	_page_end_msg (_page_switcher, cmd_line)
,	_page_levels (_page_switcher, snd_drv)
,	_page_pedalboard_config (_page_switcher, _loc_edit_pedal)
,	_page_pedal_action_type (_page_switcher, _loc_edit_pedal)
,	_page_ctrl_prog (_page_switcher, _loc_edit_pedal)
,	_page_bank_menu (_page_switcher, _loc_edit_pedal)
,	_page_move_fx (_page_switcher, _loc_edit)
,	_page_pedal_edit_group (_page_switcher, _loc_edit_pedal)
,	_page_pedal_edit_cycle (_page_switcher, _loc_edit_pedal)
,	_page_pedal_edit_step (_page_switcher, _loc_edit_pedal)
,	_page_pedal_edit_action (_page_switcher, _loc_edit_pedal)
,	_page_edit_label (_page_switcher)
,	_page_edit_fxid (_page_switcher, _pi_aud_type_list, _pi_sig_type_list)
,	_page_fx_peq (_page_switcher, _loc_edit)
,	_page_settings_other (_page_switcher)
,	_page_menu_presets (_page_switcher, _loc_edit)
,	_page_list_presets (_page_switcher, _loc_edit)
,	_page_bank_orga (_page_switcher, _loc_edit_pedal)
,	_page_bank_move (_page_switcher)
,	_page_prog_move (_page_switcher)
,	_page_fx_lfo (_page_switcher, _loc_edit)
,	_page_menu_backup (_page_switcher)
,	_page_edit_date (_page_switcher, cmd_line)
,	_page_prog_catalog (_page_switcher)
,	_page_rec2disk (_page_switcher)
{
#if (PV_VERSION == 2)
 #if 1
	// True large fonts with anti-aliasing
	int            ret_val = 0;
	if (ret_val == 0)
	{
		// DejaVuSansMono-Bold.ttf, 46.0
		_fnt_8x12.init (
			Cst::_font_dir + "/" + "font-48.data",
			ui::FontMapping8859::_char_per_table,
			7169, 49,
			ui::FontMapping8859::_data.data (),
			37
		);
	}
	if (ret_val == 0)
	{
		// DejaVuSansMono.ttf, 31.0
		_fnt_6x8.init (
			Cst::_font_dir + "/" + "font-32.data",
			ui::FontMapping8859::_char_per_table,
			4864, 33,
			ui::FontMapping8859::_data.data (),
			25
		);
	}
	if (ret_val == 0)
	{
		// DejaVuSansMono.ttf, 23.0
		_fnt_6x6.init (
			Cst::_font_dir + "/" + "font-24.data",
			ui::FontMapping8859::_char_per_table,
			3584, 25,
			ui::FontMapping8859::_data.data (),
			18
		);
	}
	if (ret_val == 0)
	{
		// DejaVuSansMono-Bold.ttf, 15.0
		_fnt_4x6.init (
			Cst::_font_dir + "/" + "font-16.data",
			ui::FontMapping8859::_char_per_table,
			2312, 17,
			ui::FontMapping8859::_data.data (),
			12
		);
	}
	if (ret_val != 0)
	{
		throw std::runtime_error ("Cannot load font file");
	}
 #else
	// Old zoomed fonts
	ui::FontDataDefault::make_32x48 (_fnt_8x12);
	ui::FontDataDefault::make_24x32 (_fnt_6x8);
	ui::FontDataDefault::make_24x24 (_fnt_6x6);
	ui::FontDataDefault::make_16x24 (_fnt_4x6);
 #endif
#else // PV_VERSION (1)
	ui::FontDataDefault::make_08x12 (_fnt_8x12);
	ui::FontDataDefault::make_06x08 (_fnt_6x8);
	ui::FontDataDefault::make_06x06 (_fnt_6x6);
	ui::FontDataDefault::make_04x06 (_fnt_4x6);
#endif // PV_VERSION

	_page_switcher.add_page (uitk::pg::PageType_CUR_PROG         , _page_cur_prog         );
	_page_switcher.add_page (uitk::pg::PageType_TUNER            , _page_tuner            );
	_page_switcher.add_page (uitk::pg::PageType_MENU_MAIN        , _page_menu_main        );
	_page_switcher.add_page (uitk::pg::PageType_EDIT_PROG        , _page_edit_prog        );
	_page_switcher.add_page (uitk::pg::PageType_PARAM_LIST       , _page_param_list       );
	_page_switcher.add_page (uitk::pg::PageType_PARAM_EDIT       , _page_param_edit       );
	_page_switcher.add_page (uitk::pg::PageType_NOT_YET          , _page_not_yet          );
	_page_switcher.add_page (uitk::pg::PageType_QUESTION         , _page_question         );
	_page_switcher.add_page (uitk::pg::PageType_PARAM_CONTROLLERS, _page_param_controllers);
	_page_switcher.add_page (uitk::pg::PageType_CTRL_EDIT        , _page_ctrl_edit        );
	_page_switcher.add_page (uitk::pg::PageType_MENU_SLOT        , _page_menu_slot        );
	_page_switcher.add_page (uitk::pg::PageType_EDIT_TEXT        , _page_edit_text        );
	_page_switcher.add_page (uitk::pg::PageType_SAVE_PROG        , _page_save_prog        );
	_page_switcher.add_page (uitk::pg::PageType_END_MSG          , _page_end_msg          );
	_page_switcher.add_page (uitk::pg::PageType_LEVELS           , _page_levels           );
	_page_switcher.add_page (uitk::pg::PageType_PEDALBOARD_CONFIG, _page_pedalboard_config);
	_page_switcher.add_page (uitk::pg::PageType_PEDAL_ACTION_TYPE, _page_pedal_action_type);
	_page_switcher.add_page (uitk::pg::PageType_CTRL_PROG        , _page_ctrl_prog        );
	_page_switcher.add_page (uitk::pg::PageType_BANK_MENU        , _page_bank_menu        );
	_page_switcher.add_page (uitk::pg::PageType_MOVE_FX          , _page_move_fx          );
	_page_switcher.add_page (uitk::pg::PageType_PEDAL_EDIT_GROUP , _page_pedal_edit_group );
	_page_switcher.add_page (uitk::pg::PageType_PEDAL_EDIT_CYCLE , _page_pedal_edit_cycle );
	_page_switcher.add_page (uitk::pg::PageType_PEDAL_EDIT_STEP  , _page_pedal_edit_step  );
	_page_switcher.add_page (uitk::pg::PageType_PEDAL_EDIT_ACTION, _page_pedal_edit_action);
	_page_switcher.add_page (uitk::pg::PageType_EDIT_LABEL       , _page_edit_label       );
	_page_switcher.add_page (uitk::pg::PageType_EDIT_FXID        , _page_edit_fxid        );
	_page_switcher.add_page (uitk::pg::PageType_FX_PEQ           , _page_fx_peq           );
	_page_switcher.add_page (uitk::pg::PageType_SETTINGS_OTHER   , _page_settings_other   );
	_page_switcher.add_page (uitk::pg::PageType_MENU_PRESETS     , _page_menu_presets     );
	_page_switcher.add_page (uitk::pg::PageType_LIST_PRESETS     , _page_list_presets     );
	_page_switcher.add_page (uitk::pg::PageType_BANK_ORGA        , _page_bank_orga        );
	_page_switcher.add_page (uitk::pg::PageType_BANK_MOVE        , _page_bank_move        );
	_page_switcher.add_page (uitk::pg::PageType_PROG_MOVE        , _page_prog_move        );
	_page_switcher.add_page (uitk::pg::PageType_FX_LFO           , _page_fx_lfo           );
	_page_switcher.add_page (uitk::pg::PageType_MENU_BACKUP      , _page_menu_backup      );
	_page_switcher.add_page (uitk::pg::PageType_EDIT_DATE        , _page_edit_date        );
	_page_switcher.add_page (uitk::pg::PageType_PROG_CATALOG     , _page_prog_catalog     );
	_page_switcher.add_page (uitk::pg::PageType_REC2DISK         , _page_rec2disk         );
}



void	PageSet::list_plugins ()
{
	_pi_aud_type_list.clear ();
	_pi_sig_type_list.clear ();
	std::vector <std::string> pi_list = _model.list_plugin_models ();
	std::map <std::string, std::string> pi_aud_map;
	std::map <std::string, std::string> pi_sig_map;
	for (std::string model_id : pi_list)
	{
		if (model_id [0] != '\?')
		{
			const mfx::piapi::PluginDescInterface &   desc =
				_model.get_model_desc (model_id);

			int            nbr_i = 1;
			int            nbr_o = 1;
			int            nbr_s = 0;
			desc.get_nbr_io (nbr_i, nbr_o, nbr_s);

			std::string    name_all = desc.get_name ();
			std::string    name     = mfx::pi::param::Tools::extract_longest_str (
				name_all.c_str (), '\n'
			);

			if (nbr_i > 0 && (nbr_o > 0 || nbr_s > 0))
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



uitk::Page &	PageSet::use_page_mgr ()
{
	return _page_mgr;
}



uitk::PageSwitcher &	PageSet::use_page_switcher ()
{
	return _page_switcher;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
