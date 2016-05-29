/*****************************************************************************

        Biquad.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/Biquad.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Biquad::Biquad ()
:	_eq_z_b ({{ 1, 0, 0 }})
,	_eq_z_a ({{ 0, 0, 0 }})
,	_mem_x ({{ 0, 0 }})
,	_mem_y ({{ 0, 0 }})
,	_mem_pos (0)
{
	// Nothing
}



/*
==============================================================================
Name: process_block
Description:
	Filters a block of samples. Can work in-place.
Input parameters:
	- in_ptr: Input sample block
	- nbr_spl: Number of samples to process. >= 0
Output parameters:
	- out_ptr: Output sample block
Throws: Nothing
==============================================================================
*/

void	Biquad::process_block (float dst_ptr [], const float src_ptr [], long nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	// If we're not on an even boudary, we process a single sample.
	if (_mem_pos != 0)
	{
		*dst_ptr = process_sample (*src_ptr);
		++ src_ptr;
		++ dst_ptr;
		-- nbr_spl;

		if (nbr_spl <= 0)
		{
			return;
		}
	}

	long           half_nbr_spl = nbr_spl >> 1;
	long           index = 0;
	if (half_nbr_spl > 0)
	{
		float          mem_x [2];
		float          mem_y [2];
		mem_x [0] = _mem_x [0];
		mem_x [1] = _mem_x [1];
		mem_y [0] = _mem_y [0];
		mem_y [1] = _mem_y [1];

		do
		{
			// First sample
			float          x = src_ptr [index];
			mem_y [1] =      _eq_z_b [0] *     x
			            + (  _eq_z_b [1] * mem_x [0]
			               + _eq_z_b [2] * mem_x [1])
			            - (  _eq_z_a [1] * mem_y [0]
			               + _eq_z_a [2] * mem_y [1]);

			mem_x [1] = x;
			dst_ptr [index    ] = mem_y [1];

			// Second sample
			x = src_ptr [index + 1];
			mem_y [0] =      _eq_z_b [0] *     x
			            + (  _eq_z_b [1] * mem_x [1]
			               + _eq_z_b [2] * mem_x [0])
			            - (  _eq_z_a [1] * mem_y [1]
			               + _eq_z_a [2] * mem_y [0]);

			mem_x [0] = x;
			dst_ptr [index + 1] = mem_y [0];

			index += 2;
			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		_mem_x [0] = mem_x [0];
		_mem_x [1] = mem_x [1];
		_mem_y [0] = mem_y [0];
		_mem_y [1] = mem_y [1];
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		dst_ptr [index] = process_sample (src_ptr [index]);
	}
}



void	Biquad::process_block (float dst_ptr [], const float src_ptr [], long nbr_spl, const float inc_b [3], const float inc_a [3])
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (inc_b != 0);
	assert (inc_a != 0);

	// If we're not on an even boudary, we process a single sample.
	if (_mem_pos != 0)
	{
		*dst_ptr = process_sample (*src_ptr, inc_b, inc_a);
		++ src_ptr;
		++ dst_ptr;
		-- nbr_spl;

		if (nbr_spl == 0)
		{
			return;
		}
	}

	long				half_nbr_spl = nbr_spl >> 1;
	long				index = 0;
	if (half_nbr_spl > 0)
	{
		float				mem_x [2];
		float				mem_y [2];
		mem_x [0] = _mem_x [0];
		mem_x [1] = _mem_x [1];
		mem_y [0] = _mem_y [0];
		mem_y [1] = _mem_y [1];

		do
		{
			// First sample
			float				x = src_ptr [index];
			mem_y [1] =      _eq_z_b [0] *     x
			            + (  _eq_z_b [1] * mem_x [0]
			               + _eq_z_b [2] * mem_x [1])
			            - (  _eq_z_a [1] * mem_y [0]
			               + _eq_z_a [2] * mem_y [1]);

			mem_x [1] = x;
			dst_ptr [index    ] = mem_y [1];
			step_z_eq (inc_b, inc_a);

			// Second sample
			x = src_ptr [index + 1];
			mem_y [0] =      _eq_z_b [0] *     x
			            + (  _eq_z_b [1] * mem_x [1]
			               + _eq_z_b [2] * mem_x [0])
			            - (  _eq_z_a [1] * mem_y [1]
			               + _eq_z_a [2] * mem_y [0]);

			mem_x [0] = x;
			dst_ptr [index + 1] = mem_y [0];
			step_z_eq (inc_b, inc_a);

			index += 2;

			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		_mem_x [0] = mem_x [0];
		_mem_x [1] = mem_x [1];
		_mem_y [0] = mem_y [0];
		_mem_y [1] = mem_y [1];
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		dst_ptr [index] = process_sample (src_ptr [index], inc_b, inc_a);
	}
}



void	Biquad::clear_buffers ()
{
	_mem_x [0] = 0;
	_mem_x [1] = 0;
	_mem_y [0] = 0;
	_mem_y [1] = 0;
	_mem_pos   = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
