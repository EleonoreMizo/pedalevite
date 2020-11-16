/*****************************************************************************

        Stop.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_Stop_HEADER_INCLUDED)
#define mfx_Stop_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/CmdLine.h"

#include <atomic>



namespace mfx
{



class Stop
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	Stop () = default;

	enum class Type
	{
		NONE = 0,
		QUIT,
		RESTART,
		REBOOT,
		SHUTDOWN
	};

	CmdLine &      use_cmd_line ();
	void           request (Type type);
	bool           is_exit_requested () const;
	void           process_request ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	CmdLine        _cmd_line;
	std::atomic <Type>
	               _request { Type::NONE };



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Stop (const Stop &other)              = delete;
	               Stop (Stop &&other)                   = delete;
	Stop &         operator = (const Stop &other)        = delete;
	Stop &         operator = (Stop &&other)             = delete;
	bool           operator == (const Stop &other) const = delete;
	bool           operator != (const Stop &other) const = delete;

}; // class Stop



}  // namespace mfx



//#include "mfx/Stop.hpp"



#endif   // mfx_Stop_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

