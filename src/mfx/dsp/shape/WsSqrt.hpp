/*****************************************************************************

        WsSqrt.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsSqrt_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsSqrt_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Vs32.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	WsSqrt::process_sample (float x) noexcept
{
	Combo          c;
	c._f = x;
	int32_t        x_int   = c._i;
	const int32_t  exp_int = x_int & _e_mask;
	if (exp_int <= _e_lsb)
	{
		x_int = 0;
	}
	else
	{
		x_int >>= 1;
		x_int  &= _s_fix;
		x_int  += _e_add;
	}
	c._i = x_int;

	return c._f;
}



template <typename VD, typename VS>
void	WsSqrt::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     exp_mask = fstb::Vs32 (_e_mask);
	const auto     exp_lsb  = fstb::Vs32 (_e_lsb);
	const auto     exp_add  = fstb::Vs32 (_e_add);
	const auto     sign_fix = fstb::Vs32 (_s_fix);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x_int   = VS::load_s32 (src_ptr + pos);
		auto           exp_int = x_int & exp_mask;
		const auto     cond    = (exp_int > exp_lsb);
		x_int >>= 1;
		x_int  &= sign_fix;
		x_int  += exp_add;
		x_int  &= cond;
		VD::store_s32 (dst_ptr + pos, x_int);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsSqrt_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
