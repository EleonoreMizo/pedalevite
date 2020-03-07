/*****************************************************************************

        DrawShapers.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/pi/cdsp/ShaperBag.h"
#include "mfx/dsp/shape/WsAsym1.h"
#include "mfx/dsp/shape/WsBadmood.h"
#include "mfx/dsp/shape/WsBelt.h"
#include "mfx/dsp/shape/WsBitcrush.h"
#include "mfx/dsp/shape/WsHardclip.h"
#include "mfx/dsp/shape/WsLight1.h"
#include "mfx/dsp/shape/WsLight2.h"
#include "mfx/dsp/shape/WsLight3.h"
#include "mfx/dsp/shape/WsRcp1.h"
#include "mfx/dsp/shape/WsRcp2.h"
#include "mfx/dsp/shape/WsSqrt.h"
#include "test/DrawShapers.h"
#include "test/EPSPlot.h"
#include "test/Gridaxis.h"

#include <algorithm>

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DrawShapers::draw_shapers ()
{
	typedef fstb::DataAlign <true> DA;

	int            ret_val = 0;

	mfx::pi::cdsp::ShaperBag &	sb = mfx::pi::cdsp::ShaperBag::use ();
	const std::string path ("results/shaper-");

	draw_shaper (sb._s_atan     , path + "atan.eps"     , -20.0f, 20.0f);
	draw_shaper (sb._s_tanh     , path + "tanh.eps"     ,  -4.0f,  4.0f);
	draw_shaper (sb._s_asinh    , path + "asinh.eps"    , -256.0f, 256.0f);
	draw_shaper (sb._s_diode    , path + "diode.eps"    , -20.0f, 20.0f);
	draw_shaper (sb._s_prog1    , path + "prog1.eps"    , -10.0f, 10.0f);
	draw_shaper (sb._s_prog2    , path + "prog2.eps"    , -20.0f, 20.0f);
	draw_shaper (sb._s_puncher1 , path + "puncher1.eps" , -20.0f, 20.0f);
	draw_shaper (sb._s_puncher2 , path + "puncher2.eps" , -20.0f, 20.0f);
	draw_shaper (sb._s_puncher3 , path + "puncher3.eps" ,  -8.0f,  8.0f);
	draw_shaper (sb._s_overshoot, path + "overshoot.eps",  -8.0f,  8.0f);
	draw_shaper (sb._s_lopsided , path + "lopsided.eps" ,  -8.0f,  8.0f);
	draw_shaper (sb._s_smarte1  , path + "smarte1.eps"  , -16.0f, 16.0f);
	draw_shaper (sb._s_smarte2  , path + "smarte2.eps"  ,  -8.0f,  8.0f);
	draw_shaper (sb._s_tanhlin  , path + "tanhlin.eps"  ,  -4.0f,  4.0f);
	draw_shaper (sb._s_break    , path + "break.eps"    ,  -4.0f,  4.0f);
	draw_shaper (sb._s_asym2    , path + "asym2.eps"    , -128.0f, 8.0f);

	draw_func (mfx::dsp::shape::WsAsym1::process_block <DA, DA>   , path + "asym1.eps"   , -2.0f, 2.0f);
	draw_func (mfx::dsp::shape::WsRcp1::process_block <DA, DA>    , path + "rcp1.eps"    , -20.0f, 20.0f);
	draw_func (mfx::dsp::shape::WsRcp2::process_block <DA, DA>    , path + "rcp2.eps"    , -10.0f, 10.0f);
	draw_func (mfx::dsp::shape::WsHardclip::process_block <DA, DA>, path + "hardclip.eps", -2.0f, 2.0f);
	draw_func (mfx::dsp::shape::WsBitcrush::process_block <DA, DA>, path + "bitcrush.eps", -2.0f, 2.0f);
	draw_func (mfx::dsp::shape::WsSqrt::process_block <DA, DA>    , path + "sqrt.eps"    , -6.0f, 6.0f);
	draw_func (mfx::dsp::shape::WsBelt::process_block <DA, DA>    , path + "belt.eps"    , -6.0f, 6.0f);
	draw_func (mfx::dsp::shape::WsBadmood::process_block <DA, DA> , path + "badmood.eps" , -6.0f, 6.0f);
	draw_func (mfx::dsp::shape::WsLight1::process_block <DA, DA>  , path + "light1.eps"  , -6.0f, 6.0f);
	draw_func (mfx::dsp::shape::WsLight2::process_block <DA, DA>  , path + "light2.eps"  , -6.0f, 6.0f);
	draw_func (mfx::dsp::shape::WsLight3::process_block <DA, DA>  , path + "light3.eps"  , -6.0f, 6.0f);

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename S>
int	DrawShapers::draw_shaper (S &shaper, std::string pathname, float xmin, float xmax)
{
	const BufAlign src_arr { create_src (xmin, xmax) };
	BufAlign       dst_arr (src_arr.size ());

	for (int pos = 0; pos < _nbr_spl; ++pos)
	{
		dst_arr [pos] = shaper (src_arr [pos]);
	}

	return save_shaper_drawing (pathname, src_arr, dst_arr);
}



template <typename F>
int	DrawShapers::draw_func (F fnc, std::string pathname, float xmin, float xmax)
{
	const BufAlign src_arr { create_src (xmin, xmax) };
	BufAlign       dst_arr (src_arr.size ());

	fnc (dst_arr.data (), src_arr.data (), int (dst_arr.size ()));

	return save_shaper_drawing (pathname, src_arr, dst_arr);
}



DrawShapers::BufAlign	DrawShapers::create_src (float xmin, float xmax)
{
	const int      nbr_spl_align = (_nbr_spl + 3) & ~3;
	BufAlign       src_arr (nbr_spl_align);

	const float    step = (xmax - xmin) / float (_nbr_spl - 1);
	for (int pos = 0; pos < nbr_spl_align; ++pos)
	{
		src_arr [pos] = xmin + step * pos;
	}

	return src_arr;
}



int	DrawShapers::save_shaper_drawing (std::string pathname, const BufAlign &x_arr, const BufAlign &y_arr)
{
	int            ret_val = 0;

	const size_t   nbr_points = size_t (_nbr_spl);

	const double   margin_x = 20;
	const double   margin_y = 20;
	const double   width    = 576;
	const double   height   = 256;
	EPSPlot        plot (
		pathname.c_str (),
		0, 0,
		float (width),
		float (height)
	);

	const float    xmin = x_arr [0];
	const float    xmax = x_arr [_nbr_spl - 1];
	float          ymin = 0;
	float          ymax = 0;
	find_boundaries (ymin, ymax, y_arr);
	ymin = std::min (ymin, -1.f);
	ymax = std::max (ymax, +1.f);

	const double   tick_x =
		  ((xmax - xmin) <   4.1) ?  0.2
		: ((xmax - xmin) <  10.1) ?  0.5
		: ((xmax - xmin) <  20.1) ?  1
		: ((xmax - xmin) <  40.1) ?  2
		: ((xmax - xmin) < 100.1) ?  5
		: ((xmax - xmin) < 200.1) ? 10
		: ((xmax - xmin) < 400.1) ? 20
		:                           50;
	const double   tick_y =
		  ((ymax - ymin) <   2.1) ?  0.2
		: ((ymax - ymin) <   5.1) ?  0.5
		: ((ymax - ymin) <  10.1) ?  1
		: ((ymax - ymin) <  20.1) ?  2
		:                            5;

	Gridaxis			grid;
	grid.set_size (float (width - margin_x * 2), float (height - margin_y * 2));
	grid.set_plot (plot, float (margin_x), float (margin_y));
	grid.set_grid (true);
	grid.use_axis (Gridaxis::Direction_H).set_scale (xmin, xmax);
	grid.use_axis (Gridaxis::Direction_H).set_tick_dist (tick_x);
	grid.use_axis (Gridaxis::Direction_H).activate_tick (true);
	grid.use_axis (Gridaxis::Direction_V).set_scale (ymin, ymax);
	grid.use_axis (Gridaxis::Direction_V).set_tick_dist (tick_y);
	grid.use_axis (Gridaxis::Direction_V).activate_tick (true);
	grid.render_background ();

	plot.setFontSize (8);
	plot.setLineWidth (1.0);

	std::vector <double> xd_arr (nbr_points);
	std::vector <double> yd_arr (nbr_points);
	for (size_t pos = 0; pos < nbr_points; ++pos)
	{
		xd_arr [pos] = x_arr [pos];
		yd_arr [pos] = y_arr [pos];
	}
	grid.render_curve (xd_arr.data (), yd_arr.data (), long (nbr_points));

	return ret_val;
}



void	DrawShapers::find_boundaries (float &xmin, float &xmax, const BufAlign &x_arr)
{
	xmin = +1e30f;
	xmax = -1e30f;

	for (int pos = 0; pos < _nbr_spl; ++pos)
	{
		const float    x = x_arr [pos];
		xmin = std::min (xmin, x);
		xmax = std::max (xmax, x);
	}

	xmax = float (ceil ( xmax * 5 + 0.25f) / 5);
	xmin = float (floor (xmin * 5 - 0.25f) / 5);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
