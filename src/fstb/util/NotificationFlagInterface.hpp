/*****************************************************************************

        NotificationFlagInterface.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_util_NotificationFlagInterface_CODEHEADER_INCLUDED)
#define fstb_util_NotificationFlagInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace fstb
{
namespace util
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	NotificationFlagInterface::operator () () const
{
	return (get_state ());
}



bool	NotificationFlagInterface::operator () (bool autoreset_flag)
{
	const bool		state_flag = get_state ();

	if (autoreset_flag && state_flag)
	{
		reset ();
	}

	return (state_flag);
}



bool	NotificationFlagInterface::get_state () const
{
	return (do_get_state ());
}



void	NotificationFlagInterface::reset ()
{
	do_reset ();
}



void	NotificationFlagInterface::set ()
{
	do_set ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace util
}  // namespace fstb



#endif   // fstb_util_NotificationFlagInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
