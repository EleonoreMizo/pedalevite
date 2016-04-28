/*****************************************************************************

        PredStringCiLt.hpp
        Author: Laurent de Soras, 2007

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_utf8_PredStringCiLt_CURRENT_CODEHEADER)
	#error Recursive inclusion of PredStringCiLt code header.
#endif
#define	fstb_txt_utf8_PredStringCiLt_CURRENT_CODEHEADER

#if ! defined (fstb_txt_utf8_PredStringCiLt_CODEHEADER_INCLUDED)
#define	fstb_txt_utf8_PredStringCiLt_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/ConvCi.h"
#include	"fstb/txt/utf8/Codec8.h"

#include	<cassert>


namespace fstb
{
namespace txt
{
namespace utf8
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	PredStringCiLt::operator () (const std::string &lhs, const std::string &rhs) const
{
	bool				inf_flag = false;		// Default

	const char *	lhs_0 = lhs.c_str ();
	const char *	rhs_0 = rhs.c_str ();
	const std::string::size_type	len_l = lhs.size ();
	const std::string::size_type	len_r = rhs.size ();
	std::string::size_type	pos_l = 0;
	std::string::size_type	pos_r = 0;

	bool				cont_flag = true;
	do
	{
		// '0' is ordered before any other character so we don't bother to
		// really test end of string.
		assert (pos_l <= len_l);
		assert (pos_r <= len_r);

		long				c_l;
		int				c_l_len;
		Codec8::decode_char (c_l, lhs_0 + pos_l, c_l_len);

		long				c_r;
		int				c_r_len;
		Codec8::decode_char (c_r, rhs_0 + pos_r, c_r_len);

		const long		c_l_u = unicode::ConvCi::convert (c_l);
		const long		c_r_u = unicode::ConvCi::convert (c_r);

		if (c_l_u != c_r_u || c_l_u == 0)
		{
			cont_flag = false;
			if (c_l_u < c_r_u)
			{
				inf_flag = true;
			}
		}

		pos_l += c_l_len;
		pos_r += c_r_len;
	}
	while (cont_flag);

	return (inf_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace utf8
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_utf8_PredStringCiLt_CODEHEADER_INCLUDED

#undef fstb_txt_utf8_PredStringCiLt_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
