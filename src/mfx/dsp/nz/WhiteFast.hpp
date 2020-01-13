/*****************************************************************************

        WhiteFast.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_nz_WhiteFast_CODEHEADER_INCLUDED)
#define mfx_dsp_nz_WhiteFast_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace nz
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	WhiteFast::process_sample ()
{
	return gen_new_val (_state);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	WhiteFast::gen_new_val (uint32_t &state)
{
	state  = (state * _a + _c) & _mm;

	Mixed          res;
	res._i = ((state << 1) & _mant_and) | _exp_or;

	return res._f;
}



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_nz_WhiteFast_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
