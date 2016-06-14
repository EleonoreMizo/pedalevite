/*****************************************************************************

        NodeEvt.cpp
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

#include "mfx/uitk/NodeEvt.h"

#include <utility>

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NodeEvt	NodeEvt::create_button (int target, Button but)
{
	assert (but >= 0);
	assert (but < Button_NBR_ELT);

	NodeEvt        tmp (target, Type_BUTTON);
	tmp._val_arr [0] = but;

	return std::move (tmp);
}



NodeEvt	NodeEvt::create_rotenc (int target, int rotenc, int val)
{
	assert (rotenc >= 0);
	assert (val != 0);

	NodeEvt        tmp (target, Type_ROTENC);
	tmp._val_arr [0] = rotenc;
	tmp._val_arr [1] = val;

	return std::move (tmp);
}



NodeEvt	NodeEvt::create_cursor (int target, Curs curs)
{
	assert (curs >= 0);
	assert (curs < Curs_NBR_ELT);

	NodeEvt        tmp (target, Type_CURSOR);
	tmp._val_arr [0] = curs;

	return std::move (tmp);
}



NodeEvt	NodeEvt::create_timer (int target)
{
	NodeEvt        tmp (target, Type_TIMER);

	return std::move (tmp);
}



int	NodeEvt::get_target () const
{
	return _target_id;
}



NodeEvt::Type	NodeEvt::get_type () const
{
	return _type;
}



Button	NodeEvt::get_button () const
{
	assert (_type == Type_BUTTON);

	return static_cast <Button> (_val_arr [0]);
}



bool	NodeEvt::is_button_ex () const
{
	return (_type == Type_BUTTON || _type == Type_ROTENC);
}



Button	NodeEvt::get_button_ex () const
{
	Button         but;
	if (_type == Type_ROTENC)
	{
		const int      r = get_rotenc ();
		int            v = get_rotenc_val ();
		v = (v > 0) ? 1 : 0;
		switch ((r << 1) + v)
		{
		case (0 << 1) + 0: but = Button_U; break;
		case (0 << 1) + 1: but = Button_D; break;
		case (1 << 1) + 0: but = Button_L; break;
		case (1 << 1) + 1: but = Button_R; break;
		}
	}
	else
	{
		but = get_button ();
	}

	return but;
}



int	NodeEvt::get_rotenc () const
{
	assert (_type == Type_ROTENC);

	return _val_arr [0];
}



int	NodeEvt::get_rotenc_val () const
{
	assert (_type == Type_ROTENC);

	return _val_arr [1];
}



NodeEvt::Curs	NodeEvt::get_cursor () const
{
	assert (_type == Type_CURSOR);

	return static_cast <Curs> (_val_arr [0]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NodeEvt::NodeEvt (int target, Type type)
:	_target_id (target)
,	_type (type)
,	_val_arr ({{ 0, 0 }})
{
	assert (target >= 0);
	assert (type >= 0);
	assert (type < Type_NBR_ELT);
}



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
