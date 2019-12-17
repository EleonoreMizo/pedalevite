/*****************************************************************************

        utf16_fnc.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/txt/unicode/ConvCi.h"
#include "fstb/txt/unicode/PredEq.h"
#include "fstb/txt/utf16/Codec16.h"
#include "fstb/txt/utf16/utf16_fnc.h"
#include "fstb/Err.h"

#include <cassert>



namespace fstb
{
namespace txt
{
namespace utf16
{



// Position is set to -1 if not found or error.
// An empty string is always found at position 0.
int	strstr_ci (const Char16 str_0 [], const Char16 subset_0 [], long &subset_pos)
{
	assert (str_0 != 0);
	assert (subset_0 != 0);

	if (subset_0 [0] != '\0')
	{
		unicode::PredEq <unicode::ConvCi>   pred;

		subset_pos = 0;
		while (str_0 [subset_pos] != '\0')
		{
			long           pos_str = subset_pos;
			long           pos_sub = 0;

			do
			{
				char32_t       c_str;
				int            c_str_len;
				int            ret_val =
					Codec16 <Char16>::decode_char (
					c_str,
					&str_0 [pos_str],
					c_str_len
				);
				if (ret_val != Err_OK)
				{
					subset_pos = -1;
					return ret_val;
				}

				char32_t       c_sub;
				int            c_sub_len;
				ret_val = Codec16 <Char16>::decode_char (
					c_sub,
					&subset_0 [pos_sub],
					c_sub_len
				);
				if (ret_val != Err_OK)
				{
					subset_pos = -1;
					return ret_val;
				}

				if (! pred (c_sub, c_str))
				{
					break;
				}
				
				pos_str += c_str_len;
				pos_sub += c_sub_len;
			}
			while (subset_0 [pos_sub] != '\0' && str_0 [pos_str] != '\0');

			if (subset_0 [pos_sub] == '\0')
			{
				return Err_OK;
			}

			else if (str_0 [pos_str] == '\0')
			{
				subset_pos = -1;
				return Err_NOT_FOUND;
			}

			int            step;
			Codec16 <Char16>::get_char_seq_len_utf (step, str_0 [subset_pos]);
			subset_pos += step;
		}

		subset_pos = -1;
		return Err_NOT_FOUND;
	}

	return Err_OK;
}



}  // namespace utf16
}  // namespace txt
}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
