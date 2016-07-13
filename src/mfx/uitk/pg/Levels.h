/*****************************************************************************

        Levels.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_Levels_HEADER_INCLUDED)
#define mfx_uitk_pg_Levels_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/Cst.h"
#include "mfx/Dir.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{

class MeterResult;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class Levels
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Levels (PageSwitcher &page_switcher);
	virtual        ~Levels () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_set_chn_mode (ChnMode mode);
	virtual void   do_set_master_vol (float vol);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_LAB_IN = 1000,
		Entry_VUM_IN,
		Entry_LAB_OUT,
		Entry_LVL_OUT,
		Entry_VUM_OUT,
		Entry_CHN,
		Entry_DSP_TXT,
		Entry_DSP
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NBitmap> BitmapSPtr;

	class DirInfo
	{
	public:
		class ChnInfo
		{
		public:
			float          _lvl_rms   = 0;
			float          _lvl_peak  = 0;
			bool           _clip_flag = false;
		};
		TxtSPtr        _lab_sptr;
		BitmapSPtr     _vum_sptr;
		std::array <ChnInfo, Cst::_nbr_chn_inout>
		               _chn_arr;
	};

	void           refresh_display ();
	void           draw_audio_meter (const DirInfo &info, int nbr_chn);
	void           draw_dsp_meter (const MeterResult &meter);
	float          conv_level_to_pix (float lvl) const;
	void           chg_chn_mode (int dir);
	void           chg_master_vol (int dir);

	static std::string
	                conv_nbr_chn_to_str (int nbr_pins, int nbr_chn);

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;

	std::array <DirInfo, Dir_NBR_ELT>
	               _dir_arr;
	TxtSPtr        _lvl_sptr;
	TxtSPtr        _chn_sptr;
	TxtSPtr        _dsp_txt_sptr;
	BitmapSPtr     _dsp_sptr;

	static const int  _meter_audio_w  = 128;
	static const int  _meter_grad_h   = 7;
	static const int  _meter_audio_h  = 9;
	static const int  _meter_dsp_w    = 86;
	static const int  _meter_dsp_h    = 9;
	static const int  _clip_audio_x   = 125;

	static const uint8_t
	               _pic_meter_grad [_meter_audio_w * _meter_grad_h];
	static const uint8_t
	               _pic_meter_mono [_meter_audio_w * _meter_audio_h];
	static const uint8_t
	               _pic_meter_stereo [_meter_audio_w * _meter_audio_h];
	static const uint8_t
	               _pic_meter_dsp [_meter_dsp_w * _meter_dsp_h];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Levels ()                               = delete;
	               Levels (const Levels &other)            = delete;
	Levels &       operator = (const Levels &other)        = delete;
	bool           operator == (const Levels &other) const = delete;
	bool           operator != (const Levels &other) const = delete;

}; // class Levels



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/Levels.hpp"



#endif   // mfx_uitk_pg_Levels_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
