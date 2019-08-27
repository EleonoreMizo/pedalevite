/*****************************************************************************

        fnc.h
        Author: Laurent de Soras, 2007

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_txt_utf8_utf8_HEADER_INCLUDED)
#define fstb_txt_utf8_utf8_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>



namespace fstb
{
namespace txt
{
namespace utf8
{



bool	strncpy_0 (char dest_0 [], const char src_0 [], std::string::size_type buf_len_byte);
bool	strncat_0 (char dest_0 [], const char src_0 [], std::string::size_type buf_len_byte);
bool	truncate_0 (char str_0 [], std::string::size_type buf_len_byte = std::string::npos);
bool	truncate (std::string & str, std::string::size_type buf_len_byte = std::string::npos);

int	conv_to_lower_case (std::string &dest, const char src_0 [], const std::string::size_type max_sz = std::string::npos);
int	conv_to_upper_case (std::string &dest, const char src_0 [], const std::string::size_type max_sz = std::string::npos);

int	strstr_ci (const char str_0 [], const char subset_0 [], long &subset_pos);



}  // namespace utf8
}  // namespace txt
}  // namespace fstb



//#include "fstb/txt/utf8/fnc.hpp"



#endif   // fstb_txt_utf8_utf8_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
