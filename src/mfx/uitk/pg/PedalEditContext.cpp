/*****************************************************************************

        PedalEditContext.cpp
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

#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalEditContext::use_layout (const View &view) const
{
	assert (_type >= 0);

	const doc::Setup &   setup = view.use_setup ();

	switch (_type)
	{
	case PedalEditContext::Type_GLOBAL:
		// Nothing
		break;

	case PedalEditContext::Type_BANK:
		{
			const int      bank_index = view.get_bank_index ();
			return setup._bank_arr [bank_index]._layout;
		}
		break;

	case PedalEditContext::Type_PRESET:
		return view.use_preset_cur ()._layout;
		break;

	default:
		assert (false);
		break;
	}

	return setup._layout;
}



PedalLoc	PedalEditContext::conv_to_loc (const View &view) const
{
	assert (_pedal >= 0);
	assert (_type >= 0);

	PedalLoc             loc;

	switch (_type)
	{
	case Type_GLOBAL:
		loc._type = PedalLoc::Type_GLOBAL;
		break;

	case Type_BANK:
		loc._type = PedalLoc::Type_BANK;
		loc._bank_index = view.get_bank_index ();
		break;

	case Type_PRESET:
		loc._type = PedalLoc::Type_PRESET_CUR;
		break;

	default:
		assert (false);
		break;
	}

	loc._pedal_index = _pedal;

	return loc;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
