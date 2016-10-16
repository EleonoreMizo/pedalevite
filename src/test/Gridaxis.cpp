/*****************************************************************************

        Gridaxis.cpp
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

#include	"EPSPlot.h"
#include	"Gridaxis.h"

#include	<vector>

#include	<cassert>
#include	<cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: std::vector related exception
==============================================================================
*/

Gridaxis::Gridaxis ()
:	_axis ()
,	_prim ()
,	_plot_ptr (0)
,	_dot_style (DotStyle_SQUARE)
,	_x (0)
,	_y (0)
,	_w (-1)
,	_h (-1)
,	_grid_flag (true)
{
	// Nothing
}



/*
==============================================================================
Name: set_size
Description:
	Sets the size of the graph.
	This function should be called at least once before drawing anything.
Input parameters:
	- w: Width of the graph, points, > 0
	- h: Height of the graph, points, > 0
Throws: Nothing
==============================================================================
*/

void	Gridaxis::set_size (float w, float h)
{
	assert (w > 0);
	assert (h > 0);

	_w = w;
	_h = h;

	_prim.set_size (w, h);
}



/*
==============================================================================
Name: set_plot
Description:
	Attach an EPSPlot object, for drawing the figures.
	This function should be called at least once before drawing anything.
Input parameters:
	- x: Location of the graph (distance from left border), points
	- y: Location of the graph (distance from bottom border), points
	- w: Width of the graph, points, > 0.
	- h: Height of the graph, points, > 0.
Input/output parameters:
	- plot: EPSPlot object to attach.
Throws: Nothing
==============================================================================
*/

void	Gridaxis::set_plot (EPSPlot &plot, float x, float y)
{
	assert (&plot != 0);

	_plot_ptr = &plot;
	_x = x;
	_y = y;

	_prim.set_plot (plot, x, y);
}



/*
==============================================================================
Name: set_grid
Description:
	Displays or remove the grid. The flag has to be set before calling the
	render_background() function, where the grid is or is not rendered.
Input parameters:
	- grid_flag: true to render the grid (this is the default behaviour)
Throws: Nothing
==============================================================================
*/

void	Gridaxis::set_grid (bool grid_flag)
{
	_grid_flag = grid_flag;
}



/*
==============================================================================
Name: use_axis
Description:
	Uses one of the axis object, to parameter its display. Call this function
	before render_background().
Input parameters:
	- dir: Direction_H (horizontal axis) or Direction_V (vertical).
Returns: The axis object.
Throws: Nothing
==============================================================================
*/

Axis &	Gridaxis::use_axis (Direction dir)
{
	assert (dir >= 0);
	assert (dir < Direction_NBR_ELT);

	return (_axis [dir]);
}



const Axis &	Gridaxis::use_axis (Direction dir) const
{
	assert (dir >= 0);
	assert (dir < Direction_NBR_ELT);

	return (_axis [dir]);
}



/*
==============================================================================
Name: use_prim
Description:
	Use the object rendering special graphic primitives, in order to parameter
	it or to make it display things.
Returns: The GraphPrim ojbect.
Throws: Nothing
==============================================================================
*/

GraphPrim &	Gridaxis::use_prim ()
{
	return (_prim);
}



const GraphPrim &	Gridaxis::use_prim () const
{
	return (_prim);
}



/*
==============================================================================
Name: render_background
Description:
	Renders grid (optional) and both axis. Everything is contained in a visible
	bounding box.
Throws: std::vector related exceptions.
==============================================================================
*/

void	Gridaxis::render_background () const
{
	assert (_plot_ptr != 0);

	_plot_ptr->gSave ();

	_plot_ptr->setGray (0);
	_plot_ptr->setLineWidth (0.5);
	_plot_ptr->drawBox (_x, _y, _x + _w, _y + _h);

	if (_grid_flag)
	{
		render_grid ();
	}

	if (use_axis (Direction_V).is_tick_active ())
	{
		render_axis_v ();
	}

	if (use_axis (Direction_H).is_tick_active ())
	{
		render_axis_h ();
	}

	_plot_ptr->gRestore ();
}



