/*****************************************************************************

        SweepingSin.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/def.h"
#include "mfx/dsp/osc/SweepingSin.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SweepingSin::SweepingSin (double fs, double f1, double f2)
:	_fs (fs)
,	_f1 (f1)
,	_f2 (f2)
{
	assert (fs > 0);
   assert (f1 > 0);
	assert (f1 < fs * 0.5);
	assert (f2 > 0);
	assert (f2 < fs * 0.5);
}



void	SweepingSin::generate (float data_ptr [], int nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

   generate (data_ptr, nbr_spl, 0, nbr_spl);
}



void	SweepingSin::generate (float data_ptr [], int nbr_spl, int block_start, int block_len)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);
	assert (block_start >= 0);
	assert (block_len > 0);
   assert (block_start + block_len <= nbr_spl);

	const double   inc_f1 = _f1 * (2 * fstb::PI) / _fs;

	if (_f1 == _f2)
	{
		double         phase = block_start * inc_f1;
		for (int pos = 0; pos < block_len; ++pos)
		{
			data_ptr [pos] = float (sin (phase));
			phase += inc_f1;
		}
	}

	else
	{
		const double   f_log_f1        = log (inc_f1);
		const double   f_log_step      = log (_f2 / _f1) / nbr_spl;
		const double   f_log_block     = f_log_f1 + block_start * f_log_step;
		const double   phase_log_block = f_log_block - log (f_log_step);
		const double   phase_offset    = exp (f_log_f1) / f_log_step;

		for (int pos = 0; pos < block_len; ++pos)
		{
			const double   phase_log = phase_log_block + pos * f_log_step;
			const double   phase     = exp (phase_log) - phase_offset;
			data_ptr [pos] = float (sin (phase));
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
