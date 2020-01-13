/*****************************************************************************

        GraphPrim.h
        Author: Laurent de Soras, 2003

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (GraphPrim_HEADER_INCLUDED)
#define	GraphPrim_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class EPSPlot;

class GraphPrim
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_size (float w, float h);
	void           set_plot (EPSPlot &plot, float x, float y);

	void           draw_arrow (double x_1, double y_1, double x_2, double y_2) const;
	void           print_legend (const char *txt_0, int level) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef float PsCoord;

	void           convert_coordinates (double &x, double &y) const;
	void           draw_arrow_head (double x, double y, double arrow_cos, double arrow_sin, double head_size) const;

	EPSPlot *      _plot_ptr { nullptr };
	PsCoord        _x        { 0 };
	PsCoord        _y        { 0 };
	PsCoord        _w        { 0 };
	PsCoord        _h        { 0 };



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

};	// class GraphPrim





//#include "GraphPrim.hpp"



#endif	// GraphPrim_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
