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

#include "mfx/pi/param/Tools.h"

#include <cassert>
#include <cstring>



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
Returns: The best string. The label may be truncated if all the provided ones
	are too long.
==============================================================================
*/

std::string	Tools::print_name_bestfit (size_t max_len, const char src_list_0 [])
{
	assert (max_len > 0);
	assert (src_list_0 != nullptr);

	class MetricUnit { public: size_t eval (char32_t /*c*/) { return 1; } };
	MetricUnit     met;

	return print_name_bestfit (max_len, src_list_0, met, &MetricUnit::eval);
}



// Just compares the number of 8-bit characters, does not take real UTF-8
// length into account.
std::string	Tools::extract_longest_str (const char src_list_0 [], char delimiter)
{
	assert (src_list_0 != nullptr);

	std::string    result;

	size_t         pos   = 0;
	while (src_list_0 [pos] != '\0')
	{
		if (! result.empty ())
		{
			result.push_back (delimiter);
		}

		std::string    name;
		const char *   delim_0 = strchr (src_list_0 + pos, delimiter);
		if (delim_0 == nullptr)
		{
			name = src_list_0 + pos;
			pos  = strchr (src_list_0 + pos, '\0') - src_list_0;
		}
		else
		{
			name = std::string (src_list_0 + pos, delim_0);
			pos  = delim_0 + 1 - src_list_0;
		}

		if (name.length () > result.length () || result.empty ())
		{
			result = name;
		}
	}

	return result;
}



std::string	Tools::join_strings_multi (const char src_list_0 [], char delimiter, std::string pre, std::string post)
{
	assert (src_list_0 != nullptr);

	std::string    result;

	size_t         pos   = 0;
	do
	{
		if (! result.empty ())
		{
			result.push_back (delimiter);
		}

		result += pre;

		std::string    name;
		const char *   delim_0 = strchr (src_list_0 + pos, delimiter);
		if (delim_0 == nullptr)
		{
			name = src_list_0 + pos;
			pos  = strchr (src_list_0 + pos, '\0') - src_list_0;
		}
		else
		{
			name = std::string (src_list_0 + pos, delim_0);
			pos  = delim_0 + 1 - src_list_0;
		}
		result += name;

		result += post;
	}
	while (src_list_0 [pos] != '\0');

	return result;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
