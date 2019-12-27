/*****************************************************************************

        Glyph.h

*Tab=3***********************************************************************/



#if ! defined (Glyph_HEADER_INCLUDED)
#define	Glyph_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cstdint>

#include	<vector>



class Glyph
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						Glyph () = default;
	virtual			~Glyph () = default;

	void				set_size (int w, int h);
	int				get_width () const;
	int				get_height () const;
	void				set_pix (int x, int y, int c);
	int				get_pix (int x, int y) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	std::vector <uint8_t>	PixMap;

	int				_w = 0;
	int				_h = 0;
	PixMap			_pix_map;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool				operator == (const Glyph &other);
	bool				operator != (const Glyph &other);

};	// class Glyph





//#include	"Glyph.hpp"



#endif	// Glyph_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
