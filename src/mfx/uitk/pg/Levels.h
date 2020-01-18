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

namespace adrv
{
	class DriverInterface;
}

namespace uitk
{

class PageSwitcher;

namespace pg
{



class Levels final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Levels (PageSwitcher &page_switcher, adrv::DriverInterface &snd_drv);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_set_chn_mode (ChnMode mode) final;
	void           do_set_master_vol (float vol) final;



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
		Entry_DSP,
		Entry_DSP_VAL
#if (PV_VERSION == 2)
		, Entry_GRAD
#endif
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
#if (PV_VERSION == 2)
		static const int  _nbr_grad = 14;
		typedef std::array <TxtSPtr, _nbr_grad> GradArray;
		GradArray      _grad_sptr_arr;
		TxtSPtr        _clip_sptr;
		static const std::array <int, _nbr_grad>
		               _grad_db_arr;
#endif // PV_VERSION
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
	adrv::DriverInterface &
	               _snd_drv;
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
	TxtSPtr        _dsp_val_sptr;

#if (PV_VERSION == 2)
	static const int  _scale = 4;
	static const int  _meter_audio_w  = _scale * 200;
	static const int  _meter_m20db    = _scale * 128;
	static const int  _meter_0db      = _scale * (128 + 48);
	static const int  _meter_grad_h   = _scale * 1;
#else  // PV_VERSION
	static const int  _scale = 1;
	static const int  _meter_audio_w  = _scale * 128;
	static const int  _meter_m20db    = _scale * 40;
	static const int  _meter_0db      = _scale * (40 + 64);
	static const int  _meter_grad_h   = _scale * 7;
#endif // PV_VERSION

	static const int  _meter_audio_h  = _scale * 9;
	static const int  _meter_dsp_w    = _meter_audio_w - _scale * 42;
	static const int  _meter_dsp_h    = _scale * 9;
	static const int  _clip_audio_x   = _meter_audio_w - _scale * 3;

#if (PV_VERSION == 1)
	static const uint8_t
	               _pic_meter_grad [_meter_audio_w * _meter_grad_h];
	static const uint8_t
	               _pic_meter_mono [_meter_audio_w * _meter_audio_h];
	static const uint8_t
	               _pic_meter_stereo [_meter_audio_w * _meter_audio_h];
	static const uint8_t
	               _pic_meter_dsp [_meter_dsp_w * _meter_dsp_h];
#endif



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Levels ()                               = delete;
	               Levels (const Levels &other)            = delete;
	               Levels (Levels &&other)                 = delete;
	Levels &       operator = (const Levels &other)        = delete;
	Levels &       operator = (Levels &&other)             = delete;
	bool           operator == (const Levels &other) const = delete;
	bool           operator != (const Levels &other) const = delete;

}; // class Levels



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/Levels.hpp"



#endif   // mfx_uitk_pg_Levels_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
