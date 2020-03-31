/*****************************************************************************

        SelectFx.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/PluginInfo.h"
#include "mfx/uitk/pg/SelectFx.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <utility>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SelectFx::SelectFx (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_main_title ()
,	_menu_sptr ( std::make_shared <NWindow> (Entry_WINDOW))
,	_title_sptr (std::make_shared <NText  > (Entry_TITLE ))
,	_catalog ()
,	_param_ptr (nullptr)
,	_categ (PiCateg_AUD)
,	_subdir ()
,	_cur_model ()
,	_tag_col ()
,	_tag_col_built_falg (false)
{
	_title_sptr->set_justification (0.5f, 0.0f, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SelectFx::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	assert (usr_ptr != nullptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	if (! _tag_col_built_falg)
	{
		build_tag_collection ();
	}

	_param_ptr  = static_cast <Param *> (usr_ptr);
	_categ      = (_param_ptr->_audio_flag) ? PiCateg_AUD : PiCateg_SIG;
	_cur_model  = _param_ptr->_model_id;
	_main_title = _param_ptr->_title;
	int            pos_cursor = 0;
	_subdir.clear ();
	if (_param_ptr->_auto_loc_flag)
	{
		find_location (_subdir, pos_cursor, _param_ptr->_model_id, _categ);
	}
	else
	{
		_subdir = _param_ptr->_subdir;
		if (! _subdir.empty ())
		{
			const TagCol & col = _tag_col [_categ];
			auto           it_col = col.find (_subdir);
			if (it_col == col.end ())
			{
				_subdir.clear ();
			}
			else
			{
				pos_cursor = find_position (_param_ptr->_model_id, _subdir, _categ);
			}
		}
	}

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid = scr_w >> 1;

	_title_sptr->set_coord (Vec2d (x_mid, 0 * h_m));
	_title_sptr->set_font (fnt._m);

	const Vec2d   menu_pos (0, h_m);
	_menu_sptr->set_coord (menu_pos);
	_menu_sptr->set_size (_page_size - menu_pos, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_page_ptr->push_back (_title_sptr);
	_page_ptr->push_back (_menu_sptr);

	update_display ();

	if (pos_cursor >= 0 && pos_cursor < int (_catalog.size ()))
	{
		_page_ptr->jump_to (Entry_LIST + pos_cursor);
	}
}



void	SelectFx::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	SelectFx::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			select (node_id);
			ret_val = EvtProp_CATCH;
			break;
		case Button_E:
			escape ();
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



// Returns a potential cursor location or -1 if none
int	SelectFx::update_display (std::string tag)
{
	assert (_fnt_ptr != nullptr);

	int            pos_cursor = -1;

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_menu_sptr->clear_all_nodes ();
	PageMgrInterface::NavLocList  nav_list;

	std::string    title_txt;
	int            pos = 0;
	const TagCol & col = _tag_col [_categ];
	_catalog.clear ();

	// Root directory
	if (_subdir.empty ())
	{
		title_txt = _main_title;

		// No plug-in
		{
			const int      node_id = Entry_LIST + pos;
			Element        elt;
			elt._model_flag   = true;
			elt._tag_or_model = "";
			elt._txt_sptr     = std::make_shared <NText> (node_id);
			elt._txt_sptr->set_font (*_fnt_ptr);
			elt._txt_sptr->set_text ("<Empty/Delete>");
			elt._txt_sptr->set_coord (Vec2d (0, pos * h_m));
			elt._txt_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
			if (_cur_model.empty ())
			{
				elt._txt_sptr->set_bold (true, true);
				pos_cursor = pos;
			}

			_menu_sptr->push_back (elt._txt_sptr);
			PageMgrInterface::add_nav (nav_list, node_id);
			_catalog.push_back (elt);
			++ pos;
		}

		// Tag list
		for (auto &tag_vt : col)
		{
			const int      node_id = Entry_LIST + pos;
			Element        elt;
			elt._model_flag   = false;
			elt._tag_or_model = tag_vt.first;
			elt._txt_sptr     = std::make_shared <NText> (node_id);
			elt._txt_sptr->set_font (*_fnt_ptr);
			elt._txt_sptr->set_text (tag_vt.first);
			elt._txt_sptr->set_coord (Vec2d (0, pos * h_m));
			elt._txt_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

			const int      pos_loc =
				find_position (_cur_model, tag_vt.first, _categ);
			elt._txt_sptr->set_bold ((pos_loc >= 0), true);

			if (tag == tag_vt.first)
			{
				pos_cursor = pos;
			}

			_menu_sptr->push_back (elt._txt_sptr);
			PageMgrInterface::add_nav (nav_list, node_id);
			_catalog.push_back (elt);
			++ pos;
		}
	}

	// Tag
	else
	{
		title_txt = _subdir + " >";

		std::string    pi_type_name;
		auto           it_col = col.find (_subdir);
		assert (it_col != col.end ());
		for (const ModelInfo &mdl_info : it_col->second)
		{
			const int      node_id = Entry_LIST + pos;
			Element        elt;
			elt._model_flag   = true;
			elt._tag_or_model = mdl_info._model_id;
			elt._txt_sptr     = std::make_shared <NText> (node_id);
			elt._txt_sptr->set_font (*_fnt_ptr);
			elt._txt_sptr->set_coord (Vec2d (0, pos * h_m));
			elt._txt_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

			pi_type_name = pi::param::Tools::print_name_bestfit (
				elt._txt_sptr->get_active_width (),
				mdl_info._name_multilabel.c_str (),
				*elt._txt_sptr, &NText::get_char_width
			);
			elt._txt_sptr->set_text (pi_type_name);

			if (_cur_model == mdl_info._model_id)
			{
				elt._txt_sptr->set_bold (true, true);
				pos_cursor = pos;
			}

			_menu_sptr->push_back (elt._txt_sptr);
			PageMgrInterface::add_nav (nav_list, node_id);
			_catalog.push_back (elt);
			++ pos;
		}
	}

	_title_sptr->set_text (title_txt);

	_page_ptr->set_nav_layout (nav_list);
	_menu_sptr->invalidate_all ();

	return pos_cursor;
}



void	SelectFx::build_tag_collection ()
{
	assert (_model_ptr != 0);

	const std::array <std::vector <std::string>, PiCateg_NBR_ELT> base_list_arr =
	{{
		_model_ptr->use_aud_pi_list (),
		_model_ptr->use_sig_pi_list ()
	}};

	const std::string tag_nocat (_tag_nocat_0);
	const std::string tag_all (_tag_all_0);

	for (int categ_cnt = 0; categ_cnt < PiCateg_NBR_ELT; ++categ_cnt)
	{
		TagCol &       col = _tag_col [categ_cnt];
		col.clear ();

		for (const std::string &model_id : base_list_arr [categ_cnt])
		{
			const piapi::PluginDescInterface &  desc =
				_model_ptr->get_model_desc (model_id);
			const piapi::PluginInfo & api_info = desc.get_info ();

			ModelInfo   loc_info;
			loc_info._model_id        = api_info._unique_id;
			loc_info._name_multilabel = api_info._name;

			for (const std::string &tag : api_info._tag_list)
			{
				col [tag].push_back (loc_info);
			}
			if (api_info._tag_list.empty ())
			{
				col [tag_nocat].push_back (loc_info);
			}
			col [tag_all].push_back (loc_info);
		}
	}

	_tag_col_built_falg = true;
}



// returns pos < 0 if not found anywhere (shouldn't happen anyway)
void	SelectFx::find_location (std::string &tag, int &pos, std::string model_id, PiCateg categ) const
{
	const TagCol & col = _tag_col [categ];
	const std::string tag_all (_tag_all_0);

	tag.clear ();
	pos = -1;
	size_t         list_size = 0;

	// Scans all the lists excepts the "all" one
	// Keeps only the most populated
	for (const auto &col_vt : col)
	{
		if (col_vt.first != tag_all)
		{
			const int      pos_tst = find_position (model_id, col_vt.first, categ);
			if (pos_tst >= 0 && col_vt.second.size () > list_size)
			{
				tag       = col_vt.first;
				pos       = pos_tst;
				list_size = col_vt.second.size ();
			}
		}
	}

	// If not found, search the "all" list
	if (pos < 0)
	{
		const int      pos_tst = find_position (model_id, tag_all, categ);
		if (pos_tst >= 0)
		{
			tag = tag_all;
			pos = pos_tst;
		}
	}
}



// Returns -1 if not found
int	SelectFx::find_position (std::string model_id, std::string tag, PiCateg categ) const
{
	assert (categ >= 0);
	assert (categ < PiCateg_NBR_ELT);

	int            pos_found = -1;

	const TagCol & col    = _tag_col [categ];
	auto           it_col = col.find (tag);
	if (it_col != col.end ())
	{
		const ModelList & model_list = it_col->second;
		for (int pos = 0; pos < int (model_list.size ()) && pos_found < 0; ++pos)
		{
			const ModelInfo & info = model_list [pos];
			if (info._model_id == model_id)
			{
				pos_found = pos;
			}
		}
	}

	return pos_found;
}



void	SelectFx::select (int node_id)
{
	assert (node_id >= Entry_LIST);
	const int      pos = node_id - Entry_LIST;
	assert (pos < int (_catalog.size ()));
	const Element& elt = _catalog [pos];

	if (elt._model_flag)
	{
		_param_ptr->_subdir   = _subdir;
		_param_ptr->_model_id = elt._tag_or_model;
		_param_ptr->_ok_flag  = true;
		_page_switcher.return_page ();
	}
	else
	{
		_subdir = elt._tag_or_model;
		update_display ();
	}
}



void	SelectFx::escape ()
{
	if (_subdir.empty ())
	{
		_param_ptr->_ok_flag  = false;
		_param_ptr->_subdir   = _subdir;
		_param_ptr->_model_id.clear ();
		_page_switcher.return_page ();
	}
	else
	{
		const std::string tag (_subdir);
		_subdir.clear ();
		const int      pos_cursor = update_display (tag);
		if (pos_cursor >= 0)
		{
			_page_ptr->jump_to (Entry_LIST + pos_cursor);
		}
	}
}



const char *	SelectFx::_tag_nocat_0 = "Unsorted";
const char *	SelectFx::_tag_all_0   = "All";



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
