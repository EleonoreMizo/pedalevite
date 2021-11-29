/*****************************************************************************

        OnePole4Simd.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_OnePole4Simd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_OnePole4Simd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/ToolsSimd.h"

#include <utility>

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



class OnePole4Simd_LoaderSingle
{
public:
	typedef float DataType;
	static fstb_FORCEINLINE fstb::Vf32 load (const DataType *ptr) noexcept
	{
		return fstb::Vf32 (*ptr);
	}
};

template <class VS>
class OnePole4Simd_LoaderMulti
{
public:
	typedef fstb::Vf32 DataType;
	static fstb_FORCEINLINE fstb::Vf32 load (const DataType *ptr) noexcept
	{
		return VS::load_f32 (ptr);
	}
};

// LD: Loader class. Must have:
//		typename LD::DataType;
//		static fstb::Vf32 LD::load (const DataType *ptr);
template <class VD, class VS, class VP, class LD = OnePole4Simd_LoaderMulti <VS> >
class OnePole4Simd_StepOn
{
public:
	typedef VD V128Dest;
	typedef VS V128Src;
	typedef VP V128Par;
	typedef LD Loader;
	typedef OnePole4Simd <VD, VS, VP> OnePole4S;

	static fstb_FORCEINLINE void
	               step_z_eq (OnePole4SimdData &data, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	static fstb_FORCEINLINE void
	               store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1) noexcept;
	static fstb_FORCEINLINE void
	               step_z_eq_store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static fstb_FORCEINLINE void
	               step_z_eq (fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
};



template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOn <VD, VS, VP, LD>::step_z_eq (OnePole4SimdData &data, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);

	step_z_eq_store_result (data, b0, b1, a1, b_inc, a_inc);
}



template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOn <VD, VS, VP, LD>::store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1) noexcept
{
	V128Par::store_f32 (data._z_eq_b [0], b0);
	V128Par::store_f32 (data._z_eq_b [1], b1);
	V128Par::store_f32 (data._z_eq_a [1], a1);
}



template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOn <VD, VS, VP, LD>::step_z_eq_store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	step_z_eq (b0, b1, a1, b_inc, a_inc);
	store_result (data, b0, b1, a1);
}



template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOn <VD, VS, VP, LD>::step_z_eq (fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	const auto     b0_inc = V128Par::load_f32 (&b_inc [0]);
	const auto     b1_inc = V128Par::load_f32 (&b_inc [1]);
	const auto     a1_inc = V128Par::load_f32 (&a_inc [1]);

	b0 += b0_inc;
	b1 += b1_inc;
	a1 += a1_inc;
}



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -



template <class VD, class VS, class VP, class LD = OnePole4Simd_LoaderMulti <VS> >
class OnePole4Simd_StepOff
{
public:
	typedef	VD		V128Dest;
	typedef	VS		V128Src;
	typedef	VP		V128Par;
	typedef	LD		Loader;

	static fstb_FORCEINLINE void
	               step_z_eq (OnePole4SimdData &data, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	static fstb_FORCEINLINE void
	               store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1) noexcept;
	static fstb_FORCEINLINE void
	               step_z_eq_store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static fstb_FORCEINLINE void
	               step_z_eq (fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
};

template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOff <VD, VS, VP, LD>::step_z_eq (OnePole4SimdData &data, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	fstb::unused (data, b_inc, a_inc);
	// Nothing
}

template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOff <VD, VS, VP, LD>::store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1) noexcept
{
	fstb::unused (data, b0, b1, a1);
	// Nothing
}

template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOff <VD, VS, VP, LD>::step_z_eq_store_result (OnePole4SimdData &data, fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	fstb::unused (data, b0, b1, a1, b_inc, a_inc);
	// Nothing
}

template <class VD, class VS, class VP, class LD>
void	OnePole4Simd_StepOff <VD, VS, VP, LD>::step_z_eq (fstb::Vf32 &b0, fstb::Vf32 &b1, fstb::Vf32 &a1, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	fstb::unused (b0, b1, a1, b_inc, a_inc);
	// Nothing
}



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -



// STP is either OnePole4Simd_StepOn or OnePole4Simd_StepOff
template <class STP>
class OnePole4Simd_Proc
{
public:
	typedef typename STP::V128Dest      V128Dest;
	typedef typename STP::V128Src       V128Src;
	typedef typename STP::V128Par       V128Par;
	typedef typename STP::Loader        Loader;
	typedef typename Loader::DataType   LoadedDataType;

	static void		process_block_parallel (OnePole4SimdData &data, fstb::Vf32 out_ptr [], const LoadedDataType in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static fstb_FORCEINLINE fstb::Vf32
						process_sample_parallel (OnePole4SimdData &data, const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	static void		process_block_serial_latency (OnePole4SimdData &data, float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static fstb_FORCEINLINE float
						process_sample_serial_latency (OnePole4SimdData &data, float x_s, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static void		process_block_serial_immediate_post (OnePole4SimdData &data, float out_ptr [], const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	static void		process_block_2x2_latency (OnePole4SimdData &data, float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static fstb_FORCEINLINE fstb::Vf32
						process_sample_2x2_latency (OnePole4SimdData &data, fstb::Vf32 x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	static void		process_block_2x2_immediate (OnePole4SimdData &data, float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static fstb_FORCEINLINE fstb::Vf32
						process_sample_2x2_immediate (OnePole4SimdData &data, fstb::Vf32 x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	static void		process_block_2x2_immediate_post (OnePole4SimdData &data, float out_ptr [], const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	static fstb_FORCEINLINE float
						process_sample_single_stage (OnePole4SimdData &data, float x_s, int stage) noexcept;
};



template <class STP>
void	OnePole4Simd_Proc <STP>::process_block_parallel (OnePole4SimdData &data, fstb::Vf32 out_ptr [], const LoadedDataType in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	int            half_nbr_spl = nbr_spl >> 1;
	int            index        = 0;
	if (half_nbr_spl > 0)
	{
		auto           b0 { V128Par::load_f32 (data._z_eq_b [0]) };
		auto           b1 { V128Par::load_f32 (data._z_eq_b [1]) };
		auto           a1 { V128Par::load_f32 (data._z_eq_a [1]) };

		auto           x1 { V128Par::load_f32 (data._mem_x) };
		auto           y1 { V128Par::load_f32 (data._mem_y) };

		do
		{
			const auto     xa { STP::Loader::load (&in_ptr [index    ]) };
			const auto     xb { STP::Loader::load (&in_ptr [index + 1]) };

			auto           y2 = b0 * xa + b1 * x1 - a1 * y1;
			auto           x2 = xa;
			STP::step_z_eq (b0, b1, a1, b_inc, a_inc);

			y1 = b0 * xb + b1 * x2 - a1 * y2;
			x1 = xb;
			STP::step_z_eq (b0, b1, a1, b_inc, a_inc);

			V128Dest::store_f32 (&out_ptr [index    ], y2);
			V128Dest::store_f32 (&out_ptr [index + 1], y1);

			index += 2;
			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		V128Par::store_f32 (data._mem_x, x1);
		V128Par::store_f32 (data._mem_y, y1);

		STP::store_result (data, b0, b1, a1);
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		const auto     x = STP::Loader::load (&in_ptr [index]);
		const auto     y = process_sample_parallel (data, x, b_inc, a_inc);
		V128Dest::store_f32 (&out_ptr [index], y);
	}
}



template <class STP>
fstb::Vf32	OnePole4Simd_Proc <STP>::process_sample_parallel (OnePole4SimdData &data, const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);

	const auto     x1 = V128Par::load_f32 (data._mem_x);
	const auto     y1 = V128Par::load_f32 (data._mem_y);

	const auto     y  = b0 * x + b1 * x1 - a1 * y1;

	V128Par::store_f32 (data._mem_x, x);
	V128Par::store_f32 (data._mem_y, y);

	STP::step_z_eq_store_result (data, b0, b1, a1, b_inc, a_inc);

	return y;
}



template <class STP>
void	OnePole4Simd_Proc <STP>::process_block_serial_latency (OnePole4SimdData &data, float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	int            half_nbr_spl = nbr_spl >> 1;
	int            index        = 0;
	if (half_nbr_spl > 0)
	{
		auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
		auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
		auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);

		auto           x1 = V128Par::load_f32 (data._mem_x);
		auto           y1 = V128Par::load_f32 (data._mem_y);

		do
		{
			const float    src_0 = in_ptr [index + 0];
			const float    src_1 = in_ptr [index + 1];

			auto           tmp { y1.template rotate <1> () };
			auto           x   { tmp.template insert <0> (src_0) };
			const auto     y2  = b0 * x  + b1 * x1 - a1 * y1;
			const auto     x2  = x;
			tmp.storeu_scalar (&out_ptr [index + 0]);
			STP::step_z_eq (b0, b1, a1, b_inc, a_inc);

			tmp = y2.template rotate <1> ();
			x   = tmp.template insert <0> (src_1);
			y1  = b0 * x  + b1 * x2 - a1 * y2;
			x1  = x;
			tmp.storeu_scalar (&out_ptr [index + 1]);
			STP::step_z_eq (b0, b1, a1, b_inc, a_inc);

			index += 2;

			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		V128Par::store_f32 (data._mem_x, x1);
		V128Par::store_f32 (data._mem_y, y1);

		STP::store_result (data, b0, b1, a1);
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		out_ptr [index] =
			process_sample_serial_latency (data, in_ptr [index], b_inc, a_inc);
	}
}



template <class STP>
float	OnePole4Simd_Proc <STP>::process_sample_serial_latency (OnePole4SimdData &data, float x_s, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);

	const auto     x1 = V128Par::load_f32 (data._mem_x);
	const auto     y1 = V128Par::load_f32 (data._mem_y);

	const auto     tmp = y1.template rotate <1> ();
	const auto     x   = tmp.template insert <0> (x_s);

	const auto     y   = b0 * x + b1 * x1 - a1 * y1;

	V128Par::store_f32 (data._mem_x, x);
	V128Par::store_f32 (data._mem_y, y);

	const float    y_s = tmp.template extract <0> ();

	STP::step_z_eq_store_result (data, b0, b1, a1, b_inc, a_inc);

	return y_s;
}



/*
Flushes data and adapt the state of the filter from serial/latency processing
to direct processing.

Filter   B1        B2        B3        B4
Before   y1 [N-4]  y2 [N-3]  y3 [N-2]  y4 [N-1]
After    y1 [N-1]  y2 [N-1]  y3 [N-1]  y4 [N-1]

Where N is the block length.

This function must be called after having serial/latency-processed the last
sample of the block. out_ptr points on sample N-4.
*/

