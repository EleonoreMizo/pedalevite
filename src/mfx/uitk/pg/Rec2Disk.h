/*****************************************************************************

        Rec2Disk.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_Rec2Disk_HEADER_INCLUDED)
#define mfx_uitk_pg_Rec2Disk_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <cstdint>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class Rec2Disk
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Rec2Disk (PageSwitcher &page_switcher);
	virtual        ~Rec2Disk () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_REC = 0,
		Entry_MAX_DUR,
		Entry_AVAIL
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           update_display ();
	void           update_rec_time ();
	void           toggle_rec (int node_id);
	void           change_limit (int dir);
	std::string    build_rec_pathname () const;
	std::string    print_duration (int minutes) const;
	int            conv_frames_to_min (int64_t nbr_frames) const;
	int            conv_bytes_to_min (int64_t nbr_bytes) const;

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	TxtSPtr        _rec_sptr;
	TxtSPtr        _max_dur_sptr;
	TxtSPtr        _avail_sptr;

	int            _time_limit;   // Minutes
	int            _disk_avail;   // Minutes. 0 = unknown
	int            _prev_refresh; // Previous ten of second of the current date, for the refresh of the display. -1: not fetched yet

	Question::QArg _msg_arg;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Rec2Disk ()                               = delete;
	               Rec2Disk (const Rec2Disk &other)          = delete;
	Rec2Disk &     operator = (const Rec2Disk &other)        = delete;
	bool           operator == (const Rec2Disk &other) const = delete;
	bool           operator != (const Rec2Disk &other) const = delete;

}; // class Rec2Disk



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/Rec2Disk.hpp"



#endif   // mfx_uitk_pg_Rec2Disk_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
