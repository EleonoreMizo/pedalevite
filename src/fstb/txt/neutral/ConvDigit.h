/*****************************************************************************

        ConvDigit.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_neutral_ConvDigit_HEADER_INCLUDED)
#define	fstb_txt_neutral_ConvDigit_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{
namespace txt
{
namespace neutral
{



class ConvDigit
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static inline bool
						is_digit_from_base (char c, int base);
	static inline char
						conv_digit_to_char (int x, bool lower_case_flag = false);
	static inline int
						conv_char_to_digit (char c);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ConvDigit ();
						ConvDigit (const ConvDigit &other);
	ConvDigit &		operator = (const ConvDigit &other);
	bool				operator == (const ConvDigit &other);
	bool				operator != (const ConvDigit &other);

};	// class ConvDigit



}	// namespace neutral
}	// namespace txt
}	// namespace fstb



#include	"fstb/txt/neutral/ConvDigit.hpp"



#endif	// fstb_txt_neutral_ConvDigit_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
