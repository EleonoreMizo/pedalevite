/*****************************************************************************

        Conv.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_txt_Conv_HEADER_INCLUDED)
#define fstb_txt_Conv_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



namespace fstb
{
namespace txt
{



class Conv
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <class C16>
	static int     utf8_to_utf16 (std::basic_string <C16> &utf16, const char utf8_0 []);

	template <class C16>
	static int     utf16_to_utf8 (std::string &utf8, const C16 utf16_0 []);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Conv ()                               = delete;
	               Conv (const Conv &other)              = delete;
	virtual        ~Conv ()                              = delete;
	Conv &         operator = (const Conv &other)        = delete;
	bool           operator == (const Conv &other) const = delete;
	bool           operator != (const Conv &other) const = delete;

}; // class Conv



}  // namespace txt
}  // namespace fstb



#include "fstb/txt/Conv.hpp"



#endif   // fstb_txt_Conv_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
