/*****************************************************************************

        MenuBackup.cpp
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

#include "fstb/def.h"
#include "mfx/uitk/pg/MenuBackup.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"

#include <cassert>
#if fstb_IS (SYS, LINUX)
#include <cstdlib>
#endif


namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MenuBackup::MenuBackup (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_date_sptr (   new NText (Entry_DATE   ))
,	_save_sptr (   new NText (Entry_SAVE   ))
,	_restore_sptr (new NText (Entry_RESTORE))
,	_export_sptr ( new NText (Entry_EXPORT ))
{
	_date_sptr   ->set_justification (0.5f, 0, false);
	_save_sptr   ->set_justification (0.5f, 0, false);
	_restore_sptr->set_justification (0.5f, 0, false);
	_export_sptr ->set_justification (0.5f, 0, false);

	_date_sptr   ->set_text ("Set date\xE2\x80\xA6");
	_save_sptr   ->set_text ("Save backup now");
	_restore_sptr->set_text ("Restore backup\xE2\x80\xA6");
	_export_sptr ->set_text ("Export to USB stick");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MenuBackup::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_page_ptr->clear_all_nodes ();

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      x_mid =  scr_w      >> 1;
	const int      w_sel = (scr_w * 3) >> 2;

	_date_sptr   ->set_font (*_fnt_ptr);
	_save_sptr   ->set_font (*_fnt_ptr);
	_restore_sptr->set_font (*_fnt_ptr);
	_export_sptr ->set_font (*_fnt_ptr);

	_date_sptr   ->set_coord (Vec2d (x_mid, h_m * 0));
	_save_sptr   ->set_coord (Vec2d (x_mid, h_m * 1));
	_restore_sptr->set_coord (Vec2d (x_mid, h_m * 2));
	_export_sptr ->set_coord (Vec2d (x_mid, h_m * 3));

	_date_sptr   ->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_save_sptr   ->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_restore_sptr->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_export_sptr ->set_frame (Vec2d (w_sel, 0), Vec2d ());

	_page_ptr->push_back (_date_sptr   );
	_page_ptr->push_back (_save_sptr   );
	_page_ptr->push_back (_restore_sptr);
	_page_ptr->push_back (_export_sptr );

	PageMgrInterface::NavLocList  nav_list;
	NavLoc         nav;
	PageMgrInterface::add_nav (nav_list, Entry_DATE   );
	PageMgrInterface::add_nav (nav_list, Entry_SAVE   );
	PageMgrInterface::add_nav (nav_list, Entry_RESTORE);
	PageMgrInterface::add_nav (nav_list, Entry_EXPORT );
	_page_ptr->set_nav_layout (nav_list);
}



void	MenuBackup::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	MenuBackup::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_DATE:
				_page_switcher.call_page (PageType_EDIT_DATE, 0, node_id);
				break;
			case Entry_SAVE:
#if fstb_IS (SYS, LINUX)
				{
					assert (_config_dir.find (' ') == std::string::npos);
					assert (_config_current.find (' ') == std::string::npos);
					std::string    cmd ("sudo ");
					cmd += Cst::_rw_cmd_script_pathname;
					cmd += " cp ";
					cmd += Cst::_config_dir;
					cmd += "/";
					cmd += Cst::_config_current;
					cmd += " ";
					cmd += Cst::_config_dir;
					cmd += "/";
					cmd += Cst::_config_current;
					cmd += ".bak-`date +%Y-%m-%d-%H%M`";
					system (cmd.c_str ());
				}
#else // fstb_IS (SYS, LINUX)
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
#endif // fstb_IS (SYS, LINUX)
				break;
			case Entry_RESTORE:
				/*** To do ***/
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
				break;
			case Entry_EXPORT:
#if fstb_IS (SYS, LINUX)
				{
					assert (_config_dir.find (' ') == std::string::npos);
					assert (_config_current.find (' ') == std::string::npos);
					system ("sudo mount -t vfat /dev/sda1 /mnt/sda1");
					std::string    cmd ("sudo cp ");
					cmd += Cst::_config_dir;
					cmd += "/";
					cmd += Cst::_config_current;
					cmd += " ";
					cmd += "/mnt/sda1/";
					cmd += Cst::_config_current;
					cmd += ".bak-`date +%Y-%m-%d-%H%M`";
					system (cmd.c_str ());
					system ("sudo umount /mnt/sda1");
				}
#else // fstb_IS (SYS, LINUX)
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
#endif // fstb_IS (SYS, LINUX)
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.return_page ();
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
