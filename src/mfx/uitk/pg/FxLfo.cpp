/*****************************************************************************

        FxLfo.cpp
        Author: Laurent de Soras, 2018

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
#include "mfx/pi/lfo1/Param.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/BypassState.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/ProcInfo.h"
#include "mfx/uitk/pg/FxLfo.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <cassert>
#include <cstring>
#include <cmath>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FxLfo::FxLfo (PageSwitcher &page_switcher, LocEdit &loc_edit)
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
,	_cur_param (Param_NBR_CYCLES)
,	_nbr_param (Param_BASE)
,	_nbr_cycles_idx (0)
,	_prec_idx (0)
,	_lfo ()
,	_evt_list ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxLfo::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
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



void	FxLfo::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	FxLfo::do_handle_evt (const NodeEvt &evt)
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



void	FxLfo::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
}



void	FxLfo::do_set_param (int slot_id, int index, float val, PiType type)
{
	fstb::unused (index, val, type);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	FxLfo::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxLfo::update_display ()
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

	const float    len_cycles  = _nbr_cycles_arr [_nbr_cycles_idx];

	auto           it_settings = slot._settings_all.find (slot._pi_model);
	if (it_settings == slot._settings_all.end ())
	{
		_nbr_param = Param_BASE;
	}
	else
	{
		const doc::PluginSettings &   settings = it_settings->second;
		const piapi::PluginDescInterface &   desc =
			_model_ptr->get_model_desc ("lfo1"); // Forces standard LFO description

		const int      nbr_param = desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
		_nbr_param = Param_BASE + nbr_param;

		// Collects parameters and applies them to the LFO
		setup_lfo (settings, desc, scr_w, len_cycles);
		std::vector <int32_t>   y_arr = build_values (scr_w, scr_h);

		// Draws the curve
		Tools::draw_curve (y_arr, disp_ptr, scr_h, stride);
	}

	// Scales
	display_scales (len_cycles);

	update_param_txt ();

	_content_sptr->invalidate_all ();
}



void	FxLfo::update_param_txt ()
{
	_cur_param = fstb::limit (_cur_param, 0, _nbr_param - 1);

	char           txt_0 [127+1];

	const int            slot_id = _loc_edit._slot_id;
	const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
	const doc::Slot &    slot    = preset.use_slot (slot_id);

	// Nbr cycles
	if (_cur_param == Param_NBR_CYCLES)
	{
		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"Cycles %.2f", _nbr_cycles_arr [_nbr_cycles_idx]
		);
		_cur_param_sptr->set_text (txt_0);
	}

	// Standard parameters
	else
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
				slot_id, PiType_MAIN, 0, *_cur_param_sptr,
				0, 0, true
			);
			
			_cur_param_sptr->set_text (name + _cur_param_sptr->get_text ());
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



void	FxLfo::setup_lfo (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int nbr_steps, float len_cycles)
{
	assert (nbr_steps > 0);
	assert (len_cycles > 0);
	assert (len_cycles * 2 < float (nbr_steps));

	int            latency = 0;
	_lfo.reset (_sample_freq, _proc_blk_size, latency);

	// Collects the parameters
	_evt_list.clear ();
	piapi::EventTs   evt;
	evt._timestamp = 0;
	evt._type      = piapi::EventType_PARAM;
	evt._evt._param._categ   = piapi::ParamCateg_GLOBAL;
	evt._evt._param._note_id = -1;
	for (int index = 0; index < pi::lfo1::Param_NBR_ELT; ++index)
	{
		float          nrm = settings._param_list [index];
		const piapi::ParamDescInterface &   desc_param =
			desc_pi.get_param_info (piapi::ParamCateg_GLOBAL, index);

		switch (index)
		{
		case pi::lfo1::Param_SPEED:
			{
				const float    speed =
					_sample_freq * len_cycles / (nbr_steps * _proc_blk_size);
				nrm = float (desc_param.conv_nat_to_nrm (speed));
			}
			break;
		case pi::lfo1::Param_AMP:
			nrm = float (desc_param.conv_nat_to_nrm (1));
			break;
		case pi::lfo1::Param_PHASE_SET:
			if (nrm > 0.9999f)
			{
				nrm = 0;
			}
			break;
		default:
			// Nothing
			break;
		}

		evt._evt._param._index = index;
		evt._evt._param._val   = nrm;
		_evt_list.push_back (evt);
	}
}



float	FxLfo::get_param_nat (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int index) const
{
	assert (index >= 0);
	assert (index < int (settings._param_list.size ()));

	const float       nrm = settings._param_list [index];
	const piapi::ParamDescInterface &   desc_param =
		desc_pi.get_param_info (piapi::ParamCateg_GLOBAL, index);
	const float       nat = float (desc_param.conv_nrm_to_nat (nrm));

	return nat;
}



std::vector <int32_t>	FxLfo::build_values (int nbr_steps, int h)
{
	assert (nbr_steps > 0);
	assert (h >= 2);

	std::vector <int32_t>   y_arr;

	std::vector <const piapi::EventTs *>   evt_ptr_list;
	for (const auto &evt : _evt_list)
	{
		evt_ptr_list.push_back (&evt);
	}

	std::array <float, _proc_blk_size>  buf_sig;
	std::array <float *, 1> sig_arr = {{ &buf_sig [0] }};
	piapi::ProcInfo proc_info;
	proc_info._byp_arr     = nullptr;
	proc_info._byp_state   = piapi::BypassState_IGNORE;
	proc_info._dst_arr     = nullptr;
	proc_info._evt_arr     = &evt_ptr_list [0];
	proc_info._dir_arr [piapi::Dir_IN ]._nbr_chn = 0;
	proc_info._dir_arr [piapi::Dir_OUT]._nbr_chn = 0;
	proc_info._nbr_evt     = int (evt_ptr_list.size ());
	proc_info._nbr_spl     = _proc_blk_size;
	proc_info._sig_arr     = &sig_arr [0];
	proc_info._src_arr     = nullptr;

	_lfo.process_block (proc_info);
	int            latency = 0;
	_lfo.reset (_sample_freq, _proc_blk_size, latency);
	proc_info._evt_arr = nullptr;
	proc_info._nbr_evt = 0;

	for (int x = 0; x < nbr_steps; ++x)
	{
		_lfo.process_block (proc_info);
		const float    val = buf_sig [0];
		const int      y   = conv_val_to_y (val, h);
		y_arr.push_back (y);
	}

	return y_arr;
}



void	FxLfo::display_scales (float len_cycles)
{
	assert (len_cycles > 0);


	/*** To do ***/
	fstb::unused (len_cycles);


}



int	FxLfo::conv_val_to_y (float val, int pix_h) const
{
	assert (pix_h >= 2);

	return fstb::round_int ((1 - val) * 0.5f * (pix_h - 1));
}



void	FxLfo::change_param (int dir)
{
	if (_cur_param == Param_NBR_CYCLES)
	{
		const int      sz = int (_nbr_cycles_arr.size ());
		_nbr_cycles_idx = (_nbr_cycles_idx + sz + dir) % sz;
		update_display ();
	}

	else
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
}



const std::array <float, 5>	FxLfo::_nbr_cycles_arr =
{{
	1, 1.25, 2, 3, 4
}};



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
