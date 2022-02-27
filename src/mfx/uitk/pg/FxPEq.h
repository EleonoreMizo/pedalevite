/*****************************************************************************

        FxPEq.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_FxPEq_HEADER_INCLUDED)
#define mfx_uitk_pg_FxPEq_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/peq/BandParam.h"
#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

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



class FxPEq final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       FxPEq (PageSwitcher &page_switcher, LocEdit &loc_edit);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_preset (int index) final;
	void           do_set_param (int slot_id, int index, float val, PiType type) final;
	void           do_remove_plugin (int slot_id) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_steps = 4;

	enum
	{
		Entry_PARAM = 0,
		Entry_CONTENT,
		Entry_PREC,
		Entry_LEGEND_BASE
	};

	enum Param
	{
		Param_RANGE = 0,
		Param_GAIN,
		Param_BAND_BASE
	};

	class Biq
	{
	public:
		typedef std::array <float, 3> Poly;
		Poly           _b {{ 1, 0, 0 }};
		Poly           _a {{ 1, 0, 0 }};
	};

	typedef std::shared_ptr <NBitmap> BitmapSPtr;
	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	void           update_display ();
	void           update_param_txt ();
	void           update_band_info ();
	std::vector <pi::peq::BandParam>
	               create_bands (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc) const;
	std::vector <Biq>
	               retrieve_z_eq (const std::vector <pi::peq::BandParam> &band_arr) const;
	std::vector <float>
	               create_freq_map (int nbr_freq, float f_beg, float f_end) const;
	void           compute_freq_resp (std::vector <float> &lvl_arr, const std::vector <float> &puls_arr, const Biq &biq) const;
	std::vector <int32_t>
	               compute_y_pos (const std::vector <float> &lvl_arr, int pix_h) const;
	void           display_graduations (float f_beg, float f_end, int nbr_freq);
	int            conv_freq_to_x (float f, float f_beg, float f_end, int nbr_freq) const;
	int            conv_pitch_to_x (float note, float f_beg, float f_end, int nbr_freq) const;
	int            conv_db_to_y (float db, int pix_h) const;
	void           move_param (int dir);
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
	int            _nbr_bands;
	int            _cur_band;     // -1: not on a band
	int            _range_db_idx; // Position within _range_db_arr, must be valid
	int            _prec_idx;     // Precision for modifying parameters, [0 ; 3]
	doc::PluginSettings           // Cached settings
	               _settings;

	static const std::array <double, 6> // Y range in dB (from -r to +r)
	               _range_db_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FxPEq ()                               = delete;
	               FxPEq (const FxPEq &other)             = delete;
	               FxPEq (FxPEq &&other)                  = delete;
	FxPEq &        operator = (const FxPEq &other)        = delete;
	FxPEq &        operator = (FxPEq &&other)             = delete;
	bool           operator == (const FxPEq &other) const = delete;
	bool           operator != (const FxPEq &other) const = delete;

}; // class FxPEq



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/FxPEq.hpp"



#endif   // mfx_uitk_pg_FxPEq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
