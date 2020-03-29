/*****************************************************************************

        HardclipBl.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/shape/HardclipBl.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	HardclipBl::set_clip_lvl (float lvl)
{
	assert (lvl > 0);

	_lvl = lvl;
}



float	HardclipBl::process_sample (float x)
{

#if mfx_dsp_shape_HardclipBl_ALGO == 0 // Original algorithm

	_x_arr [0] = x;

	// Implements clipping and detects clipping points (corners)
	_y_arr [0] = x;
	const bool     clip_flag = (fabs (x) > _lvl);
	if (clip_flag)
	{
		_y_arr [0] = std::copysign (_lvl, x);
	}
	_clip_flag_arr [0] = clip_flag;

	// If clipping point then implements correction
	if (_clip_flag_arr [1] != _clip_flag_arr [0])
	{
		_req_clip_flag = true;
	}
	else if (_req_clip_flag)
	{
		_req_clip_flag = false;

		// Performs polynomial interpolation around clipping boundaries
		const float    p_a =  (-1.f/6) * _x_arr [3] +  0.5f   *_x_arr [2] - 0.5f * _x_arr [1] + (1.f/6) * _x_arr [0];
		const float    p_b =             _x_arr [3] - (5.f/2) *_x_arr [2] + 2    * _x_arr [1] -  0.5f   * _x_arr [0];
		const float    p_c = (-11.f/6) * _x_arr [3] +  3      *_x_arr [2] - 1.5f * _x_arr [1] + (1.f/3) * _x_arr [0];
		const float    p_e =             _x_arr [3];
		const float    pa_3 = p_a * 3;
		const float    pb_2 = p_b * 2;

		// Performs Newton-Raphson to estimate clipping point
		float          xd      = 1.5f;
		const float    cv      = std::copysign (_lvl, _x_arr [1]); // Clipping value
		const float    max_dif = 1e-6f;
		for (int nbr_it = 0; nbr_it < _max_nbr_it; ++nbr_it)
		{
			const float   f0  = ((p_a * xd + p_b) * xd + p_c ) * xd + p_e - cv;
			const float   f1  = (           pa_3  * xd + pb_2) * xd + p_c;
			const float   dif = f0 / f1;
			if (fabs (dif) < max_dif)
			{
				break;
			}
			xd -= dif;
		}

		// Estimates slope at clipping point
		const float    mu = fabs ((pa_3 * xd + pb_2) * xd + p_c);

		// Fractional delay required to center polyBLAMP
		const float    d  = xd - 1;

		// Computes polyBLAMP coefficients
		const float    d2 = d  * d;
		const float    d4 = d2 * d2;
		const float    h0 = d4 * (d * -1.f/120 + 1.f/24) + d2 * (d * -1.f/12 + 1.f/12) + (d * -1.f/24 + 1.f/120);
		const float    h1 = d4 * (d *  1.f/40  - 1.f/12) + d2 * (d *         + 1.f/3 ) + (d * -1.f/2  + 7.f/30 );
		const float    h2 = d4 * (d * -1.f/40  + 1.f/24) + d2 * (d * +1.f/12 + 1.f/12) + (d *  1.f/24 + 1.f/120);
		const float    h3 = d4 * (d *  1.f/120         );

		// Superimposes polyBLAMP at clipping point
		const float    smu = std::copysign (mu, _x_arr [1]);
		_y_arr [3] -= h0 * smu;
		_y_arr [2] -= h1 * smu;
		_y_arr [1] -= h2 * smu;
		_y_arr [0] -= h3 * smu;
	}

	// Output sample
	const float    y = _y_arr [3];

	// Update ALL control variables
	_clip_flag_arr [1] = _clip_flag_arr [0];

#else // Modified algorithm

	_x_arr [0] = x;
	_y_arr [0] = fstb::limit (x, -_lvl, +_lvl);

	CrossingList & cxl_0 = _buf_cxl [ _buf_pos                 ];
	CrossingList & cxl_1 = _buf_cxl [(_buf_pos - 1) & _buf_mask];
	find_crossings (cxl_1, cxl_0, _x_arr [2], _x_arr [1], _x_arr [0]);

	for (int cx_cnt = 0; cx_cnt < cxl_1._nbr_cx; ++cx_cnt)
	{
		/*** To do: check a possible improvement: refine the crossing location
		and slope with a few NR iterations on a 3rd-order polynomial. ***/

		const Crossing &  cx = cxl_1._arr [cx_cnt];

		// Estimates slope at clipping point
		// We limit here the amplitude of the correction because it truns into
		// an error when the slope is very high. The limit is arbitrary;
		// something between 4 and 10 looks like an acceptable compromise.
		const float    mu = std::min (fabs (cx._yd), 8.0f);

		// Fractional delay required to center polyBLAMP
		const float    d  = cx._xrel;

		// Computes polyBLAMP coefficients
		const float    d2 = d  * d;
		const float    d4 = d2 * d2;
		const float    h0 = d4 * (d * -1.f/120 + 1.f/24) + d2 * (d * -1.f/12 + 1.f/12) + (d * -1.f/24 + 1.f/120);
		const float    h1 = d4 * (d *  1.f/40  - 1.f/12) + d2 * (d *         + 1.f/3 ) + (d * -1.f/2  + 7.f/30 );
		const float    h2 = d4 * (d * -1.f/40  + 1.f/24) + d2 * (d * +1.f/12 + 1.f/12) + (d *  1.f/24 + 1.f/120);
		const float    h3 = d4 * (d *  1.f/120         );

		// Superimposes polyBLAMP at clipping point
		const float    smu = std::copysign (mu, cx._y);
		_y_arr [3] -= h0 * smu;
		_y_arr [2] -= h1 * smu;
		_y_arr [1] -= h2 * smu;
		_y_arr [0] -= h3 * smu;
	}

	// Output sample
	const float    y = _y_arr [3];

	// Update ALL control variables
	_buf_pos = (_buf_pos + 1) & _buf_mask;

