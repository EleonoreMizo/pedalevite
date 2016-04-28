/*****************************************************************************

        Codec8.cpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/utf8/Codec8.h"
#include	"fstb/txt/utf8/def.h"
#include	"fstb/Err.h"

#include	<cassert>



namespace fstb
{
namespace txt
{
namespace utf8
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Codec8::is_valid_utf8_byte (char c)
{
	const int		val = static_cast <unsigned char> (c);

	return (val != 0xC0 && val != 0xC1 && val <= 0xF4);
}



bool	Codec8::is_valid_utf8_lead_byte (char c)
{
	const int		val = static_cast <unsigned char> (c);

	return (val < 0x80 || (val >= 0xC2 && val <= 0xF4));
}



bool	Codec8::is_valid_utf8_follow_byte (char c)
{
	return ((c & 0xC0) == 0x80);
}



bool	Codec8::is_char_single_byte (char c)
{
	return ((c & 0x80) == 0);
}



// Sets len to 0 if the character is invalid.
int	Codec8::get_char_seq_len_ucs (int &len, char32_t ucs4)
{
	assert (&len != 0);

	int				ret_val = Err_OK;

	len = 1;
	if (ucs4 >= 0x80)
	{
		if (ucs4 <= 0x7FF)
		{
			len = 2;
		}
		else if (ucs4 <= 0xFFFFL)
		{
			len = 3;
		}
		else if (ucs4 <= 0x10FFFFL)
		{
			len = 4;
		}
		else
		{
			len = 0;
			ret_val = Err_CHAR_OUT_OF_RANGE;
		}
	}

	if (ucs4 < 0)
	{
		len = 0;
		ret_val = Err_CHAR_OUT_OF_RANGE;
	}

	return (ret_val);
}



// Also checks the validity of this first byte
int	Codec8::get_char_seq_len_utf (int &len, char utf8_lead_byte)
{
	assert (&len != 0);

	int				ret_val = Err_OK;

	len = 1;

	const int		val = static_cast <unsigned char> (utf8_lead_byte);
	if (val >= 0x80)
	{
		if (val < 0xC2 || val > 0xF4)
		{
			ret_val = Err_INVALID_UTF8_LEAD_BYTE;
		}
		else if (val < 0xE0)
		{
			len = 2;
		}
		else if (val < 0xF0)
		{
			len = 3;
		}
		else if (val < 0xF8)
		{
			len = 4;
		}
		else if (val < 0xFC)
		{
			len = 5;
		}
		else
		{
			len = 6;
		}
	}

	return (ret_val);
}



/*
==============================================================================
Name: encode_char
Description: Encodes a UCS-4 character (31 bits) into an UTF-8 string, made
	of a sequence of 1 to 4 characters.
Input parameters:
	- ucs4: character in 31 bit encoding.
Output parameters:
	- utf8: string receiving the encoded character sequence. If a problem
		occured, the content of this string is not changed.
Returns:
	- true if character is valid
	- false if character is invalid
Throws: std::string related exception
==============================================================================
*/

int	Codec8::encode_char (std::string &utf8, char32_t ucs4)
{
	assert (&utf8 != 0);

	char				temp_0 [MAX_BYTE_SEQ_LEN+1];
	const int		ret_val = encode_char (temp_0, ucs4);
	if (ret_val == Err_OK)
	{
		utf8 = temp_0;
	}

	return (ret_val);
}



/*
==============================================================================
Name: encode_char
Description: Encodes a UCS character (0x0 - 0x10FFFF) into an UTF-8 null-
	terminated string, made of a sequence of 2 to 5 characters.
Input parameters:
	- ucs4: UCS character to encode, in the [0x0; 0x10FFFF] range.
Output parameters:
	- utf8_0: pointer on nul-terminated string receiving the encoded character
		sequence. If a problem occured, the string is made empty.
		Caller should reserve enough memory to encode the char, up to
		MAX_BYTE_SEQ_LEN+1 bytes.
Returns:
	- true if character is valid
	- false if character is invalid. The state 
Throws: Nothing
==============================================================================
*/

int	Codec8::encode_char (char utf8_0 [], char32_t ucs4)
{
	assert (utf8_0 != 0);

	int				len = 0;
	const int		ret_val = encode_char (utf8_0, ucs4, len);
	assert (   (ret_val == Err_OK && len > 0)
	        || (ret_val != Err_OK && len == 0));
	utf8_0 [len] = '\0';

	return (ret_val);
}



