/*****************************************************************************

        MenuBackup.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_MenuBackup_HEADER_INCLUDED)
#define mfx_uitk_pg_MenuBackup_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class MenuBackup final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       MenuBackup (PageSwitcher &page_switcher);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_DATE = 0,
		Entry_SAVE,
		Entry_RESTORE,
		Entry_EXPORT
	};

	enum class State
	{
		NORMAL = 0,
		DATE_CHECK,
		DATE_SET
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           save (int node_id);
	void           save_now (int node_id);

	static std::string
	               make_backup_filename ();

	PageSwitcher & _page_switcher;
	Model *        _model_ptr = nullptr;   // 0 = not connected
	const View *   _view_ptr  = nullptr;   // 0 = not connected
	PageMgrInterface *                     // 0 = not connected
	               _page_ptr  = nullptr;
	Vec2d          _page_size;
	const ui::Font *                       // 0 = not connected
	               _fnt_ptr   = nullptr;

	TxtSPtr        _date_sptr;
	TxtSPtr        _save_sptr;
	TxtSPtr        _restore_sptr;
	TxtSPtr        _export_sptr;

	State          _state     = State::NORMAL;
	Question::QArg _msg_arg;

	// Indicates that the current date is *considered* valid, and the user
	// will not be prompted to check it.
	bool           _date_valid_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MenuBackup ()                               = delete;
	               MenuBackup (const MenuBackup &other)        = delete;
	               MenuBackup (MenuBackup &&other)             = delete;
	MenuBackup &   operator = (const MenuBackup &other)        = delete;
	MenuBackup &   operator = (MenuBackup &&other)             = delete;
	bool           operator == (const MenuBackup &other) const = delete;
	bool           operator != (const MenuBackup &other) const = delete;

}; // class MenuBackup



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/MenuBackup.hpp"



#endif   // mfx_uitk_pg_MenuBackup_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
