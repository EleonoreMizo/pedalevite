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
#include "mfx/uitk/pg/Levels.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/MeterResultSet.h"
#include "mfx/Model.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Levels::Levels (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_dir_arr ()
,	_chn_sptr (    TxtSPtr (   new NText (  Entry_CHN)))
,	_dsp_txt_sptr (TxtSPtr (   new NText (  Entry_DSP_TXT)))
,	_dsp_sptr (    BitmapSPtr (new NBitmap (Entry_DSP)))
{
	_dir_arr [Dir_IN ]._lvl_sptr = TxtSPtr (   new NText (  Entry_LVL_IN));
	_dir_arr [Dir_IN ]._vum_sptr = BitmapSPtr (new NBitmap (Entry_VU_IN));
	_dir_arr [Dir_OUT]._lvl_sptr = TxtSPtr (   new NText (  Entry_LVL_OUT));
	_dir_arr [Dir_OUT]._vum_sptr = BitmapSPtr (new NBitmap (Entry_VU_OUT));

	_dir_arr [Dir_IN ]._lvl_sptr->set_text ("Input");
	_dir_arr [Dir_IN ]._vum_sptr->set_size (Vec2d (
		_meter_audio_w,
		_meter_audio_h + _meter_grad_h
	));
	_dir_arr [Dir_OUT]._lvl_sptr->set_text ("Output");
	_dir_arr [Dir_OUT]._vum_sptr->set_size (Vec2d (
		_meter_audio_w,
		_meter_audio_h + _meter_grad_h
	));
	_chn_sptr->set_justification (0, 1, false);
	_dsp_txt_sptr->set_text ("DSP");
	_dsp_txt_sptr->set_justification (1, 1, false);
	_dsp_sptr->set_size (Vec2d (_meter_dsp_w, _meter_dsp_h));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Levels::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	const int      h_m   = fnt_m.get_char_h ();

	_dir_arr [Dir_IN ]._lvl_sptr->set_font (fnt_m);
	_dir_arr [Dir_OUT]._lvl_sptr->set_font (fnt_m);
	_chn_sptr->set_font (fnt_m);
	_dsp_txt_sptr->set_font (fnt_m);

	_dir_arr [Dir_IN ]._lvl_sptr->set_coord (Vec2d (0,  0));
	_dir_arr [Dir_IN ]._vum_sptr->set_coord (Vec2d (0,  0 + h_m));
	_dir_arr [Dir_OUT]._lvl_sptr->set_coord (Vec2d (0, 28));
	_dir_arr [Dir_OUT]._vum_sptr->set_coord (Vec2d (0, 28 + h_m));
	_chn_sptr->set_coord (Vec2d (0, _page_size [1]));
	_dsp_txt_sptr->set_coord (_page_size - Vec2d (_meter_dsp_w, 0));
	_dsp_sptr->set_coord (_page_size - Vec2d (_meter_dsp_w, _meter_dsp_h));

	_page_ptr->push_back (_dir_arr [Dir_IN ]._lvl_sptr);
	_page_ptr->push_back (_dir_arr [Dir_IN ]._vum_sptr);
	_page_ptr->push_back (_dir_arr [Dir_OUT]._lvl_sptr);
	_page_ptr->push_back (_dir_arr [Dir_OUT]._vum_sptr);
	_page_ptr->push_back (_chn_sptr);
	_page_ptr->push_back (_dsp_txt_sptr);
	_page_ptr->push_back (_dsp_sptr);

	PageMgrInterface::NavLocList  nav_list (3);
	nav_list [0]._node_id = Entry_LVL_IN;
	nav_list [1]._node_id = Entry_LVL_OUT;
	nav_list [2]._node_id = Entry_CHN;
	page.set_nav_layout (nav_list);
	page.jump_to (nav_list [0]._node_id);

	_page_ptr->set_timer (0, true);

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

	if (evt.is_cursor ())
	{

		/*** To do ***/

	}

	else if (evt.is_timer ())
	{
		refresh_display ();
		ret_val = EvtProp_CATCH;
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_E:
			_page_switcher.switch_to (pg::PageType_CUR_PROG, 0);
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

	const int      nbr_chn_out = 2; /*** To do: check the real number of channels ***/
	draw_audio_meter (_dir_arr [Dir_IN ], 2);
	draw_audio_meter (_dir_arr [Dir_OUT], nbr_chn_out);
	draw_dsp_meter (meters._dsp_use);
	_chn_sptr->set_text (               // U+26AC and U+26AD
		(nbr_chn_out == 1) ? "\xE2\x9A\xAC" : "\xE2\x9A\xAD"
	);
}



void	Levels::draw_audio_meter (const DirInfo &info, int nbr_chn)
{
	assert (nbr_chn > 0);

	uint8_t *      buf_ptr   = info._vum_sptr->use_buffer ();
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
			const int      ofs = (yb + y) * size [0];
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
		const int      ofs = y * size [0];
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
