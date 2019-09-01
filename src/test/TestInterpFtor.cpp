/*****************************************************************************

        TestInterpFtor.cpp
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
#include "fstb/fnc.h"
#include "mfx/dsp/nz/WhiteFast.h"
#include "mfx/dsp/rspl/InterpFtor.h"
#include "test/TestInterpFtor.h"

#include <array>

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// We assume here that the float->float versions are working references.
int	TestInterpFtor::perform_test ()
{
	int            ret_val = 0;

	mfx::dsp::nz::WhiteFast noise_gen;

	const int      frac_bits = 8;
	const int      nbr_frac  = 1 << frac_bits;
	const uint32_t frac_mul_u32 = 0xFFFFFFFFU / (nbr_frac - 1);
	const float    frac_mul_flt = frac_mul_u32 * float (fstb::TWOPM32);
	const int32_t  scale_int16  = 0x8000;

	typedef mfx::dsp::rspl::InterpFtor::CubicHermite Interp;
	const int      itrp_len = Interp::DATA_PRE + 1 + Interp::DATA_POST;
	Interp         ftor;
	for (int tst_cnt = 0; tst_cnt < 1000 && ret_val == 0; ++tst_cnt)
	{
		std::array <float, itrp_len>   src_flt;
		noise_gen.process_block (&src_flt [0], int (src_flt.size ()));

		std::array <int16_t, itrp_len> src_i16;
		for (int pos = 0; pos < itrp_len; ++pos)
		{
			const float    val = src_flt [pos];
			assert (val >= -1);
			assert (val < 1);
			src_i16 [pos] = int16_t (fstb::round_int (val * scale_int16));
		}

		for (int frac_cnt = 0; frac_cnt < nbr_frac && ret_val == 0; ++frac_cnt)
		{
			const uint32_t frac_u32 = frac_cnt * frac_mul_u32;
			const float    frac_flt = frac_cnt * frac_mul_flt;
			const float    res_flt = ftor (frac_flt, &src_flt [Interp::DATA_PRE]);
			const int32_t  res_i32 = ftor (frac_u32, &src_i16 [Interp::DATA_PRE]);

			const float    res_i32f = float (res_i32) * (1.f / scale_int16);
			if (! fstb::is_eq (res_flt, res_i32f, 1e-3f))
			{
				printf ("TestInterpFtor: wrong result.\n");
				ret_val = -1;
			}
		}
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
