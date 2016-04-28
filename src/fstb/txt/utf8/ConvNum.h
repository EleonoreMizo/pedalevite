/*****************************************************************************

        ConvNum.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_utf8_ConvNum_HEADER_INCLUDED)
#define	fstb_txt_utf8_ConvNum_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace fstb
{
namespace txt
{
namespace utf8
{



class ConvNum
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int64_t conv_str_to_int64 (const char *txt_0, int base = 10);
	static bool    conv_str_to_int64 (int64_t &val, const char *txt_0, int base = 10, long *stop_pos_ptr = 0);
	static int     conv_int64_to_str (char *txt_0, int64_t val, long max_len, int base = 10);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ConvNum ();
						ConvNum (const ConvNum &other);
	ConvNum &		operator = (const ConvNum &other);
	bool				operator == (const ConvNum &other);
	bool				operator != (const ConvNum &other);

};	// class ConvNum



}	// namespace utf8
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/utf8/ConvNum.hpp"



#endif	// fstb_txt_utf8_ConvNum_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
