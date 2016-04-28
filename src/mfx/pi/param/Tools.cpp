/*****************************************************************************

        Tools.cpp
        Author: Laurent de Soras, 2016

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

#include "fstb/txt/utf8/Codec8.h"
#include "mfx/pi/param/Tools.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: print_name_bestfit
Description:
	Prints a label given in different lengthes at the biggest length equal or
	below a specified length.
	Strings are assumed valid UTF-8 sequences and lengths are in unicode
	characters.
Input parameters:
	- max_len: Maximum length of the string, without the terminal '\0'. > 0.
	- src_list_0: List of labels, as a single string, separated with a '\n'.
		No '\n' needed for the last label.
Throws: The best string. The label may be truncated if all the provided ones
	are too long.
==============================================================================
*/

std::string	Tools::print_name_bestfit (long max_len, const char src_list_0 [])
{
	assert (max_len > 0);
	assert (src_list_0 != 0);

	long				sel_label_len_ucs = 0;
	long				sel_label_len_utf = 0;
	long				sel_label_pos     = 0;
	long				cur_label_len_ucs = 0;
	long				cur_label_len_utf = 0;
	long				cur_label_pos     = 0;
	long				pos               = 0;
	bool				exit_flag         = false;
	do
	{
		const char		c = src_list_0 [pos];
		int            c_len;
		fstb::txt::utf8::Codec8::get_char_seq_len_utf (c_len, c);
		if (c == '\n' || c == '\0')
		{
			if (cur_label_len_ucs > 0)
			{
				if (   (cur_label_len_ucs > sel_label_len_ucs && cur_label_len_ucs <= max_len)
				    || (cur_label_len_ucs < sel_label_len_ucs && sel_label_len_ucs > max_len))
				{
					sel_label_len_ucs = cur_label_len_ucs;
					sel_label_len_utf = cur_label_len_utf;
					sel_label_pos     = cur_label_pos;
				}
			}

			cur_label_len_ucs = 0;
			cur_label_len_utf = 0;
			cur_label_pos     = pos + c_len;

			if (c == '\0')
			{
				exit_flag = true;
			}
		}

		else
		{
			if (cur_label_len_ucs < max_len)
			{
				cur_label_len_utf += c_len;
			}
			++ cur_label_len_ucs;
		}

		pos += c_len;
	}
	while (! exit_flag);

	assert (sel_label_len_utf <= max_len);

	return (std::string (src_list_0 + sel_label_pos, sel_label_len_utf));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
