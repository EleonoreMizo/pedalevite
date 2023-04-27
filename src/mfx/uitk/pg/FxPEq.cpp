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



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/peq/Param.h"
#include "mfx/uitk/grap/PrimLine.h"
#include "mfx/uitk/grap/PrimBox.h"
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
#include <climits>
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



void	FxPEq::do_activate_prog (int index)
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
	const doc::Program & prog    = _view_ptr->use_prog_cur ();
	const doc::Slot &    slot    = prog.use_slot (slot_id);

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
			const int      x =
				conv_freq_to_x (f * float (m), f_beg, f_end, nbr_freq);
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

	// Keyboard
#if PV_VERSION == 2
	constexpr auto st_per_oct = 12;
	constexpr auto kc_ol = 40;
	constexpr auto kc_w  = 80;
	constexpr auto kc_b  = 0;
	const auto     ky_w  = height - h_t * 2;
	const auto     ky_b  = ky_w - h_t;
	const auto     ky_0  = ky_b - h_t;
	constexpr std::array <bool, st_per_oct> bk_flag_arr =
	{
		false, true, false, true, false, false, true, // C - F#
		false, true, false, true, false               // G - B
	};

	// Background, fill everything in white
	constexpr auto note_beg  = 12 + 5;
	constexpr auto note_end  = 125;
	const int      kbd_x_l   =
		conv_pitch_to_x (float (note_beg) - 0.5f, f_beg, f_end, nbr_freq);
	const int      kbd_x_r   =
		conv_pitch_to_x (float (note_end) - 0.5f, f_beg, f_end, nbr_freq);
	grap::PrimBox::draw_filled (
		ctx, kbd_x_l, ky_0, kbd_x_r - kbd_x_l, ky_w - ky_0, kc_w
	);

	bool           bk_p_flag = false; // Blackness of the previous key
	for (int note_idx = note_beg; note_idx < note_end; ++note_idx)
	{
		const int      oct_idx   = note_idx / st_per_oct;
		const int      st_idx    = note_idx - oct_idx * st_per_oct;
		const auto     kx_l      =
			conv_pitch_to_x (float (note_idx) - 0.5f, f_beg, f_end, nbr_freq);
		const auto     kx_h      =
			conv_pitch_to_x (float (note_idx) + 0.5f, f_beg, f_end, nbr_freq);
		const auto     kx_m      = (kx_l + kx_h + 1) / 2;
		const auto     bk_c_flag = bk_flag_arr [st_idx];
		switch ((bk_c_flag ? 1 : 0) + (bk_p_flag ? 2 : 0))
		{
		case 0: // White preceded by white
			grap::PrimLine::draw_v (ctx, kx_l, ky_0, ky_w, kc_ol, false);
			break;
		case 1: // Black preceded by white
			grap::PrimBox::draw_filled (
				ctx, kx_l, ky_0, kx_h - kx_l, ky_b - ky_0, kc_b
			);
			grap::PrimLine::draw_v (ctx, kx_m, ky_b, ky_w, kc_ol, false);
			break;
		case 2: // White preceded by black
			// Nothing
			break;
		default:
			assert (false);
			break;
		}

		bk_p_flag = bk_c_flag;
	}

#endif

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
	const doc::Program & prog    = _view_ptr->use_prog_cur ();
	const doc::Slot &    slot    = prog.use_slot (slot_id);

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
	const doc::Program & prog    = _view_ptr->use_prog_cur ();
	const doc::Slot &    slot    = prog.use_slot (slot_id);

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
	const auto     v_base    = fstb::Vf32 (base);
	const auto     v_mul     = fstb::Vf32 (mul);
	const auto     v_linstep = fstb::Vf32 (0, 1, 2, 3);
	for (int f_idx = 0; f_idx < nbr_freq; f_idx += fstb::Vf32::_length)
	{
		auto          v_idx  = fstb::Vf32 (float (f_idx));
		v_idx += v_linstep;
		auto          v_puls = fstb::Approx::exp2 (v_base + v_idx * v_mul);

		v_puls.storeu_part (&puls_arr [f_idx], nbr_freq - f_idx);
	}

	return puls_arr;
}



// Output positions can be located out of the rendering zone.
std::vector <int32_t>	FxPEq::compute_y_pos (const std::vector <float> &lvl_arr, int pix_h) const
{
	const int      nbr_freq = int (lvl_arr.size ());
	std::vector <int> y_arr (nbr_freq);

	const float    range = float (_range_db_arr [_range_db_idx]);
	const float    hh    = float (pix_h) * 0.5f;
	const auto     mul   = fstb::Vf32 (float (
		-20 * fstb::LOG10_2 * hh / range
	));
	const auto     ofs   = fstb::Vf32 (hh);
	const auto     secu  = fstb::Vf32 (1e-15f);
	for (int f_idx = 0; f_idx < nbr_freq; f_idx += fstb::Vf32::_length)
	{
		const int      ns    = nbr_freq - f_idx;

		auto           lvl   = fstb::Vf32::loadu_part (&lvl_arr [f_idx], ns);
		lvl = fstb::max (lvl, secu);
		const auto     y_flt = fstb::Approx::log2 (lvl) * mul + ofs;
		const auto     y     = fstb::ToolsSimd::conv_f32_to_s32 (y_flt);
		y.storeu_part (&y_arr [f_idx], ns);
	}

	return y_arr;
}



