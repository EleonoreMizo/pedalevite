/*****************************************************************************

        FxPEq.cpp
        Author: Laurent de Soras, 2017

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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/peq/Param.h"
#include "mfx/uitk/grap/PrimLine.h"
#include "mfx/uitk/grap/RenderCtx.h"
#include "mfx/uitk/pg/FxPEq.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <algorithm>

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



FxPEq::FxPEq (PageSwitcher &page_switcher, LocEdit &loc_edit)
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
,	_cur_param (Param_RANGE)
,	_nbr_param (Param_BAND_BASE)
,	_nbr_bands (0)
,	_cur_band (-1)
,	_range_db_idx (1) // Default: +/-12 dB
,	_prec_idx (0)
,	_settings ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxPEq::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
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
	_prec_sptr     ->set_justification (1, 1, false);
	_prec_sptr     ->set_font (*_fnt_s_ptr);
	_prec_sptr     ->set_blend_mode (ui::DisplayInterface::BlendMode_MAX);

	update_band_info ();
	update_display ();
}



void	FxPEq::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	FxPEq::do_handle_evt (const NodeEvt &evt)
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
			move_param (-1);
			break;
		case Button_D:
			move_param (+1);
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



void	FxPEq::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
}



void	FxPEq::do_set_param (int slot_id, int index, float val, PiType type)
{
	fstb::unused (index, val, type);

	if (slot_id == _loc_edit._slot_id)
	{
		update_band_info ();
		update_display ();
	}
}



void	FxPEq::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Requires update_band_info() before
void	FxPEq::update_display ()
{
	_page_ptr->clear_all_nodes ();
	_page_ptr->push_back (_content_sptr);
	_page_ptr->push_back (_cur_param_sptr);
	_page_ptr->push_back (_prec_sptr);

	const int      scr_w = _page_size [0];

	const float    f_beg = 20;    // Hz
	const float    f_end = 20000; // Hz
	const int      nbr_freq = scr_w;
	const int      height   = _page_size [1];

	_content_sptr->set_size (_page_size);
	const int      stride   = _content_sptr->get_stride ();
	uint8_t *      disp_ptr = _content_sptr->use_buffer ();
	memset (disp_ptr, 0, _page_size [0] * _page_size [1]);

	// Graduations
	display_graduations (f_beg, f_end, nbr_freq);

	// Curve
	const int            slot_id = _loc_edit._slot_id;
	const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
	const doc::Slot &    slot    = preset.use_slot (slot_id);

	if (_nbr_bands > 0)
	{
		const piapi::PluginDescInterface &   desc =
			_model_ptr->get_model_desc (slot._pi_model);
		const piapi::PluginDescInterface &   desc_mix =
			_model_ptr->get_model_desc (Cst::_plugin_dwm);

		const float    gain = get_param (
			slot._settings_mixer, desc_mix, pi::dwm::Param_GAIN
		);

		// Populates band parameters
		auto           band_arr = create_bands (_settings, desc);

		// Retrieve the z-equations as biquads
		auto           biq_arr  = retrieve_z_eq (band_arr);

		// Creates a frequency map
		auto           puls_arr = create_freq_map (nbr_freq, f_beg, f_end);

#if PV_VERSION == 2
		// First, draws only the current band contribution
		if (_cur_band >= 0)
		{
			std::vector <float>  lvl_arr (nbr_freq, 1);
			compute_freq_resp (lvl_arr, puls_arr, biq_arr [_cur_band]);
			std::vector <int32_t>   y_arr = compute_y_pos (lvl_arr, height);
			Tools::draw_curve (y_arr, disp_ptr, height, stride, 96);
		}
#endif

		// Now the main curve. Starts with the main gain
		std::vector <float>  lvl_arr (nbr_freq, gain);

		// Computes the response (linear gain) for each frequency
		for (const auto &biq : biq_arr)
		{
			compute_freq_resp (lvl_arr, puls_arr, biq);
		}

		// Transforms levels into pixel positions
		std::vector <int32_t>   y_arr = compute_y_pos (lvl_arr, height);

		// Draws the curve
		Tools::draw_curve (y_arr, disp_ptr, height, stride, 255);
	}

	update_param_txt ();

	_content_sptr->invalidate_all ();
}



void	FxPEq::display_graduations (float f_beg, float f_end, int nbr_freq)
{
	const int      h_t      = _fnt_t_ptr->get_char_h ();

	const int      stride   = _content_sptr->get_stride ();
	uint8_t *      disp_ptr = _content_sptr->use_buffer ();
	const int      height   =
		_content_sptr->get_bounding_box ().get_size () [1];

	_legend_sptr_arr.clear ();
	int            node_id  = Entry_LEGEND_BASE;

#if PV_VERSION == 2
	grap::RenderCtx   ctx {
		disp_ptr, _content_sptr->get_bounding_box ().get_size (), stride
	};
#endif

	// Hz
	static const std::array <const char *, 5> freq_0_list =
	{{
		"1", "10", "100", "1k", "10k"
	}};
	for (int p = 1; p < int (freq_0_list.size ()); ++ p)
	{
		const float    f = float (pow (10, p));
		for (int m = 1; m < 10; ++ m)
		{
			const int      x = conv_freq_to_x (f * m, f_beg, f_end, nbr_freq);
			if (x >= 0 && x < nbr_freq)
			{
#if PV_VERSION == 2
				const uint8_t  col = (m == 1) ? 64 : 40;
				grap::PrimLine::draw_v (ctx, x, 0, height, col, false);
#else
				disp_ptr [ 0           * stride + x] = 255;
				disp_ptr [(height - 1) * stride + x] = 255;
				if (m == 1 || m == 5)
				{
					disp_ptr [ 1           * stride + x] = 255;
					disp_ptr [(height - 2) * stride + x] = 255;
				}
#endif
			}
		}
		const int      x = conv_freq_to_x (f, f_beg, f_end, nbr_freq);
		if (x >= 0 && x < nbr_freq)
		{
#if PV_VERSION != 2
			for (int y = 0; y < height; y += 4)
			{
				disp_ptr [y * stride + x] = 255;
			}
#endif

			TxtSPtr        txt_sptr { std::make_shared <NText> (node_id) };
			txt_sptr->set_justification (0.5f, 0, false);
			txt_sptr->set_font (*_fnt_t_ptr);
			txt_sptr->set_coord (Vec2d (x, 3));
			txt_sptr->set_blend_mode (ui::DisplayInterface::BlendMode_MAX);
			txt_sptr->set_text (freq_0_list [p]);
			_legend_sptr_arr.push_back (txt_sptr);
			_page_ptr->push_back (txt_sptr);
			++ node_id;
		}
	}

	// dB
#if PV_VERSION != 2
	const int      height_h = height / 2;
	for (int x = 0; x < nbr_freq; x += 3)
	{
		disp_ptr [height_h * stride + x] = 255;
	}
#endif

	char           txt_0 [127+1];
#if PV_VERSION == 2
	static const std::array <float, 7> db_arr =
	{{ -1, -2/3.f, -1/3.f, 0, 1/3.f, 2/3.f, 1 }};
#else
	static const std::array <float, 4> db_arr = {{ -1, -0.5f, 0.5f, 1 }};
#endif
	for (int r_idx = 0; r_idx < int (db_arr.size ()); ++r_idx)
	{
		const float    db =
			float (_range_db_arr [_range_db_idx]) * db_arr [r_idx];
		const int      y  =
			fstb::limit (conv_db_to_y (db, height), 0, height - 1);

#if PV_VERSION == 2
		const uint8_t  col = fstb::is_null (db) ? 64 : 40;
		grap::PrimLine::draw_h (ctx, 0, y, nbr_freq, col, false);
#else
		disp_ptr [y * stride +            0] = 255;
		disp_ptr [y * stride +            1] = 255;
		disp_ptr [y * stride + nbr_freq - 2] = 255;
		disp_ptr [y * stride + nbr_freq - 1] = 255;
#endif

		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"%+.0f", db
		);

		TxtSPtr        txt_sptr { std::make_shared <NText> (node_id) };
		txt_sptr->set_justification (1, (y > h_t) ? 1.f : 0.f, false);
		txt_sptr->set_font (*_fnt_t_ptr);
		txt_sptr->set_coord (Vec2d (nbr_freq, y));
		txt_sptr->set_blend_mode (ui::DisplayInterface::BlendMode_MAX);
		txt_sptr->set_text (txt_0);
		_legend_sptr_arr.push_back (txt_sptr);
		_page_ptr->push_back (txt_sptr);
		++ node_id;
	}
}



// Requires update_band_info() before
void	FxPEq::update_param_txt ()
{
	_cur_param = fstb::limit (_cur_param, 0, _nbr_param - 1);

	char           txt_0 [127+1];

	const int            slot_id = _loc_edit._slot_id;
	const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
	const doc::Slot &    slot    = preset.use_slot (slot_id);

	// Range
	if (_cur_param == Param_RANGE)
	{
		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"R %.0f", _range_db_arr [_range_db_idx]
		);
		_cur_param_sptr->set_text (txt_0);
	}

	// Gain
	else if (_cur_param == Param_GAIN)
	{
		const piapi::PluginDescInterface &   desc_mix =
			_model_ptr->get_model_desc (Cst::_plugin_dwm);
		const float    gain    = get_param (
			slot._settings_mixer, desc_mix, pi::dwm::Param_GAIN
		);
		const float    gain_db = float (20 * log10 (gain));
		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"V %+.1f", gain_db
		);
		_cur_param_sptr->set_text (txt_0);
	}

	// Band parameters
	else
	{
		const piapi::PluginDescInterface &   desc =
			_model_ptr->get_model_desc (slot._pi_model);

		const int      param_idx =
			(_cur_param - Param_BAND_BASE) % pi::peq::Param_NBR_ELT;
		const float    nat       = get_param (
			_settings, desc, _cur_band * pi::peq::Param_NBR_ELT + param_idx);
		switch (param_idx)
		{
		case pi::peq::Param_FREQ:
			if (nat < 9999.5)
			{
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%dF %.0f", _cur_band + 1, nat
				);
			}
			else
			{
				const int      hecto = fstb::round_int (nat / 100);
				const int      kilo  = hecto / 10;
				const int      hectr = hecto % 10;
				fstb::snprintf4all (
					txt_0, sizeof (txt_0),
					"%dF %dk%d", _cur_band + 1, kilo, hectr
				);
			}
			break;

		case pi::peq::Param_Q:
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "%dQ %.2f", _cur_band + 1, nat
			);
			break;

		case pi::peq::Param_GAIN:
			{
				const float    db = float (20 * log10 (std::max (nat, 1e-9f)));
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), "%dG %+.1f", _cur_band + 1, db
				);
			}
			break;

		case pi::peq::Param_TYPE:
			switch (fstb::round_int (nat))
			{
			case pi::peq::PEqType_PEAK:
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%dPeak", _cur_band + 1);
				break;
			case pi::peq::PEqType_SHELF_LO:
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%dSh-L", _cur_band + 1);
				break;
			case pi::peq::PEqType_HP:
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%dHPF" , _cur_band + 1);
				break;
			case pi::peq::PEqType_SHELF_HI:
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%dSh-H", _cur_band + 1);
				break;
			case pi::peq::PEqType_LP:
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%dLPF" , _cur_band + 1);
				break;
			default:
				assert (false);
				break;
			}
			break;

		case pi::peq::Param_BYPASS:
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"%d%s", _cur_band + 1, (nat >= 0.5f) ? "Off" : "On"
			);
			break;

		default:
			assert (false);
			break;
		}
		_cur_param_sptr->set_text (txt_0);
	}

	// Precision
	std::string    txt_prec;
	for (int prec_cnt = 0; prec_cnt <= _prec_idx; ++prec_cnt)
	{
		txt_prec += "\xE2\x9A\xAB";   // MEDIUM BLACK CIRCLE U+26AB
	}
	_prec_sptr->set_text (txt_prec);
}



void	FxPEq::update_band_info ()
{
	const int            slot_id = _loc_edit._slot_id;
	const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
	const doc::Slot &    slot    = preset.use_slot (slot_id);

	auto           it_settings = slot._settings_all.find (slot._pi_model);
	if (it_settings == slot._settings_all.end ())
	{
		_nbr_param = Param_BAND_BASE;
		_nbr_bands = 0;
	}
	else
	{
		_settings = it_settings->second;
		const piapi::PluginDescInterface &   desc =
			_model_ptr->get_model_desc (slot._pi_model);
		const int      nbr_param = desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
		_nbr_bands = nbr_param / pi::peq::Param_NBR_ELT;
		_nbr_param = Param_BAND_BASE + _nbr_bands * pi::peq::Param_NBR_ELT;
	}

	_cur_param = fstb::limit (_cur_param, 0, _nbr_param - 1);
	if (_nbr_bands <= 0)
	{
		_cur_band  = -1;
	}
	else
	{
		if (_cur_param < Param_BAND_BASE)
		{
			_cur_band = -1;
		}
		else
		{
			_cur_band = (_cur_param - Param_BAND_BASE) / pi::peq::Param_NBR_ELT;
			if (_cur_band >= _nbr_bands)
			{
				assert (false);
				_cur_band = -1;
			}
		}
	}
}



std::vector <pi::peq::BandParam>	FxPEq::create_bands (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc) const
{
	const int      nbr_param = desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
	const int      nbr_bands = nbr_param / pi::peq::Param_NBR_ELT;
	std::vector <pi::peq::BandParam> band_arr (nbr_bands);

	// Populates band parameters
	for (int b_cnt = 0; b_cnt < nbr_bands; ++b_cnt)
	{
		const int      base   = b_cnt * pi::peq::Param_NBR_ELT;
		auto &         band   = band_arr [b_cnt];

		const float    freq   =
			get_param (settings, desc, base + pi::peq::Param_FREQ);
		const float    q      =
			get_param (settings, desc, base + pi::peq::Param_Q);
		const float    gain   =
			get_param (settings, desc, base + pi::peq::Param_GAIN);
		const float    type   =
			get_param (settings, desc, base + pi::peq::Param_TYPE);
		const float    bypass =
			get_param (settings, desc, base + pi::peq::Param_BYPASS);

		band.set_freq (freq);
		band.set_q    (q);
		band.set_gain (gain);
		band.set_type (static_cast <pi::peq::PEqType> (fstb::round_int (type)));
		band.set_bypass (bypass >= 0.5f);
	}

	return band_arr;
}



std::vector <FxPEq::Biq>	FxPEq::retrieve_z_eq (const std::vector <pi::peq::BandParam> &band_arr) const
{
	std::vector <Biq> biq_arr;

	const int      nbr_bands = int (band_arr.size ());
	const float    fs        = float (_model_ptr->get_sample_freq ());
	const float    inv_fs    = 1.0f / fs;
	for (int b_cnt = 0; b_cnt < nbr_bands; ++b_cnt)
	{
		Biq            biq;
		const auto &   band = band_arr [b_cnt];
		if (! band.is_bypass ())
		{
			band.create_filter (&biq._b [0], &biq._a [0], fs, inv_fs);
		}
		biq_arr.push_back (biq);
	}

	return biq_arr;
}



std::vector <float>	FxPEq::create_freq_map (int nbr_freq, float f_beg, float f_end) const
{
	assert (nbr_freq > 0);
	assert (f_beg > 0);
	assert (f_end > f_beg);

	// For each displayed pixel column, rad/s
	std::vector <float>  puls_arr (nbr_freq);
	const float    fs        = float (_model_ptr->get_sample_freq ());
	const float    base      = float (log2 (double (2 * fstb::PI) * f_beg / fs));
	const float    mul       = float (log2 (f_end / f_beg) / nbr_freq);
	const auto     v_base    = fstb::ToolsSimd::set1_f32 (base);
	const auto     v_mul     = fstb::ToolsSimd::set1_f32 (mul);
	const auto     v_linstep = fstb::ToolsSimd::set_f32 (0, 1, 2, 3);
	for (int f_idx = 0; f_idx < nbr_freq; f_idx += 4)
	{
		auto          v_idx  = fstb::ToolsSimd::set1_f32 (float (f_idx));
		v_idx += v_linstep;
		auto          v_puls =
			fstb::ToolsSimd::exp2_approx (v_base + v_idx * v_mul);

		fstb::ToolsSimd::storeu_f32_part (
			&puls_arr [f_idx], v_puls, nbr_freq - f_idx
		);
	}

	return puls_arr;
}



// Output positions can be located out of the rendering zone.
std::vector <int32_t>	FxPEq::compute_y_pos (const std::vector <float> &lvl_arr, int pix_h) const
{
	const int      nbr_freq = int (lvl_arr.size ());
	std::vector <int> y_arr (nbr_freq);

	const float    range = float (_range_db_arr [_range_db_idx]);
	const float    hh    = pix_h * 0.5f;
	const auto     mul   = fstb::ToolsSimd::set1_f32 (float (
		-20 * fstb::LOG10_2 * hh / range
	));
	const auto     ofs   = fstb::ToolsSimd::set1_f32 (hh);
	const auto     secu  = fstb::ToolsSimd::set1_f32 (1e-15f);
	for (int f_idx = 0; f_idx < nbr_freq; f_idx += 4)
	{
		const int      ns    = nbr_freq - f_idx;

		auto           lvl   =
			fstb::ToolsSimd::loadu_f32_part (&lvl_arr [f_idx], ns);
		lvl = fstb::ToolsSimd::max_f32 (lvl, secu);
		const auto     y_flt =
			fstb::ToolsSimd::log2_approx (lvl) * mul + ofs;
		const auto     y     = fstb::ToolsSimd::conv_f32_to_s32 (y_flt);
		fstb::ToolsSimd::storeu_s32_part (&y_arr [f_idx], y, ns);
	}

	return y_arr;
}



/*
H (z) = (b0 + b1 * z^-1 + b2 * z^-2) / (1 + a1 * z^-1 + a2 * z^-2)

z -> exp (j * w)

c1 = cos (w)
s1 = sin (w)

|H (z)|^2 = H (z) * H*(z)
= (((b0 + b2) * c1 + b1)^2 + ((b0 - b2) * s1)^2) / (((1 + a2) * c1 + a1)^2 + ((1 - a2) * s1)^2)
*/
void	FxPEq::compute_freq_resp (std::vector <float> &lvl_arr, const std::vector <float> &puls_arr, const Biq &biq) const
{
	const int      nbr_freq = int (puls_arr.size ());
	assert (int (lvl_arr.size ()) == nbr_freq);

	const auto     one  = fstb::ToolsSimd::set1_f32 (1);
	const auto     b0   = fstb::ToolsSimd::set1_f32 (biq._b [0]);
	const auto     b1   = fstb::ToolsSimd::set1_f32 (biq._b [1]);
	const auto     b2   = fstb::ToolsSimd::set1_f32 (biq._b [2]);
	const auto     a1   = fstb::ToolsSimd::set1_f32 (biq._a [1]);
	const auto     a2   = fstb::ToolsSimd::set1_f32 (biq._a [2]);

	for (int f_idx = 0; f_idx < nbr_freq; f_idx += 4)
	{
		const int      ns = nbr_freq - f_idx;

		const auto     w  = fstb::ToolsSimd::loadu_f32_part (
			&puls_arr [f_idx], ns
		);

		fstb::ToolsSimd::VectF32   c1;
		fstb::ToolsSimd::VectF32   s1;
		fstb::Approx::cos_sin_rbj (c1, s1, w);

		const auto     h2_nc = (b0  + b2) * c1 + b1;
		const auto     h2_dc = (one + a2) * c1 + a1;
		const auto     h2_ns = (b0  - b2) * s1;
		const auto     h2_ds = (one - a2) * s1;
		const auto     h2_n  = h2_nc * h2_nc + h2_ns * h2_ns;
		const auto     h2_d  = h2_dc * h2_dc + h2_ds * h2_ds;
		const auto     h2    = h2_n * fstb::ToolsSimd::rcp_approx2 (h2_d);
		const auto     h_abs = fstb::ToolsSimd::sqrt (h2);

		auto           l =
			fstb::ToolsSimd::loadu_f32_part (&lvl_arr [f_idx], ns);
		l *= h_abs;
		fstb::ToolsSimd::storeu_f32_part (&lvl_arr [f_idx], l, ns);
	}
}



