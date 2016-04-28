/*****************************************************************************

        CharDataInfo.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_CharDataInfo_HEADER_INCLUDED)
#define	fstb_txt_unicode_CharDataInfo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace fstb
{
namespace txt
{
namespace unicode
{



class CharDataInfo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline unsigned long
	               compute_internal_sort_code1 () const;
	inline unsigned long
	               compute_internal_sort_code2 () const;

	int32_t        _upper;                       // UnicodeData.txt, column 12. Code point of the upper case equivalent, or 0 if none
	int32_t        _lower;                       // UnicodeData.txt, column 13. Code point of the lower case equivalent, or 0 if none
	int32_t        _title;                       // UnicodeData.txt, column 14. Code point of the title case equivalent, or 0 if none
	uint8_t        _digit;                       // UnicodeData.txt, column 6. Decimal digit, or 0 if none
	uint8_t        _categ;                       // UnicodeData.txt, column 2. General Category (Categ type)
	uint8_t        _kangxi_radical_stroke_cnt;   // (Currently ignored, always 0) Unihan.txt, kRSUnicode field, first number. KangXi Radical Stroke Count
	uint8_t        _residual_stroke_cnt;         // (Currently ignored, always 0) Unihan.txt, kRSUnicode field, second number. Residual Stroke Count
	uint8_t        _grapheme_break_prop;         // auxiliary/GraphemeBreakProperty.txt. Information for Grapheme Cluster Break (GraphemeBreakProp type)
	uint8_t        _line_break_prop;             // LineBreak.txt. Information for line break detection (LineBreakProp type)
	uint8_t        _word_break_prop;             // WordBreakProperty.txt. Information for word boudnary detection (WordBreakProp type)
	uint8_t        _bidi_class;                  // UnicodeData.txt, column 4. Bidi Class (BidiClass type)



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

};	// class CharDataInfo



static inline bool	operator < (const CharDataInfo &lhs, const CharDataInfo &rhs);



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#include	"fstb/txt/unicode/CharDataInfo.hpp"



#endif	// fstb_txt_unicode_CharDataInfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
