/*****************************************************************************

        GaussFast.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_nz_GaussFast_CODEHEADER_INCLUDED)
#define mfx_dsp_nz_GaussFast_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace nz
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// lvl is approximately the standard deviation
template <int NIT>
void	GaussFast <NIT>::set_rough_level (float lvl)
{
	_mult = lvl * (1.0f / (1 << _resol));
}



template <int NIT>
float	GaussFast <NIT>::process_sample ()
{
	int32_t        acc = -_sub;
	for (int cnt = 0; cnt < _nbr_it; ++cnt)
	{
		acc += int32_t (_gen () >> (31 - _resol));
	}

	const float    val = float (acc) * _mult;

	return val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_nz_GaussFast_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
