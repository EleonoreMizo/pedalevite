/*****************************************************************************

        UniqueRscLinux.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/UniqueRscLinux.h"

#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Name should be a valid filename without any path.
UniqueRscLinux::UniqueRscLinux (std::string name)
:	_name (name)
,	_pathname ("/run/lock/" + _name)
,	_lock_fd (-1)
{
	/*** To do: better check ***/
	assert (! name.empty ());
	assert (name.find ('/') == std::string::npos);

	_lock_fd = open (_pathname.c_str (), O_CREAT | O_EXCL);
	if (_lock_fd < 0)
	{
		throw Error (
			"Resource \"" + name + "\" is already in use "
			"(" + _pathname + " locked)."
		);
	}
}



UniqueRscLinux::~UniqueRscLinux ()
{
	if (_lock_fd >= 0)
	{
		close (_lock_fd);
		_lock_fd = -1;
		remove (_pathname.c_str ());
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
