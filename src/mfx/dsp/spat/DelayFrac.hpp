/*****************************************************************************

        DelayFrac.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_DelayFrac_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_DelayFrac_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/rspl/InterpFtor.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{



template <typename T>
static void	DelayFrac_interpolate_block_std (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr, const float * fstb_RESTRICT phase_ptr, int len) noexcept
{
	const T        i0 { phase_ptr [0] };
	const T        i1 { phase_ptr [1] };
	const T        i2 { phase_ptr [2] };
	const T        i3 { phase_ptr [3] };

	T              v0 { src_ptr [0] };
	T              v1 { src_ptr [1] };
	T              v2 { src_ptr [2] };
	const int      len_m4 = len & ~3;
	for (int pos = 0; pos < len_m4; pos += 4)
	{
		const T        v3 { src_ptr [pos + 3] };
		const T        x0 { (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3) };
		v0 = src_ptr [pos + 4];
		const T        x1 { (v1 * i0 + v2 * i1) + (v3 * i2 + v0 * i3) };
		v1 = src_ptr [pos + 5];
		const T        x2 { (v2 * i0 + v3 * i1) + (v0 * i2 + v1 * i3) };
		v2 = src_ptr [pos + 6];
		const T        x3 { (v3 * i0 + v0 * i1) + (v1 * i2 + v2 * i3) };
		dst_ptr [pos    ] = x0;
		dst_ptr [pos + 1] = x1;
		dst_ptr [pos + 2] = x2;
		dst_ptr [pos + 3] = x3;
	}
	for (int pos = len_m4; pos < len; ++pos)
	{
		const T        v3 { src_ptr [pos + 3] };
		dst_ptr [pos] = (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3);
		v0 = v1;
		v1 = v2;
		v2 = v3;
	}
}



template <typename T>
static fstb_FORCEINLINE void	DelayFrac_interpolate_block (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr, const float * fstb_RESTRICT phase_ptr, int len) noexcept
{
	DelayFrac_interpolate_block_std (dst_ptr, src_ptr, phase_ptr, len);
}

#if defined (fstb_HAS_SIMD)

template <>
inline void	DelayFrac_interpolate_block (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr, const float * fstb_RESTRICT phase_ptr, int len) noexcept
{
	const auto     i0 = fstb::ToolsSimd::set1_f32 (phase_ptr [0]);
	const auto     i1 = fstb::ToolsSimd::set1_f32 (phase_ptr [1]);
	const auto     i2 = fstb::ToolsSimd::set1_f32 (phase_ptr [2]);
	const auto     i3 = fstb::ToolsSimd::set1_f32 (phase_ptr [3]);

#if fstb_ARCHI == fstb_ARCHI_ARM

	// len - 1 because vg collects an extra sample
	const int      len_m16 = (len - 1) & ~15;
	auto           v0 = fstb::ToolsSimd::loadu_f32 (src_ptr);
	for (int pos = 0; pos < len_m16; pos += 16)
	{
		const auto     v4 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  4]);
		const auto     v8 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  8]);
		const auto     vc = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 12]);
		const auto     vg = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 16]);
		const auto     v1 = fstb::ToolsSimd::Shift <1>::compose (v0, v4);
		const auto     v2 = fstb::ToolsSimd::Shift <2>::compose (v0, v4);
		const auto     v3 = fstb::ToolsSimd::Shift <3>::compose (v0, v4);
		const auto     v5 = fstb::ToolsSimd::Shift <1>::compose (v4, v8);
		const auto     v6 = fstb::ToolsSimd::Shift <2>::compose (v4, v8);
		const auto     v7 = fstb::ToolsSimd::Shift <3>::compose (v4, v8);
		const auto     v9 = fstb::ToolsSimd::Shift <1>::compose (v8, vc);
		const auto     va = fstb::ToolsSimd::Shift <2>::compose (v8, vc);
		const auto     vb = fstb::ToolsSimd::Shift <3>::compose (v8, vc);
		const auto     vd = fstb::ToolsSimd::Shift <1>::compose (vc, vg);
		const auto     ve = fstb::ToolsSimd::Shift <2>::compose (vc, vg);
		const auto     vf = fstb::ToolsSimd::Shift <3>::compose (vc, vg);
		const auto     x0 = (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3);
		const auto     x4 = (v4 * i0 + v5 * i1) + (v6 * i2 + v7 * i3);
		const auto     x8 = (v8 * i0 + v9 * i1) + (va * i2 + vb * i3);
		const auto     xc = (vc * i0 + vd * i1) + (ve * i2 + vf * i3);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos     ], x0);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos +  4], x4);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos +  8], x8);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos + 12], xc);
		v0 = vg;
	}

#else // fstb_ARCHI

	const int      len_m16 = len & ~15;
	for (int pos = 0; pos < len_m16; pos += 16)
	{
		const auto     v0 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos     ]);
		const auto     v1 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  1]);
		const auto     v2 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  2]);
		const auto     v3 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  3]);
		const auto     v4 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  4]);
		const auto     v5 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  5]);
		const auto     v6 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  6]);
		const auto     v7 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  7]);
		const auto     v8 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  8]);
		const auto     v9 = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos +  9]);
		const auto     va = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 10]);
		const auto     vb = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 11]);
		const auto     vc = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 12]);
		const auto     vd = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 13]);
		const auto     ve = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 14]);
		const auto     vf = fstb::ToolsSimd::loadu_f32 (&src_ptr [pos + 15]);
		const auto     x0 = (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3);
		const auto     x4 = (v4 * i0 + v5 * i1) + (v6 * i2 + v7 * i3);
		const auto     x8 = (v8 * i0 + v9 * i1) + (va * i2 + vb * i3);
		const auto     xc = (vc * i0 + vd * i1) + (ve * i2 + vf * i3);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos     ], x0);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos +  4], x4);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos +  8], x8);
		fstb::ToolsSimd::storeu_f32 (&dst_ptr [pos + 12], xc);
	}

#endif // fstb_ARCHI

	const int      rem = len - len_m16;
	if (rem > 0)
	{
		DelayFrac_interpolate_block_std (
			dst_ptr + len_m16, src_ptr + len_m16, phase_ptr, rem
		);
	}
}

#endif // fstb_HAS_SIMD



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// len = samples
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::set_max_len (int len)
{
	assert (len >= _delay_min);

   if (! _interp_init_flag)
   {
		init_interpolator ();
   }

	const int      buf_len_l2 = fstb::get_next_pow_2 (len + _nbr_phases - 1);
	const int      buf_len    = 1 << buf_len_l2;
	_delay_len = len;
	_buf_msk   = buf_len - 1;
	_buffer.resize (buf_len);

	clear_buffers ();
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::set_delay_flt (float len_spl) noexcept
{
	set_delay_fix (fstb::conv_int_fast (len_spl * _nbr_phases));
}



// len_fixp = samples * _nbr_phases
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::set_delay_fix (int len_fixp) noexcept
{
	assert (len_fixp >= _delay_min * _nbr_phases);

	find_phase_and_delay (_phase_ptr, _delay_int, _delay_frc, len_fixp);
}



// Returns the integer part of the current delay.
template <typename T, int NPL2>
int	DelayFrac <T, NPL2>::get_delay_len_int () const noexcept
{
	assert (_phase_ptr != nullptr);

	return _delay_int;
}



template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::read () const noexcept
{
	assert (_phase_ptr != nullptr);

	return read_safe (_pos_write - _delay_int, *_phase_ptr);
}



// Non-interpolated read at random position
// Not constrained by the current delay length
template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::read_at (int delay) const noexcept
{
	assert (delay > 0);
	assert (delay <= _delay_len);

	return _buffer [(_pos_write - delay) & _buf_msk];
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::write (T x) noexcept
{
	assert (_phase_ptr != nullptr);

	_buffer [_pos_write] = x;
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::step () noexcept
{
	assert (_phase_ptr != nullptr);

	_pos_write = (_pos_write + 1) & _buf_msk;
}



template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::process_sample (T x) noexcept
{
	assert (_phase_ptr != nullptr);

	write (x);
	const T        val { read () };
	step ();

	return val;
}



// Returns the maximum block length when processing with feedback
template <typename T, int NPL2>
int	DelayFrac <T, NPL2>::get_max_block_len () const noexcept
{
	assert (_phase_ptr != nullptr);

	return _delay_int - (_phase_len - 1);
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::read_block (T dst_ptr [], int len) const noexcept
{
	assert (_phase_ptr != nullptr);
	assert (len > 0);
	assert (len <= _delay_len);

	const int      buf_len  = int (_buffer.size ());
	int            pos_read = (_pos_write - _delay_int) & _buf_msk;
	int            pos      = 0; // Position within the block
	do
	{
		const int      room     = buf_len - (pos_read + _ph_post);
		const int      rem      = len - pos;
		int            len_part = std::min (rem, room);

		// Read without pointer wrap
		if (pos_read >= _ph_pre && len_part > 0)
		{
			DelayFrac_interpolate_block (
				dst_ptr + pos,
				&_buffer [pos_read] - _ph_pre,
				_phase_ptr->data (),
				len_part
			);
		}

		// Read with pointer wrap
		else
		{
			len_part = std::min (rem, _phase_len);
			for (int k = 0; k < len_part; ++k)
			{
				dst_ptr [pos + k] = read_safe (pos_read + k, *_phase_ptr);
			}
		}

		pos     += len_part;
		pos_read = (pos_read + len_part) & _buf_msk;
	}
	while (pos < len);
}



// Delay is updated with the last dly_fix_ptr value
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::read_block_var_dly (T dst_ptr [], const int32_t dly_fix_ptr [], int len) noexcept
{
	assert (_phase_ptr != nullptr);
	assert (len > 0);
	assert (len <= _delay_len);

	const Phase *  phase_ptr = _phase_ptr;
	int            delay_int = _delay_int;
	int            delay_frc = _delay_frc;

	for (int pos = 0; pos < len; ++pos)
	{
		find_phase_and_delay (phase_ptr, delay_int, delay_frc, dly_fix_ptr [pos]);
		dst_ptr [pos] = read_safe (_pos_write - delay_int + pos, *phase_ptr);
	}

	_phase_ptr = phase_ptr;
	_delay_int = delay_int;
	_delay_frc = delay_frc;
}



// Non-interpolated read at random position
// Not constrained by the current delay length
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::read_block_at (T dst_ptr [], int delay, int len) const noexcept
{
	assert (delay >= 0);
	assert (delay <= _delay_len);
	assert (len > 0);
	assert (len <= delay + 1);

	const int      buf_len  = int (_buffer.size ());
	int            pos_read = (_pos_write - delay) & _buf_msk;
	const int      room     = buf_len - pos_read;
	const int      len_1    = std::min (len, room);
	const int      len_2    = len - len_1;
	fstb::copy_no_overlap (dst_ptr, &_buffer [pos_read], len_1);
	if (len_2 > 0)
	{
		fstb::copy_no_overlap (dst_ptr + len_1, _buffer.data (), len_2);
	}
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::write_block (const T src_ptr [], int len) noexcept
{
	assert (_phase_ptr != nullptr);
	assert (src_ptr != 0);
	assert (len > 0);
	assert (len <= _delay_len);

	const int      buf_len = int (_buffer.size ());
	const int      room    = buf_len - _pos_write;
	const int      len_1   = std::min (len, room);
	const int      len_2    = len - len_1;
	fstb::copy_no_overlap (&_buffer [_pos_write], src_ptr, len_1);
	if (len_2 > 0)
	{
		fstb::copy_no_overlap (_buffer.data (), src_ptr + len_1, len_2);
	}
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::step_block (int len) noexcept
{
	assert (_phase_ptr != nullptr);
	assert (len > 0);
	assert (len <= _delay_len);

	_pos_write = (_pos_write + len) & _buf_msk;
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::clear_buffers () noexcept
{
	std::fill (_buffer.begin (), _buffer.end (), T (0.f));
	_pos_write = 0;
}



template <typename T, int NPL2>
constexpr int	DelayFrac <T, NPL2>::_nbr_phases_l2;
template <typename T, int NPL2>
constexpr int	DelayFrac <T, NPL2>::_nbr_phases;
template <typename T, int NPL2>
constexpr int	DelayFrac <T, NPL2>::_delay_min;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int NPL2>
bool	DelayFrac <T, NPL2>::_interp_init_flag = false;

template <typename T, int NPL2>
typename DelayFrac <T, NPL2>::PhaseArray	DelayFrac <T, NPL2>::_phase_arr;



template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::read_safe (int pos_read, const Phase &phase) const noexcept
{
	const T        v0 { _buffer [(pos_read - 2) & _buf_msk] };
	const T        v1 { _buffer [(pos_read - 1) & _buf_msk] };
	const T        v2 { _buffer [(pos_read    ) & _buf_msk] };
	const T        v3 { _buffer [(pos_read + 1) & _buf_msk] };

#if 1

	// FIR interpolation
	const T        i0 { phase [0] };
	const T        i1 { phase [1] };
	const T        i2 { phase [2] };
	const T        i3 { phase [3] };

	return (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3);

#else

	// Simple linear interpolation, for testing
	return v2 + _delay_frc * (v1 - v2) * T (1.f / _nbr_phases);

#endif
}



template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::read_nocheck (int pos_read, const Phase &phase) const noexcept
{
	const T        v0 { _buffer [pos_read - 2] };
	const T        v1 { _buffer [pos_read - 1] };
	const T        v2 { _buffer [pos_read    ] };
	const T        v3 { _buffer [pos_read + 1] };

	// FIR interpolation
	const T        i0 { phase [0] };
	const T        i1 { phase [1] };
	const T        i2 { phase [2] };
	const T        i3 { phase [3] };

	return (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3);
}



template <typename T, int NPL2>
void  DelayFrac <T, NPL2>::find_phase_and_delay (const Phase * &phase_ptr, int &delay_int, int &delay_frc, int len_fixp) const noexcept
{
	assert (len_fixp >= _delay_min * _nbr_phases);

	delay_frc = len_fixp & _phase_msk;
	phase_ptr = &_phase_arr [delay_frc];
	delay_int = len_fixp >> _nbr_phases_l2;
	assert (delay_int <= _delay_len);
}



template <typename T, int NPL2>
void  DelayFrac <T, NPL2>::init_interpolator () noexcept
{
	// We use a cubic hermite interpolator to build the FIR.
	// Probably not optimal but close enough to rock'n'roll.
	rspl::InterpFtor::CubicHermite   interp;

	constexpr float   frac_mul = 1.f / _nbr_phases;
	for (int ph_idx = 0; ph_idx < _nbr_phases; ++ph_idx)
	{
		Phase &        phase = _phase_arr [ph_idx];
		const float    frac  = 1 - ph_idx * frac_mul;
		for (int pos = 0; pos < _phase_len; ++pos)
		{
			std::array <float, _phase_len> imp {};
			imp [pos] = 1;
			phase [pos] = interp (frac, imp.data () + 1);
		}
	}

	_interp_init_flag = true;
}



template <typename T, int NPL2>
constexpr int DelayFrac <T, NPL2>::_phase_len;
template <typename T, int NPL2>
constexpr int DelayFrac <T, NPL2>::_ph_pre;
template <typename T, int NPL2>
constexpr int DelayFrac <T, NPL2>::_ph_post;
template <typename T, int NPL2>
constexpr int DelayFrac <T, NPL2>::_phase_msk;



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_DelayFrac_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
