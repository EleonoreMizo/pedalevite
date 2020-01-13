/*****************************************************************************

        GraphPrim.cpp
        Author: Laurent de Soras, 2003

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

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

#include "EPSPlot.h"
#include "GraphPrim.h"

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_size
Description:
	Sets the size of the drawing zone (rectangular).
	Call this function at least once before drawing anything.
Input parameters:
	- w: Width of the zone, in points. > 0
	- h: Height of the zone, in points. > 0
Throws: Nothing
==============================================================================
*/

void	GraphPrim::set_size (float w, float h)
{
	assert (w > 0);
	assert (h > 0);

	_w = w;
	_h = h;
}



/*
==============================================================================
Name: set_plot
Description:
	Sets the EPSPlot object used to draw figures.
	Call this function at least once before drawing anything.
Input parameters:
	- x: Location of the graph (distance from left border), points
	- y: Location of the graph (distance from bottom border), points
Input/output parameters:
	- plot: EPSPlot object.
Throws: Nothing
==============================================================================
*/

void	GraphPrim::set_plot (EPSPlot &plot, float x, float y)
{
	_plot_ptr = &plot;
	_x        = x;
	_y        = y;
}



/*
==============================================================================
Name: draw_arrow
Description:
	Draws an arrow, the head is filled. Coordinates may be out of the box.
Input parameters:
	- x_1: Coordinates of the tail of the arrow. Range [0 ; 1] is in the box.
	- y_1: Coordinates of the tail of the arrow. Range [0 ; 1] is in the box.
	- x_2: Coordinates of the head of the arrow. Range [0 ; 1] is in the box.
	- y_2: Coordinates of the head of the arrow. Range [0 ; 1] is in the box.
Throws: Nothing
==============================================================================
*/

void	GraphPrim::draw_arrow (double x_1, double y_1, double x_2, double y_2) const
{
	assert (_plot_ptr != nullptr);
	assert (_w > 0);
	assert (_h > 0);

	convert_coordinates (x_1, y_1);
	convert_coordinates (x_2, y_2);

	const double   head_size = 8;
	const double   d_x = x_2 - x_1;
	const double   d_y = y_2 - y_1;
	const double   arrow_size = sqrt (d_x * d_x + d_y * d_y);
	assert (arrow_size > 0);
	const double   arrow_cos = d_x / arrow_size;
	const double   arrow_sin = d_y / arrow_size;

	const double   line_size = arrow_size - head_size;
	const double   head_base_x = x_1 + arrow_cos * line_size;
	const double   head_base_y = y_1 + arrow_sin * line_size;

	PsCoord		line_x_arr [2] =
	{
		static_cast <PsCoord> (x_1),
		static_cast <PsCoord> (head_base_x)
	};
	PsCoord		line_y_arr [2] = {
		static_cast <PsCoord> (y_1),
		static_cast <PsCoord> (head_base_y)
	};
	_plot_ptr->drawLines (
		&line_x_arr [0],
		&line_y_arr [0],
		2,
		0.02f
	);

	draw_arrow_head (x_2, y_2, arrow_cos, arrow_sin, head_size);
}



/*
==============================================================================
Name: print_legend
Description:
	Prints a text (single line). It is aligned on the bottom right, but the
	ordinate may be choosen. The text is followed with a small line (current
	pen).
Input parameters:
	- txt_0: Text to print, finished by '\0'
	- level: Line position, one line has a 8-point height. When positive, the
		origin is the top of the figure. When negative, the bottom.
Throws: Nothing.
==============================================================================
*/

void	GraphPrim::print_legend (const char *txt_0, int level) const
{
	assert (_plot_ptr != nullptr);
	assert (_w > 0);
	assert (_h > 0);
	assert (txt_0 != nullptr);

	const float    offset = (level < 0) ? 0 : _h;
	_plot_ptr->drawLegend (
		_x + _w,
		_y + offset - 4 - 8 * level,
		txt_0
	);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	GraphPrim::convert_coordinates (double &x, double &y) const
{
	x = x * _w + _x;
	y = y * _h + _y;
}



// In PS coordinates, (x, y) is the arrow vertex, (arrow_cos, arrow_sin) the arrow directing vector
void	GraphPrim::draw_arrow_head (double x, double y, double arrow_cos, double arrow_sin, double head_size) const
{
	assert (_plot_ptr != nullptr);
	assert (fabs (arrow_cos) + fabs (arrow_sin) > 0);
	assert (head_size > 0);

	const bool     save_closepath_flag = _plot_ptr->is_path_closed ();
	const bool     save_fill_flag = _plot_ptr->is_path_filled ();
	_plot_ptr->set_close_path (true);
	_plot_ptr->set_fill_path (true);

	const double   dx_a = head_size * -arrow_cos;
	const double   dy_a = head_size * -arrow_sin;
	const double   qhs = head_size * 0.25;
	const double   dx_b = qhs * arrow_sin;
	const double   dy_b = qhs * arrow_cos;
	const double   dx_1 = dx_a - dx_b;
	const double   dy_1 = dy_a + dy_b;
	const double   dx_2 = dx_a + dx_b;
	const double   dy_2 = dy_a - dy_b;
	const PsCoord  x_arr [3] =
	{
		static_cast <PsCoord> (x       ),
		static_cast <PsCoord> (x + dx_1),
		static_cast <PsCoord> (x + dx_2)
	};
	const PsCoord	y_arr [3] =
	{
		static_cast <PsCoord> (y       ),
		static_cast <PsCoord> (y + dy_1),
		static_cast <PsCoord> (y + dy_2)
	};

	_plot_ptr->drawLines (&x_arr [0], &y_arr [0], 3, 0.02f);

	_plot_ptr->set_close_path (save_closepath_flag);
	_plot_ptr->set_fill_path (save_fill_flag);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
