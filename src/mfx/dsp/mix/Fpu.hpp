/*****************************************************************************

        Fpu.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_mix_Fpu_CODEHEADER_INCLUDED)
#define mfx_dsp_mix_Fpu_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace mix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename OP>
void	Fpu::vec_op_1_1 (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr, int nbr_spl, OP op)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (src_ptr != dst_ptr);
	assert (nbr_spl > 0);

	const int      n4 = nbr_spl & ~3;
	for (int k = 0; k < n4; k += 4)
	{
		const auto     xa = src_ptr [k    ];
		const auto     xb = src_ptr [k + 1];
		const auto     xc = src_ptr [k + 2];
		const auto     xd = src_ptr [k + 3];
		const auto     ya = op (xa);
		const auto     yb = op (xb);
		const auto     yc = op (xc);
		const auto     yd = op (xd);
		dst_ptr [k    ] = ya;
		dst_ptr [k + 1] = yb;
		dst_ptr [k + 2] = yc;
		dst_ptr [k + 3] = yd;
	}
	for (int k = n4; k < nbr_spl; ++k)
	{
		dst_ptr [k] = op (src_ptr [k]);
	}
}



template <typename OP>
void	Fpu::vec_op_2_1 (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT lhs_ptr, const float * fstb_RESTRICT rhs_ptr, int nbr_spl, OP op)
{
	assert (dst_ptr != nullptr);
	assert (lhs_ptr != nullptr);
	assert (rhs_ptr != nullptr);
	assert (lhs_ptr != dst_ptr);
	assert (rhs_ptr != dst_ptr);
	assert (nbr_spl > 0);

	const int      n4 = nbr_spl & ~3;
	for (int k = 0; k < n4; k += 4)
	{
		const auto     xal = lhs_ptr [k    ];
		const auto     xbl = lhs_ptr [k + 1];
		const auto     xcl = lhs_ptr [k + 2];
		const auto     xdl = lhs_ptr [k + 3];
		const auto     xar = rhs_ptr [k    ];
		const auto     xbr = rhs_ptr [k + 1];
		const auto     xcr = rhs_ptr [k + 2];
		const auto     xdr = rhs_ptr [k + 3];
		const auto     ya  = op (xal, xar);
		const auto     yb  = op (xbl, xbr);
		const auto     yc  = op (xcl, xcr);
		const auto     yd  = op (xdl, xdr);
		dst_ptr [k    ] = ya;
		dst_ptr [k + 1] = yb;
		dst_ptr [k + 2] = yc;
		dst_ptr [k + 3] = yd;
	}
	for (int k = n4; k < nbr_spl; ++k)
	{
		dst_ptr [k] = op (lhs_ptr [k], rhs_ptr [k]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_mix_Fpu_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