// Result can be out of the window
int	FxPEq::conv_freq_to_x (float f, float f_beg, float f_end, int nbr_freq) const
{
	const float    f_rel = f     / f_beg;
	const float    f_amp = f_end / f_beg;
	const float    l_rel = log2 (f_rel);
	const float    l_amp = log2 (f_amp);
	const float    x_flt = l_rel * nbr_freq / l_amp;
	const int      x     = fstb::round_int (x_flt);

	return x;
}



int	FxPEq::conv_db_to_y (float db, int pix_h) const
{
	const float    range_db = float (_range_db_arr [_range_db_idx]);
	const float    pos_rel  = db / range_db;
	const float    hh       = pix_h * 0.5f;
	const float    y_flt    = hh * (1 - pos_rel);
	const int      y        = fstb::round_int (y_flt);

	return y;
}



void	FxPEq::move_param (int dir)
{
	const int      band_old = _cur_band;
	_cur_param = (_cur_param + _nbr_param + dir) % _nbr_param;
	update_band_info ();
	if (_cur_band != band_old)
	{
		update_display ();
	}
	else
	{
		update_param_txt ();
	}
}



void	FxPEq::change_param (int dir)
{
	if (_cur_param == Param_RANGE)
	{
		const int      nbr_ranges = int (_range_db_arr.size ());
		_range_db_idx = (_range_db_idx + nbr_ranges + dir) % nbr_ranges;
		update_display ();
	}

	else
	{
		float          step    = float (Cst::_step_param / pow (10, _prec_idx));
		int            slot_id = -1;
		PiType         type    = PiType_INVALID;
		int            index   = -1;

		if (_cur_param == Param_GAIN)
		{
			slot_id = _loc_edit._slot_id;
			index   = pi::dwm::Param_GAIN;
			type    = PiType_MIX;
		}
		else
		{
			slot_id = _loc_edit._slot_id;
			index   = _cur_param - Param_BAND_BASE;
			type    = PiType_MAIN;
		}

		Tools::change_param (
			*_model_ptr, *_view_ptr, slot_id, type,
			index, step, _prec_idx, dir
		);
	}
}



float	FxPEq::get_param (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int index)
{
	return float (Tools::get_param_nat (settings, desc_pi, index));
}



const std::array <double, 6>	FxPEq::_range_db_arr =
{{
	6, 12, 18, 24, 36, 48
}};



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
