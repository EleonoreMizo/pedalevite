/*****************************************************************************

        DelayAllPass.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_DelayAllPass_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_DelayAllPass_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <algorithm>
#include <array>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{



template <typename T>
fstb_FORCEINLINE void	DelayAllPass_mac_vec_std (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_1_ptr, const T * fstb_RESTRICT src_2_ptr, T coef, int nbr_spl) noexcept
{
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = src_1_ptr [pos] + src_2_ptr [pos] * coef;
	}
}

template <typename T>
fstb_FORCEINLINE void	DelayAllPass_mac_vec (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_1_ptr, const T * fstb_RESTRICT src_2_ptr, T coef, int nbr_spl) noexcept
{
	assert (dst_ptr != src_1_ptr);
	assert (dst_ptr != src_2_ptr);
	assert (src_1_ptr != src_2_ptr);

	DelayAllPass_mac_vec_std (dst_ptr, src_1_ptr, src_2_ptr, coef, nbr_spl);
}

#if defined (fstb_HAS_SIMD)

template <>
fstb_FORCEINLINE void	DelayAllPass_mac_vec (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_1_ptr, const float * fstb_RESTRICT src_2_ptr, float coef, int nbr_spl) noexcept
{
	const auto        c = fstb::Vf32 (coef);

	const int         len_m16 = nbr_spl & ~15;
	for (int pos = 0; pos < len_m16; pos += 16)
	{
		auto              x0 = fstb::Vf32::loadu (src_1_ptr + pos     );
		auto              x1 = fstb::Vf32::loadu (src_1_ptr + pos +  4);
		auto              x2 = fstb::Vf32::loadu (src_1_ptr + pos +  8);
		auto              x3 = fstb::Vf32::loadu (src_1_ptr + pos + 12);
		const auto        b0 = fstb::Vf32::loadu (src_2_ptr + pos     );
		const auto        b1 = fstb::Vf32::loadu (src_2_ptr + pos +  4);
		const auto        b2 = fstb::Vf32::loadu (src_2_ptr + pos +  8);
		const auto        b3 = fstb::Vf32::loadu (src_2_ptr + pos + 12);
		x0.mac (b0, c);
		x1.mac (b1, c);
		x2.mac (b2, c);
		x3.mac (b3, c);
		x0.storeu (dst_ptr + pos     );
		x1.storeu (dst_ptr + pos +  4);
		x2.storeu (dst_ptr + pos +  8);
		x3.storeu (dst_ptr + pos + 12);
	}

	const int      rem = nbr_spl - len_m16;
	if (rem > 0)
	{
		DelayAllPass_mac_vec_std (
			dst_ptr   + len_m16,
			src_1_ptr + len_m16,
			src_2_ptr + len_m16,
			coef,
			rem
		);
	}
}

#endif // fstb_HAS_SIMD



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_max_len (int len)
{
	_delay.set_max_len (len);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_delay_flt (float len_spl) noexcept
{
	set_delay_fix (fstb::conv_int_fast (len_spl * _nbr_phases));
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_delay_fix (int len_fixp) noexcept
{
	assert (len_fixp >= _delay_min * _nbr_phases);

	_delay.set_delay_fix (len_fixp);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_coef (T coef) noexcept
{
	assert (coef >= T (-1.f));
	assert (coef <= T (+1.f));

	_coef = coef;
}



/*
Returns:
first: allpass output y(n)
second: v(n) to feed to write()

Usage, equivalent to y = process_sample (x):
std::tie (y, v) = read (x);
write (v);
step ();
*/

template <typename T, int NPL2>
std::pair <T, T>	DelayAllPass <T, NPL2>::read (T x) const noexcept
{
	const T        u { _delay.read () };
	const T        v { x - u * _coef };
	const T        y { u + v * _coef };

	return std::make_pair (y, v);
}



template <typename T, int NPL2>
T	DelayAllPass <T, NPL2>::read_at (int delay) const noexcept
{
	return _delay.read_at (delay);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::write (T v) noexcept
{
	_delay.write (v);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::step () noexcept
{
	_delay.step ();
}



template <typename T, int NPL2>
T	DelayAllPass <T, NPL2>::process_sample (T x) noexcept
{
	T              y { _delay.read () };
	x -= y * _coef;
	_delay.write (x);
	y += x * _coef;
	_delay.step ();

	return y;
}



template <typename T, int NPL2>
int	DelayAllPass <T, NPL2>::get_max_block_len () const noexcept
{
	return _delay.get_max_block_len ();
}




template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::read_block_at (T dst_ptr [], int delay, int len) const noexcept
{
	_delay.read_block_at (dst_ptr, delay, len);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept
{
	assert (src_ptr != nullptr);
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= get_max_block_len ());

	constexpr int  buf_len = 128;
	std::array <T, buf_len> buf_x;
	std::array <T, buf_len> buf_y;
	T * fstb_RESTRICT buf_x_ptr = buf_x.data ();
	T * fstb_RESTRICT buf_y_ptr = buf_y.data ();

	int            pos = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, buf_len);

		_delay.read_block (buf_y_ptr, work_len);
		DelayAllPass_mac_vec (
			buf_x_ptr, src_ptr + pos, buf_y_ptr, -_coef, work_len
		);
		_delay.write_block (buf_x_ptr, work_len);
		_delay.step_block (work_len);
		DelayAllPass_mac_vec (
			dst_ptr + pos, buf_y_ptr, buf_x_ptr, +_coef, work_len
		);

		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::process_block_var_dly (T dst_ptr [], const T src_ptr [], const int32_t dly_frc_ptr [], int nbr_spl) noexcept
{
	assert (src_ptr != nullptr);
	assert (dst_ptr != nullptr);
	assert (dly_frc_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= get_max_block_len ());

	constexpr int  buf_len = 128;
	std::array <T, buf_len> buf_x;
	std::array <T, buf_len> buf_y;
	T * fstb_RESTRICT buf_x_ptr = buf_x.data ();
	T * fstb_RESTRICT buf_y_ptr = buf_y.data ();

	int            pos = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, buf_len);

		_delay.read_block_var_dly (buf_y_ptr, dly_frc_ptr + pos, work_len);
		DelayAllPass_mac_vec (
			buf_x_ptr, src_ptr + pos, buf_y_ptr, -_coef, work_len
		);
		_delay.write_block (buf_x_ptr, work_len);
		_delay.step_block (work_len);
		DelayAllPass_mac_vec (
			dst_ptr + pos, buf_y_ptr, buf_x_ptr, +_coef, work_len
		);

		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::clear_buffers () noexcept
{
	_delay.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_DelayAllPass_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