/*
==============================================================================
Name: draw_line
Description:
	Draws a line.
Input parameters:
	- x_1: Coordinates of the first point, in the horizontal axis scale.
	- y_1: Coordinates of the first point, in the vertical axis scale.
	- x_2: Coordinates of the second point, in the horizontal axis scale.
	- y_2: Coordinates of the second point, in the vertical axis scale.
Throws: Nothing
==============================================================================
*/

void	Gridaxis::draw_line (double x_1, double y_1, double x_2, double y_2) const
{
	assert (_plot_ptr != 0);

	const double	x_arr [2] = { x_1, x_2 };
	const double	y_arr [2] = { y_1, y_2 };

	render_curve (x_arr, y_arr, 2);
}



/*
==============================================================================
Name: draw_point
Description:
	Draws a point, whose shape depends on the dot settings.
Input parameters:
	- x: Coordinates of the point, in the horizontal axis scale.
	- y: Coordinates of the point, in the vertical axis scale.
Throws: std::vector related exceptions
==============================================================================
*/

void	Gridaxis::draw_point (double x, double y) const
{
	const double	x_arr [1] = { x };
	const double	y_arr [1] = { y };

	render_point_set (x_arr, y_arr, 1);
}



/*
==============================================================================
Name: render_curve
Description:
	Draws a curve composed of linked segments.
Input parameters:
	- x_arr: Array of coordinates of the points, in the horizontal axis scale.
	- y_arr: Array of coordinates of the points, in the vertical axis scale.
	- nbr_points: Number of points constituting the segments.
Throws: std::vector related exceptions
==============================================================================
*/

void	Gridaxis::render_curve (const double x_arr [], const double y_arr [], long nbr_points) const
{
	assert (_plot_ptr != 0);
	assert (x_arr != 0);
	assert (y_arr != 0);
	assert (nbr_points >= 2);

	PsCoordArr		x_conv;
	PsCoordArr		y_conv;
	convert_coordinates (x_conv, y_conv, x_arr, y_arr, nbr_points);

	_plot_ptr->drawLines (
		&x_conv [0],
		&y_conv [0],
		nbr_points,
		0.02f,
		_x,
		_y,
		_x + _w,
		_y + _h
	);
}



/*
==============================================================================
Name: render_point_set
Description:
	Draws multiple points, whose shape depends on the dot settings.
Input parameters:
	- x_arr: Array of coordinates of the points, in the horizontal axis scale.
	- y_arr: Array of coordinates of the points, in the vertical axis scale.
	- nbr_points: Number of points to display
Throws: std::vector related exceptions
==============================================================================
*/

void	Gridaxis::render_point_set (const double x_arr [], const double y_arr [], long nbr_points) const
{
	assert (_plot_ptr != 0);
	assert (x_arr != 0);
	assert (y_arr != 0);
	assert (nbr_points >= 1);

	PsCoordArr		x_conv;
	PsCoordArr		y_conv;
	convert_coordinates (x_conv, y_conv, x_arr, y_arr, nbr_points);

	for (long pos = 0; pos < nbr_points; ++pos)
	{
		const PsCoord	vx = x_conv [pos];
		const PsCoord	vy = y_conv [pos];

		switch (_dot_style)
		{
		case	DotStyle_PLUS:
			_plot_ptr->drawLine (vx - 2, vy, vx + 2, vy);
			_plot_ptr->drawLine (vx, vy - 2, vx, vy + 2);
			break;

		case	DotStyle_SQUARE:
			{
				const PsCoord	x_conv [4] = { vx - 2, vx + 2, vx + 2, vx - 2 };
				const PsCoord	y_conv [4] = { vy - 2, vy - 2, vy + 2, vy + 2 };
				const bool		save_closepath_flag = _plot_ptr->is_path_closed ();
				_plot_ptr->set_close_path (true);
				_plot_ptr->drawLines (
					&x_conv [0],
					&y_conv [0],
					4,
					0.02f,
					_x,
					_y,
					_x + _w,
					_y + _h
				);
				_plot_ptr->set_close_path (save_closepath_flag);
			}
			break;

		default:
			assert (false);
			break;
		}
	}
}



