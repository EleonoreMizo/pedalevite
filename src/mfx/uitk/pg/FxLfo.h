/*****************************************************************************

        FxLfo.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_FxLfo_HEADER_INCLUDED)
#define mfx_uitk_pg_FxLfo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/lfo1/Lfo.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/PluginPoolHostMini.h"

#include <array>
#include <memory>
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



class FxLfo final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       FxLfo (PageSwitcher &page_switcher, LocEdit &loc_edit);



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

	static const int  _nbr_steps     =    4;
	static const int  _proc_blk_size =    4;
	static const int  _sample_freq   = 1000;  // To keep the speed param at a valid value

	enum
	{
		Entry_PARAM = 0,
		Entry_CONTENT,
		Entry_PREC
	};

	enum Param
	{
		Param_NBR_CYCLES = 0,
		Param_BASE
	};

	typedef std::shared_ptr <NBitmap> BitmapSPtr;
	typedef std::shared_ptr <NText> TxtSPtr;

	void           update_display ();
	void           update_param_txt ();
	void           setup_lfo (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int nbr_steps, float len_cycles);
	float          get_param_nat (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int index) const;
	std::vector <int32_t>
	               build_values (int nbr_steps, int h);
	void           display_scales (float len_cycles);
	int            conv_val_to_y (float val, int pix_h) const;
	void           change_param (int dir);

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

	int            _cur_param;    // A Param enum value
	int            _nbr_param;
	int            _nbr_cycles_idx;  // Position within _nbr_cycles_arr, must be valid
	int            _prec_idx;     // Precision for modifying parameters, [0 ; 3]

	PluginPoolHostMini
	               _host_dummy;
	pi::lfo1::Lfo <false>
	               _lfo;
	std::vector <piapi::EventTs>
	               _evt_list;

	static const std::array <float, 5> // Number of displayed cycles (can be fractional)
	               _nbr_cycles_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FxLfo ()                               = delete;
	               FxLfo (const FxLfo &other)             = delete;
	               FxLfo (FxLfo &&other)                  = delete;
	FxLfo &        operator = (const FxLfo &other)        = delete;
	FxLfo &        operator = (FxLfo &&other)             = delete;
	bool           operator == (const FxLfo &other) const = delete;
	bool           operator != (const FxLfo &other) const = delete;

}; // class FxLfo



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/FxLfo.hpp"



#endif   // mfx_uitk_pg_FxLfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
