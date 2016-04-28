/*****************************************************************************

        LineBreakProp.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_LineBreakProp_HEADER_INCLUDED)
#define	fstb_txt_unicode_LineBreakProp_HEADER_INCLUDED

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



enum LineBreakProp
{

	LineBreakProp_OP = 0,	// open
	LineBreakProp_CL,	// close
	LineBreakProp_QU,	// quotation
	LineBreakProp_GL,	// glue
	LineBreakProp_NS,	// no-start
	LineBreakProp_EX,	// exclamation/interrogation
	LineBreakProp_SY,	// Syntax (slash)
	LineBreakProp_IS,	// infix (numeric) separator
	LineBreakProp_PR,	// prefix
	LineBreakProp_PO,	// postfix
	LineBreakProp_NU,	// numeric
	LineBreakProp_AL,	// alphabetic
	LineBreakProp_ID,	// ideograph (atomic)
	LineBreakProp_IN,	// inseparable
	LineBreakProp_HY,	// hyphen
	LineBreakProp_BA,	// break after
	LineBreakProp_BB,	// break before
	LineBreakProp_B2,	// break both
	LineBreakProp_ZW,	// ZW space
	LineBreakProp_CM,	// combining mark
	LineBreakProp_WJ,	// word joiner

	// used for Korean Syllable Block pair table
	LineBreakProp_H2,	// Hamgul 2 Jamo Syllable
	LineBreakProp_H3,	// Hangul 3 Jamo Syllable
	LineBreakProp_JL,	// Jamo leading consonant
	LineBreakProp_JV,	// Jamo vowel
	LineBreakProp_JT,	// Jamo trailing consonant

	LineBreakProp_NBR_ELT_TABLE,

	// these are not handled in the pair tables
	LineBreakProp_SA = LineBreakProp_NBR_ELT_TABLE,	// South (East) Asian
	LineBreakProp_SP,	// space
	LineBreakProp_PS,	// paragraph and line separators
	LineBreakProp_BK,	// hard break (newline)
	LineBreakProp_CR,	// carriage return
	LineBreakProp_LF,	// line feed
	LineBreakProp_NL,	// next line
	LineBreakProp_CB,	// contingent break opportunity
	LineBreakProp_SG,	// surrogate
	LineBreakProp_AI,	// ambiguous
	LineBreakProp_XX,	// unknown

	LineBreakProp_NBR_ELT

};	// enum LineBreakProp



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/unicode/LineBreakProp.hpp"



#endif	// fstb_txt_unicode_LineBreakProp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
