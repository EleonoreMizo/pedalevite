/*****************************************************************************

        fnc.cpp
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

#include "fstb/txt/neutral/fnc.h"
#include "fstb/txt/neutral/ConvDigit.h"
#include "fstb/Err.h"

#include <cassert>
#include <cctype>
#include <cstring>



namespace fstb
{
namespace txt
{
namespace neutral
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: strstr_ci
Description:
	Search the position of a string within another one, without taking the
	case into account.
	Works only on pure ASCII strings.
Input parameters:
	- s1_0: pointer on the null-terminated string as search area.
	- s2_0: pointer on the null-terminated string to find.
Returns:
	- Index of the string if found.
	- Err_NOT_FOUND if... guess what.
Throws: Nothing
==============================================================================
*/

long	strstr_ci (const char *s1_0, const char *s2_0)
{
	long				offset = 0;
	for ( ; ; )
	{
		long           cnt = 0;
		while (s2_0 [cnt] != '\0')
		{
			if (s1_0 [cnt + offset] == '\0')
			{
				return (Err_NOT_FOUND);
			}
			if (toupper (s2_0 [cnt]) != toupper (s1_0 [cnt + offset]))
			{
				break;
			}
			++cnt;
		}

		if (s2_0 [cnt] == '\0')
		{
			return (offset);
		}

		++offset;
	}
}



/*
==============================================================================
Name: strcmp_ci
Description:
	Evaluates the case-insensitve-equality of two strings.
	Works only on pure ASCII strings.
Input parameters:
	- s1_0: pointer on the first null-terminated string.
	- s2_0: pointer on the second null-terminated string.
Returns: true
Throws: Nothing
==============================================================================
*/

bool	strcmp_ci (const char *s1_0, const char *s2_0)
{
	long           pos = 0;
	bool           ok_flag = true;
	do
	{
		ok_flag = (toupper (s1_0 [pos]) == toupper (s2_0 [pos]));
		++pos;
	}
	while (ok_flag && s1_0 [pos-1] != '\0');

	return ok_flag;
}



/*
==============================================================================
Name: to_lcase
Description:
	Convertit une chaine en minuscules
Input parameters:
	- txt: Chaine a convertir
Returns: Chaine en minuscules
Throws: ?
==============================================================================
*/

std::string	to_lcase (const std::string &txt)
{
	std::string    result (txt);
	to_lcase_inplace (result);

	return result;
}



/*
==============================================================================
Name: to_ucase
Description:
	Convertit une chaine en majuscules
Input parameters:
	- txt: Chaine a convertir
Returns: Chaine en majuscules
Throws: ?
==============================================================================
*/

std::string	to_ucase (const std::string &txt)
{
	std::string      result (txt);
	to_ucase_inplace (result);

	return result;
}



/*
==============================================================================
Name: to_lcase_inplace
Description:
	Convertit une chaine en minuscules, sur place
Input/output parameters:
	- txt: Chaine a convertir
Throws: Nothing
==============================================================================
*/

void	to_lcase_inplace (std::string &txt)
{
	const std::string::size_type  len = txt.length ();
	for (std::string::size_type i = 0; i < len; ++i)
	{
		txt [i] = char (tolower (txt [i]));
	}
}



/*
==============================================================================
Name: to_lcase_inplace
Description:
	Convertit une chaine en minuscules, sur place
Input/output parameters:
	- txt_0: Chaine a convertir
Throws: Nothing
==============================================================================
*/

void	to_lcase_inplace (char *txt_0)
{
	assert (txt_0 != nullptr);

	const size_t   len = strlen (txt_0);
	for (size_t i = 0; i < len; ++i)
	{
		txt_0 [i] = char (tolower (txt_0 [i]));
	}
}



/*
==============================================================================
Name: to_ucase_inplace
Description:
	Convertit une chaine en majuscules, sur place
Input/output parameters:
	- txt: Chaine a convertir
Throws: Nothing
==============================================================================
*/

void	to_ucase_inplace (std::string &txt)
{
	const size_t   len = txt.length ();
	for (size_t i = 0; i < len; ++i)
	{
		txt [i] = char (toupper (txt [i]));
	}
}



/*
==============================================================================
Name: to_ucase_inplace
Description:
	Convertit une chaine en majuscules, sur place
Input/output parameters:
	- txt_0: Chaine a convertir
Throws: Nothing
==============================================================================
*/

void	to_ucase_inplace (char *txt_0)
{
	assert (txt_0 != nullptr);

	const size_t   len = strlen (txt_0);
	for (size_t i = 0; i < len; ++i)
	{
		txt_0 [i] = char (toupper (txt_0 [i]));
	}
}



std::string	trim_spaces (const char *txt_0)
{
	assert (txt_0 != nullptr);

	// Skips spaces at the beginning
	size_t         pos = 0;
	while (txt_0 [pos] != '\0' && isspace (txt_0 [pos]))
	{
		++ pos;
	}

	// Finds the last non-space position
	const size_t   copy_start_pos = pos;
	size_t         copy_end_pos   = pos;
	while (txt_0 [pos] != '\0')
	{
		if (! isspace (txt_0 [pos]))
		{
			copy_end_pos = pos + 1;
		}

		++ pos;
	}

	const size_t      len = copy_end_pos - copy_start_pos;
	const std::string txt (&txt_0 [copy_start_pos], len);

	return txt;
}



// Returns true if conversion is OK.
// false :
// - val == 0: failed to decode a number
// - val == archi::INT64_MAX_VAL: positive overflow
// - val == archi::INT64_MIN_VAL: negative overflow
// In case of success (even overflow), stop_pos_ptr indicates the stop
// position (after the last translated character).
bool	conv_str_to_int64 (int64_t &val, const char *txt_0, int base, long *stop_pos_ptr)
{
	assert (txt_0 != nullptr);
	assert (base > 0);
	assert (base <= 'Z' - 'A' + 1 + 10);

	val = 0;

	int            s   = 1;
	long           pos = 0;

	// Skips whitespaces
	bool           skip_flag = true;
	do
	{
		const char     c = txt_0 [pos];
		if (c == '\0' || ! isspace (c))
		{
			skip_flag = false;
		}
		else
		{
			++ pos;
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
	while (ConvDigit::is_digit_from_base (txt_0 [pos], base))
	{
		const int64_t  old_val = val;

		const int      digit = ConvDigit::conv_char_to_digit (txt_0 [pos]);
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace neutral
}	// namespace txt
}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
