/*****************************************************************************

        Stop.cpp
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

#include "fstb/def.h"
#include "mfx/Stop.h"

#if fstb_SYS == fstb_SYS_LINUX
	#include <unistd.h>
#endif

#include <cassert>
#if fstb_SYS == fstb_SYS_LINUX
	#include <cstdlib>
#endif



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CmdLine &	Stop::use_cmd_line () noexcept
{
	return _cmd_line;
}



void	Stop::request (Type type) noexcept
{
	_request.store (type);
}



bool	Stop::is_exit_requested () const noexcept
{
	return (_request.load () != Type::NONE);
}



void	Stop::process_request ()
{
#if (fstb_SYS == fstb_SYS_LINUX)

	const Type     r = _request.load ();
	switch (r)
	{
	case Type::NONE:
	case Type::QUIT:
		// Nothing
		break;
	case Type::RESTART:
		{
			char * const * argv = _cmd_line.use_argv ();
			char * const * envp = _cmd_line.use_envp ();
			execve (argv [0], argv, envp);
		}
		break;
	case Type::REBOOT:
		system ("sudo shutdown -r now");
		break;
	case Type::SHUTDOWN:
		system ("sudo shutdown -h now");
		break;
	}

#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
