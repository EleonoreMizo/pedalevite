/*****************************************************************************

        ThreadLinux.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_ThreadLinux_HEADER_INCLUDED)
#define mfx_hw_ThreadLinux_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <thread>

#include <cstdio>



namespace mfx
{
namespace hw
{



class ThreadLinux
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     set_priority (std::thread &thrd, int prio_below_max, FILE *err_msg_ptr);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ThreadLinux ()                               = delete;
	               ThreadLinux (const ThreadLinux &other)       = delete;
	virtual        ~ThreadLinux ()                              = delete;
	ThreadLinux &  operator = (const ThreadLinux &other)        = delete;
	bool           operator == (const ThreadLinux &other) const = delete;
	bool           operator != (const ThreadLinux &other) const = delete;

}; // class ThreadLinux



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/ThreadLinux.hpp"



#endif   // mfx_hw_ThreadLinux_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
