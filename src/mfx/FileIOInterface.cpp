/*****************************************************************************

        FileIOInterface.cpp
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

#include "mfx/FileIOInterface.h"

#include <cassert>
#include <cstdio>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	FileIOInterface::write_txt_file (const std::string &pathname, const std::string &content)
{
	assert (! pathname.empty ());

	return do_write_txt_file (pathname, content);
}



int	FileIOInterface::read_txt_file (const std::string &pathname, std::string &content)
{
	assert (! pathname.empty ());

	return do_read_txt_file (pathname, content);
}



int	FileIOInterface::write_txt_file_direct (const std::string &pathname, const std::string &content)
{
	int            ret_val = Err_OK;

	FILE *         f_ptr = 0;
	if (ret_val == Err_OK)
	{
		f_ptr = fopen (pathname.c_str (), "w");
		if (f_ptr == 0)
		{
			ret_val = Err_CANNOT_OPEN;
		}
	}

	if (ret_val == Err_OK)
	{
		const size_t   s = fwrite (content.c_str (), 1, content.size (), f_ptr);
		if (s != content.size ())
		{
			ret_val = Err_WRITE_ERROR;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	return ret_val;
}



int	FileIOInterface::read_txt_file_direct (const std::string &pathname, std::string &content)
{
	int            ret_val = Err_OK;

	content.clear ();

	FILE *         f_ptr = 0;
	if (ret_val == Err_OK)
	{
		f_ptr = fopen (pathname.c_str (), "r");
		if (f_ptr == 0)
		{
			ret_val = Err_CANNOT_OPEN;
		}
	}

	if (ret_val == Err_OK)
	{
		int				c;
		do
		{
			c = fgetc (f_ptr);
			if (c != EOF)
			{
				content += c;
			}
		}
		while (c != EOF);

		if (ferror (f_ptr) != 0)
		{
			ret_val = Err_READ_ERROR;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
