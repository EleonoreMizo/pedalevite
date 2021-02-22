/*****************************************************************************

        utf16_fnc.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_txt_utf16_utf16_fnc_HEADER_INCLUDED)
#define fstb_txt_utf16_utf16_fnc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/Char16.h"



namespace fstb
{
namespace txt
{
namespace utf16
{



int	strstr_ci (const Char16 str_0 [], const Char16 subset_0 [], long &subset_pos);



}  // namespace utf16
}  // namespace txt
}  // namespace fstb



//#include "fstb/txt/utf16/utf16_fnc.hpp"



#endif   // fstb_txt_utf16_utf16_fnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