/*
==============================================================================
Name: put_annotation
Description:
	Prints some text on the graph. Text is horizontally centered, and aligned
	to bottom.
Input parameters:
	- x: Coordinates of the point, in the horizontal axis scale.
	- y: Coordinates of the point, in the vertical axis scale.
	- txt_0: Text to display, with trailing '\0'.
Throws: std::vector related exceptions
==============================================================================
*/

void	Gridaxis::put_annotation (double x, double y, const char txt_0 []) const
{
	assert (_plot_ptr != 0);
	assert (txt_0 != 0);

	convert_coordinates (x, y);

	const double		x_err = 20;
	const double		y_err = 10;

	if (   x >= _x - x_err && x < _x + _w + x_err
	    && y >= _y - y_err && y < _y + _h + y_err)
	{
		y += 4;
		_plot_ptr->drawText (float (x), float (y), txt_0, 0.5f, 0.0f);
	}
}



/*
==============================================================================
Name: put_annotation_pos
Description:
	Same as put_annotation(), but coordinates are relative to the graph
	([0 ; 1] is inside the box).
Input parameters:
	- x: Coordinates of the point, relative to the graph.
	- y: Coordinates of the point, relative to the graph.
	- txt_0: Text to display, with trailing '\0'.
Throws: std::vector related exceptions
==============================================================================
*/

void	Gridaxis::put_annotation_pos (double x, double y, const char txt_0 []) const
{
	assert (_plot_ptr != 0);
	assert (txt_0 != 0);

	// Into PS coordinates
	x = x * _w + _x;
	y = y * _h + _y;

	const double		x_err = 20;
	const double		y_err = 10;

	if (   x >= _x - x_err && x < _x + _w + x_err
	    && y >= _y - y_err && y < _y + _h + y_err)
	{
		y += 4;
		_plot_ptr->drawText (float (x), float (y), txt_0, 0.5f, 0.0f);
	}
}



/*
==============================================================================
Name: conv_nbr_2_str
Description:
	Convert a number into a string. Value is displayed with thousand powers,
	for example 100000 gives "100k" and 1.57e-8 gives "15.7n"
Input parameters:
	- val: Value to display.
Output parameters:
	- txt_0: String where to put the result. Memory should be already allocated
		with enough space to display the value. '\0' added to the end.
Throws: Nothing
==============================================================================
*/

void	Gridaxis::conv_nbr_2_str (char txt_0 [], double val)
{
	assert (txt_0 != 0);

	if (val == 0)
	{
		txt_0 [0] = '0';
		txt_0 [1] = '\0';
	}

	else
	{
		using namespace std;

		const double	p = log10 (fabs (val)) / 3;
		if (   (p >= -4 && p < -1.0/3)
		    || (p >=  1 && p < 4+1))
		{
			static const char *	mult_0 [] =
			{
				"p", "n", "u", "m",
				"",
				"k", "M", "T", "P"
			};
			const int	m = static_cast <int> (floor (p));
			val /= pow (1000.0, static_cast <double> (m));
			sprintf (txt_0, "%.3g%s", val, mult_0 [m + 4]);
		}

		else
		{
			sprintf (txt_0, "%.3g", val);
		}
	}
}




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Gridaxis::render_grid () const
{
	assert (_plot_ptr != 0);

	_plot_ptr->setLineCap (0);
	_plot_ptr->setLineWidth (0.25);
	_plot_ptr->setDash (1.0, 3.0);

	// Vertical axis (horizontal lines)
	{
		const Axis &	axis = use_axis (Direction_V);
		for (long it = axis.tick_begin (); it < axis.tick_end (); ++it)
		{
			const double	pos = axis.get_tick_pos (it);
			const double	line_y = _y + pos * _h;
			_plot_ptr->drawLine (_x, float (line_y), _x + _w, float (line_y));
		}
	}

	// Horizontal axis (vertical lines)
	{
		const Axis &	axis = use_axis (Direction_H);
		for (long it = axis.tick_begin (); it < axis.tick_end (); ++it)
		{
			const double	pos = axis.get_tick_pos (it);
			const double	line_x = _x + pos * _w;
			_plot_ptr->drawLine (float (line_x), _y, float (line_x), _y + _h);
		}
	}
}



