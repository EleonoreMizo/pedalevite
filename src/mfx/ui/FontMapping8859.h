/*****************************************************************************

        FontMapping8859.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_FontMapping8859_HEADER_INCLUDED)
#define mfx_ui_FontMapping8859_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace mfx
{
namespace ui
{



class FontMapping8859
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _char_per_table = 256;
	static const std::array <char32_t, _char_per_table>
	               _data;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FontMapping8859 ()                               = delete;
	               FontMapping8859 (const FontMapping8859 &other)   = delete;
	virtual        ~FontMapping8859 ()                              = delete;
	FontMapping8859 &
	               operator = (const FontMapping8859 &other)        = delete;
	bool           operator == (const FontMapping8859 &other) const = delete;
	bool           operator != (const FontMapping8859 &other) const = delete;

}; // class FontMapping8859



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/FontMapping8859.hpp"



#endif   // mfx_ui_FontMapping8859_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
