/*****************************************************************************

        Char16.h
        Author: Laurent de Soras, 2019

16-bit characters. Byte order is platform-specific, encoding is context-
specific.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_txt_Char16_HEADER_INCLUDED)
#define fstb_txt_Char16_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cwchar>



namespace fstb
{
namespace txt
{



#if (WCHAR_MAX == 65535)
	#define fstb_txt_CHAR16_IS_WCHAR_T
	typedef wchar_t Char16;
#else
	#undef  fstb_txt_CHAR16_IS_WCHAR_T
	typedef char16_t Char16;
#endif


}  // namespace txt
}  // namespace fstb



//#include "fstb/txt/Char16.hpp"



#endif   // fstb_txt_Char16_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
