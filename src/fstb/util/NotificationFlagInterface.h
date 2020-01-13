/*****************************************************************************

        NotificationFlagInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_NotificationFlagInterface_HEADER_INCLUDED)
#define fstb_util_NotificationFlagInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/util/ObserverInterface.h"



namespace fstb
{
namespace util
{



class NotificationFlagInterface
:	public ObserverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline bool		operator () () const;
	inline bool		operator () (bool autoreset_flag);

	inline bool		get_state () const;
	inline void		reset ();
	inline void		set ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual bool	do_get_state () const = 0;
	virtual void	do_reset () = 0;
	virtual void	do_set () = 0;



}; // class NotificationFlagInterface



}  // namespace util
}  // namespace fstb



#include "fstb/util/NotificationFlagInterface.hpp"



#endif   // fstb_util_NotificationFlagInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
