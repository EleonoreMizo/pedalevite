/*****************************************************************************

        BidiClass.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_BidiClass_HEADER_INCLUDED)
#define	fstb_txt_unicode_BidiClass_HEADER_INCLUDED

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



enum BidiClass
{

	BidiClass_L,	// Left-to-Right
	BidiClass_LRE,	// Left-to-Right Embedding
	BidiClass_LRO,	// Left-to-Right Override
	BidiClass_R,	// Right-to-Left
	BidiClass_AL,	// Right-to-Left Arabic
	BidiClass_RLE,	// Right-to-Left Embedding
	BidiClass_RLO,	// Right-to-Left Override
	BidiClass_PDF,	// Pop Directional Format
	BidiClass_EN,	// European Number
	BidiClass_ES,	// European Number Separator
	BidiClass_ET,	// European Number Terminator
	BidiClass_AN,	// Arabic Number
	BidiClass_CS,	// Common Number Separator
	BidiClass_NSM,	// Non-Spacing Mark
	BidiClass_BN,	// Boundary Neutral
	BidiClass_B,	// Paragraph Separator
	BidiClass_S,	// Segment Separator
	BidiClass_WS,	// Whitespace
	BidiClass_ON,	// Other Neutrals

	BidiClass_NBR_ELT

};	// class BidiClass



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/unicode/BidiClass.hpp"



#endif	// fstb_txt_unicode_BidiClass_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
