/*****************************************************************************

        FileIOInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_FileIOInterface_HEADER_INCLUDED)
#define mfx_FileIOInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



namespace mfx
{



class FileIOInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Err
	{
		Err_OK = 0,

		Err_GENERIC_IO = -999,
		Err_CANNOT_OPEN,
		Err_WRITE_ERROR,
		Err_READ_ERROR
	};

	virtual        ~FileIOInterface () = default;

	int            set_fs_write_mode (bool write_flag);
	int            write_txt_file (const std::string &pathname, const std::string &content);
	int            read_txt_file (const std::string &pathname, std::string &content);

	// Shared utility functions
	static int     write_txt_file_direct (const std::string &pathname, const std::string &content);
	static int     read_txt_file_direct (const std::string &pathname, std::string &content);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_write_txt_file (const std::string &pathname, const std::string &content) = 0;
	virtual int    do_read_txt_file (const std::string &pathname, std::string &content) = 0;


}; // class FileIOInterface



}  // namespace mfx



//#include "mfx/FileIOInterface.hpp"



#endif   // mfx_FileIOInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