template <class STP>
void	OnePole4Simd_Proc <STP>::process_block_serial_immediate_post (OnePole4SimdData &data, float out_ptr [], const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	out_ptr [0] = data._mem_y [3];
	STP::step_z_eq (data, b_inc, a_inc);

	float           x_s = data._mem_y [2];
	out_ptr [1] = process_sample_single_stage (data, x_s, 3);
	STP::step_z_eq (data, b_inc, a_inc);

	x_s = data._mem_y [1];
	x_s = process_sample_single_stage (data, x_s, 2);
	out_ptr [2] = process_sample_single_stage (data, x_s, 3);
	STP::step_z_eq (data, b_inc, a_inc);

	x_s = data._mem_y [0];
	x_s = process_sample_single_stage (data, x_s, 1);
	x_s = process_sample_single_stage (data, x_s, 2);
	out_ptr [3] = process_sample_single_stage (data, x_s, 3);
	STP::step_z_eq (data, b_inc, a_inc);
}



template <class STP>
void	OnePole4Simd_Proc <STP>::process_block_2x2_latency (OnePole4SimdData &data, float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	int             half_nbr_spl = nbr_spl >> 1;
	int             index        = 0;
	if (half_nbr_spl > 0)
	{
		auto           b0 { V128Par::load_f32 (data._z_eq_b [0]) };
		auto           b1 { V128Par::load_f32 (data._z_eq_b [1]) };
		auto           a1 { V128Par::load_f32 (data._z_eq_a [1]) };

		auto           x1 { V128Par::load_f32 (data._mem_x) };
		auto           y1 { V128Par::load_f32 (data._mem_y) };

		do
		{
#if 0
			const auto     src_0 = fstb::Vf32::loadu_pair (&in_ptr [index + 0]);
			const auto     src_1 = fstb::Vf32::loadu_pair (&in_ptr [index + 2]);
#else
			const auto     src_0 = fstb::Vf32::loadu (&in_ptr [index]);
			const auto     src_1 = src_0.swap_pairs ();
#endif

			auto           x = src_0;
			x  = fstb::Vf32::interleave_pair_lo (x, y1); // y1[1 0] x[1 0]
			const auto     y2 = b0 * x + b1 * x1 - a1 * y1;
			const auto     x2 = x;
			auto           y { y2.template rotate <2> () };
			y.storeu_pair (&out_ptr [index + 0]);
			STP::step_z_eq (b0, b1, a1, b_inc, a_inc);

			x  = src_1;
			x  = fstb::Vf32::interleave_pair_lo (x, y2);
			y1 = b0 * x + b1 * x2 - a1 * y2;
			x1 = x;
			y  = y1.template rotate <2> ();
			y.storeu_pair (&out_ptr [index + 2]);
			STP::step_z_eq (b0, b1, a1, b_inc, a_inc);

			index += 4;

			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		V128Par::store_f32 (data._mem_x, x1);
		V128Par::store_f32 (data._mem_y, y1);

		STP::store_result (data, b0, b1, a1);
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		const auto     x = fstb::Vf32::loadu_pair (&in_ptr [index]);
		const auto     y = process_sample_2x2_latency (data, x, b_inc, a_inc);
		y.storeu_pair (&out_ptr [index]);
	}
}



template <class STP>
fstb::Vf32	OnePole4Simd_Proc <STP>::process_sample_2x2_latency (OnePole4SimdData &data, fstb::Vf32 x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);

	const auto     x1 = V128Par::load_f32 (data._mem_x);
	const auto     y1 = V128Par::load_f32 (data._mem_y);

	x = fstb::Vf32::interleave_pair_lo (x, y1); // y1[1 0] x[1 0]

	const auto     y  = b0 * x + b1 * x1 - a1 * y1;

	V128Par::store_f32 (data._mem_x, x);
	V128Par::store_f32 (data._mem_y, y);

	y = y.template rotate <2> ();

	STP::step_z_eq_store_result (data, b0, b1, a1, b_inc, a_inc);

	return y;
}