/*
==============================================================================
Name: encode_char
Description: Encodes a UCS-4 character (31 bits) into an UTF-8 string, made
	of a sequence of 1 to 4 characters.
Input parameters:
	- ucs4: UCS character to encode. Must be in the [0x000000; 0x10FFFF] range.
Output parameters:
	- utf8_ptr: pointer on the string receiving the encoded character sequence.
		If a problem occured, the content of this string is not affected. Caller
		should reserve enough memory to encode the char, here up to
		MAX_BYTE_SEQ_LEN bytes.
	- len: length of the encoded sequence, > 0 if all is ok, 0 if character
		is invalid.
Returns:
	- true if character is valid
	- false if character is invalid.
Throws: Nothing
==============================================================================
*/

int	Codec8::encode_char (char utf8_ptr [], char32_t ucs4, int &len)
{
	assert (utf8_ptr != 0);
	assert (&len != 0);

	int				ret_val = get_char_seq_len_ucs (len, ucs4);

	if (ret_val == Err_OK)
	{
		switch (len)
		{
		case	1:
			utf8_ptr [0] = static_cast <char> (ucs4);
			break;

		case	2:
			utf8_ptr [0] = static_cast <char> (((ucs4 >>  6) & 0x1F) | 0xC0);
			utf8_ptr [1] = static_cast <char> (( ucs4        & 0x3F) | 0x80);
			break;

		case	3:
			utf8_ptr [0] = static_cast <char> (((ucs4 >> 12) & 0x0F) | 0xE0);
			utf8_ptr [1] = static_cast <char> (((ucs4 >>  6) & 0x3F) | 0x80);
			utf8_ptr [2] = static_cast <char> (( ucs4        & 0x3F) | 0x80);
			break;

		case	4:
			utf8_ptr [0] = static_cast <char> (((ucs4 >> 18) & 0x07) | 0xF0);
			utf8_ptr [1] = static_cast <char> (((ucs4 >> 12) & 0x3F) | 0x80);
			utf8_ptr [2] = static_cast <char> (((ucs4 >>  6) & 0x3F) | 0x80);
			utf8_ptr [3] = static_cast <char> (( ucs4        & 0x3F) | 0x80);
			break;

		default:
			assert (false);
			break;
		}
	}

	return (ret_val);
}



int	Codec8::decode_char (char32_t &ucs4, const char utf8_ptr [])
{
	assert (&ucs4 != 0);
	assert (&utf8_ptr != 0);

	int				dummy_len;

	return (decode_char (ucs4, utf8_ptr, dummy_len));
}



// In case of invalid byte sequence, len is set to 0.
int	Codec8::decode_char (char32_t &ucs4, const char utf8_ptr [], int &len)
{
	assert (&ucs4 != 0);
	assert (&utf8_ptr != 0);

	ucs4 = 0;

	int				ret_val = get_char_seq_len_utf (len, utf8_ptr [0]);
	if (ret_val == Err_OK)
	{
		if (len == 1)
		{
			ucs4 = static_cast <unsigned char> (utf8_ptr [0]);
		}

		else
		{
			const int		mask_lead = 0x7F >> len;
			ucs4 = utf8_ptr [0] & mask_lead;

			for (int pos = 1; pos < len && ret_val == Err_OK; ++pos)
			{
				const char		x = utf8_ptr [pos];
				if (is_valid_utf8_follow_byte (x))
				{
					ucs4 <<= 6;
					ucs4 |= x & 0x3F;
				}
				else
				{
					ret_val = Err_TRUNCATED_UTF8_SEQ;
				}
			}
		}
	}

	return (ret_val);
}



int	Codec8::check_char_valid (const char utf8_ptr [])
{
	assert (utf8_ptr != 0);

	char32_t       dummy_ucs4;

	return (decode_char (dummy_ucs4, utf8_ptr));
}



int	Codec8::check_string_valid (const char utf8_0 [])
{
	assert (utf8_0 != 0);

	int				ret_val = Err_OK;
	long				pos = 0;

	while (ret_val == Err_OK && utf8_0 [pos] != '\0')
	{
		char32_t       dummy_ucs4;
		int            len;

		ret_val = decode_char (dummy_ucs4, &utf8_0 [pos], len);

		pos += len;
	}

	return (ret_val);
}



int	Codec8::check_string_valid (const std::string &utf8)
{
	return (check_string_valid (utf8.c_str ()));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace utf8
}	// namespace txt
}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
