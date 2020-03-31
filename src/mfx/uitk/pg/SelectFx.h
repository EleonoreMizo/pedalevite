/*****************************************************************************

        SelectFx.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_SelectFx_HEADER_INCLUDED)
#define mfx_uitk_pg_SelectFx_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <array>
#include <map>
#include <string>
#include <vector>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class SelectFx final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Param
	{
	public:
		std::string    _title;           // Input
		bool           _audio_flag = true; // Input
		bool           _auto_loc_flag = true; // Input: true if the directory should be automatically selected
		std::string    _subdir;          // Input/onput: Empty for the root, or a tag for a sub-directory
		std::string    _model_id;        // Input/output, on input: empty string if none, on output: empty string if empty/delete
		bool           _ok_flag = false; // Output: OK or cancel
	};

	explicit       SelectFx (PageSwitcher &page_switcher);
	               ~SelectFx () = default;



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
		Entry_WINDOW = 100000,
		Entry_TITLE,
		Entry_LIST = 0,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;

	class Element
	{
	public:
		TxtSPtr        _txt_sptr;
		std::string    _tag_or_model;
		bool           _model_flag = false; // true = model id, false = tag
	};
	typedef std::vector <Element> EltArray;

	class ModelInfo
	{
	public:
		std::string    _model_id;
		std::string    _name_multilabel;
		// This could be extended to show other data
	};
	typedef std::vector <ModelInfo> ModelList;
	typedef std::map <std::string, ModelList> TagCol; // [tag] = model list

	enum PiCateg
	{
		PiCateg_AUD = 0,
		PiCateg_SIG,

		PiCateg_NBR_ELT
	};

	int            update_display (std::string tag = "");
	void           build_tag_collection ();
	void           find_location (std::string &tag, int &pos, std::string model_id, PiCateg categ) const;
	int            find_position (std::string model_id, std::string tag, PiCateg categ) const;
	void           select (int node_id);
	void           escape ();

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	std::string    _main_title;

	WinSPtr        _menu_sptr;    // Contains a few entries (selectable) + the bank list
	TxtSPtr        _title_sptr;
	EltArray       _catalog;

	Param *        _param_ptr;
	PiCateg        _categ;
	std::string    _subdir;       // Current subdirectory: empty for root, or a tag
	std::string    _cur_model;    // Can be empty

	std::array <TagCol, PiCateg_NBR_ELT>
	               _tag_col;
	bool           _tag_col_built_falg;

	static const char *           // Tag for unsorted plug-ins
	               _tag_nocat_0;
	static const char *           // Tag for all plug-ins
	               _tag_all_0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SelectFx ()                               = delete;
	               SelectFx (const SelectFx &other)          = delete;
	               SelectFx (SelectFx &&other)               = delete;
	SelectFx &     operator = (const SelectFx &other)        = delete;
	SelectFx &     operator = (SelectFx &&other)             = delete;
	bool           operator == (const SelectFx &other) const = delete;
	bool           operator != (const SelectFx &other) const = delete;

}; // class SelectFx



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/SelectFx.hpp"



#endif   // mfx_uitk_pg_SelectFx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
