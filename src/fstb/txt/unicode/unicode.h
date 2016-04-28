/*****************************************************************************

		unicode.h
        Author: Laurent de Soras, 2009

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_unicode_unicode_fnc_CURRENT_HEADER)
	#error Recursive inclusion of fstb/txt/unicode/unicode_fnc header.
#endif
#define	fstb_txt_unicode_unicode_fnc_CURRENT_HEADER

#if ! defined (fstb_txt_unicode_unicode_fnc_HEADER_INCLUDED)
#define	fstb_txt_unicode_unicode_fnc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/StringU.h"

#include	<string>



namespace fstb
{
namespace txt
{
namespace unicode
{



int	conv_utf8_to_unicode (StringU & dest, const char src_0 []);
int	conv_unicode_to_utf8 (std::string & dest, const char32_t src_0 []);

template <class C>	// C = ConvCi or ConvNeutral
int	compare (const StringU &s1, const StringU &s2);



}	// namespace unicode
}	// namespace txt
}	// namespace fstb


/*\\\ CODE HEADER INCLUSION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/unicode/unicode.hpp"



#endif	// fstb_txt_unicode_unicode_fnc_HEADER_INCLUDED

#undef fstb_txt_unicode_unicode_fnc_CURRENT_HEADER



/*****************************************************************************

	LEGAL

	(c) 2002 fstb Force

	Use of this file or part of its content without written permission is
	strictly forbidden.
	For more information, please contact fstb Force at :
	contact@fstbforce.com

*****************************************************************************/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