/*
H (z) = (b0 + b1 * z^-1 + b2 * z^-2) / (a0 + a1 * z^-1 + a2 * z^-2)

z -> exp (j * w)

c1 = cos (w)
s1 = sin (w)

|H (z)|^2 = H (z) * H*(z)
          =   (((b0 + b2) * c1 + b1)^2 + ((b0 - b2) * s1)^2)
            / (((a0 + a2) * c1 + a1)^2 + ((a0 - a2) * s1)^2)

Cosine problem: at low frequencies, cos (w) can be very close to 1, possibly
causing issues in the response calculation in single precision (thanks to RBJ).
https://dsp.stackexchange.com/a/16911

phi = (sin (w / 2))^2
|H (z)|^2 =
	  (((b0 + b1 + b2) / 2)^2 - phi * (4 * b0 * b1 * (1 - phi) + b1 * (b0 + b2))
	/ (((a0 + a1 + a2) / 2)^2 - phi * (4 * a0 * a1 * (1 - phi) + a1 * (a0 + a2))

*/

void	FxPEq::compute_freq_resp (std::vector <float> &lvl_arr, const std::vector <float> &puls_arr, const Biq &biq) const
{
#define mfx_uitk_pg_FxPEq_COSFIX

	const int      nbr_freq = int (puls_arr.size ());
	assert (int (lvl_arr.size ()) == nbr_freq);

	const auto     one  = fstb::Vf32 (1);
	const auto     b0   = fstb::Vf32 (biq._b [0]);
	const auto     b1   = fstb::Vf32 (biq._b [1]);
	const auto     b2   = fstb::Vf32 (biq._b [2]);
	const auto     a0   = one;
	const auto     a1   = fstb::Vf32 (biq._a [1]);
	const auto     a2   = fstb::Vf32 (biq._a [2]);
#if defined (mfx_uitk_pg_FxPEq_COSFIX)
	const auto     half = fstb::Vf32 (0.5f);
	const auto     four = fstb::Vf32 (4.0f);
	const auto     b_c0 = fstb::sq ((b0 + b1 + b2) * half);
	const auto     a_c0 = fstb::sq ((a0 + a1 + a2) * half);
	const auto     b_c1 = four * b0 * b2;
	const auto     a_c1 = four * a0 * a2;
	const auto     b_c2 = b1 * (b0 + b2);
	const auto     a_c2 = a1 * (a0 + a2);
#endif // mfx_uitk_pg_FxPEq_COSFIX

	for (int f_idx = 0; f_idx < nbr_freq; f_idx += fstb::Vf32::_length)
	{
		const int      ns = nbr_freq - f_idx;

		const auto     w  = fstb::Vf32::loadu_part (&puls_arr [f_idx], ns);

#if defined (mfx_uitk_pg_FxPEq_COSFIX)
		const auto     s_hw  = fstb::Approx::sin_rbj (w * half);
		const auto     phi   = s_hw * s_hw;
		const auto     omphi = one - phi;
		const auto     h2_n  = b_c0 - phi * (b_c1 * omphi + b_c2);
		const auto     h2_d  = a_c0 - phi * (a_c1 * omphi + a_c2);
#else // mfx_uitk_pg_FxPEq_COSFIX
		fstb::Vf32     c1;
		fstb::Vf32     s1;
		fstb::Approx::cos_sin_rbj (c1, s1, w);

		const auto     h2_nc = (b0 + b2) * c1 + b1;
		const auto     h2_dc = (a0 + a2) * c1 + a1;
		const auto     h2_ns = (b0 - b2) * s1;
		const auto     h2_ds = (a0 - a2) * s1;
		const auto     h2_n  = h2_nc * h2_nc + h2_ns * h2_ns;
		const auto     h2_d  = h2_dc * h2_dc + h2_ds * h2_ds;
#endif // mfx_uitk_pg_FxPEq_COSFIX

		const auto     h2    = h2_n / h2_d;
		const auto     h_abs = fstb::sqrt (h2);

		auto           l     = fstb::Vf32::loadu_part (&lvl_arr [f_idx], ns);
		l *= h_abs;
		l.storeu_part (&lvl_arr [f_idx], ns);
	}

#undef mfx_uitk_pg_FxPEq_COSFIX
}



// Result can be out of the window
int	FxPEq::conv_freq_to_x (float f, float f_beg, float f_end, int nbr_freq) const
{
	const float    f_rel = f     / f_beg;
	const float    f_amp = f_end / f_beg;
	const float    l_rel = log2f (f_rel);
	const float    l_amp = log2f (f_amp);
	const float    x_flt = l_rel * float (nbr_freq) / l_amp;
	const int      x     = fstb::round_int (x_flt);

	return x;
}



// Result can be out of the window
int	FxPEq::conv_pitch_to_x (float note, float f_beg, float f_end, int nbr_freq) const
{
	/*** To do: simplify formula to remove the exp/log stuff. ***/

	constexpr auto st_per_oct = 12;
	constexpr auto oct_per_st = 1.0 / double (st_per_oct);
	constexpr auto note_a440  = 69; // MIDI note for A440
	const auto     oct_a      = float ((note - note_a440) * oct_per_st);
	const auto     freq       = 440.f * exp2f (oct_a);
	const auto     x          = conv_freq_to_x (freq, f_beg, f_end, nbr_freq);

	return x;
}



int	FxPEq::conv_db_to_y (float db, int pix_h) const
{
	const float    range_db = float (_range_db_arr [_range_db_idx]);
	const float    pos_rel  = db / range_db;
	const float    hh       = float (pix_h) * 0.5f;
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
		const int      step_scale =
			_page_ptr->get_shift (PageMgrInterface::Shift::R) ? 1 : 0;
		float          step    =
			float (Cst::_step_param / pow (10, _prec_idx + step_scale));
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
