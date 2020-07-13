/*****************************************************************************

        Codec8.h
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_utf8_Codec8_HEADER_INCLUDED)
#define	fstb_txt_utf8_Codec8_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>



namespace fstb
{
namespace txt
{
namespace utf8
{



class Codec8
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr bool
	               is_valid_utf8_byte (char c);
	static constexpr bool
	               is_valid_utf8_lead_byte (char c);
	static constexpr bool
	               is_valid_utf8_follow_byte (char c);
	static constexpr bool
	               is_char_single_byte (char c);

	static int     get_char_seq_len_ucs (int &len, char32_t ucs4);
	static int     get_char_seq_len_utf (int &len, char utf8_lead_byte);

	static int     encode_char (std::string &utf8, char32_t ucs4);
	static int     encode_char (char utf8_0 [], char32_t ucs4);
	static int     encode_char (char utf8_ptr [], char32_t ucs4, int &len);

	static int     decode_char (char32_t &ucs4, const char utf8_ptr []);
	static int     decode_char (char32_t &ucs4, const char utf8_ptr [], int &len);

	static int     check_char_valid (const char utf8_ptr []);
	static int     check_string_valid (const char utf8_0 []);
	static int     check_string_valid (const std::string &utf8);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Codec8 ();
						Codec8 (const Codec8 &other);
	Codec8 &			operator = (const Codec8 &other);
	bool				operator == (const Codec8 &other);
	bool				operator != (const Codec8 &other);

};	// class Codec8



}	// namespace utf8
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/utf8/Codec8.hpp"



#endif	// fstb_txt_utf8_Codec8_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