template <class STP>
void	OnePole4Simd_Proc <STP>::process_block_2x2_immediate_post (OnePole4SimdData &data, float out_ptr [], const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	float          x_s;

	x_s = data._mem_y [0];
	out_ptr [0] = process_sample_single_stage (data, x_s, 2);
	x_s = data._mem_y [1];
	out_ptr [1] = process_sample_single_stage (data, x_s, 3);
	STP::step_z_eq (data, b_inc, a_inc);
}



template <class STP>
float	OnePole4Simd_Proc <STP>::process_sample_single_stage (OnePole4SimdData &data, float x_s, int stage) noexcept
{
	assert (stage >= 0);
	assert (stage < (OnePole4Simd <V128Dest, V128Src, V128Par>::_nbr_units));

	const float    y_s =
		  x_s                 * data._z_eq_b [0] [stage]
		+ data._mem_x [stage] * data._z_eq_b [1] [stage]
		- data._mem_y [stage] * data._z_eq_a [1] [stage];

	data._mem_x [stage] = x_s;
	data._mem_y [stage] = y_s;

	return y_s;
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
OnePole4Simd <VD, VS, VP>::OnePole4Simd () noexcept
:	_data ()
{
	assert (V128Par::check_ptr (&_data._z_eq_a [0]));
	assert (V128Par::check_ptr (&_data._z_eq_b [0]));
	assert (V128Par::check_ptr (&_data._mem_x));
	assert (V128Par::check_ptr (&_data._mem_y));

	neutralise ();
	clear_buffers ();
}



template <class VD, class VS, class VP>
OnePole4Simd <VD, VS, VP>::OnePole4Simd (const OnePole4Simd <VD, VS, VP> &other) noexcept
:	_data (other._data)
{
	// Nothing
}



template <class VD, class VS, class VP>
OnePole4Simd <VD, VS, VP>::OnePole4Simd (OnePole4Simd <VD, VS, VP> &&other) noexcept
:	_data (std::move (other._data))
{
	// Nothing
}



template <class VD, class VS, class VP>
OnePole4Simd <VD, VS, VP> &	OnePole4Simd <VD, VS, VP>::operator = (const OnePole4Simd <VD, VS, VP> &other) noexcept
{
	if (this != &other)
	{
		_data = other._data;
	}

	return *this;
}



template <class VD, class VS, class VP>
OnePole4Simd <VD, VS, VP> &	OnePole4Simd <VD, VS, VP>::operator = (OnePole4Simd <VD, VS, VP> &&other) noexcept
{
	if (this != &other)
	{
		_data = std::move (other._data);
	}

	return *this;
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::neutralise () noexcept
{
	V128Par::store_f32 (_data._z_eq_b [0], fstb::Vf32 (1));
	V128Par::store_f32 (_data._z_eq_b [1], fstb::Vf32::zero ());

	V128Par::store_f32 (_data._z_eq_a [1], fstb::Vf32::zero ());
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::neutralise_one (int elt) noexcept
{
	assert (elt >= 0);
	assert (elt < _nbr_units);

	_data._z_eq_b [0] [elt] = 1;
	_data._z_eq_b [1] [elt] = 0;

	_data._z_eq_a [1] [elt] = 0;
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::set_z_eq (const VectFloat4 b [2], const VectFloat4 a [2]) noexcept
{
	assert (b != nullptr);
	assert (a != nullptr);

	V128Par::store_f32 (_data._z_eq_b [0], V128Par::load_f32 (b [0]));
	V128Par::store_f32 (_data._z_eq_b [1], V128Par::load_f32 (b [1]));

	const auto     a1 = V128Par::load_f32 (a [1]);
	V128Par::store_f32 (_data._z_eq_a [1], a1);
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::set_z_eq_same (const float b [2], const float a [2]) noexcept
{
	assert (b != nullptr);
	assert (a != nullptr);

	V128Par::store_f32 (_data._z_eq_b [0], fstb::Vf32 (b [0]));
	V128Par::store_f32 (_data._z_eq_b [1], fstb::Vf32 (b [1]));

	V128Par::store_f32 (_data._z_eq_a [1], fstb::Vf32 (a [1]));
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::set_z_eq_one (int elt, const float b [2], const float a [2]) noexcept
{
	assert (elt >= 0);
	assert (elt < _nbr_units);
	assert (b != nullptr);
	assert (a != nullptr);

	_data._z_eq_b [0] [elt] = b [0];
	_data._z_eq_b [1] [elt] = b [1];

	_data._z_eq_a [1] [elt] = a [1];
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::get_z_eq (VectFloat4 b [2], VectFloat4 a [2]) const noexcept
{
	assert (b != nullptr);
	assert (a != nullptr);

	V128Par::store_f32 (b [0], V128Par::load_f32 (_data._z_eq_b [0]));
	V128Par::store_f32 (b [1], V128Par::load_f32 (_data._z_eq_b [1]));

	V128Par::store_f32 (a [1], V128Par::load_f32 (_data._z_eq_a [1]));
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::get_z_eq_one (int elt, float b [2], float a [2]) const noexcept
{
	assert (elt >= 0);
	assert (elt < _nbr_units);
	assert (b != nullptr);
	assert (a != nullptr);

	b [0] = _data._z_eq_b [0] [elt];
	b [1] = _data._z_eq_b [1] [elt];

	a [1] = _data._z_eq_a [1] [elt];
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::copy_z_eq (const OnePole4Simd <VD, VS, VP> &other) noexcept
{
	V128Par::store_f32 (_data._z_eq_b [0], V128Par::load_f32 (other._data._z_eq_b [0]));
	V128Par::store_f32 (_data._z_eq_b [1], V128Par::load_f32 (other._data._z_eq_b [1]));

	V128Par::store_f32 (_data._z_eq_a [1], V128Par::load_f32 (other._data._z_eq_a [1]));
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::set_state_one (int elt, float mem_x, float mem_y) noexcept
{
	assert (elt >= 0);
	assert (elt < _nbr_units);

	_data._mem_x [elt] = mem_x;
	_data._mem_y [elt] = mem_y;
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::get_state_one (int elt, float &mem_x, float &mem_y) const noexcept
{
	assert (elt >= 0);
	assert (elt < _nbr_units);

	mem_x = _data._mem_x [elt];
	mem_y = _data._mem_y [elt];
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_parallel (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_block_parallel (
		_data, out_ptr, in_ptr, nbr_spl, nullptr, nullptr
	);
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_parallel (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

	Proc::process_block_parallel (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



// Input samples are broadcasted over the 4 filters
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_parallel (fstb::Vf32 out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP, OnePole4Simd_LoaderSingle> >	Proc;

	Proc::process_block_parallel (
		_data, out_ptr, in_ptr, nbr_spl, nullptr, nullptr
	);
}



// Input samples are broadcasted over the 4 filters
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_parallel (fstb::Vf32 out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP, OnePole4Simd_LoaderSingle> >	Proc;

	Proc::process_block_parallel (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



template <class VD, class VS, class VP>
fstb::Vf32	OnePole4Simd <VD, VS, VP>::process_sample_parallel (const fstb::Vf32 &x) noexcept
{
	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	return (Proc::process_sample_parallel (_data, x, nullptr, nullptr));
}



template <class VD, class VS, class VP>
fstb::Vf32	OnePole4Simd <VD, VS, VP>::process_sample_parallel (const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

	return (Proc::process_sample_parallel (_data, x, b_inc, a_inc));
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_block_serial_latency (
		_data, out_ptr, in_ptr, nbr_spl, nullptr, nullptr
	);
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

	Proc::process_block_serial_latency (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



/*
==============================================================================
Name: process_sample_serial_latency
Description:

Processes the 4 filters in serial, outputting the result with a latency of 4
samples (seems more optimised than 3-sample latency code).

In the following pseudo-code, variables in capitals are vectors.

y = Y1[3]
X = [x Y1[0:2]]
Y = X*B0 + X1*B1 + X2*B2 - Y1*A1 - Y2*B2
(X2 = X1)
X1 = X
(Y2 = Y1)
Y1 = Y

Below, 32-bit words in reverse order (LSQ...MSQ)
a ---elt0--> A ---elt1--> A' ---elt2--> A" ---elt3--> A"'

Iter. begin  |  Iteration end
x_s   (X)    |    X1        X2        Y1        Y2     y_s
a   a 0 0 0  |  a 0 0 0   0 0 0 0   A 0 0 0   0 0 0 0   0
b   b A 0 0  |  b A 0 0   a 0 0 0   B A'0 0   A 0 0 0   0
c   c B A'0  |  c B A'0   b A 0 0   C B'A"0   B A'0 0   0
d   d C B'A" |  d C B'A"  c B A'0   D C'B"A"' C B'A"0   0
e   e D C'B" |  e D C'B"  d C B'A"  E D'C"B"' D C'B"A"' A"'
...

Input parameters:
	- x_s: input sample.
Returns: Output sample, delayed from 4 samples.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS, class VP>
float	OnePole4Simd <VD, VS, VP>::process_sample_serial_latency (float x_s) noexcept
{
	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	return Proc::process_sample_serial_latency (_data, x_s, 0, 0);
}



template <class VD, class VS, class VP>
float	OnePole4Simd <VD, VS, VP>::process_sample_serial_latency (float x_s, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

	return Proc::process_sample_serial_latency (_data, x_s, b_inc, a_inc);
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_SERIAL + 1.
	if (nbr_spl < _latency_serial + 1)
	{
		int            pos = 0;
		do
		{
			out_ptr [pos] = process_sample_serial_immediate (in_ptr [pos]);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

		process_block_serial_immediate_pre (in_ptr);
		process_block_serial_latency (
			&out_ptr [0],
			&in_ptr [_latency_serial],
			nbr_spl - _latency_serial
		);
		Proc::process_block_serial_immediate_post (
			_data,
			&out_ptr [nbr_spl - _latency_serial],
			0,
			0
		);
	}
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));
	assert (out_ptr != nullptr);
	assert (in_ptr  != nullptr);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_SERIAL + 1.
	if (nbr_spl < _latency_serial + 1)
	{
		int            pos = 0;
		do
		{
			out_ptr [pos] = process_sample_serial_immediate (in_ptr [pos], b_inc, a_inc);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

		process_block_serial_immediate_pre (in_ptr);
		process_block_serial_latency (
			&out_ptr [0],
			&in_ptr [_latency_serial],
			nbr_spl - _latency_serial,
			b_inc,
			a_inc
		);
		Proc::process_block_serial_immediate_post (
			_data,
			&out_ptr [nbr_spl - _latency_serial],
			b_inc,
			a_inc
		);
	}
}



template <class VD, class VS, class VP>
float	OnePole4Simd <VD, VS, VP>::process_sample_serial_immediate (float x_s) noexcept
{
	x_s = process_sample_single_stage_noswap (0, x_s);
	x_s = process_sample_single_stage_noswap (1, x_s);
	x_s = process_sample_single_stage_noswap (2, x_s);
	x_s = process_sample_single_stage_noswap (3, x_s);

	return x_s;
}



template <class VD, class VS, class VP>
float	OnePole4Simd <VD, VS, VP>::process_sample_serial_immediate (float x_s, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	const float		y_s = process_sample_serial_immediate (x_s);

	OnePole4Simd_StepOn <VD, VS, VP>::step_z_eq (_data, b_inc, a_inc);

	return y_s;
}



// Input and output are stereo interlaced data
// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_block_2x2_latency (
		_data, out_ptr, in_ptr, nbr_spl, nullptr, nullptr
	);
}



// Input and output are stereo interlaced data
// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

	Proc::process_block_2x2_latency (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



/*
==============================================================================
Name: process_sample_2x2_latency
Description:

Processes the 4 filters arranged in two parallel paths made of two filters in
serial. First path is made of filters 0 and 2, second path is made of filters
1 and 3. Outputs the result with a latency of 1 sample.

In the following pseudo-code, variables in capitals are vectors.

yl = Y1[2]
yr = Y1[3]
X = [xl xr Y1[0:1]]
Y = X*B0 + X1*B1 + X2*B2 - Y1*A1 - Y2*B2
(X2 = X1)
X1 = X
(Y2 = Y1)
Y1 = Y

Below, 32-bit words in reverse order (LSQ...MSQ)
a ---elt0--> A ---elt1--> A'

Iter.   begin  |  Iteration end
x_s     (X)    |    X1        X2        Y1        Y2      y_s
a b   a b 0 0  |  a b 0 0   0 0 0 0   A B 0 0   0 0 0 0   0 0
c d   c d A B  |  c d A B   a b 0 0   C D A'B'  A B 0 0   A'B'
e f   e f C D  |  e f C D   c d A B   E F C'D'  C D A'B'  C'D'
...

Input parameters:
	- x: Vector containing a pair of sample. The samples are located at the
		beginning of the vector stored in memory (LSQs on little endian archi).
Returns:
	A vector containing the output sample pair, delayed of 2 samples. Vector
	structure is the same as the input.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS, class VP>
fstb::Vf32	OnePole4Simd <VD, VS, VP>::process_sample_2x2_latency (const fstb::Vf32 &x) noexcept
{
	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	return Proc::process_sample_2x2_latency (_data, x, 0, 0);
}



template <class VD, class VS, class VP>
fstb::Vf32	OnePole4Simd <VD, VS, VP>::process_sample_2x2_latency (const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

	return Proc::process_sample_2x2_latency (_data, x, b_inc, a_inc);
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_2X2 + 1.
	if (nbr_spl < _latency_2x2 + 1)
	{
		int            pos = 0;
		do
		{
			const auto     x = fstb::Vf32::loadu_pair (in_ptr + pos * 2);
			const auto     y = process_sample_2x2_immediate (x);
			y.storeu_pair (out_ptr + pos * 2);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

		process_block_2x2_immediate_pre (in_ptr);
		process_block_2x2_latency (
			&out_ptr [0],
			&in_ptr [_latency_2x2 * 2],
			nbr_spl - _latency_2x2
		);
		Proc::process_block_2x2_immediate_post (
			_data,
			&out_ptr [(nbr_spl - _latency_2x2) * 2],
			0,
			0
		);
	}
}



// Can work in-place
template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_2X2 + 1.
	if (nbr_spl < _latency_2x2 + 1)
	{
		int            pos = 0;
		do
		{
			const auto     x = fstb::Vf32::loadu_pair (in_ptr + pos * 2);
			const auto     y = process_sample_2x2_immediate (x, b_inc, a_inc);
			y.storeu_pair (out_ptr + pos * 2);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	OnePole4Simd_Proc <OnePole4Simd_StepOn <VD, VS, VP> >	Proc;

		process_block_2x2_immediate_pre (in_ptr);
		process_block_2x2_latency (
			&out_ptr [0],
			&in_ptr [_latency_2x2 * 2],
			nbr_spl - _latency_2x2,
			b_inc,
			a_inc
		);
		Proc::process_block_2x2_immediate_post (
			_data,
			&out_ptr [(nbr_spl - _latency_2x2) * 2],
			b_inc,
			a_inc
		);
	}
}



template <class VD, class VS, class VP>
fstb::Vf32	OnePole4Simd <VD, VS, VP>::process_sample_2x2_immediate (const fstb::Vf32 &x) noexcept
{
	float          x_0 = x.template extract <0> ();
	float          x_1 = x.template extract <1> ();

	x_0 = process_sample_single_stage_noswap (0, x_0);
	x_1 = process_sample_single_stage_noswap (1, x_1);
	x_0 = process_sample_single_stage_noswap (2, x_0);
	x_1 = process_sample_single_stage_noswap (3, x_1);

	const auto     y = fstb::Vf32::set_pair (x_0, x_1);

	return y;
}



template <class VD, class VS, class VP>
fstb::Vf32	OnePole4Simd <VD, VS, VP>::process_sample_2x2_immediate (const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	const auto     y = process_sample_2x2_immediate (x);

	OnePole4Simd_StepOn <VD, VS, VP>::step_z_eq (_data, b_inc, a_inc);

	return y;
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::clear_buffers () noexcept
{
	V128Par::store_f32 (_data._mem_x, fstb::Vf32::zero ());
	V128Par::store_f32 (_data._mem_y, fstb::Vf32::zero ());
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::clear_buffers_one (int elt) noexcept
{
	assert (elt >= 0);
	assert (elt < _nbr_units);

	_data._mem_x [elt] = 0;
	_data._mem_y [elt] = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
float	OnePole4Simd <VD, VS, VP>::process_sample_single_stage_noswap (int stage, float x_s) noexcept
{
	const float    y_s =
		   x_s                 * _data._z_eq_b [0] [stage]
		+ _data._mem_x [stage] * _data._z_eq_b [1] [stage]
		- _data._mem_y [stage] * _data._z_eq_a [1] [stage];

	_data._mem_x [stage] = x_s;
	_data._mem_y [stage] = y_s;

	return y_s;
}



/*
Changes the states of the filters in order to accomodate them to serial/latency
processing.

Filter   B1        B2        B3        B4
Before   y1 [-1]   y2 [-1]   y3 [-1]   y4 [-1]
After    y1 [ 3]   y2 [ 2]   y3 [ 1]   y4 [ 0]

Where yi [k] is the z^-1 memory of the i-th filter containing a sample
correspounding to final index k within the processing block.

This function must be called before serial/latency-processing the block,
starting at the 4th sample. in_ptr points on the first sample.
*/

template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_serial_immediate_pre (const float in_ptr []) noexcept
{
	assert (in_ptr != nullptr);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	float          x_s;

	x_s = Proc::process_sample_single_stage (_data, in_ptr [0], 0);
	x_s = Proc::process_sample_single_stage (_data, x_s, 1);
	x_s = Proc::process_sample_single_stage (_data, x_s, 2);
	Proc::process_sample_single_stage (_data, x_s, 3);

	x_s = Proc::process_sample_single_stage (_data, in_ptr [1], 0);
	x_s = Proc::process_sample_single_stage (_data, x_s, 1);
	Proc::process_sample_single_stage (_data, x_s, 2);

	x_s = Proc::process_sample_single_stage (_data, in_ptr [2], 0);
	Proc::process_sample_single_stage (_data, x_s, 1);

	Proc::process_sample_single_stage (_data, in_ptr [3], 0);
}



template <class VD, class VS, class VP>
void	OnePole4Simd <VD, VS, VP>::process_block_2x2_immediate_pre (const float in_ptr []) noexcept
{
	assert (in_ptr != nullptr);

	typedef	OnePole4Simd_Proc <OnePole4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_sample_single_stage (_data, in_ptr [0], 0);
	Proc::process_sample_single_stage (_data, in_ptr [1], 1);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_OnePole4Simd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
