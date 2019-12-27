/*****************************************************************************

        FontRenderer.h

*Tab=3***********************************************************************/



#pragma once
#if ! defined (FontRenderer_HEADER_INCLUDED)
#define	FontRenderer_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Glyph.h"

#include	"freetype/include/ft2build.h"
#include	FT_FREETYPE_H

#include <array>
#include <string>
#include <vector>

#include <cstdint>



class FontRenderer
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       FontRenderer (std::string filename, double size_pt, double scale_h = 1.0);
	virtual        ~FontRenderer ();

	int            export_to_raw (std::string filename);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_char   = 256;
	static const int  _size_scale = 64;

	typedef	std::vector <Glyph>	GlyphList;
	typedef std::vector <uint8_t> PicData;

	int            render_font_to_pic_data (PicData &pic_data, int &pic_w, int &pic_h);
	int            render_font ();
	int            find_dimensions ();
	int            compute_total_width ();

	GlyphList      _glyph_list;
	std::string    _font_filename;
	double         _size_pt;
	bool           _rendered_flag;
	FT_Library     _library;
	FT_Face        _face;
	int            _max_width;
	int            _max_top;
	int            _max_bot;

	static const std::array <char32_t, _nbr_char>
	               _unicode_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FontRenderer ()                          = delete;
	               FontRenderer (const FontRenderer &other) = delete;
	FontRenderer & operator = (const FontRenderer &other)   = delete;
	bool           operator == (const FontRenderer &other)  = delete;
	bool           operator != (const FontRenderer &other)  = delete;

};	// class FontRenderer





//#include	"FontRenderer.hpp"



#endif	// FontRenderer_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
