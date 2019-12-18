/*****************************************************************************

        Conv.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_Conv_CODEHEADER_INCLUDED)
#define fstb_txt_Conv_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/utf16/Codec16.h"
#include "fstb/txt/utf16/def.h"
#include "fstb/txt/utf8/Codec8.h"
#include "fstb/txt/utf8/def.h"

#include <cassert>



namespace fstb
{
namespace txt
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: conv_utf8_to_utf16
Description:
	Converts a string from UTF-8 to UTF-16.
Input parameters:
	- utf8_0: String in UTF-8
Output parameters:
	- utf16: String in UTF-16
Returns: Error code
Throws: Nothing
==============================================================================
*/

template <class C16>
int	Conv::utf8_to_utf16 (std::basic_string <C16> &utf16, const char utf8_0 [])
{
	assert (utf8_0 != 0);

	int            ret_val = Err_OK;

	try
	{
		utf16.erase (utf16.begin (), utf16.end ());

		size_t         pos = 0;
		while (utf8_0 [pos] != '\0' && ret_val == Err_OK)
		{
			char32_t       c;
			int            char_len;
			ret_val = utf8::Codec8::decode_char (c, &utf8_0 [pos], char_len);
			if (ret_val == Err_OK)
			{
				C16            tmp_0 [utf16::MAX_WORD_SEQ_LEN + 1];
				ret_val = utf16::Codec16 <C16>::encode_char (tmp_0, c);
				assert (ret_val == Err_OK);
				utf16 += tmp_0;
				pos   += char_len;
			}
		}
	}

	catch (...)
	{
		ret_val = Err_EXCEPTION;
		assert (false);
	}

	return ret_val;
}



/*
==============================================================================
Name: conv_utf16_to_utf8
Description:
	Converts a string from UTF-16 to UTF-8.
Input parameters:
	- utf16_0: String in UTF-16
Output parameters:
	- utf8: String in UTF-8
Returns: Error code
Throws: Nothing
==============================================================================
*/

template <class C16>
int	Conv::utf16_to_utf8 (std::string &utf8, const C16 utf16_0 [])
{
	assert (utf16_0 != 0);

	int            ret_val = Err_OK;

	try
	{
		utf8.erase (utf8.begin (), utf8.end ());

		size_t         pos = 0;
		while (utf16_0 [pos] != 0 && ret_val == Err_OK)
		{
			char32_t       c;
			int            char_len;
			ret_val = utf16::Codec16 <C16>::decode_char (c, &utf16_0 [pos], char_len);
			if (ret_val == Err_OK)
			{
				char        tmp_0 [utf8::MAX_BYTE_SEQ_LEN + 1];
				ret_val = utf8::Codec8::encode_char (tmp_0, c);
				assert (ret_val == Err_OK);
				utf8 += tmp_0;
				pos  += char_len;
			}
		}
	}

	catch (...)
	{
		ret_val = Err_EXCEPTION;
		assert (false);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace txt
}  // namespace fstb



#endif   // fstb_txt_Conv_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
