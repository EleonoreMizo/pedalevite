/*****************************************************************************

        InterpPhaseFpu.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpPhaseFpu_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpPhaseFpu_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include	<cassert>



#if defined (_MSC_VER)
	#pragma inline_depth (255)
#endif



namespace mfx
{
namespace dsp
{
namespace rspl
{



template <int REM>
class InterpPhaseFpu_Util
{
public:
	static fstb_FORCEINLINE void
	               sum_start (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept;
	static fstb_FORCEINLINE void
	               sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept;

	static fstb_FORCEINLINE void
	               lerp_imp (float q, float lerp_ptr [], const float imp_ptr [], const float dif_ptr []) noexcept;
	static fstb_FORCEINLINE void
	               sum_start (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept;
	static fstb_FORCEINLINE void
	               sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept;
};



template <int REM>
inline void	InterpPhaseFpu_Util <REM>::sum_start (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept
{
	assert (fir_len == REM);

	c_0 = (imp_ptr [0] + dif_ptr [0] * q) * data_ptr [0];
	c_1 = (imp_ptr [1] + dif_ptr [1] * q) * data_ptr [1];

	InterpPhaseFpu_Util <REM - 2>::sum_rec (
		fir_len, c_0, c_1, data_ptr, q, imp_ptr, dif_ptr
	);
}

template <>
inline void	InterpPhaseFpu_Util <1>::sum_start (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept
{
	fstb::unused (fir_len);
	assert (fir_len == 1);

	c_0 = (imp_ptr [0] + dif_ptr [0] * q) * data_ptr [0];
	c_1 = 0;
}



template <int REM>
inline void	InterpPhaseFpu_Util <REM>::sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept
{
	const int		pos = fir_len - REM;
	c_0 += (imp_ptr [pos+0] + dif_ptr [pos+0] * q) * data_ptr [pos+0];
	c_1 += (imp_ptr [pos+1] + dif_ptr [pos+1] * q) * data_ptr [pos+1];

	InterpPhaseFpu_Util <REM - 2>::sum_rec (
		fir_len, c_0, c_1, data_ptr, q, imp_ptr, dif_ptr
	);
}

template <>
inline void	InterpPhaseFpu_Util <1>::sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept
{
	fstb::unused (c_1);

	c_0 += (imp_ptr [fir_len-1] + dif_ptr [fir_len-1] * q) * data_ptr [fir_len-1];
}

template <>
inline void	InterpPhaseFpu_Util <0>::sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], float q, const float imp_ptr [], const float dif_ptr []) noexcept
{
	fstb::unused (fir_len, c_0, c_1, data_ptr, q, imp_ptr, dif_ptr);
	// Nothing, stops the recursion
}



template <int REM>
inline void	InterpPhaseFpu_Util <REM>::lerp_imp (float q, float lerp_ptr [], const float imp_ptr [], const float dif_ptr []) noexcept
{
	InterpPhaseFpu_Util <REM - 1>::lerp_imp (q, lerp_ptr, imp_ptr, dif_ptr);
	lerp_ptr [REM - 1] = imp_ptr [REM - 1] + dif_ptr [REM - 1] * q;
}

template <>
inline void	InterpPhaseFpu_Util <0>::lerp_imp (float q, float lerp_ptr [], const float imp_ptr [], const float dif_ptr []) noexcept
{
	fstb::unused (q, lerp_ptr, imp_ptr, dif_ptr);
	// Nothing, stops the recursion
}



template <int REM>
inline void	InterpPhaseFpu_Util <REM>::sum_start (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept
{
	assert (fir_len == REM);

	c_0 = lerp_ptr [0] * data_ptr [0];
	c_1 = lerp_ptr [1] * data_ptr [1];

	InterpPhaseFpu_Util <REM - 2>::sum_rec (
		fir_len, c_0, c_1, data_ptr, lerp_ptr
	);
}

template <>
inline void	InterpPhaseFpu_Util <1>::sum_start (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept
{
	fstb::unused (fir_len);
	assert (fir_len == 1);

	c_0 = lerp_ptr [0] * data_ptr [0];
	c_1 = 0;
}



template <int REM>
inline void	InterpPhaseFpu_Util <REM>::sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept
{
	const int		pos = fir_len - REM;
	c_0 += lerp_ptr [pos+0] * data_ptr [pos+0];
	c_1 += lerp_ptr [pos+1] * data_ptr [pos+1];

	InterpPhaseFpu_Util <REM - 2>::sum_rec (
		fir_len, c_0, c_1, data_ptr, lerp_ptr
	);
}

template <>
inline void	InterpPhaseFpu_Util <1>::sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept
{
	fstb::unused (c_1);

	c_0 += lerp_ptr [fir_len-1] * data_ptr [fir_len-1];
}

template <>
inline void	InterpPhaseFpu_Util <0>::sum_rec (const int fir_len, float &c_0, float &c_1, const float data_ptr [], const float lerp_ptr []) noexcept
{
	fstb::unused (fir_len, c_0, c_1, data_ptr, lerp_ptr);
	// Nothing, stops the recursion
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int PL>
void	InterpPhaseFpu <PL>::set_data (int pos, float imp, float dif) noexcept
{
	assert (pos >= 0);
	assert (pos < PHASE_LEN);

	_imp [pos] = imp;
	_dif [pos] = dif;
}



template <int PL>
void	InterpPhaseFpu <PL>::precompute_impulse (Buffer &imp, float q) const noexcept
{
	assert (q >= 0);
	assert (q <= 1);

	InterpPhaseFpu_Util <PHASE_LEN>::lerp_imp (q, &imp [0], &_imp [0], &_dif [0]);
}



template <int PL>
float	InterpPhaseFpu <PL>::convolve (const float data_ptr [], const Buffer &imp) const noexcept
{
	assert (data_ptr != nullptr);

	float          c_0;
	float          c_1;

	InterpPhaseFpu_Util <PHASE_LEN>::sum_start (PHASE_LEN, c_0, c_1, data_ptr, &imp [0]);

	return c_0 + c_1;
}



template <int PL>
float	InterpPhaseFpu <PL>::convolve (const float data_ptr [], float q) const noexcept
{
	assert (_imp [0] != float (_chk_impulse_not_set));
	assert (data_ptr != nullptr);
	assert (q >= 0);
	assert (q <= 1);

	float          c_0;
	float          c_1;

#if 1

	InterpPhaseFpu_Util <PHASE_LEN>::sum_start (
		PHASE_LEN, c_0, c_1, data_ptr, q, &_imp [0], &_dif [0]
	);

#else

	// Naive implementation (slow)
	c_0 = 0;
	c_1 = 0;
	for (int k = 0; k < PHASE_LEN - 1; k += 2)
	{
		c_0 += (_imp [k+0] + _dif [k+0] * q) * data_ptr [k+0];
		c_1 += (_imp [k+1] + _dif [k+1] * q) * data_ptr [k+1];
	}
	if ((PHASE_LEN & 1) != 0)
	{
		c_0 += (_imp [PHASE_LEN-1] + _dif [PHASE_LEN-1] * q) * data_ptr [PHASE_LEN-1];
	}

#endif

	return c_0 + c_1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpPhaseFpu_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
