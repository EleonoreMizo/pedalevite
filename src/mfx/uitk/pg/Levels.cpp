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

#include "fstb/fnc.h"
#include "mfx/adrv/DriverInterface.h"
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
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_dir_arr ()
,	_lvl_sptr (    TxtSPtr (   new NText (  Entry_LVL_OUT)))
,	_chn_sptr (    TxtSPtr (   new NText (  Entry_CHN)))
,	_dsp_txt_sptr (TxtSPtr (   new NText (  Entry_DSP_TXT)))
,	_dsp_sptr (    BitmapSPtr (new NBitmap (Entry_DSP)))
,	_dsp_val_sptr (TxtSPtr (   new NText (  Entry_DSP_VAL)))
{
	_dir_arr [Dir_IN ]._lab_sptr = TxtSPtr (   new NText (  Entry_LAB_IN));
	_dir_arr [Dir_IN ]._vum_sptr = BitmapSPtr (new NBitmap (Entry_VUM_IN));
	_dir_arr [Dir_OUT]._lab_sptr = TxtSPtr (   new NText (  Entry_LAB_OUT));
	_dir_arr [Dir_OUT]._vum_sptr = BitmapSPtr (new NBitmap (Entry_VUM_OUT));

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
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Levels::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
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

	_dir_arr [Dir_IN ]._lab_sptr->set_coord (Vec2d (0,  0));
	_dir_arr [Dir_IN ]._vum_sptr->set_coord (Vec2d (0,  0 + h_m));
	_dir_arr [Dir_OUT]._lab_sptr->set_coord (Vec2d (0, 28));
	_dir_arr [Dir_OUT]._vum_sptr->set_coord (Vec2d (0, 28 + h_m));
	_lvl_sptr->set_coord (Vec2d (_page_size [0], 28));
	_chn_sptr->set_coord (Vec2d (0, _page_size [1]));
	_dsp_txt_sptr->set_coord (_page_size - Vec2d (_meter_dsp_w, 0));
	_dsp_sptr->set_coord (_page_size - Vec2d (_meter_dsp_w, _meter_dsp_h));
	_dsp_val_sptr->set_coord (_page_size - Vec2d (1, 1));

	_page_ptr->push_back (_dir_arr [Dir_IN ]._lab_sptr);
	_page_ptr->push_back (_dir_arr [Dir_IN ]._vum_sptr);
	_page_ptr->push_back (_dir_arr [Dir_OUT]._lab_sptr);
	_page_ptr->push_back (_dir_arr [Dir_OUT]._vum_sptr);
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
			_page_switcher.switch_to (pg::PageType_CUR_PROG, 0);
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
	refresh_display ();
}



void	Levels::do_set_master_vol (float vol)
{
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

	const Vec2d    size  = info._vum_sptr->get_bounding_box ().get_size ();
	assert (nbr_chn <= 2);
	const int      h =
		  (nbr_chn == 1)
		? _meter_audio_h - 2
		: ((_meter_audio_h - 3) >> 1);
	for (int chn = 0; chn < nbr_chn; ++chn)
	{
		const DirInfo::ChnInfo &   chn_info = info._chn_arr [chn];
		const int      yb     = 1 + chn * (h + 1);
		const float    peak_f = conv_level_to_pix (chn_info._lvl_peak);
		const float    rms_f  = conv_level_to_pix (chn_info._lvl_rms);
		const int      x_peak = fstb::round_int (peak_f);
		const int      x_rms  = fstb::round_int (rms_f);

		for (int y = 0; y < h; ++y)
		{
			const int      ofs = (yb + y) * stride;
			for (int p = 0; p < x_rms; ++p)
			{
				buf_ptr [ofs + p] = 255;
			}

			for (int p = std::max (x_peak - 1, 0); p <= x_peak; ++p)
			{
				buf_ptr [ofs + p] = 255;
			}

			if (chn_info._clip_flag)
			{
				for (int p = _clip_audio_x; p < size [0] - 1; ++p)
				{
					buf_ptr [ofs + p] = 255;
				}
			}
		}
	}

	info._vum_sptr->invalidate_all ();
}



void	Levels::draw_dsp_meter (const MeterResult &meter)
{
	uint8_t *      buf_ptr = _dsp_sptr->use_buffer ();
	const int      stride  = _dsp_sptr->get_stride ();
	const int      nbr_pix = _meter_dsp_w * _meter_dsp_h;
	for (int p = 0; p < nbr_pix; ++p)
	{
		buf_ptr [p] = _pic_meter_dsp [p] * 255;
	}

	const Vec2d    size  = _dsp_sptr->get_bounding_box ().get_size ();
	const int      bar_w = size [0] - 2;
	int            pl   = fstb::floor_int (meter._rms * (bar_w - 1));
	int            pr   = fstb::ceil_int (meter._peak * (bar_w - 1));
	pr = std::max (pr, pl + 1);

	for (int y = 1; y < size [1] - 1; ++y)
	{
		const int      ofs = y * stride;
		for (int x = 1 + (y & 1); x < pl; x += 2)
		{
			buf_ptr [ofs + x] = 255;
		}
		for (int x = pl; x < pr; ++x)
		{
			buf_ptr [ofs + x] = 255;
		}
	}

	_dsp_sptr->invalidate_all ();
}



float	Levels::conv_level_to_pix (float lvl) const
{
	const float    db = 20 * log10 (std::max (lvl, 1e-10f));
	// -80...-20 -> 0...40
	// -20...0...? -> 40...104...124
	float          x = 0;
	if (db > -80 && db < -20)
	{
		x =      (db - -80.f) * (40 / (80.f - 20.f));
	}
	else
	{
		x = 40 + (db - -20.f) * (64 / 20.f);
	}
	x = fstb::limit (x, 0.f, float (_clip_audio_x - 1));

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
	_model_ptr->set_master_vol (vol);
}



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



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
