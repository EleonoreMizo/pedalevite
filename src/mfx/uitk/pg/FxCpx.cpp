/*****************************************************************************

        FxCpx.cpp
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
#include "mfx/pi/cpx/Param.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/ProcInfo.h"
#include "mfx/uitk/grap/PrimLine.h"
#include "mfx/uitk/grap/RenderCtx.h"
#include "mfx/uitk/pg/FxCpx.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FxCpx::FxCpx (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_t_ptr (nullptr)
,	_fnt_s_ptr (nullptr)
,	_cur_param_sptr (std::make_shared <NText  > (Entry_PARAM  ))
,	_prec_sptr (     std::make_shared <NText  > (Entry_PREC   ))
,	_content_sptr (  std::make_shared <NBitmap> (Entry_CONTENT))
,	_legend_sptr_arr ()
,	_cur_param (Param_BASE)
,	_nbr_param (Param_BASE)
,	_prec_idx (0)
,	_gain_fnc ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxCpx::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_t_ptr = &fnt._t;
	_fnt_s_ptr = &fnt._s;

	const int      scr_w = _page_size [0];

	_content_sptr->set_coord (Vec2d (0, 0));

	_cur_param_sptr->set_coord (Vec2d (0            , _page_size [1] - 1));
	_cur_param_sptr->set_justification (0, 1, false);
	_cur_param_sptr->set_font (*_fnt_s_ptr);

	_prec_sptr     ->set_coord (Vec2d (3 * scr_w / 4, _page_size [1] - 1));
	_prec_sptr     ->set_justification (0, 1, false);
	_prec_sptr     ->set_font (*_fnt_s_ptr);
	_prec_sptr     ->set_blend_mode (ui::DisplayInterface::BlendMode_MAX);

	update_display ();
}



void	FxCpx::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	FxCpx::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			_prec_idx = (_prec_idx + 1) % _nbr_steps;
			update_param_txt ();
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PARAM_LIST, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		case Button_U:
			_cur_param = (_cur_param + _nbr_param - 1) % _nbr_param;
			update_param_txt ();
			break;
		case Button_D:
			_cur_param = (_cur_param              + 1) % _nbr_param;
			update_param_txt ();
			break;
		case Button_L:
			change_param (-1);
			ret_val = EvtProp_CATCH;
			break;
		case Button_R:
			change_param (+1);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	FxCpx::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
}



void	FxCpx::do_set_param (int slot_id, int index, float val, PiType type)
{
	fstb::unused (index, val, type);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	FxCpx::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxCpx::update_display ()
{
	_page_ptr->clear_all_nodes ();
	_page_ptr->push_back (_content_sptr);
	_page_ptr->push_back (_cur_param_sptr);
	_page_ptr->push_back (_prec_sptr);

	const int      scr_w = _page_size [0];
	const int      scr_h = _page_size [1];

	_content_sptr->set_size (_page_size);
	const int      stride   = _content_sptr->get_stride ();
	uint8_t *      disp_ptr = _content_sptr->use_buffer ();
	memset (disp_ptr, 0, _page_size [0] * _page_size [1]);

	const int            slot_id = _loc_edit._slot_id;
	const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
	const doc::Slot &    slot    = preset.use_slot (slot_id);

	// Scales
	display_scales ();

	auto           it_settings = slot._settings_all.find (slot._pi_model);
	if (it_settings == slot._settings_all.end ())
	{
		_nbr_param = Param_BASE;
	}
	else
	{
		const doc::PluginSettings &   settings = it_settings->second;
		const piapi::PluginDescInterface &   desc =
			_model_ptr->get_model_desc (slot._pi_model);

		const int      nbr_param = desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
		_nbr_param = Param_BASE + nbr_param;

		// Collects the parameters and applies them to the gain function
		setup_gain_fnc (settings, desc);
		std::vector <int32_t>   y_arr = build_values (scr_w, scr_h);

		// Draws the curve
		Tools::draw_curve (y_arr, disp_ptr, scr_h, stride, 255);
	}

	update_param_txt ();

	_content_sptr->invalidate_all ();
}



void	FxCpx::update_param_txt ()
{
	if (_nbr_param > 0)
	{
		_cur_param = fstb::limit (_cur_param, 0, _nbr_param - 1);

		char           txt_0 [127+1];

		const int            slot_id = _loc_edit._slot_id;
		const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
		const doc::Slot &    slot    = preset.use_slot (slot_id);

		// Standard parameters
		{
			assert (_cur_param >= Param_BASE);

			auto           it_settings = slot._settings_all.find (slot._pi_model);
			if (it_settings == slot._settings_all.end ())
			{
				fstb::snprintf4all (txt_0, sizeof (txt_0), "---");
				_cur_param_sptr->set_text (txt_0);
			}
			else
			{
				/*** To do:
				- Put this code in Tools
				- Make it simpler to use, with a maximum width in pixels for
					name + value + unit. If the unit is too large, omit it.
				***/

				const int      param_idx = _cur_param - Param_BASE;
				const doc::PluginSettings &   settings = it_settings->second;
				const float    val_nrm = settings._param_list [param_idx];
				const piapi::PluginDescInterface &   desc =
					_model_ptr->get_model_desc (slot._pi_model);
				const piapi::ParamDescInterface &    desc_param =
					desc.get_param_info (piapi::ParamCateg_GLOBAL, param_idx);
				std::string    name  = desc_param.get_name (0);
				name = pi::param::Tools::join_strings_multi (
					name.c_str (), '\n', "", " "
				);
				const int      char_w =
					_cur_param_sptr->get_char_width (char32_t ('A'));
				int            width =
					  _prec_sptr->get_coord () [0]
					- _cur_param_sptr->get_coord () [0]
					- char_w;
				assert (width > 0);
				size_t         pos_utf8;
				size_t         len_utf8;
				size_t         len_metric;
				pi::param::Tools::cut_str_bestfit (
					pos_utf8, len_utf8, len_metric,
					char_w * (3 + 1), name.c_str (), '\n',
					*_cur_param_sptr, &NText::get_char_width
				);
				name = name.substr (pos_utf8, len_utf8);
				width -= int (len_metric);
				assert (width > 0);
				Tools::set_param_text (
					*_model_ptr, *_view_ptr, width, param_idx, val_nrm,
					slot_id, PiType_MAIN, nullptr, *_cur_param_sptr,
					nullptr, nullptr, true
				);
			
				_cur_param_sptr->set_text (name + _cur_param_sptr->get_text ());
			}
		}
	}

	// Precision
	std::string    txt_prec;
	for (int prec_cnt = 0; prec_cnt <= _prec_idx; ++prec_cnt)
	{
		txt_prec += "\xE2\x9A\xAB";   // MEDIUM BLACK CIRCLE U+26AB
	}
	_prec_sptr->set_text (txt_prec);
}



