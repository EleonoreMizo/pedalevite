/*****************************************************************************

        GraphPrim.h
        Copyright (c) 2003 Laurent de Soras

--- Legal stuff ---

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

						GraphPrim ();
	virtual			~GraphPrim () {}

	void				set_size (float w, float h);
	void				set_plot (EPSPlot &plot, float x, float y);

	void				draw_arrow (double x_1, double y_1, double x_2, double y_2) const;
	void				print_legend (const char *txt_0, int level) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	float	PsCoord;

	void				convert_coordinates (double &x, double &y) const;
	void				draw_arrow_head (double x, double y, double arrow_cos, double arrow_sin, double head_size) const;

	EPSPlot *		_plot_ptr;
	PsCoord			_x;
	PsCoord			_y;
	PsCoord			_w;
	PsCoord			_h;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						GraphPrim (const GraphPrim &other);
	GraphPrim &		operator = (const GraphPrim &other);
	bool				operator == (const GraphPrim &other);
	bool				operator != (const GraphPrim &other);

};	// class GraphPrim





//#include	"GraphPrim.hpp"



#endif	// GraphPrim_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
