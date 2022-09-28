/*****************************************************************************

        CtrlEdit.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_CtrlEdit_HEADER_INCLUDED)
#define mfx_uitk_pg_CtrlEdit_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CtrlLink.h"
#include "mfx/doc/CtrlLinkSet.h"
#include "mfx/uitk/pg/CtrlSrcNamed.h"
#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <vector>



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class CtrlEdit final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       CtrlEdit (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <CtrlSrcNamed> &csn_list);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_prog (int index) final;
	void           do_set_param (int slot_id, int index, float val, PiType type) final;
	void           do_set_param_beats (int slot_id, int index, float beats) final;
	void           do_remove_plugin (int slot_id) final;
	void           do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_steps = 4;

	enum Entry
	{
		Entry_WINDOW = 1000,
		Entry_SRC,
		Entry_STEP_REL,
		Entry_LABEL_MIN,
		Entry_VAL_MIN,
		Entry_STEP_MIN,
		Entry_LABEL_MAX = Entry_STEP_MIN + _nbr_steps,
		Entry_VAL_MAX,
		Entry_STEP_MAX,
		Entry_VAL_MOD = Entry_STEP_MAX + _nbr_steps,
		Entry_CURVE,
		Entry_CONV_U2B,
		Entry_MOD_MIN,
		Entry_MOD_MAX,
		Entry_CURVE_GFX,
		Entry_CLIP,
		Entry_CLIP_S_B,
		Entry_CLIP_S_E,
		Entry_CLIP_D_B,
		Entry_CLIP_D_E
	};

	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NBitmap> BitmapSPtr;

	class MinMax
	{
	public:
		TxtSPtr        _label_sptr;
		TxtSPtr        _val_unit_sptr;
		std::array <TxtSPtr, _nbr_steps>
		               _step_sptr_arr;
	};

	class EntryDesc
	{
	public:
		Entry          _entry;
		const char *   _txt_0;
	};

	void           update_display ();
	void           update_cur_mod_val ();
	void           update_ctrl_link ();
	doc::CtrlLink &
	               use_ctrl_link (doc::CtrlLinkSet &cls) const;
	const doc::CtrlLink &
	               use_ctrl_link (const doc::CtrlLinkSet &cls) const;
	EvtProp        change_something (int node_id, int dir);
	void           change_source (int dir);
	void           change_step (int dir);
	void           change_curve (int dir);
	void           change_u2b ();
	void           change_val (int mm, int step_index, int dir);
	void           change_clip ();
	void           change_clip_val (int clip_index, int dir);
	int            find_next_source (int dir) const;
	doc::CtrlLinkSet::LinkSPtr
	               create_controller (int csn_index) const;
	ControlSource  create_source (int csn_index) const;

	static void    draw_curve (NBitmap &gfx, ControlCurve curve);
	static float & use_clip_val (doc::CtrlLink &cl, int index);
	static const float &
	               use_clip_val (const doc::CtrlLink &cl, int index);
	static bool    check_new_clip_val (const doc::CtrlLink &cl, int index, float val);

	const std::vector <CtrlSrcNamed> &
	               _csn_list_base;
	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _win_sptr;
	TxtSPtr        _src_sptr;
	TxtSPtr        _step_rel_sptr;
	std::array <MinMax, 2>
	               _minmax;
	TxtSPtr        _val_mod_sptr;
	TxtSPtr        _curve_sptr;
	BitmapSPtr     _curve_gfx_sptr;
	TxtSPtr        _u2b_sptr;
	std::array <TxtSPtr, 2>       // Labels and values for "min" and "max"
	               _mod_minmax_arr;
	TxtSPtr        _clip_sptr;
	std::array <TxtSPtr, 4>       // src_beg, src_end, dst_beg, dst_end
	               _clip_val_sptr_arr;

	int            _step_index;
	int            _val_unit_w;
	bool           _curve_mode_flag;

	static const std::array <Entry, 2>
	               _id_label_arr;
	static const std::array <Entry, 2>
	               _id_val_arr;
	static const std::array <Entry, 2>
	               _id_step_arr;
	static const std::array <EntryDesc, 4>
	               _clip_desc_arr;

	// Cached data
	std::vector <CtrlSrcNamed>
	               _csn_list_full;
	doc::CtrlLinkSet              // Updated on connect()
	               _cls;
	doc::CtrlLink  _ctrl_link;    // Only for display purpose, and to keep track of the values when the source is <Empty> (controller removed)
	int            _ctrl_index;   // -1 if we started with an empty controller
	bool           _src_unknown_flag;   // Updated on connect()
	ControlSource  _src_unknown;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               CtrlEdit ()                               = delete;
	               CtrlEdit (const CtrlEdit &other)          = delete;
	               CtrlEdit (CtrlEdit &&other)               = delete;
	CtrlEdit &     operator = (const CtrlEdit &other)        = delete;
	CtrlEdit &     operator = (CtrlEdit &&other)             = delete;
	bool           operator == (const CtrlEdit &other) const = delete;
	bool           operator != (const CtrlEdit &other) const = delete;

}; // class CtrlEdit



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/CtrlEdit.hpp"



#endif   // mfx_uitk_pg_CtrlEdit_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
