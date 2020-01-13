/*****************************************************************************

        ConvNum.cpp
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

#include	"fstb/txt/neutral/ConvDigit.h"
#include	"fstb/txt/unicode/Categ.h"
#include	"fstb/txt/unicode/CharData.h"
#include	"fstb/txt/utf8/Codec8.h"
#include	"fstb/txt/utf8/ConvNum.h"
#include	"fstb/Err.h"

#include	<cassert>



namespace fstb
{
namespace txt
{
namespace utf8
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int64_t	ConvNum::conv_str_to_int64 (const char *txt_0, int base)
{
	assert (txt_0 != nullptr);
	assert (base > 0);
	assert (base <= 'Z' - 'A' + 1 + 10);

	int64_t        val = 0;
	conv_str_to_int64 (val, txt_0, base);	// Ignores overflow or failure

	return (val);
}



//#* LDS: some duplicated code with neutral::conv_str_to_int64() to factorise
// Returns true if conversion is OK.
// false :
// - val == 0: failed to decode a number
// - val == archi::INT64_MAX_VAL: positive overflow
// - val == archi::INT64_MIN_VAL: negative overflow
// In case of success (even overflow), stop_pos_ptr indicates the stop
// position (after the last translated character).
bool	ConvNum::conv_str_to_int64 (int64_t &val, const char *txt_0, int base, long *stop_pos_ptr)
{
	assert (txt_0 != nullptr);
	assert (base > 0);
	assert (base <= 'Z' - 'A' + 1 + 10);

	val = 0;

	int            s = 1;
	long           pos = 0;

	// Skips whitespaces
	bool           skip_flag = true;
	do
	{
		char32_t       c;
		int            char_len;
		int            ret_val = Codec8::decode_char (c, txt_0 + pos, char_len);
		if (ret_val != Err_OK)
		{
			// Will produce a decoding failure later in the function
			skip_flag = false;
		}
		else if (c == 0)
		{
			skip_flag = false;
		}
		else
		{
			const unicode::Categ       categ = unicode::CharData::get_categ (c);
			const unicode::CategClass  cat_c = unicode::Categ_conv_to_class (categ);
			if (   cat_c == unicode::CategClass_OTHER
				 || cat_c == unicode::CategClass_SEPARATOR)
			{
				pos += char_len;
			}
			else
			{
				skip_flag = false;
			}
		}
	}
	while (skip_flag);

	// Sign
	if (txt_0 [pos] == '-')
	{
		s = -1;
		++ pos;
	}
	else if (txt_0 [pos] == '+')
	{
		++ pos;
	}

	// Digits
	bool           overflow_flag      = false;
	int            nbr_decoded_digits = 0;
	while (neutral::ConvDigit::is_digit_from_base (txt_0 [pos], base))
	{
		const int64_t  old_val = val;

		const int      digit = neutral::ConvDigit::conv_char_to_digit (txt_0 [pos]);
		val *= base;
		val += digit * s;

		if ((val / base) != old_val)
		{
			overflow_flag = true;
		}

		++ nbr_decoded_digits;
		++ pos;
	}

	if (overflow_flag)
	{
		if (s > 0)
		{
			val = INT64_MAX;
		}
		else
		{
			val = INT64_MIN;
		}
	}

	if (stop_pos_ptr != nullptr)
	{
		*stop_pos_ptr = pos;
	}

	return ((! overflow_flag) && (nbr_decoded_digits > 0));
}



int	ConvNum::conv_int64_to_str (char *txt_0, int64_t val, long max_len, int base)
{
	assert (txt_0 != nullptr);
	assert (max_len > 0);
	assert (base > 0);
	assert (base <= 'Z' - 'A' + 1 + 10);

	// Counts the number of required digits
   int            nbr_digits = 0;
   {
		int64_t        x = val;
		do
		{
			x /= base;
			++ nbr_digits;
		}
		while (x != 0);
	}

	const int      sign_offset = (val < 0) ? 1 : 0;
	const int      nbr_char = nbr_digits + sign_offset;

	int            ret_val = 0;
	if (nbr_char > max_len)
	{
		ret_val = Err_NO_ROOM;
	}

	else
	{
		if (val < 0)
		{
			txt_0 [0] = '-';
		}

		for (int pos = 0; pos < nbr_digits; ++pos)
		{
			const int64_t  x = val / base;
			const int      d = std::abs (static_cast <int> (val - x * base));
			txt_0 [nbr_char - 1 - pos] =
				neutral::ConvDigit::conv_digit_to_char (d);
			val = x;
		}

		txt_0 [nbr_char] = '\0';
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace utf8
}	// namespace txt
}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
