/*****************************************************************************

        Codec16.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_utf16_Codec16_CODEHEADER_INCLUDED)
#define fstb_txt_utf16_Codec16_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/utf16/def.h"
#include "fstb/Err.h"

#include <cassert>



namespace fstb
{
namespace txt
{
namespace utf16
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class C16>
bool	Codec16 <C16>::is_valid_utf16_lead_word (C16 w)
{
	return ((w & 0xFC00) != 0xDC00);
}



template <class C16>
bool	Codec16 <C16>::is_valid_utf16_end_word (C16 w)
{
	return ((w & 0xFC00) == 0xDC00);
}



template <class C16>
bool	Codec16 <C16>::is_char_single_word (C16 w)
{
	return ((w & 0xF800) != 0xD800);
}



// Sets len to 0 if the character is invalid.
template <class C16>
int	Codec16 <C16>::get_char_seq_len_ucs (int &len, char32_t ucs4)
{
	int            ret_val = Err_OK;

	len = 1;
	if ((ucs4 & 0xF800) == 0xD800 || ucs4 > 0x10FFFFL)
	{
		len     = 0;
		ret_val = Err_CHAR_OUT_OF_RANGE;
	}
	else if (ucs4 >= 0x10000L)
	{
		len = 2;
	}

#if 0 // char32_t is unsigned
	if (ucs4 < 0)
	{
		len     = 0;
		ret_val = Err_CHAR_OUT_OF_RANGE;
	}
#endif

	return ret_val;
}



template <class C16>
int	Codec16 <C16>::get_char_seq_len_utf (int &len, C16 utf16_lead_word)
{
	int            ret_val = Err_OK;

	len = 1;
	if (! is_valid_utf16_lead_word (utf16_lead_word))
	{
		ret_val = Err_INVALID_UTF16_LEAD_WORD;
	}
	else if (! is_char_single_word (utf16_lead_word))
	{
		len = 2;
	}

	return ret_val;
}



template <class C16>
int	Codec16 <C16>::encode_char (std::basic_string <C16> &utf16, char32_t ucs4)
{
	C16            temp_0 [MAX_WORD_SEQ_LEN+1];
	const int      ret_val = encode_char (temp_0, ucs4);
	if (ret_val == Err_OK)
	{
		utf16 = temp_0;
	}

	return ret_val;
}



template <class C16>
int	Codec16 <C16>::encode_char (C16 utf16_0 [], char32_t ucs4)
{
	assert (utf16_0 != 0);

	int            len = 0;
	const int      ret_val = encode_char (utf16_0, ucs4, len);
	assert (   (ret_val == Err_OK && len > 0)
	        || (ret_val != Err_OK && len == 0));
	utf16_0 [len] = 0;

	return ret_val;
}



template <class C16>
int	Codec16 <C16>::encode_char (C16 utf16_ptr [], char32_t ucs4, int &len)
{
	assert (utf16_ptr != 0);

	int            ret_val = get_char_seq_len_ucs (len, ucs4);
	if (ret_val == Err_OK)
	{
		if (len == 1)
		{
			utf16_ptr [0] = static_cast <C16> (ucs4);
		}

		else
		{
			const int      a = int (((ucs4 >> 10) - 0x0040) & 0x03FF);
			const int      b = int (  ucs4                  & 0x03FF);
			utf16_ptr [0] = static_cast <C16> (a | 0xD800);
			utf16_ptr [1] = static_cast <C16> (b | 0xDC00);
		}
	}

	return ret_val;
}



template <class C16>
int	Codec16 <C16>::decode_char (char32_t &ucs4, const C16 utf16_ptr [])
{
	assert (utf16_ptr != 0);

	int            dummy_len;

	return decode_char (ucs4, utf16_ptr, dummy_len);
}



template <class C16>
int	Codec16 <C16>::decode_char (char32_t &ucs4, const C16 utf16_ptr [], int &len)
{
	assert (utf16_ptr != 0);

	const C16      beg = utf16_ptr [0];
	int            ret_val = get_char_seq_len_utf (len, beg);
	if (ret_val == Err_OK)
	{
		if (len == 1)
		{
			ucs4 = beg;
		}

		else
		{
			const C16      end = utf16_ptr [1];
			if (! is_valid_utf16_end_word (end))
			{
				ret_val = Err_INVALID_UTF16_END_WORD;
			}
			else
			{
				ucs4  = char32_t ((beg & 0x03FF) + 0x0040) << 10;
				ucs4 += char32_t ( end & 0x03FF          );
			}
		}
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace utf16
}  // namespace txt
}  // namespace fstb



#endif   // fstb_txt_utf16_Codec16_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
