/*****************************************************************************

        Categ.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_Categ_HEADER_INCLUDED)
#define	fstb_txt_unicode_Categ_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/CategClass.h"



namespace fstb
{
namespace txt
{
namespace unicode
{



enum Categ
{
	Categ_BITS	= 4,	// Private use, reserved

	Categ_LETTER_UPPERCASE           = CategClass_LETTER      << Categ_BITS,
	Categ_LETTER_LOWERCASE,
	Categ_LETTER_TITLECASE,
	Categ_LETTER_MODIFIER,
	Categ_LETTER_OTHER,

	Categ_MARK_NONSPACING            = CategClass_MARK        << Categ_BITS,
	Categ_MARK_SPACING_COMBINING,
	Categ_MARK_ENCLOSING,

	Categ_NUMBER_DECIMAL_DIGIT       = CategClass_NUMBER      << Categ_BITS,
	Categ_NUMBER_LETTER,
	Categ_NUMBER_OTHER,

	Categ_PUNCTUATION_CONNECTOR      = CategClass_PUNCTUATION << Categ_BITS,
	Categ_PUNCTUATION_DASH,
	Categ_PUNCTUATION_OPEN,
	Categ_PUNCTUATION_CLOSE,
	Categ_PUNCTUATION_INITIAL_QUOTE,
	Categ_PUNCTUATION_FINAL_QUOTE,
	Categ_PUNCTUATION_OTHER,

	Categ_SYMBOL_MATH                = CategClass_SYMBOL      << Categ_BITS,
	Categ_SYMBOL_CURRENCY,
	Categ_SYMBOL_MODIFIER,
	Categ_SYMBOL_OTHER,

	Categ_SEPARATOR_SPACE            = CategClass_SEPARATOR   << Categ_BITS,
	Categ_SEPARATOR_LINE,
	Categ_SEPARATOR_PARAGRAPH,

	Categ_OTHER_CONTROL              = CategClass_OTHER       << Categ_BITS,
	Categ_OTHER_FORMAT,
	Categ_OTHER_SURROGATE,
	Categ_OTHER_PRIVATE_USE,
	Categ_OTHER_NOT_ASSIGNED

};	// enum Categ



inline CategClass	Categ_conv_to_class (Categ categ);



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#include	"fstb/txt/unicode/Categ.hpp"



#endif	// fstb_txt_unicode_Categ_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
