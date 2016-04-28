/*****************************************************************************

        NotificationFlagCascadeMixin.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_util_NotificationFlagCascadeMixin_CODEHEADER_INCLUDED)
#define fstb_util_NotificationFlagCascadeMixin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{
namespace util
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NotificationFlagCascadeMixin::NotificationFlagCascadeMixin (bool state_flag)
:	_state_flag (state_flag)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NotificationFlagCascadeMixin::do_update (ObservableInterface &subject)
{
	set ();
}



bool	NotificationFlagCascadeMixin::do_get_state () const
{
	return (_state_flag);
}



void	NotificationFlagCascadeMixin::do_reset ()
{
	_state_flag = false;
}



void	NotificationFlagCascadeMixin::do_set ()
{
	_state_flag = true;
	notify_observers ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace util
}  // namespace fstb



#endif   // fstb_util_NotificationFlagCascadeMixin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
