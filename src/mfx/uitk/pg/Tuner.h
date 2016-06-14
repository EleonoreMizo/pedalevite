/*****************************************************************************

        Tuner.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_Tuner_HEADER_INCLUDED)
#define mfx_uitk_pg_Tuner_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <memory>



namespace mfx
{

namespace ui
{
	class LedInterface;
}

namespace uitk
{
namespace pg
{



class Tuner
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Tuner (ui::LedInterface &led);
	virtual        ~Tuner () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, ContainerInterface &page, Vec2d page_size, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_set_tuner_freq (float freq);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_led = 3;

	enum Scale
	{
		Scale_CHROMA = 0,
		Scale_GUITAR_EBGDAE,
		Scale_BASS_GDAE,

		Scale_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           i_set_freq (float freq);

	static float   find_closest_note (float note, Scale sc);

	ui::LedInterface &
	               _led;
	const int      _leg_beg;
	const View *   _view_ptr;     // 0 = not set
	ContainerInterface *          // 0 = not set
	               _page_ptr;
	Vec2d          _page_size;
	Scale          _scale;

	TxtSPtr        _note_sptr;
	TxtSPtr        _scale_sptr;

	const ui::Font *
	               _fnt_l_ptr;

	static const char * const
	               _note_0_arr [12];
	static const std::array <std::vector <float>, Scale_NBR_ELT>
	               _scale_data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Tuner ()                               = delete;
	               Tuner (const Tuner &other)             = delete;
	Tuner &        operator = (const Tuner &other)        = delete;
	bool           operator == (const Tuner &other) const = delete;
	bool           operator != (const Tuner &other) const = delete;

}; // class Tuner



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/Tuner.hpp"



#endif   // mfx_uitk_pg_Tuner_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
