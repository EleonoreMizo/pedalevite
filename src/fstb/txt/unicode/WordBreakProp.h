/*****************************************************************************

        WordBreakProp.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_WordBreakProp_HEADER_INCLUDED)
#define	fstb_txt_unicode_WordBreakProp_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{
namespace txt
{
namespace unicode
{



enum WordBreakProp
{

	WordBreakProp_OTHER = 0,
	WordBreakProp_CR,
	WordBreakProp_LF,
	WordBreakProp_NEWLINE,
	WordBreakProp_EXTEND,
	WordBreakProp_FORMAT,
	WordBreakProp_KATAKANA,
	WordBreakProp_ALETTER,
	WordBreakProp_MIDLETTER,
	WordBreakProp_MIDNUM,
	WordBreakProp_MIDNUMLET,
	WordBreakProp_NUMERIC,
	WordBreakProp_EXTENDNUMLET,

	WordBreakProp_NBR_ELT

};	// enum WordBreakProp



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/unicode/WordBreakProp.hpp"



#endif	// fstb_txt_unicode_WordBreakProp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
