/*****************************************************************************

        fnc.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_neutral_fnc_CURRENT_CODEHEADER)
	#error Recursive inclusion of fnc code header.
#endif
#define	fstb_txt_neutral_fnc_CURRENT_CODEHEADER

#if ! defined (fstb_txt_neutral_fnc_CODEHEADER_INCLUDED)
#define	fstb_txt_neutral_fnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/neutral/ConvDigit.h"

#include	<cctype>



namespace fstb
{
namespace txt
{
namespace neutral
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: change_case
Description:
	Change the case of a character.
Input parameters:
	- c: The character which case is to be changed.
	- lower_case_flag: true if the character should be converted to lower case,
		false if it should be converted to upper case.
Returns: The converted character.
Throws: Nothing
==============================================================================
*/

char	change_case (char c, bool lower_case_flag)
{
	if (lower_case_flag)
	{
		c = char (tolower (c));
	}
	else
	{
		c = char (toupper (c));
	}

	return c;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace neutral
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_neutral_fnc_CODEHEADER_INCLUDED

#undef fstb_txt_neutral_fnc_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
