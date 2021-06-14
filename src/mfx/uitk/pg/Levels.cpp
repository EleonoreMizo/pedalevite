/*****************************************************************************

        Levels.cpp
        Author: Laurent de Soras, 2016

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
#include "fstb/fnc.h"
#include "mfx/adrv/DriverInterface.h"
#include "mfx/uitk/grap/PrimBox.h"
#include "mfx/uitk/grap/PrimLine.h"
#include "mfx/uitk/grap/RenderCtx.h"
#include "mfx/uitk/pg/Levels.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/ChnMode.h"
#include "mfx/MeterResultSet.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Levels::Levels (PageSwitcher &page_switcher, adrv::DriverInterface &snd_drv)
:	_page_switcher (page_switcher)
,	_snd_drv (snd_drv)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_dir_arr ()
,	_lvl_sptr (    std::make_shared <NText  > (Entry_LVL_OUT))
,	_chn_sptr (    std::make_shared <NText  > (Entry_CHN    ))
,	_dsp_txt_sptr (std::make_shared <NText  > (Entry_DSP_TXT))
,	_dsp_sptr (    std::make_shared <NBitmap> (Entry_DSP    ))
,	_dsp_val_sptr (std::make_shared <NText  > (Entry_DSP_VAL))
{
	_dir_arr [Dir_IN ]._lab_sptr = std::make_shared <NText  > (Entry_LAB_IN );
	_dir_arr [Dir_IN ]._vum_sptr = std::make_shared <NBitmap> (Entry_VUM_IN );
	_dir_arr [Dir_OUT]._lab_sptr = std::make_shared <NText  > (Entry_LAB_OUT);
	_dir_arr [Dir_OUT]._vum_sptr = std::make_shared <NBitmap> (Entry_VUM_OUT);

	_dir_arr [Dir_IN ]._lab_sptr->set_text ("Input");
	_dir_arr [Dir_IN ]._vum_sptr->set_size (Vec2d (
		_meter_audio_w,
		_meter_audio_h + _meter_grad_h
	));
	_dir_arr [Dir_OUT]._lab_sptr->set_text ("Output");
	_dir_arr [Dir_OUT]._vum_sptr->set_size (Vec2d (
		_meter_audio_w,
		_meter_audio_h + _meter_grad_h
	));
	_lvl_sptr->set_justification (1, 0, false);
	_chn_sptr->set_justification (0, 1, false);
	_dsp_txt_sptr->set_text ("DSP");
	_dsp_txt_sptr->set_justification (1, 1, false);
	_dsp_val_sptr->set_justification (1, 1, false);
	_dsp_val_sptr->set_blend_mode (ui::DisplayInterface::BlendMode_XOR);
	_dsp_sptr->set_size (Vec2d (_meter_dsp_w, _meter_dsp_h));

#if (PV_VERSION == 2)
	char           txt_0 [255+1];
	int            node_id = Entry_GRAD;
	for (int dir = 0; dir < Dir_NBR_ELT; ++dir)
	{
		DirInfo &      info = _dir_arr [dir];
		for (int g_cnt = 0; g_cnt < DirInfo::_nbr_grad; ++g_cnt)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"%+d", DirInfo::_grad_db_arr [g_cnt]
			);
			TxtSPtr &      txt_sptr = info._grad_sptr_arr [g_cnt];
			txt_sptr = std::make_shared <NText> (node_id);
			txt_sptr->set_justification ((g_cnt == 0) ? 0 : 0.5f, 0, false);
			txt_sptr->set_text (txt_0);
			++ node_id;
		}
		info._clip_sptr = std::make_shared <NText> (node_id);
		info._clip_sptr->set_justification (1, 0, false);
		info._clip_sptr->set_text ("CLIP");
		++ node_id;
	}
#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Levels::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	const int      h_m   = fnt._m.get_char_h ();

	_dir_arr [Dir_IN ]._lab_sptr->set_font (fnt._m);
	_dir_arr [Dir_OUT]._lab_sptr->set_font (fnt._m);
	_lvl_sptr->set_font (fnt._m);
	_chn_sptr->set_font (fnt._m);
	_dsp_txt_sptr->set_font (fnt._m);
	_dsp_val_sptr->set_font (fnt._s);

	const int      meter_skip_h = _meter_audio_h + h_m * 2 + (h_m >> 1);
	_dir_arr [Dir_IN ]._lab_sptr->set_coord (Vec2d (0,  0));
	_dir_arr [Dir_IN ]._vum_sptr->set_coord (Vec2d (0,  0 + h_m));
	_dir_arr [Dir_OUT]._lab_sptr->set_coord (Vec2d (0, meter_skip_h));
	_dir_arr [Dir_OUT]._vum_sptr->set_coord (Vec2d (0, meter_skip_h + h_m));
	_lvl_sptr->set_coord (Vec2d (_meter_audio_w, meter_skip_h));
	_chn_sptr->set_coord (Vec2d (0, _page_size [1]));
	const Vec2d    dsp_pos (_meter_audio_w, _page_size [1]);
	_dsp_txt_sptr->set_coord (dsp_pos - Vec2d (_meter_dsp_w, 0));
	_dsp_sptr->set_coord (dsp_pos - Vec2d (_meter_dsp_w, _meter_dsp_h));
	_dsp_val_sptr->set_coord (dsp_pos - Vec2d (1, 1));

	_page_ptr->push_back (_dir_arr [Dir_IN ]._lab_sptr);
	_page_ptr->push_back (_dir_arr [Dir_IN ]._vum_sptr);
	_page_ptr->push_back (_dir_arr [Dir_OUT]._lab_sptr);
	_page_ptr->push_back (_dir_arr [Dir_OUT]._vum_sptr);
#if (PV_VERSION == 2)
	for (int dir = 0; dir < Dir_NBR_ELT; ++dir)
	{
		DirInfo &      info = _dir_arr [dir];
		const int      y    =
			h_m + dir * meter_skip_h + _meter_audio_h + _meter_grad_h + _scale;
		for (int g_cnt = 0; g_cnt < DirInfo::_nbr_grad; ++g_cnt)
		{
			TxtSPtr &      txt_sptr = info._grad_sptr_arr [g_cnt];
			const int      db  = DirInfo::_grad_db_arr [g_cnt];
			const float    lvl = float (pow (10, double (db) / 20));
			const int      x   = fstb::round_int (conv_level_to_pix (lvl));
			txt_sptr->set_font (fnt._t);
			txt_sptr->set_coord (Vec2d (x, y));
			_page_ptr->push_back (txt_sptr);
		}
		info._clip_sptr->set_font (fnt._t);
		info._clip_sptr->set_coord (Vec2d (_meter_audio_w, y));
		_page_ptr->push_back (info._clip_sptr);
	}
#endif
	_page_ptr->push_back (_lvl_sptr);
	_page_ptr->push_back (_chn_sptr);
	_page_ptr->push_back (_dsp_txt_sptr);
	_page_ptr->push_back (_dsp_sptr);
	_page_ptr->push_back (_dsp_val_sptr);

	PageMgrInterface::NavLocList  nav_list;
	PageMgrInterface::add_nav (nav_list, Entry_VUM_IN );
	PageMgrInterface::add_nav (nav_list, Entry_LVL_OUT);
	PageMgrInterface::add_nav (nav_list, Entry_VUM_OUT);
	PageMgrInterface::add_nav (nav_list, Entry_CHN    );
	PageMgrInterface::add_nav (nav_list, Entry_DSP_TXT);
	page.set_nav_layout (nav_list);
	page.jump_to (Entry_LVL_OUT);

	_page_ptr->set_timer (0, true);

	for (auto & d : _dir_arr)
	{
		for (auto & c : d._chn_arr)
		{
			c._clip_flag = false;
			c._lvl_peak  = 0;
			c._lvl_rms   = 0;
		}
	}

	refresh_display ();
}



void	Levels::do_disconnect ()
{
	_page_ptr->set_timer (0, false);
}



MsgHandlerInterface::EvtProp	Levels::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_timer ())
	{
		refresh_display ();
		ret_val = EvtProp_CATCH;
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_CHN:
				chg_chn_mode (+1);
				break;
			case Entry_VUM_IN:
				for (auto &c : _dir_arr [Dir_IN ]._chn_arr)
				{
					c._clip_flag = false;
				}
				refresh_display ();
				break;
			case Entry_VUM_OUT:
				for (auto &c : _dir_arr [Dir_OUT]._chn_arr)
				{
					c._clip_flag = false;
				}
				refresh_display ();
				break;
			case Entry_DSP_TXT:
				_snd_drv.restart ();
				refresh_display ();
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PROG_CUR, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			if (node_id == Entry_LVL_OUT)
			{
				chg_master_vol (-1);
				ret_val = EvtProp_CATCH;
			}
			else if (node_id == Entry_CHN)
			{
				chg_chn_mode (-1);
				ret_val = EvtProp_CATCH;
			}
			break;
		case Button_R:
			if (node_id == Entry_LVL_OUT)
			{
				chg_master_vol (+1);
				ret_val = EvtProp_CATCH;
			}
			else if (node_id == Entry_CHN)
			{
				chg_chn_mode (+1);
				ret_val = EvtProp_CATCH;
			}
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	Levels::do_set_chn_mode (ChnMode mode)
{
	fstb::unused (mode);

	refresh_display ();
}



void	Levels::do_set_master_vol (float vol)
{
	fstb::unused (vol);

	refresh_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Levels::refresh_display ()
{
	MeterResultSet &  meters = _model_ptr->use_meters ();
	for (int dir = 0; dir < Dir_NBR_ELT; ++dir)
	{
		for (int chn = 0; chn < Cst::_nbr_chn_inout; ++chn)
		{
			const auto &   src = meters._audio_io [dir]._chn_arr [chn];
			auto &         dst = _dir_arr [dir]._chn_arr [chn];

			dst._lvl_peak   = src._peak;
			dst._lvl_rms    = src._rms;
			dst._clip_flag |= (src._peak > Cst::_clip_lvl);
		}
	}

	const doc::Setup &   setup = _view_ptr->use_setup ();

	const double   master_vol  = std::max (setup._master_vol, 1e-10); // Just in case the volume is 0.
	const double   vol_db      =
		fstb::limit (20 * log10 (master_vol), -99.9, +99.9);
	char           txt_0 [255+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%+5.1f dB", vol_db);
	_lvl_sptr->set_text (txt_0);

	int            nbr_chn_in   = 1;
	int            nbr_pins_in  = 1;
	int            nbr_chn_out  = 1;
	int            nbr_pins_out = 1;
	switch (setup._chn_mode)
	{
	case ChnMode_1M_1M:
		nbr_chn_out = 1;
		break;
	case ChnMode_1M_1S:
		nbr_chn_out = 2;
		break;
	case ChnMode_1S_1S:
		nbr_chn_in  = 2;
		nbr_chn_out = 2;
		break;
	default:
		assert (false);
		break;
	}
	std::string    txt;
	txt  = conv_nbr_chn_to_str (nbr_pins_in , nbr_chn_in );
	txt += "\xE2\x86\x92"; // U+2192 rightwards arrow
	txt += conv_nbr_chn_to_str (nbr_pins_out, nbr_chn_out);
	_chn_sptr->set_text (txt);

	draw_audio_meter (_dir_arr [Dir_IN ], 2);
	draw_audio_meter (_dir_arr [Dir_OUT], nbr_chn_out);
	draw_dsp_meter (meters._dsp_use);

	const float    cpu_percent = meters._dsp_use._rms * 100;
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%5.1f", cpu_percent);
	_dsp_val_sptr->set_text (txt_0);
}



std::string	Levels::conv_nbr_chn_to_str (int nbr_pins, int nbr_chn)
{
	std::string    str;

	for (int p = 0; p < nbr_pins; ++p)
	{
		str += (nbr_chn == 1) ? "\xE2\x9A\xAC" : "\xE2\x9A\xAD";
	}

	return str;
}



void	Levels::draw_audio_meter (const DirInfo &info, int nbr_chn)
{
	assert (nbr_chn > 0);

	uint8_t *      buf_ptr   = info._vum_sptr->use_buffer ();
	const int      stride    = info._vum_sptr->get_stride ();
	grap::RenderCtx   ctx (
		buf_ptr,
		Vec2d (_meter_audio_w, _meter_audio_h + _meter_grad_h),
		stride
	);
#if (PV_VERSION == 2)
	{
		grap::PrimBox::draw_filled (
			ctx,
			0, 0,
			_meter_audio_w, _meter_audio_h + _meter_grad_h,
			0
		);
		int            y0 = 0;
		for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
		{
			const int      y1 = _meter_audio_h * (chn_cnt + 1) / nbr_chn;
			const int      h  = y1 - y0;
			grap::PrimBox::draw_outline (
				ctx,
				0, y0,
				_clip_audio_x - _scale, h,
				255
			);
			grap::PrimBox::draw_outline (
				ctx,
				_clip_audio_x - _scale, y0,
				_meter_audio_w - _clip_audio_x + _scale, h,
				255
			);
			y0 = y1;
		}
		for (int g_cnt = 0; g_cnt < DirInfo::_nbr_grad; ++g_cnt)
		{
			const int      db  = DirInfo::_grad_db_arr [g_cnt];
			const float    lvl = float (pow (10, double (db) / 20));
			const int      x   = fstb::round_int (conv_level_to_pix (lvl));
			grap::PrimLine::draw_v (
				ctx, x, _meter_audio_h, _meter_audio_h + _scale, 255, false
			);
		}
	}
#else
	int            nbr_pix   = _meter_audio_w * _meter_audio_h;
	const uint8_t* meter_ptr =
		  (nbr_chn == 1)
		? _pic_meter_mono
		: _pic_meter_stereo;
	for (int p = 0; p < nbr_pix; ++p)
	{
		buf_ptr [p] = meter_ptr [p] * 255;
	}
	{
		int            ofs = nbr_pix;
		nbr_pix = _meter_audio_w * _meter_grad_h;
		for (int p = 0; p < nbr_pix; ++p)
		{
			buf_ptr [ofs + p] = _pic_meter_grad [p] * 255;
		}
	}
#endif

	assert (nbr_chn <= 2);
	const int      h = (_meter_audio_h + (nbr_chn - 1)) / nbr_chn - 2 * _scale;
	const int      mid_gap  = _meter_audio_h - h * nbr_chn - 2 * _scale;
	const int      ofs_pk_r = (_scale + 1) >> 1;
	const int      ofs_pk_l = -ofs_pk_r;
#if (PV_VERSION == 2)
	const int      max_x    = _clip_audio_x - _scale * 2;
#else // PV_VERSION
	const int      max_x    = _clip_audio_x - _scale;
#endif // PV_VERSION
	for (int chn = 0; chn < nbr_chn; ++chn)
	{
		const DirInfo::ChnInfo &   chn_info = info._chn_arr [chn];
		const int      yb     = _scale + chn * (h + mid_gap);
		const float    peak_f = conv_level_to_pix (chn_info._lvl_peak);
		const float    rms_f  = conv_level_to_pix (chn_info._lvl_rms);
		const int      x_peak = fstb::round_int (peak_f);
		const int      x_rms  = std::min (fstb::round_int (rms_f), max_x);

		if (x_rms > _scale)
		{
			grap::PrimBox::draw_filled (
				ctx,
				_scale, yb,
				x_rms - _scale, h,
				255
			);
		}
		const int      x_pk_l = std::max (x_peak + ofs_pk_l, int (_scale));
		const int      x_pk_r = std::min (x_peak + ofs_pk_r, max_x);
		if (x_pk_l < x_pk_r)
		{
			grap::PrimBox::draw_filled (
				ctx,
				x_pk_l, yb,
				x_pk_r - x_pk_l, h,
				255
			);
		}
#if (PV_VERSION == 2)
		const int      x_rms_clip = std::max (x_rms, int (_scale));
		if (x_rms_clip < x_pk_l)
		{
			grap::PrimBox::draw_filled (
				ctx,
				x_rms_clip, yb,
				x_pk_l - x_rms_clip, h,
				128
			);
		}
#endif // PV_VERSION
		if (chn_info._clip_flag)
		{
			grap::PrimBox::draw_filled (
				ctx,
				_clip_audio_x, yb,
				_meter_audio_w - _scale - _clip_audio_x, h,
				255
			);
		}
	}

	info._vum_sptr->invalidate_all ();
}



void	Levels::draw_dsp_meter (const MeterResult &meter)
{
	uint8_t *      buf_ptr = _dsp_sptr->use_buffer ();
	const int      stride  = _dsp_sptr->get_stride ();
#if (PV_VERSION == 2)
	grap::RenderCtx   ctx (
		buf_ptr,
		Vec2d (_meter_dsp_w, _meter_dsp_h),
		stride
	);
	grap::PrimBox::draw_filled (
		ctx,
		0, 0,
		_meter_dsp_w, _meter_dsp_h,
		0
	);
	grap::PrimBox::draw_outline (
		ctx,
		0, 0,
		_meter_dsp_w, _meter_dsp_h,
		255
	);
#else // PV_VERSION
	const int      nbr_pix = _meter_dsp_w * _meter_dsp_h;
	for (int p = 0; p < nbr_pix; ++p)
	{
		buf_ptr [p] = _pic_meter_dsp [p] * 255;
	}
#endif // PV_VERSION
	const Vec2d    size  = _dsp_sptr->get_bounding_box ().get_size ();
	const int      bar_w = size [0] - 2 * _scale;
	int            pl   = fstb::floor_int (meter._rms * float (bar_w));
	int            pr   = fstb::ceil_int (meter._peak * float (bar_w));
	pl = fstb::limit (pl,      1, bar_w - 1);
	pr = fstb::limit (pr, pl + 1, bar_w    );

#if (PV_VERSION == 2)
	grap::PrimBox::draw_filled (
		ctx,
		_scale, _scale,
		pl, _meter_dsp_h - _scale * 2,
		192
	);
	grap::PrimBox::draw_filled (
		ctx,
		_scale + pl, _scale,
		pr - pl, _meter_dsp_h - _scale * 2,
		255
	);
#else // PV_VERSION
	for (int y = _scale; y < size [1] - _scale; ++y)
	{
		const int      ofs = y * stride;
		for (int x = _scale + (y & 1); x < pl; x += 2)
		{
			buf_ptr [ofs + x] = 255;
		}
		for (int x = pl; x < pr; ++x)
		{
			buf_ptr [ofs + x] = 255;
		}
	}
#endif // PV_VERSION

	_dsp_sptr->invalidate_all ();
}



float	Levels::conv_level_to_pix (float lvl) const
{
#if (PV_VERSION == 2)
	const float    db_min = -100;
#else // PV_VERSION
	const float    db_min = -80;
#endif // PV_VERSION
	const float    db = 20 * log10f (std::max (lvl, 1e-10f));
	// -80...-20 -> 0...40
	// -20...0...? -> 40...104...124
	float          x = 0;
	if (db >= -20)
	{
		x = _meter_m20db + (db - -20.f) * (_meter_0db - _meter_m20db) / 20.f;
	}
	else if (db >= db_min)
	{
		x = (db - db_min) * _meter_m20db / (-20.f - db_min);
	}

	return x;
}



void	Levels::chg_chn_mode (int dir)
{
	const doc::Setup &   setup = _view_ptr->use_setup ();
	ChnMode        chn_mode = setup._chn_mode;
	chn_mode = static_cast <ChnMode> (
		(int (chn_mode) + ChnMode_NBR_ELT + dir) % ChnMode_NBR_ELT
	);
	_model_ptr->set_chn_mode (chn_mode);
}



void	Levels::chg_master_vol (int dir)
{
	const doc::Setup &   setup = _view_ptr->use_setup ();
	double               vol = setup._master_vol;
	vol *= pow (10, dir / 20.0);
	vol = fstb::limit (vol, pow (10, -99 / 20.0), pow (10, +99 / 20.0));

	// Fixes rounding errors to make 0 dB always exact
	if (fstb::is_eq (vol, 1.0, 1e-3))
	{
		vol = 1;
	}

	_model_ptr->set_master_vol (vol);
}



#if (PV_VERSION == 2)



const std::array <int, Levels::DirInfo::_nbr_grad>	Levels::DirInfo::_grad_db_arr =
{{
	-100, -90, -80, -70, -60, -50, -40, -30, -20, -12, -6, -3, 0, +3
}};



#else // PV_VERSION



const uint8_t	Levels::_pic_meter_grad [_meter_audio_w * _meter_grad_h] =
{
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,0,1,0,1,1,1,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,
	0,0,0,0,0,0,0,0,1,0,0,1,0,1,1,1,
	0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,
	0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,
	0,1,1,1,0,0,0,0,1,0,0,1,0,0,0,0,
	0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,
	0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,
	0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,
	1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,
	0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
	1,1,0,0,1,0,0,1,0,1,0,0,0,0,0,0,
	0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,
	0,0,0,0,0,0,0,1,1,0,0,0,1,0,1,0,
	1,0,0,0,0,0,0,1,1,0,0,1,0,1,1,0,
	0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,1,
	0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,1,
	0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,
	0,0,0,0,0,0,1,1,0,1,0,1,0,1,1,0,
	0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,
	1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
	0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,
	0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,
	0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,
	0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,
	0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,
	0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,
	0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,1,1,1,0,1,0,1,0,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0
};

const uint8_t	Levels::_pic_meter_mono [_meter_audio_w * _meter_audio_h] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

const uint8_t	Levels::_pic_meter_stereo [_meter_audio_w * _meter_audio_h] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
	1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

const uint8_t	Levels::_pic_meter_dsp [_meter_dsp_w * _meter_dsp_h] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1
};



#endif // PV_VERSION



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
