/*****************************************************************************

        Codec16.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_txt_utf16_Codec16_HEADER_INCLUDED)
#define fstb_txt_utf16_Codec16_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>

#include	<climits>



namespace fstb
{
namespace txt
{
namespace utf16
{



template <class C16>
class Codec16
{

	static_assert ((sizeof (C16) * CHAR_BIT >= 16), "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static bool    is_valid_utf16_lead_word (C16 w);
	static bool    is_valid_utf16_end_word (C16 w);
	static bool    is_char_single_word (C16 w);

	static int     get_char_seq_len_ucs (int &len, char32_t ucs4);
	static int     get_char_seq_len_utf (int &len, C16 utf16_lead_word);

	static int     encode_char (std::basic_string <C16> &utf16, char32_t ucs4);
	static int     encode_char (C16 utf16_0 [], char32_t ucs4);
	static int     encode_char (C16 utf16_ptr [], char32_t ucs4, int &len);

	static int     decode_char (char32_t &ucs4, const C16 utf16_ptr []);
	static int     decode_char (char32_t &ucs4, const C16 utf16_ptr [], int &len);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Codec16 ()                               = delete;
	               Codec16 (const Codec16 &other)           = delete;
	Codec16 &      operator = (const Codec16 &other)        = delete;
	bool           operator == (const Codec16 &other) const = delete;
	bool           operator != (const Codec16 &other) const = delete;

}; // class Codec16



}  // namespace utf16
}  // namespace txt
}  // namespace fstb



#include "fstb/txt/utf16/Codec16.hpp"



#endif   // fstb_txt_utf16_Codec16_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