#endif // Algorithms

	_x_arr [3] = _x_arr [2];
	_y_arr [3] = _y_arr [2];
	_x_arr [2] = _x_arr [1];
	_y_arr [2] = _y_arr [1];
	_x_arr [1] = _x_arr [0];
	_y_arr [1] = _y_arr [0];

	return y;
}



void	HardclipBl::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



void	HardclipBl::clear_buffers ()
{
	_x_arr.fill (0);
	_y_arr.fill (0);
#if mfx_dsp_shape_HardclipBl_ALGO == 0
	_req_clip_flag = false;
	_clip_flag_arr.fill (false);
#else
	for (auto &cxl : _buf_cxl)
	{
		cxl._nbr_cx = 0;
	}
	_buf_pos = 0;
#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#if mfx_dsp_shape_HardclipBl_ALGO == 1

// Checks the crossing between y = +/-_lvl and the parabola defined by the
// three points.
// cxm_arr receives the crossing before x0 ( < 0),
// cxp_arr receives the crossing after or at x0 ( >= 0)
void	HardclipBl::find_crossings (CrossingList &cxm, CrossingList &cxp, float xm1, float x0, float xp1) const
{
	// Resets the list of the crossings after x0,
	// and accumulates the crossings before x0
	cxp._nbr_cx = 0;

	// Computes the 2nd-order polynomial interpolating the points
	// f(x)  = a * x^2 + b * x + c
	// f'(x) = 2 * a * x + b
	const float    a = (xp1 + xm1) * 0.5f - x0;
	const float    b = (xp1 - xm1) * 0.5f;
	const float    c = x0;

	// Checks the intersections with y = +_lvl and y = -lvl
	const float    b2  = b * b;
	const float    a_4 = a * 4;
	const float    rcp2a = 0.5f / a;

	for (int y_cnt = 0; y_cnt < 2; ++y_cnt)
	{
		const float    y  = (y_cnt == 0) ? _lvl : -_lvl;
		const float    cy = c - y;
		const float    d  = b2 - a_4 * cy;
		// Excludes d == 0 because this would be a local minimum or maximum;
		// the slope would be 0 anyway.
		if (d > 0)
		{
			// Checks both solutions
			const float   ds = sqrt (d);
			for (int x_cnt = 0; x_cnt < 2; ++x_cnt)
			{
				const float   dsx = (x_cnt == 0) ? ds : -ds;
				float         x   = (dsx - b) * rcp2a;
				if (x >= -0.5f && x < 0.5f)
				{
					const float    yd     = 2 * a * x + b;
					Crossing *     cx_ptr = 0;
					if (x < 0)
					{
						// Makes negative x relative to xm1 instead of x0.
						x += 1;
						assert (cxm._nbr_cx < int (cxm._arr.size ()));
						cx_ptr = &cxm._arr [cxm._nbr_cx];
						++ cxm._nbr_cx;
					}
					else
					{
						assert (cxp._nbr_cx < int (cxp._arr.size ()));
						cx_ptr = &cxp._arr [cxp._nbr_cx];
						++ cxp._nbr_cx;
					}
					assert (x >= 0 && x < 1);
					cx_ptr->_xrel = x;
					cx_ptr->_y    = y;
					cx_ptr->_yd   = yd;
				}
			}
		}
	}
}

#endif



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