void	FxCpx::setup_gain_fnc (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc)
{
	const float    rl =
		get_param (settings, desc, pi::cpx::Param_RATIO_L);
	const float    rh =
		get_param (settings, desc, pi::cpx::Param_RATIO_H);
	const float    knee_l2 =
		get_param (settings, desc, pi::cpx::Param_KNEE_SHAPE);
	const float    thr_l2  =
		get_param (settings, desc, pi::cpx::Param_KNEE_LVL);
	const float    gain_l2 =
		get_param (settings, desc, pi::cpx::Param_GAIN);
	const bool     autofix_flag = true;

	_gain_fnc.update_curve (rl, rh, thr_l2, gain_l2, knee_l2, autofix_flag);
}



std::vector <int32_t>	FxCpx::build_values (int nbr_steps, int h)
{
	assert (nbr_steps >= 2);
	assert (h >= 2);

	std::vector <int32_t>   y_arr (nbr_steps);

	const float       mul_x = fstb::rcp_uint <float> (nbr_steps - 1);
	const float       mul_y = float (h - 1) / (_lvl_l2_max - _lvl_l2_min);

	for (int x = 0; x < nbr_steps; ++x)
	{
		const float    lvl_l2_i =
			fstb::lerp (_lvl_l2_min, _lvl_l2_max, x * mul_x);
		const float    gain_l2  =
			_gain_fnc.compute_gain_l2 <false> (lvl_l2_i * 2);
		const float    lvl_l2_o = lvl_l2_i + gain_l2;
			
		const int32_t  y        =
			fstb::round_int ((_lvl_l2_max - lvl_l2_o) * mul_y);

		y_arr [x] = y;
	}

	return y_arr;
}



