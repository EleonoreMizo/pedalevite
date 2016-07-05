/*****************************************************************************

        CmdLine.cpp
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

#include "mfx/CmdLine.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CmdLine::set (int argc, char *argv [], char *envp [])
{
	assert (argc > 0);
	assert (argv != 0);
	assert (argv [argc - 1] != 0);
	assert (argv [argc    ] == 0);

	_argc = argc;
	_argv = argv;
	if (envp == 0)
	{
		static char *  n_ptr = 0;
		_envp = &n_ptr;
	}
	else
	{
		_envp = envp;
	}
}



int	CmdLine::get_argc () const
{
	assert (_argc > 0);

	return (_argc);
}



char * const *	CmdLine::use_argv () const
{
	assert (_argv != 0);

	return (_argv);
}



char * const *	CmdLine::use_envp () const
{
	assert (_envp != 0);

	return (_envp);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
