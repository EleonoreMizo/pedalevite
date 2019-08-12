/*****************************************************************************

        PageSet.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_PageSet_HEADER_INCLUDED)
#define mfx_PageSet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/Font.h"
#include "mfx/uitk/pg/BankMenu.h"
#include "mfx/uitk/pg/BankOrga.h"
#include "mfx/uitk/pg/BankMove.h"
#include "mfx/uitk/pg/CurProg.h"
#include "mfx/uitk/pg/CtrlEdit.h"
#include "mfx/uitk/pg/CtrlProg.h"
#include "mfx/uitk/pg/CtrlSrcNamed.h"
#include "mfx/uitk/pg/EditDate.h"
#include "mfx/uitk/pg/EditFxId.h"
#include "mfx/uitk/pg/EditLabel.h"
#include "mfx/uitk/pg/EditProg.h"
#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/pg/EndMsg.h"
#include "mfx/uitk/pg/FxLfo.h"
#include "mfx/uitk/pg/FxPEq.h"
#include "mfx/uitk/pg/Levels.h"
#include "mfx/uitk/pg/ListPresets.h"
#include "mfx/uitk/pg/MenuBackup.h"
#include "mfx/uitk/pg/MenuMain.h"
#include "mfx/uitk/pg/MenuPresets.h"
#include "mfx/uitk/pg/MenuSlot.h"
#include "mfx/uitk/pg/MoveFx.h"
#include "mfx/uitk/pg/NotYet.h"
#include "mfx/uitk/pg/PageType.h"
#include "mfx/uitk/pg/ParamControllers.h"
#include "mfx/uitk/pg/ParamEdit.h"
#include "mfx/uitk/pg/ParamList.h"
#include "mfx/uitk/pg/PedalActionType.h"
#include "mfx/uitk/pg/PedalboardConfig.h"
#include "mfx/uitk/pg/PedalEditAction.h"
#include "mfx/uitk/pg/PedalEditCycle.h"
#include "mfx/uitk/pg/PedalEditGroup.h"
#include "mfx/uitk/pg/PedalEditStep.h"
#include "mfx/uitk/pg/ProgCatalog.h"
#include "mfx/uitk/pg/ProgMove.h"
#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/pg/SaveProg.h"
#include "mfx/uitk/pg/SettingsOther.h"
#include "mfx/uitk/pg/Tuner.h"
#include "mfx/uitk/Page.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/LocEdit.h"

#include <string>
#include <vector>



namespace mfx
{



class PageSet
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PageSet (Model &model, View &view, ui::DisplayInterface &display, ui::UserInputInterface::MsgQueue &queue_input_to_gui, ui::UserInputInterface &input_device, ui::LedInterface &leds, const CmdLine &cmd_line, adrv::DriverInterface &snd_drv, const std::vector <uitk::pg::CtrlSrcNamed> &csn_list);
	virtual        ~PageSet () = default;

	void           list_plugins ();

	uitk::Page &   use_page_mgr ();
	uitk::PageSwitcher &
	               use_page_switcher ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Model &        _model;

	ui::Font       _fnt_8x12;
	ui::Font       _fnt_6x8;
	ui::Font       _fnt_6x6;
	ui::Font       _fnt_4x6;

	std::vector <std::string>  // Audio plug-ins, at least 1 audio in and 1 audio out
	               _pi_aud_type_list;
	std::vector <std::string>  // Signal generators, (0 in or 0 out) and at least 1 signal output
	               _pi_sig_type_list;

	LocEdit        _loc_edit;
	uitk::pg::PedalEditContext
	               _loc_edit_pedal;

	uitk::Page     _page_mgr;
	uitk::PageSwitcher
	               _page_switcher;
	uitk::pg::CurProg
	               _page_cur_prog;
	uitk::pg::Tuner
	               _page_tuner;
	uitk::pg::MenuMain
	               _page_menu_main;
	uitk::pg::EditProg
	               _page_edit_prog;
	uitk::pg::ParamList
	               _page_param_list;
	uitk::pg::ParamEdit
	               _page_param_edit;
	uitk::pg::NotYet
	               _page_not_yet;
	uitk::pg::Question
	               _page_question;
	uitk::pg::ParamControllers
	               _page_param_controllers;
	uitk::pg::CtrlEdit
	               _page_ctrl_edit;
	uitk::pg::MenuSlot
	               _page_menu_slot;
	uitk::pg::EditText
	               _page_edit_text;
	uitk::pg::SaveProg
	               _page_save_prog;
	uitk::pg::EndMsg
	               _page_end_msg;
	uitk::pg::Levels
	               _page_levels;
	uitk::pg::PedalboardConfig
	               _page_pedalboard_config;
	uitk::pg::PedalActionType
	               _page_pedal_action_type;
	uitk::pg::CtrlProg
	               _page_ctrl_prog;
	uitk::pg::BankMenu
	               _page_bank_menu;
	uitk::pg::MoveFx
	               _page_move_fx;
	uitk::pg::PedalEditGroup
	               _page_pedal_edit_group;
	uitk::pg::PedalEditCycle
	               _page_pedal_edit_cycle;
	uitk::pg::PedalEditStep
	               _page_pedal_edit_step;
	uitk::pg::PedalEditAction
	               _page_pedal_edit_action;
	uitk::pg::EditLabel
	               _page_edit_label;
	uitk::pg::EditFxId
	               _page_edit_fxid;
	uitk::pg::FxPEq
	               _page_fx_peq;
	uitk::pg::SettingsOther
	               _page_settings_other;
	uitk::pg::MenuPresets
	               _page_menu_presets;
	uitk::pg::ListPresets
	               _page_list_presets;
	uitk::pg::BankOrga
	               _page_bank_orga;
	uitk::pg::BankMove
	               _page_bank_move;
	uitk::pg::ProgMove
	               _page_prog_move;
	uitk::pg::FxLfo
	               _page_fx_lfo;
	uitk::pg::MenuBackup
	               _page_menu_backup;
	uitk::pg::EditDate
	               _page_edit_date;
	uitk::pg::ProgCatalog
	               _page_prog_catalog;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PageSet ()                               = delete;
	               PageSet (const PageSet &other)           = delete;
	PageSet &      operator = (const PageSet &other)        = delete;
	bool           operator == (const PageSet &other) const = delete;
	bool           operator != (const PageSet &other) const = delete;

}; // class PageSet



}  // namespace mfx



//#include "mfx/PageSet.hpp"



#endif   // mfx_PageSet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
