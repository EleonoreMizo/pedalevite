/*****************************************************************************

        unicode.hpp
        Author: Laurent de Soras, 2015

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_unicode_CODEHEADER_INCLUDED)
#define	fstb_txt_unicode_unicode_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>

#include <algorithm>



namespace fstb
{
namespace txt
{
namespace unicode
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class C>
int	compare (const std::u32string &s1, const std::u32string &s2)
{
	int				   ret_val = 0;

	size_t            pos     = 0;
	const char32_t *  s1_0    = s1.c_str ();
	const char32_t *  s2_0    = s2.c_str ();
	const size_t      max_len = std::max (s1.length (), s2.length ());
	while (ret_val == 0 && pos < max_len)
	{
		const char32_t c1 = C::convert (s1 [pos]);
		const char32_t c2 = C::convert (s2 [pos]);
		ret_val = c1 - c2;
		++ pos;
	}

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_unicode_unicode_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
