/*****************************************************************************

        unicode.cpp
        Author: Laurent de Soras, 2009

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/unicode.h"
#include	"fstb/txt/utf8/Codec8.h"
#include	"fstb/txt/utf8/def.h"
#include	"fstb/Err.h"

#include	<cassert>



namespace fstb
{
namespace txt
{
namespace unicode
{



/*
==============================================================================
Name: conv_utf8_to_unicode
Input parameters:
	- src_0: Utf8 source string, '\0'-terminated.
Output parameters:
	- dest: Unicode destination string.
Returns:
	0 if successful, otherwise one of Err
==============================================================================
*/

int	conv_utf8_to_unicode (std::u32string & dest, const char src_0 [])
{
	int            err = Err_OK;

	dest.clear ();
	size_t         i   = 0;
	while ((err == Err_OK) && (src_0 [i] != '\0'))
	{
		char32_t       ucs4_char = 0;
		int            len       = 0;

		err = utf8::Codec8::decode_char (ucs4_char, &src_0 [i], len);
		
		if (err == Err_OK)
		{
			i    += len;
			dest += ucs4_char;
		}
	}
	
	return err;
}



/*
==============================================================================
Name: conv_unicode_to_utf8
Input parameters:
	- src_0: Utf8 source string, '\0'-terminated.
Output parameters:
	- dest: Unicode destination string.
Returns:
	0 if successful, otherwise one of Err
==============================================================================
*/

int	conv_unicode_to_utf8 (std::string & dest, const char32_t src_0 [])
{
	int            err = Err_OK;

	dest.clear ();
	size_t         i = 0;
	while ((err == Err_OK) && (src_0 [i] != 0L))
	{
		char utf8_0 [utf8::MAX_BYTE_SEQ_LEN + 1];
	
		err = utf8::Codec8::encode_char (utf8_0, src_0 [i]);
		
		if (err == Err_OK)
		{
			++ i;
			dest += utf8_0;
		}
	}
	
	return err;
}



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
