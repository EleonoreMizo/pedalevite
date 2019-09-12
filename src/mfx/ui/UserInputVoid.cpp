/*****************************************************************************

        UserInputVoid.cpp
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

#include "mfx/ui/UserInputVoid.h"
#include "mfx/Cst.h"

#include <cassert>
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <ctime>
#endif



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



UserInputVoid::UserInputVoid ()
:	_recip_list ()
{
	for (int i = 0; i < UserInputType_NBR_ELT; ++i)
	{
		const int      nbr_dev =
			do_get_nbr_param (static_cast <UserInputType> (i));
		_recip_list [i].resize (nbr_dev, 0);
	}
}



void	UserInputVoid::send_message (std::chrono::microseconds date, UserInputType type, int index, float val)
{
	// The cell well be lost but we don't care, this is for debugging.
	conc::LockFreeCell <UserInputMsg> * cell_ptr =
		new conc::LockFreeCell <UserInputMsg>;
	cell_ptr->_next_ptr = 0;
	cell_ptr->_val.set (date, type, index, val);
	_recip_list [type] [index]->enqueue (*cell_ptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	UserInputVoid::do_get_nbr_param (UserInputType /*type*/) const
{
	return Cst::_max_input_param;
}



void	UserInputVoid::do_set_msg_recipient (UserInputType type, int index, MsgQueue * queue_ptr)
{
	_recip_list [type] [index] = queue_ptr;
}



void	UserInputVoid::do_return_cell (MsgCell &/*cell*/)
{
	// Nothing
}



std::chrono::microseconds	UserInputVoid::do_get_cur_date () const
{
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	const auto     ns     = std::chrono::nanoseconds (
		int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec
	);

	return std::chrono::duration_cast <std::chrono::microseconds> (ns);

#else

	return std::chrono::duration_cast <std::chrono::microseconds> (
		_clk.now ().time_since_epoch ()
	);

#endif
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