void	Gridaxis::render_axis_v () const
{
	assert (_plot_ptr != 0);

	_plot_ptr->setLineCap (0);
	_plot_ptr->setFontSize (8);
	_plot_ptr->setLineWidth (0.5);
	_plot_ptr->setNoDash ();

	const Axis &	axis = use_axis (Direction_V);
	const bool		log_flag = axis.is_log_scale ();
	for (long it = axis.tick_begin (); it < axis.tick_end (); ++it)
	{
		using namespace std;

		const double	pos = axis.get_tick_pos (it);
		const double	line_y = _y + pos * _h;
		_plot_ptr->drawLine (_x, float (line_y), _x - 2, float (line_y));

		bool				print_flag = true;
		if (log_flag)
		{
			const double	tens = floor (static_cast <double> (it) / 9);
			const long		units = it - static_cast <long> (tens) * 9;
			print_flag = (units == 0);
		}
		if (print_flag)
		{
			const double	val = axis.get_tick_val (it);
			char				txt_0 [31+1];
			conv_nbr_2_str (txt_0, val);
			_plot_ptr->drawText (_x - 4, float (line_y), txt_0, 1, 0.5);
		}
	}

	_plot_ptr->drawText (
		_x - 20,
		_y + _h * 0.5f,
		axis.get_name ().c_str (),
		0.5f,
		0,
		90
	);
}



void	Gridaxis::render_axis_h () const
{
	assert (_plot_ptr != 0);

	_plot_ptr->setLineCap (0);
	_plot_ptr->setFontSize (8);
	_plot_ptr->setLineWidth (0.5);
	_plot_ptr->setNoDash ();

	const Axis &	axis = use_axis (Direction_H);
	const bool		log_flag = axis.is_log_scale ();
	for (long it = axis.tick_begin (); it < axis.tick_end (); ++it)
	{
		using namespace std;

		const double	pos = axis.get_tick_pos (it);
		const double	line_x = _x + pos * _w;
		_plot_ptr->drawLine (float (line_x), _y, float (line_x), _y - 2);

		bool				print_flag = true;
		if (log_flag)
		{
			const double	tens = floor (static_cast <double> (it) / 9);
			const long		units = it - static_cast <long> (tens) * 9;
			print_flag = (units == 0);
		}
		if (print_flag)
		{
			const double	val = axis.get_tick_val (it);
			char				txt_0 [31+1];
			conv_nbr_2_str (txt_0, val);
			_plot_ptr->drawText (float (line_x), _y - 4, txt_0, 0.5f, 1);
		}
	}

	_plot_ptr->drawText (
		_x + _w * 0.5f,
		_y - 12,
		axis.get_name ().c_str (),
		0.5f,
		1
	);
}



void	Gridaxis::convert_coordinates (double &x, double &y) const
{
	assert (&x != 0);
	assert (&y != 0);

	const Axis &	x_axis = use_axis (Direction_H);
	const Axis &	y_axis = use_axis (Direction_V);

	// Into [0 ; 1] range
	x = x_axis.conv_val_to_pos (x);
	y = y_axis.conv_val_to_pos (y);

	// Into PS coordinates
	x = x * _w + _x;
	y = y * _h + _y;
}



void	Gridaxis::convert_coordinates (PsCoordArr &x_conv, PsCoordArr &y_conv, const double x_arr [], const double y_arr [], long nbr_points) const
{
	assert (&x_conv != 0);
	assert (&y_conv != 0);
	assert (x_arr != 0);
	assert (y_arr != 0);
	assert (nbr_points > 0);

	x_conv.resize (nbr_points);
	y_conv.resize (nbr_points);

	const Axis &	x_axis = use_axis (Direction_H);
	const Axis &	y_axis = use_axis (Direction_V);

	for (int vtx = 0; vtx < nbr_points; ++vtx)
	{
		// Into [0 ; 1] range
		const double	x_pos = x_axis.conv_val_to_pos (x_arr [vtx]);
		const double	y_pos = y_axis.conv_val_to_pos (y_arr [vtx]);

		// Into PS coordinates
		x_conv [vtx] = static_cast <PsCoord> (_x + x_pos * _w);
		y_conv [vtx] = static_cast <PsCoord> (_y + y_pos * _h);
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
