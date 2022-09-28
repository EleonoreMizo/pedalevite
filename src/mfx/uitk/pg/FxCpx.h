/*****************************************************************************

        FxCpx.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_FxCpx_HEADER_INCLUDED)
#define mfx_uitk_pg_FxCpx_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/cpx/CompexGainFnc.h"
#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <vector>

#include <cstdint>



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class FxCpx final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       FxCpx (PageSwitcher &page_switcher, LocEdit &loc_edit);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_prog (int index) final;
	void           do_set_param (int slot_id, int index, float val, PiType type) final;
	void           do_remove_plugin (int slot_id) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _nbr_steps = 4;

	// -60 dB. Could be extended to -84 dB
	static constexpr float  _lvl_l2_min = -10;

	// +12 dB
	static constexpr float  _lvl_l2_max = +2;

	enum
	{
		Entry_PARAM = 0,
		Entry_CONTENT,
		Entry_PREC,
		Entry_LEGEND_BASE
	};

	enum Param
	{
		Param_BASE = 0 // No specific parameter
	};

	typedef std::shared_ptr <NBitmap> BitmapSPtr;
	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	void           update_display ();
	void           update_param_txt ();
	void           setup_gain_fnc (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc);
	std::vector <int32_t>
	               build_values (int nbr_steps, int h);
	void           display_scales ();
	void           change_param (int dir);

	static float   get_param (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int index);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_t_ptr;
	const ui::Font *              // 0 = not connected
	               _fnt_s_ptr;

	TxtSPtr        _cur_param_sptr;
	TxtSPtr        _prec_sptr;
	BitmapSPtr     _content_sptr;
	TxtArray       _legend_sptr_arr;

	int            _cur_param;    // A Param enum value
	int            _nbr_param;
	int            _prec_idx;     // Precision for modifying parameters, [0 ; 3]

	pi::cpx::CompexGainFnc
	               _gain_fnc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FxCpx ()                               = delete;
	               FxCpx (const FxCpx &other)             = delete;
	               FxCpx (FxCpx &&other)                  = delete;
	FxCpx &        operator = (const FxCpx &other)        = delete;
	FxCpx &        operator = (FxCpx &&other)             = delete;
	bool           operator == (const FxCpx &other) const = delete;
	bool           operator != (const FxCpx &other) const = delete;

}; // class FxCpx



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/FxCpx.hpp"



#endif   // mfx_uitk_pg_FxCpx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
