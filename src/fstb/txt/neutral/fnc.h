/*****************************************************************************

        fnc.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_neutral_fnc_HEADER_INCLUDED)
#define	fstb_txt_neutral_fnc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>

#include <cstdint>



namespace fstb
{
namespace txt
{
namespace neutral
{



inline char	change_case (char c, bool lower_case_flag);
std::string	to_lcase (const std::string &txt);
std::string	to_ucase (const std::string &txt);
void	to_lcase_inplace (std::string &txt);
void	to_lcase_inplace (char *txt_0);
void	to_ucase_inplace (std::string &txt);
void	to_ucase_inplace (char *txt_0);

std::string	trim_spaces (const char *txt_0);

long	strstr_ci (const char *s1_0, const char *s2_0);
bool	strcmp_ci (const char *s1_0, const char *s2_0);

bool	conv_str_to_int64 (int64_t &val, const char *txt_0, int base = 10, long *stop_pos_ptr = 0);



}	// namespace neutral
}	// namespace txt
}	// namespace fstb



#include	"fstb/txt/neutral/fnc.hpp"



#endif	// fstb_txt_neutral_fnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
