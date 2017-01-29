/*****************************************************************************

        EditText.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_EditText_HEADER_INCLUDED)
#define mfx_uitk_pg_EditText_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"

#include <map>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class EditText
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Param
	{
	public:
		std::string    _title;           // Input
		std::string    _text;            // Input/output
		bool           _ok_flag = false; // Output
	};

	explicit       EditText (PageSwitcher &page_switcher);
	virtual        ~EditText () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);




/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 0,
		Entry_TITLE,
		Entry_TEXT,
		Entry_CURS,

		Entry_NAV_BEG,
		Entry_OK = Entry_NAV_BEG,
		Entry_CANCEL,
		Entry_SPACE,
		Entry_DEL,
		Entry_LEFT,
		Entry_RIGHT,
		Entry_CHAR_BEG
	};

	enum Span : char32_t
	{
		Span_R = 2,
		Span_L
	};

	typedef std::shared_ptr <NBitmap> BitmapSPtr;
	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;
	typedef std::vector <int> NodeIdRow;
	typedef std::vector <NodeIdRow> NodeIdTable;
	typedef std::map <int, char32_t> CharMap;

	void           update_display ();
	void           update_curs ();
	void           fill_nav (PageMgrInterface::NavLocList &nav_list, const NodeIdTable &nit, int x, int y) const;

	PageSwitcher & _page_switcher;
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;
	Param *        _param_ptr;

	WinSPtr        _menu_sptr;
	BitmapSPtr     _curs_sptr;
	TxtSPtr        _title_sptr;
	TxtSPtr        _text_sptr;
	TxtSPtr        _ok_sptr;
	TxtSPtr        _cancel_sptr;
	TxtSPtr        _space_sptr;
	TxtSPtr        _del_sptr;
	TxtSPtr        _left_sptr;
	TxtSPtr        _right_sptr;
	TxtArray       _char_list;

	int            _curs_pos;
	bool           _curs_blink_flag;
	std::u32string _txt;

	CharMap        _char_map;

	static const std::array <std::u32string, 1+7>
	               _char_table;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EditText ()                               = delete;
	               EditText (const EditText &other)          = delete;
	EditText &     operator = (const EditText &other)        = delete;
	bool           operator == (const EditText &other) const = delete;
	bool           operator != (const EditText &other) const = delete;

}; // class EditText



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/EditText.hpp"



#endif   // mfx_uitk_pg_EditText_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
