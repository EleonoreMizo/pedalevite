/*****************************************************************************

        Glyph.cpp
        Copyright (c) 2003 Ohm Force

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Glyph.h"

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Glyph::set_size (int w, int h)
{
	assert (w > 0);
	assert (h > 0);

	_w = w;
	_h = h;
	const int		len = w * h;
	_pix_map.resize (len, 0);
}



int	Glyph::get_width () const
{
	return (_w);
}



int	Glyph::get_height () const
{
	return (_h);
}



void	Glyph::set_pix (int x, int y, int c)
{
	assert (x >= 0);
	assert (x < _w);
	assert (y >= 0);
	assert (y < _h);
	assert (c >= 0);
	assert (c <= 255);

	_pix_map.at (_w * y + x) = uint8_t (c);
}



int	Glyph::get_pix (int x, int y) const
{
	assert (x >= 0);
	assert (x < _w);
	assert (y >= 0);
	assert (y < _h);

	return (_pix_map.at (_w * y + x));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/





/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
