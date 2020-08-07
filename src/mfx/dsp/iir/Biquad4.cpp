/*****************************************************************************

        Biquad4.cpp
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

#include "mfx/dsp/iir/Biquad4.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Biquad4::Biquad4 ()
:	_biq_arr ()
{
	neutralise ();
	clear_buffers ();
}



void	Biquad4::neutralise ()
{
	for (auto &biq : _biq_arr)
	{
		biq.neutralise ();
	}
}



void	Biquad4::neutralise_one (int biq)
{
	assert (biq >= 0);
	assert (biq < _nbr_units);

	_biq_arr [biq].neutralise ();
}



void	Biquad4::set_z_eq (const VectFloat4 b [3], const VectFloat4 a [3])
{
	assert (b != nullptr);
	assert (a != nullptr);

	for (int cnt = 0; cnt < _nbr_units; ++cnt)
	{
		const float    sb [3] = { b [0] [cnt], b [1] [cnt], b [2] [cnt] };
		const float    sa [3] = { 1          , a [1] [cnt], a [2] [cnt] };
		_biq_arr [cnt].set_z_eq (sb, sa);
	}
}



void	Biquad4::set_z_eq_same (const float b [3], const float a [3])
{
	assert (b != nullptr);
	assert (a != nullptr);

	for (auto &biq : _biq_arr)
	{
		biq.set_z_eq (b, a);
	}
}



void	Biquad4::set_z_eq_one (int biq, const float b [3], const float a [3])
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (b != nullptr);
	assert (a != nullptr);

	_biq_arr [biq].set_z_eq (b, a);
}



void	Biquad4::get_z_eq (VectFloat4 b [3], VectFloat4 a [3]) const
{
	assert (b != nullptr);
	assert (a != nullptr);

	for (int cnt = 0; cnt < _nbr_units; ++cnt)
	{
		float          sb [3];
		float          sa [3];
		_biq_arr [cnt].get_z_eq (sb, sa);
		b [0] [cnt] = sb [0];
		b [1] [cnt] = sb [1];
		b [2] [cnt] = sb [2];
		a [1] [cnt] = sa [1];
		a [2] [cnt] = sa [2];
	}
}



void	Biquad4::get_z_eq_one (int biq, float b [3], float a [3]) const
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (b != nullptr);
	assert (a != nullptr);

	_biq_arr [biq].get_z_eq (b, a);
}



void	Biquad4::copy_z_eq (const Biquad4 &other)
{
	for (int cnt = 0; cnt < _nbr_units; ++cnt)
	{
		_biq_arr [cnt].copy_z_eq (other._biq_arr [cnt]);
	}
}



void	Biquad4::set_state_one (int biq, float const mem_x [2], const float mem_y [2])
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (mem_x != nullptr);
	assert (mem_y != nullptr);

	_biq_arr [biq].set_state (mem_x, mem_y);
}



void	Biquad4::get_state_one (int biq, float mem_x [2], float mem_y [2]) const
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (mem_x != nullptr);
	assert (mem_y != nullptr);

	_biq_arr [biq].get_state (mem_x, mem_y);
}



void	Biquad4::process_block_serial_immediate (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int cnt = 0; cnt < _nbr_units; ++cnt)
	{
		_biq_arr [cnt].process_block (dst_ptr, src_ptr, nbr_spl);
		src_ptr = dst_ptr;
	}
}



void	Biquad4::process_block_2x2_immediate (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		float          x = src_ptr [pos * 2];
		x = _biq_arr [0].process_sample (x);
		x = _biq_arr [2].process_sample (x);
		dst_ptr [pos * 2] = x;
	}

	++ src_ptr;
	++ dst_ptr;

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		float          x = src_ptr [pos * 2];
		x = _biq_arr [1].process_sample (x);
		x = _biq_arr [3].process_sample (x);
		dst_ptr [pos * 2] = x;
	}
}



void	Biquad4::clear_buffers ()
{
	for (auto &biq : _biq_arr)
	{
		biq.clear_buffers ();
	}
}



void	Biquad4::clear_buffers_one (int biq)
{
	assert (biq >= 0);
	assert (biq < _nbr_units);

	_biq_arr [biq].clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
