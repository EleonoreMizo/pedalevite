/*****************************************************************************

        ConvCi.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_unicode_ConvCi_CURRENT_CODEHEADER)
	#error Recursive inclusion of ConvCi code header.
#endif
#define	fstb_txt_unicode_ConvCi_CURRENT_CODEHEADER

#if ! defined (fstb_txt_unicode_ConvCi_CODEHEADER_INCLUDED)
#define	fstb_txt_unicode_ConvCi_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/CharData.h"



namespace fstb
{
namespace txt
{
namespace unicode
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



char32_t	ConvCi::convert (char32_t c)
{
	return (CharData::to_upper (c));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_unicode_ConvCi_CODEHEADER_INCLUDED

#undef fstb_txt_unicode_ConvCi_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
