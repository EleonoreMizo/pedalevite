/*****************************************************************************

        UniqueRscLinux.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_UniqueRscLinux_HEADER_INCLUDED)
#define mfx_hw_UniqueRscLinux_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <stdexcept>



namespace mfx
{
namespace hw
{



class UniqueRscLinux
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Error
	:	public std::runtime_error
	{
		using runtime_error::runtime_error;
	};

	explicit       UniqueRscLinux (std::string name);
	virtual        ~UniqueRscLinux ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::string    _name;
	int            _lock_fd = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               UniqueRscLinux (const UniqueRscLinux &other)    = delete;
	               UniqueRscLinux (UniqueRscLinux &&other)         = delete;
	UniqueRscLinux &
	               operator = (const UniqueRscLinux &other)        = delete;
	UniqueRscLinux &
	               operator = (UniqueRscLinux &&other)             = delete;
	bool           operator == (const UniqueRscLinux &other) const = delete;
	bool           operator != (const UniqueRscLinux &other) const = delete;

}; // class UniqueRscLinux



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/UniqueRscLinux.hpp"



#endif   // mfx_hw_UniqueRscLinux_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

