/*****************************************************************************

        Gridaxis.h
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



#if ! defined (Gridaxis_HEADER_INCLUDED)
#define	Gridaxis_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Axis.h"
#include	"GraphPrim.h"

#include	<vector>



class EPSPlot;

class Gridaxis
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Direction
	{
		Direction_H = 0,
		Direction_V,

		Direction_NBR_ELT
	};

	enum DotStyle
	{
		DotStyle_PLUS = 0,
		DotStyle_SQUARE,

		DotStyle_NBR_ELT
	};

						Gridaxis ();
	virtual			~Gridaxis () {}

	void				set_size (float w, float h);
	void				set_plot (EPSPlot &plot, float x, float y);
	void				set_grid (bool grid_flag);

	Axis &			use_axis (Direction dir);
	const Axis &	use_axis (Direction dir) const;
	GraphPrim &		use_prim ();
	const GraphPrim &
						use_prim () const;

	void				render_background () const;
	void				draw_line (double x_1, double y_1, double x_2, double y_2) const;
	void				draw_point (double x, double y) const;
	void				render_curve (const double x_arr [], const double y_arr [], long nbr_points) const;
	void				render_point_set (const double x_arr [], const double y_arr [], long nbr_points) const;
	void				put_annotation (double x, double y, const char txt_0 []) const;
	void				put_annotation_pos (double x, double y, const char txt_0 []) const;

	static void		conv_nbr_2_str (char txt_0 [], double val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	float	PsCoord;
	typedef	std::vector <PsCoord>	PsCoordArr;

	void				render_grid () const;
	void				render_axis_v () const;
	void				render_axis_h () const;
	void				convert_coordinates (double &x, double &y) const;
	void				convert_coordinates (PsCoordArr &x_conv, PsCoordArr &y_conv, const double x_arr [], const double y_arr [], long nbr_points) const;
	void				draw_arrow_head (double x, double y, double arrow_cos, double arrow_sin, double head_size) const;

	Axis				_axis [Direction_NBR_ELT];
	GraphPrim		_prim;
	EPSPlot *		_plot_ptr;
	DotStyle			_dot_style;
	PsCoord			_x;
	PsCoord			_y;
	PsCoord			_w;
	PsCoord			_h;
	bool				_grid_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Gridaxis (const Gridaxis &other);
	Gridaxis &		operator = (const Gridaxis &other);
	bool				operator == (const Gridaxis &other);
	bool				operator != (const Gridaxis &other);

};	// class Gridaxis





//#include	"Gridaxis.hpp"



#endif	// Gridaxis_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
