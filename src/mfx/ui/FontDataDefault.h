/*****************************************************************************

        FontDataDefault.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_FontDataDefault_HEADER_INCLUDED)
#define mfx_ui_FontDataDefault_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/Font.h"
#include "mfx/ui/FontMapping8859.h"

#include <cstdint>



namespace mfx
{
namespace ui
{



class FontDataDefault
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static void    make_04x06 (Font &fnt);
	static void    make_06x06 (Font &fnt);
	static void    make_06x08 (Font &fnt);
	static void    make_08x12 (Font &fnt);

	// Scale 4
	static void    make_16x24 (Font &fnt);
	static void    make_24x24 (Font &fnt);
	static void    make_24x32 (Font &fnt);
	static void    make_32x48 (Font &fnt);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _char_per_table = FontMapping8859::_char_per_table;
	static const int  _char_per_row   = 16;

	static void    map_extra (Font &fnt);

	static const uint8_t
	               _data_04x06 [_char_per_table * 4*6];
	static const uint8_t
	               _data_06x06 [_char_per_table * 6*6];
	static const uint8_t
	               _data_06x08 [_char_per_table * 6*8];
	static const uint8_t
	               _data_08x12 [_char_per_table * 8*12];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FontDataDefault ()                               = delete;
	               FontDataDefault (const FontDataDefault &other)   = delete;
	virtual        ~FontDataDefault ()                              = delete;
	FontDataDefault &
	               operator = (const FontDataDefault &other)        = delete;
	bool           operator == (const FontDataDefault &other) const = delete;
	bool           operator != (const FontDataDefault &other) const = delete;

}; // class FontDataDefault



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/FontDataDefault.hpp"



#endif   // mfx_ui_FontDataDefault_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
