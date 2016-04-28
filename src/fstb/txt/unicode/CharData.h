/*****************************************************************************

        CharData.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_CharData_HEADER_INCLUDED)
#define	fstb_txt_unicode_CharData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/unicode/BidiClass.h"
#include "fstb/txt/unicode/Categ.h"
#include "fstb/txt/unicode/CharDataInfo.h"
#include "fstb/txt/unicode/GraphemeBreakProp.h"
#include "fstb/txt/unicode/LineBreakAction.h"
#include "fstb/txt/unicode/LineBreakProp.h"
#include "fstb/txt/unicode/WordBreakProp.h"
#include "fstb/txt/unicode/WordBreakPropTableNext.h"
#include "fstb/txt/unicode/WordBreakPropTablePrev.h"

#include <cstddef>
#include <cstdint>



namespace fstb
{
namespace txt
{
namespace unicode
{



class CharData
{

	friend class CharDataBuilder;

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	size_t	FindCplxBreakFnc (LineBreakAction action_arr [], const LineBreakProp prop_arr [], LineBreakProp cls, size_t len);

	enum {			MAX_NBR_CODE_POINTS	= 0x110000	};

	static Categ	get_categ (char32_t c);
	static char32_t
	               to_upper (char32_t c);
	static char32_t
	               to_lower (char32_t c);
	static char32_t
	               to_title (char32_t c);
	static GraphemeBreakProp
						get_grapheme_break_prop (char32_t c);
	static LineBreakProp
						get_line_break_prop (char32_t c);
	static WordBreakProp
						get_word_break_prop (char32_t c);
	static BidiClass
						get_bidi_class (char32_t c);

	static bool		is_grapheme_cluster_boundary (char32_t c_prev, char32_t c_next);
	static size_t	find_line_breaks (LineBreakAction action_arr [], const LineBreakProp prop_arr [], size_t len, FindCplxBreakFnc &find_complex_line_breaks = CharData::find_complex_line_breaks_default);
	static void		find_word_breaks (uint8_t action_arr [], const WordBreakProp prop_arr [], size_t len);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			PAGE_SIZE_L2	= 8	};
	enum {			DIRECT_L2		= 15	};	// Bit must fit in an archi::UInt16

	static const CharDataInfo &
						use_info (char32_t c);
	static size_t	find_complex_line_breaks_default (LineBreakAction action_arr [], const LineBreakProp prop_arr [], LineBreakProp cls, size_t len);

	static const LineBreakAction
						_table_break_line [LineBreakProp_NBR_ELT_TABLE] [LineBreakProp_NBR_ELT_TABLE];

	// Maximum size: 0x8000 elements
	// First element always describes an unassigned character.
	static const CharDataInfo
						_info_unique_list [];

	// List of indexes in _info_unique_list
	// Maximum size: 0x110000 elements (more likely something like 0x30000)
	static const int16_t
						_index_info_list [];

	// Input is (character code point) >> PAGE_SIZE_L2
	// Bit DIRECT_L2 clear: Index >> PAGE_SIZE_L2 for blocks in _index_info_list
	// Bit DIRECT_L2 set  : Index in _info_unique_list (for uniform blocks)
	// Maximum size: 0x1100 elements
	static const uint16_t
						_index_page_list [MAX_NBR_CODE_POINTS >> PAGE_SIZE_L2];

	static const bool
						_table_break_grapheme [GraphemeBreakProp_NBR_ELT] [GraphemeBreakProp_NBR_ELT];

	static const bool
						_table_break_word [WordBreakPropTablePrev_NBR_ELT] [WordBreakPropTableNext_NBR_ELT];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						CharData ();
						CharData (const CharData &other);
	CharData &		operator = (const CharData &other);
	bool				operator == (const CharData &other);
	bool				operator != (const CharData &other);

};	// class CharData



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/unicode/CharData.hpp"



#endif	// fstb_txt_unicode_CharData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
