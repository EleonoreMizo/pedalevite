/*****************************************************************************

        PredStringCiEq.hpp
        Author: Laurent de Soras, 2007

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_utf8_PredStringCiEq_CURRENT_CODEHEADER)
	#error Recursive inclusion of PredStringCiEq code header.
#endif
#define	fstb_txt_utf8_PredStringCiEq_CURRENT_CODEHEADER

#if ! defined (fstb_txt_utf8_PredStringCiEq_CODEHEADER_INCLUDED)
#define	fstb_txt_utf8_PredStringCiEq_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/PredEq.h"
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



bool	PredStringCiEq::operator () (const std::string &lhs, const std::string &rhs) const
{
	unicode::PredEq <unicode::ConvCi>	pred;

	bool				eq_flag = true;		// Default

	const char *	lhs_0 = lhs.c_str ();
	const char *	rhs_0 = rhs.c_str ();
	const std::string::size_type	len_l = lhs.size ();
	const std::string::size_type	len_r = rhs.size ();
	std::string::size_type	pos_l = 0;
	std::string::size_type	pos_r = 0;

	bool				cont_flag = true;
	do
	{
		assert (pos_l <= len_l);
		assert (pos_r <= len_r);

		char32_t       c_l;
		int            c_l_len;
		Codec8::decode_char (c_l, lhs_0 + pos_l, c_l_len);

		char32_t       c_r;
		int            c_r_len;
		Codec8::decode_char (c_r, rhs_0 + pos_r, c_r_len);

		const bool		char_eq_flag = pred (c_l, c_r);
		if (c_l == 0 || ! char_eq_flag)
		{
			cont_flag = false;
			if (! char_eq_flag)
			{
				eq_flag = false;
			}
		}

		pos_l += c_l_len;
		pos_r += c_r_len;
	}
	while (cont_flag);

	return (eq_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace utf8
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_utf8_PredStringCiEq_CODEHEADER_INCLUDED

#undef fstb_txt_utf8_PredStringCiEq_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
