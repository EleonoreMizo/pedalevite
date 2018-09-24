/*****************************************************************************

        CurProg.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_CurProg_HEADER_INCLUDED)
#define mfx_uitk_pg_CurProg_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <memory>
#include <set>



namespace mfx
{

class ControlSource;

namespace adrv
{
	class DriverInterface;
}

namespace uitk
{

class PageSwitcher;

namespace pg
{



class CurProg
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       CurProg (PageSwitcher &page_switcher, adrv::DriverInterface &snd_drv);
	virtual        ~CurProg () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_set_tempo (double bpm);
	virtual void   do_select_bank (int index);
	virtual void   do_set_bank_name (std::string name);
	virtual void   do_set_preset_name (std::string name);
	virtual void   do_activate_preset (int index);
	virtual void   do_set_param (int slot_id, int index, float val, PiType type);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_PROG_IDX = 0,
		Entry_PROG_NAME,
		Entry_BANK_IDX,
		Entry_BANK_NAME,
		Entry_FX_NAME,
		Entry_PARAM_UNIT,
		Entry_PARAM_NAME,
		Entry_PARAM_VAL,
		Entry_MOD_LIST,
		Entry_IP,

		Entry_NBR_ELT
	};

	static const int  _mag_prog_nbr = 2;

	typedef std::shared_ptr <NText> TxtSPtr;

	void           i_set_bank_nbr (int index);
	void           i_set_prog_nbr (int index);
	void           i_set_bank_name (std::string name);
	void           i_set_prog_name (std::string name);
	void           i_set_param (bool show_flag, int slot_id, int index, float val, PiType type);
	void           i_show_mod_list ();
	void           add_mod_source (std::set <ControlSource> &src_list, const ControlSource &src);

	static std::string
	               get_ip_address ();

	PageSwitcher & _page_switcher;
	adrv::DriverInterface &
	               _snd_drv;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	std::string    _ip_addr;

	TxtSPtr        _prog_nbr_sptr;
	TxtSPtr        _prog_name_sptr;
	TxtSPtr        _bank_nbr_sptr;
	TxtSPtr        _bank_name_sptr;
	TxtSPtr        _fx_name_sptr;
	TxtSPtr        _param_unit_sptr;
	TxtSPtr        _param_name_sptr;
	TxtSPtr        _param_val_sptr;
	TxtSPtr        _modlist_sptr;
	TxtSPtr        _ip_sptr;
	int            _size_max_bank_name;

	int            _bank_index;
	int            _preset_index;

	std::chrono::microseconds
	               _tempo_date;
	int            _esc_count;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               CurProg ()                               = delete;
	               CurProg (const CurProg &other)           = delete;
	CurProg &      operator = (const CurProg &other)        = delete;
	bool           operator == (const CurProg &other) const = delete;
	bool           operator != (const CurProg &other) const = delete;

}; // class CurProg



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/CurProg.hpp"



#endif   // mfx_uitk_pg_CurProg_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