void	FxCpx::display_scales ()
{
	const int      db6_min = fstb::ceil_int (_lvl_l2_min);
	const int      db6_max = fstb::floor_int (_lvl_l2_max);

	const int      stride   = _content_sptr->get_stride ();
	uint8_t *      disp_ptr = _content_sptr->use_buffer ();
	Vec2d          c_size   = _content_sptr->get_bounding_box ().get_size ();
#if PV_VERSION == 2
	grap::RenderCtx   ctx { disp_ptr, c_size, stride };
#endif // PV_VERSION
	const int      w        = c_size [0];
	const int      h        = c_size [1];

	const float    mul_lvl  = 1 / (_lvl_l2_max - _lvl_l2_min);

	_legend_sptr_arr.clear ();
	int            node_id  = Entry_LEGEND_BASE;

	for (int pass = 0; pass < 2; ++pass)
	{
		for (int lvl_l2_pos = db6_min; lvl_l2_pos <= db6_max; ++lvl_l2_pos)
		{
			const bool     coarse_flag = ((lvl_l2_pos & 3) == 0);

			const float    r = (lvl_l2_pos - _lvl_l2_min) * mul_lvl;
			const int      x =
				fstb::round_int (fstb::lerp (0.f, float (w - 1), r));
			const int      y =
				fstb::round_int (fstb::lerp (float (h - 1), 0.f, r));

			if ((pass == 0) ^ (coarse_flag))
			{
#if PV_VERSION == 2
				const uint8_t  col = (coarse_flag) ? 64 : 40;
				grap::PrimLine::draw_h (ctx, 0, y, w, col, false);
				grap::PrimLine::draw_v (ctx, x, 0, h, col, false);
#else // PV_VERSION
				disp_ptr [      0 * stride +       x] = 255;
				disp_ptr [(h - 1) * stride +       x] = 255;
				disp_ptr [      y * stride +       0] = 255;
				disp_ptr [      y * stride + (w - 1)] = 255;
				if (coarse_flag)
				{
					disp_ptr [      1 * stride +       x] = 255;
					disp_ptr [(h - 2) * stride +       x] = 255;
					disp_ptr [      y * stride +       1] = 255;
					disp_ptr [      y * stride + (w - 2)] = 255;
				}
#endif // PV_VERSION
			}

			// Displays the value every 4 ticks
			if (pass == 1 && coarse_flag)
			{
				const int      lvl_db = lvl_l2_pos * 6;
				char           txt_0 [127+1];
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%+d", lvl_db);
				for (int dir = 0; dir < 2; ++dir)
				{
					TxtSPtr        txt_sptr { std::make_shared <NText> (node_id) };
					if (dir == 0)
					{
						txt_sptr->set_justification (0.5f, 0, false);
						txt_sptr->set_coord (Vec2d (x, 3));
					}
					else
					{
						txt_sptr->set_justification (1, 0.5f, false);
						txt_sptr->set_coord (Vec2d (w - 3, y));
					}
					txt_sptr->set_font (*_fnt_t_ptr);
					txt_sptr->set_blend_mode (ui::DisplayInterface::BlendMode_MAX);
					txt_sptr->set_text (txt_0);
					_legend_sptr_arr.push_back (txt_sptr);
					_page_ptr->push_back (txt_sptr);
					++ node_id;
				}
			}
		}
	}
}



void	FxCpx::change_param (int dir)
{
	float          step    = float (Cst::_step_param / pow (10, _prec_idx));
	int            slot_id = _loc_edit._slot_id;
	PiType         type    = PiType_MAIN;
	int            index   = _cur_param - Param_BASE;

	Tools::change_param (
		*_model_ptr, *_view_ptr, slot_id, type,
		index, step, _prec_idx, dir
	);
}



float	FxCpx::get_param (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int index)
{
	return float (Tools::get_param_nat (settings, desc_pi, index));
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
