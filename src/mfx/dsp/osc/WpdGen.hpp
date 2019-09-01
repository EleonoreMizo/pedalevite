/*****************************************************************************

        WpdGen.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_WpdGen_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_WpdGen_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// T: a WavetablePolyphaseData for impulse storage
// sample_scale is for integer output data (typically 0x4000 for int16_t).
template <class T>
void	WpdGen::build_blep_pure (T &steptable, wnd::WndInterface <double> &win_gen, float sample_scale)
{
	typedef T Steptable;

	assert (&steptable != 0);
	assert (sample_scale != 0);

	const int      table_sz = Steptable::TABLE_SIZE;
	assert ((table_sz & 1) == 0);
	assert (table_sz >= 2);

	const int      table_sz_h = table_sz >> 1;
	std::vector <double> temp_steptable (table_sz);

	const int      ovr_l2 = fstb::limit (14 - Steptable::NBR_PHASES_LOG2, 2, 8);
	const int      ovr    = 1 << ovr_l2;  // Oversampling, for integration
	const int      table_ovr_sz   = table_sz * ovr;
	const int      table_ovr_sz_h = table_ovr_sz >> 1;
	std::vector <double> win_arr (table_ovr_sz);

	// Generates the window, centered on table_ovr_sz / 2
	win_arr [0] = 0;
	win_gen.make_win (&win_arr [1], table_ovr_sz - 1);

	// Integrated sinc
	const double   phase_mult = 1.0 / (ovr * Steptable::NBR_PHASES);
	double         prev_integ_val = 0;
	for (int pos = 0; pos < table_sz; ++pos)
	{
		const int      pos_ovr_base = pos * ovr;
		for (int pos_frac = 0; pos_frac < ovr; ++pos_frac)
		{
			const int      pos_ovr   = pos_ovr_base + pos_frac;
			const double   win_val   = win_arr [pos_ovr];
			const double   sinc_pos  = (pos_ovr - table_ovr_sz_h) * phase_mult;
			const double   val       = fstb::sinc (sinc_pos) * win_val;
			const double   integ_val = prev_integ_val + val;
			prev_integ_val = integ_val;
			if (pos_frac == 0)
			{
				temp_steptable [pos] = integ_val * phase_mult;
			}
		}
	}

	// Last value may not be exactly 1 because of the truncated impulse and
	// the windowing, so we have to fix the scale of the whole step.
	// Then we subtract the naive step.
	const double   scale = 1.0 / (prev_integ_val * phase_mult);
	for (int pos = 0; pos < table_sz; ++pos)
	{
		double         val = temp_steptable [pos];
		val *= scale;

		const double   discont = (pos < table_sz_h) ? 0.0 : 1.0;
		val -= discont;

		steptable.set_sample (
			pos,
			static_cast <typename Steptable::DataType> (val * sample_scale)
		);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_WpdGen_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
