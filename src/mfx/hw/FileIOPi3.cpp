/*****************************************************************************

        FileIOPi3.cpp
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

#include "mfx/ui/LedInterface.h"
#include "mfx/hw/FileIOPi3.h"
#include "mfx/Cst.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FileIOPi3::FileIOPi3 (ui::LedInterface &led)
:	_led (led)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	FileIOPi3::do_write_txt_file (const std::string &pathname, const std::string &content)
{
	_led.set_led (1, 1.0f);

	std::string    pathname_tmp ("/tmp");
	const size_t   last_delim = pathname.rfind ('/');
	if (last_delim == std::string::npos)
	{
		pathname_tmp += "/";
		pathname_tmp += pathname;
	}
	else
	{
		pathname_tmp += pathname.substr (last_delim);
	}

	const int      ret_val = write_txt_file_direct (pathname_tmp, content);
	if (ret_val == 0)
	{
		std::string    cmd = "sudo ";
		cmd += Cst::_rw_cmd_script_pathname;
		cmd += " ";
		cmd += "mv \'";
		cmd += pathname_tmp;
		cmd += "\' \'";
		cmd += pathname;
		cmd += "\'";
	
		system (cmd.c_str ());
	}

	_led.set_led (1, 0);

	return ret_val;
}



int	FileIOPi3::do_read_txt_file (const std::string &pathname, std::string &content)
{
	return read_txt_file_direct (pathname, content);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
