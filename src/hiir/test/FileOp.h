/*****************************************************************************

        FileOp.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_FileOp_HEADER_INCLUDED)
#define	hiir_test_FileOp_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{
namespace test
{



template <typename T>
class FileOp
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     save_raw_data (const char *filename_0, const T src_ptr [], long nbr_spl, int bits, float scale);
	static int     save_raw_data_stereo (const char *filename_0, const T src_0_ptr [], const T src_1_ptr [], long nbr_spl, int bits, float scale);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FileOp ()                             = delete;
	               FileOp (const FileOp <T> &other)      = delete;
	               FileOp (FileOp <T> &&other)           = delete;
	               ~FileOp ()                            = delete;
	FileOp <T> &   operator = (const FileOp <T> &other)  = delete;
	FileOp <T> &   operator = (FileOp <T> &&other)       = delete;
	bool           operator == (const FileOp <T> &other) = delete;
	bool           operator != (const FileOp <T> &other) = delete;

}; // class FileOp



}  // namespace test
}  // namespace hiir



#include "hiir/test/FileOp.hpp"



#endif   // hiir_test_FileOp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
