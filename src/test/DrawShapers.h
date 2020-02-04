/*****************************************************************************

        DrawShapers.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (DrawShapers_HEADER_INCLUDED)
#define DrawShapers_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"

#include <vector>



class DrawShapers
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     draw_shapers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_spl = 2048 + 1;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	template <typename S>
	static int     draw_shaper (S &shaper, std::string pathname, float xmin, float xmax);
	template <typename F>
	static int     draw_func (F fnc, std::string pathname, float xmin, float xmax);
	static BufAlign
	               create_src (float xmin, float xmax);
	static void    find_boundaries (float &xmin, float &xmax, const BufAlign &x_arr);
	static int     save_shaper_drawing (std::string pathname, const BufAlign &x_arr, const BufAlign &y_arr);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DrawShapers ()                               = delete;
	               DrawShapers (const DrawShapers &other)       = delete;
	               DrawShapers (DrawShapers &&other)            = delete;
	virtual        ~DrawShapers ()                              = delete;
	DrawShapers &  operator = (const DrawShapers &other)        = delete;
	DrawShapers &  operator = (DrawShapers &&other)             = delete;
	bool           operator == (const DrawShapers &other) const = delete;
	bool           operator != (const DrawShapers &other) const = delete;

}; // class DrawShapers



//#include "DrawShapers.hpp"



#endif   // DrawShapers_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

