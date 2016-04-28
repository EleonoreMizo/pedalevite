/*****************************************************************************

        ConvDigit.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_neutral_ConvDigit_CURRENT_CODEHEADER)
	#error Recursive inclusion of ConvDigit code header.
#endif
#define	fstb_txt_neutral_ConvDigit_CURRENT_CODEHEADER

#if ! defined (fstb_txt_neutral_ConvDigit_CODEHEADER_INCLUDED)
#define	fstb_txt_neutral_ConvDigit_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>
#include	<cctype>



namespace std {}



namespace fstb
{
namespace txt
{
namespace neutral
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: is_digit_from_base
Description:
	Indicates if a character is a digit from a specified base.
Input parameters:
	- c: Character to analyse
	- base: Base, > 0.
Returns: true if the character is a digit.
Throws: Nothing
==============================================================================
*/

bool	ConvDigit::is_digit_from_base (char c, int base)
{
	assert (base > 0);

	const int		bm10 = base - 10;
	const bool		ok_flag =
		(   (c >= '0' && c <= '9' && c < '0' + base)
		 || (bm10 > 0 && (   (c >= 'A' && c < 'A' + bm10)
		                  || (c >= 'a' && c < 'a' + bm10))));

	return (ok_flag);
}



/*
==============================================================================
Name: conv_digit_to_char
Description:
	Convert a value into a digit (character). This digit may be in any base.
	Function to use to print hexadecimal digits.
Input parameters:
	- x: Number to convert into a character.
	- lower_case_flag: if true, generated digits >= 10 are lower case.
Returns: The character
Throws: Nothing
==============================================================================
*/

char	ConvDigit::conv_digit_to_char (int x, bool lower_case_flag)
{
	assert (x >= 0);
	assert (x <= 'Z' - 'A' + 10);

	char				c;
	if (x < 10)
	{
		c = x + '0';
	}
	else
	{
		c = x + 'A' - 10;
		if (lower_case_flag)
		{
			c = x + 'a' - 10;
		}
		else
		{
			c = x + 'A' - 10;
		}
	}

	return (c);
}



/*
==============================================================================
Name: conv_char_to_digit
Description:
	Convert a digit or letter into a number. This number is the value of the
	character. Thus 'A' = 10, 'C' = 12, etc. Use this function to analyse
	hexadecimal characters.
Input parameters:
	- c: The character to convert
Returns: Digit value
Throws: Nothing
==============================================================================
*/

int	ConvDigit::conv_char_to_digit (char c)
{
	using namespace std;

	c = toupper (c);

	assert (   (c >= '0' && c <= '9')
	        || (c >= 'A' && c <= 'Z'));

	int				x;
	if (c >= '0' && c <= '9')
	{
		x = c - '0';
	}
	else
	{
		x = c - 'A' + 10;
	}

	return (x);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace neutral
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_neutral_ConvDigit_CODEHEADER_INCLUDED

#undef fstb_txt_neutral_ConvDigit_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
