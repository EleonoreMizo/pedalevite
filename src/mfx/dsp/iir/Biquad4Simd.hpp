/*****************************************************************************

        Biquad4Simd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Biquad4Simd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4Simd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



class Biquad4Simd_LoaderSingle
{
public:
	typedef	float	DataType;
	static fstb_FORCEINLINE fstb::ToolsSimd::VectF32	load (const DataType *ptr)
	{
		return (fstb::ToolsSimd::set1_f32 (*ptr));
	}
};

template <class VS>
class Biquad4Simd_LoaderMulti
{
public:
	typedef	fstb::ToolsSimd::VectF32	DataType;
	static fstb_FORCEINLINE fstb::ToolsSimd::VectF32	load (const DataType *ptr)
	{
		return (VS::load_f32 (ptr));
	}
};

// LD: Loader class. Must have:
//		typename LD::DataType;
//		static fstb::ToolsSimd::VectF32 LD::load (const DataType *ptr);
template <class VD, class VS, class VP, class LD = Biquad4Simd_LoaderMulti <VS> >
class Biquad4Simd_StepOn
{
public:
	typedef VD V128Dest;
	typedef VS V128Src;
	typedef VP V128Par;
	typedef LD Loader;

	static fstb_FORCEINLINE void
	               step_z_eq (Biquad4SimdData &data, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	static fstb_FORCEINLINE void
	               store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2);
	static fstb_FORCEINLINE void
	               step_z_eq_store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static fstb_FORCEINLINE void
	               step_z_eq (fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
};



template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOn <VD, VS, VP, LD>::step_z_eq (Biquad4SimdData &data, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
	auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

	step_z_eq_store_result (data, b0, b1, b2, a1, a2, b_inc, a_inc);
}



template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOn <VD, VS, VP, LD>::store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2)
{
	V128Par::store_f32 (data._z_eq_b [0], b0);
	V128Par::store_f32 (data._z_eq_b [1], b1);
	V128Par::store_f32 (data._z_eq_b [2], b2);
	V128Par::store_f32 (data._z_eq_a [1], a1);
	V128Par::store_f32 (data._z_eq_a [2], a2);
}



template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOn <VD, VS, VP, LD>::step_z_eq_store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);
	store_result (data, b0, b1, b2, a1, a2);
}



template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOn <VD, VS, VP, LD>::step_z_eq (fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	const auto     b0_inc = V128Par::load (&b_inc [0]);
	const auto     b1_inc = V128Par::load (&b_inc [1]);
	const auto     b2_inc = V128Par::load (&b_inc [2]);
	const auto     a1_inc = V128Par::load (&a_inc [1]);
	const	auto     a2_inc = V128Par::load (&a_inc [2]);

	b0 += b0_inc;
	b1 += b1_inc;
	b2 += b2_inc;
	a1 += a1_inc;
	a2 += a2_inc;
}



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -



template <class VD, class VS, class VP, class LD = Biquad4Simd_LoaderMulti <VS> >
class Biquad4Simd_StepOff
{
public:
	typedef	VD		V128Dest;
	typedef	VS		V128Src;
	typedef	VP		V128Par;
	typedef	LD		Loader;

	static fstb_FORCEINLINE void
	               step_z_eq (Biquad4SimdData &data, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	static fstb_FORCEINLINE void
	               store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2);
	static fstb_FORCEINLINE void
	               step_z_eq_store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static fstb_FORCEINLINE void
	               step_z_eq (fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
};

template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOff <VD, VS, VP, LD>::step_z_eq (Biquad4SimdData &data, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	// Nothing
}

template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOff <VD, VS, VP, LD>::store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2)
{
	// Nothing
}

template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOff <VD, VS, VP, LD>::step_z_eq_store_result (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	// Nothing
}

template <class VD, class VS, class VP, class LD>
void	Biquad4Simd_StepOff <VD, VS, VP, LD>::step_z_eq (fstb::ToolsSimd::VectF32 &b0, fstb::ToolsSimd::VectF32 &b1, fstb::ToolsSimd::VectF32 &b2, fstb::ToolsSimd::VectF32 &a1, fstb::ToolsSimd::VectF32 &a2, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	// Nothing
};



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -



// STP is either Biquad4Simd_StepOn or Biquad4Simd_StepOff
template <class STP>
class Biquad4Simd_Proc
{
public:
	typedef typename STP::V128Dest      V128Dest;
	typedef typename STP::V128Src       V128Src;
	typedef typename STP::V128Par       V128Par;
	typedef typename STP::Loader        Loader;
	typedef typename Loader::DataType   LoadedDataType;

	static void		process_block_parallel (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 out_ptr [], const LoadedDataType in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_parallel (Biquad4SimdData &data, const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	static void		process_block_serial_latency (Biquad4SimdData &data, float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static fstb_FORCEINLINE float
						process_sample_serial_latency (Biquad4SimdData &data, float x_s, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static void		process_block_serial_immediate_post (Biquad4SimdData &data, float out_ptr [], const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	static void		process_block_2x2_latency (Biquad4SimdData &data, float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_2x2_latency (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	static void		process_block_2x2_immediate (Biquad4SimdData &data, float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_2x2_immediate (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	static void		process_block_2x2_immediate_post (Biquad4SimdData &data, float out_ptr [], const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	static fstb_FORCEINLINE float
						process_sample_single_stage (Biquad4SimdData &data, float x_s, int stage, int mem_pos, int alt_pos);
};



template <class STP>
void	Biquad4Simd_Proc <STP>::process_block_parallel (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 out_ptr [], const LoadedDataType in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	// If we are not on an even boudary, we process a single sample.
	if (data._mem_pos != 0)
	{
		const auto     x = STP::Loader::load_f32 (in_ptr);
		const auto     y = process_sample_parallel (data, x, b_inc, a_inc);
		V128Dest::store_f32 (out_ptr, y);
		++ in_ptr;
		++ out_ptr;
		-- nbr_spl;

		if (nbr_spl == 0)
		{
			return;
		}
	}

	long           half_nbr_spl = nbr_spl >> 1;
	long           index = 0;
	if (half_nbr_spl > 0)
	{
		auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
		auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
		auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
		auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
		auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

		auto           x1 = V128Par::load_f32 (data._mem_x [0]);
		auto           x2 = V128Par::load_f32 (data._mem_x [1]);
		auto           y1 = V128Par::load_f32 (data._mem_y [0]);
		auto           y2 = V128Par::load_f32 (data._mem_y [1]);

		fstb::ToolsSimd::VectF32   sb;
		fstb::ToolsSimd::VectF32   sa;
		fstb::ToolsSimd::VectF32   x;

		do
		{
			x  = STP::Loader::load_f32 (&in_ptr [index]);
			sb = b1 * x1 +  b2 * x2;
			sa = a1 * y1 +  a2 * y2;
			y2 = b0 * x  + (sb - sa);
			x2 = x;
			V128Dest::store_f32 (&out_ptr [index    ], y2);
			STP::step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);

			x  = STP::Loader::load (&in_ptr [index + 1]);
			sb = b1 * x2 +  b2 * x1;
			sa = a1 * y2 +  a2 * y1;
			y1 = b0 * x  + (sb - sa);
			x1 = x;
			V128Dest::store_f32 (&out_ptr [index + 1], y1);
			STP::step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);

			index += 2;

			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		V128Par::store_f32 (data._mem_x [0], x1);
		V128Par::store_f32 (data._mem_x [1], x2);
		V128Par::store_f32 (data._mem_y [0], y1);
		V128Par::store_f32 (data._mem_y [1], y2);

		STP::store_result (data, b0, b1, b2, a1, a2);
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		const auto     x = STP::Loader::load_f32 (&in_ptr [index]);
		const auto     y = process_sample_parallel (data, x, b_inc, a_inc);
		V128Dest::store_f32 (&out_ptr [index], y);
	}
}



template <class STP>
fstb::ToolsSimd::VectF32	Biquad4Simd_Proc <STP>::process_sample_parallel (Biquad4SimdData &data, const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
	auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

	const int      alt_pos = 1 - data._mem_pos;

	const auto     x1 = V128Par::load_f32 (data._mem_x [data._mem_pos]);
	const auto     x2 = V128Par::load_f32 (data._mem_x [      alt_pos]);
	const auto     y1 = V128Par::load_f32 (data._mem_y [data._mem_pos]);
	const auto     y2 = V128Par::load_f32 (data._mem_y [      alt_pos]);

	auto           y  = b0 * x;
	const auto     sb = b1 * x1 + b2 * x2;
	const auto     sa = a1 * y1 + a2 * y2;
	y += sb - sa;

	V128Par::store_f32 (data._mem_x [alt_pos], x);
	V128Par::store_f32 (data._mem_y [alt_pos], y);
	data._mem_pos = alt_pos;

	STP::step_z_eq_store_result (data, b0, b1, b2, a1, a2, b_inc, a_inc);

	return y;
}



template <class STP>
void	Biquad4Simd_Proc <STP>::process_block_serial_latency (Biquad4SimdData &data, float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	// If we are not on an even boudary, we process a single sample.
	if (data._mem_pos != 0)
	{
		out_ptr [0] =
			process_sample_serial_latency (data, in_ptr [0], b_inc, a_inc);
		++ in_ptr;
		++ out_ptr;
		-- nbr_spl;

		if (nbr_spl == 0)
		{
			return;
		}
	}

	long           half_nbr_spl = nbr_spl >> 1;
	long           index = 0;
	if (half_nbr_spl > 0)
	{
		auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
		auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
		auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
		auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
		auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

		auto           x1 = V128Par::load_f32 (data._mem_x [0]);
		auto           x2 = V128Par::load_f32 (data._mem_x [1]);
		auto           y1 = V128Par::load_f32 (data._mem_y [0]);
		auto           y2 = V128Par::load_f32 (data._mem_y [1]);

		fstb::ToolsSimd::VectF32   sb;
		fstb::ToolsSimd::VectF32   sa;
		fstb::ToolsSimd::VectF32   x;
		fstb::ToolsSimd::VectF32   tmp;

		do
		{
			tmp = fstb::ToolsSimd::Shift <1>::rotate (y1);
			x   = fstb::ToolsSimd::Shift <0>::insert (tmp, in_ptr [index + 0]);
			sb  = b1 * x1 +  b2 * x2;
			sa  = a1 * y1 +  a2 * y2;
			y2  = b0 * x  + (sb - sa);
			x2  = x;
			fstb::ToolsSimd::storeu_1f32 (&out_ptr [index + 0], tmp);
			STP::step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);

			tmp = fstb::ToolsSimd::Shift <1>::rotate (y2);
			x   = fstb::ToolsSimd::Shift <0>::insert (tmp, in_ptr [index + 1]);
			sb  = b1 * x2 +  b2 * x1;
			sa  = a1 * y2 +  a2 * y1;
			y1  = b0 * x  + (sb - sa);
			x1  = x;
			fstb::ToolsSimd::storeu_1f32 (&out_ptr [index + 1], tmp);
			STP::step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);

			index += 2;

			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		V128Par::store_f32 (data._mem_x [0], x1);
		V128Par::store_f32 (data._mem_x [1], x2);
		V128Par::store_f32 (data._mem_y [0], y1);
		V128Par::store_f32 (data._mem_y [1], y2);

		STP::store_result (data, b0, b1, b2, a1, a2);
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		out_ptr [index] =
			process_sample_serial_latency (data, in_ptr [index], b_inc, a_inc);
	}
}



template <class STP>
float	Biquad4Simd_Proc <STP>::process_sample_serial_latency (Biquad4SimdData &data, float x_s, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
	auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

	const int      alt_pos = 1 - data._mem_pos;

	const auto     x1 = V128Par::load_f32 (data._mem_x [data._mem_pos]);
	const auto     x2 = V128Par::load_f32 (data._mem_x [      alt_pos]);
	const auto     y1 = V128Par::load_f32 (data._mem_y [data._mem_pos]);
	const auto     y2 = V128Par::load_f32 (data._mem_y [      alt_pos]);

	const auto     tmp = fstb::ToolsSimd::Shift <1>::rotate (y1);
	const auto     x   = fstb::ToolsSimd::Shift <0>::insert (tmp, x_s);

	auto           y   = b0 * x;
	const auto     sb  = b1 * x1 + b2 * x2;
	const auto     sa  = a1 * y1 + a2 * y2;
	y += sb - sa;

	V128Par::store_f32 (data._mem_x [alt_pos], x);
	V128Par::store_f32 (data._mem_y [alt_pos], y);
	data._mem_pos = alt_pos;

	const float    y_s = fstb::ToolsSimd::Shift <0>::extract (tmp);

	STP::step_z_eq_store_result (data, b0, b1, b2, a1, a2, b_inc, a_inc);

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
void	Biquad4Simd_Proc <STP>::process_block_serial_immediate_post (Biquad4SimdData &data, float out_ptr [], const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	const int       alt_pos = 1 - data._mem_pos;
	float           x_s;

	out_ptr [0] = data._mem_y [data._mem_pos] [3];
	STP::step_z_eq (data, b_inc, a_inc);

	x_s = data._mem_y [data._mem_pos] [2];
	out_ptr [1] = process_sample_single_stage (data, x_s, 3, data._mem_pos, alt_pos);
	STP::step_z_eq (data, b_inc, a_inc);

	x_s = data._mem_y [data._mem_pos] [1];
	x_s = process_sample_single_stage (data, x_s, 2, data._mem_pos, alt_pos);
	out_ptr [2] = process_sample_single_stage (data, x_s, 3, data._mem_pos, alt_pos);
	STP::step_z_eq (data, b_inc, a_inc);

	x_s = data._mem_y [data._mem_pos] [0];
	x_s = process_sample_single_stage (data, x_s, 1, data._mem_pos, alt_pos);
	x_s = process_sample_single_stage (data, x_s, 2, data._mem_pos, alt_pos);
	out_ptr [3] = process_sample_single_stage (data, x_s, 3, data._mem_pos, alt_pos);
	STP::step_z_eq (data, b_inc, a_inc);
}



template <class STP>
void	Biquad4Simd_Proc <STP>::process_block_2x2_latency (Biquad4SimdData &data, float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	// If we are not on an even boudary, we process a single sample.
	if (data._mem_pos != 0)
	{
		const auto     x = fstb::ToolsSimd::loadu_2f32 (&in_ptr [0]);
		const auto     y = process_sample_2x2_latency (data, x, b_inc, a_inc);
		fstb::ToolsSimd::storeu_2f32 (&out_ptr [0], y);
		in_ptr += 2;
		out_ptr += 2;
		-- nbr_spl;

		if (nbr_spl == 0)
		{
			return;
		}
	}

	long            half_nbr_spl = nbr_spl >> 1;
	long            index = 0;
	if (half_nbr_spl > 0)
	{
		auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
		auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
		auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
		auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
		auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

		auto           x1 = V128Par::load_f32 (data._mem_x [0]);
		auto           x2 = V128Par::load_f32 (data._mem_x [1]);
		auto           y1 = V128Par::load_f32 (data._mem_y [0]);
		auto           y2 = V128Par::load_f32 (data._mem_y [1]);

		fstb::ToolsSimd::VectF32   sb;
		fstb::ToolsSimd::VectF32   sa;
		fstb::ToolsSimd::VectF32   x;
		fstb::ToolsSimd::VectF32   y;

		do
		{
			x  = fstb::ToolsSimd::loadu_2f32 (&in_ptr [index + 0]);
			x  = fstb::ToolsSimd::interleave_2f32_low (x, y1);		// y1[1 0] x[1 0]
			sb = b1 * x1 +  b2 * x2;
			sa = a1 * y1 +  a2 * y2;
			y2 = b0 * x  + (sb - sa);
			x2 = x;
			y  = fstb::ToolsSimd::Shift <2>::rotate (y2);
			fstb::ToolsSimd::storeu_2f32 (&out_ptr [index + 0], y);
			STP::step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);

			x  = fstb::ToolsSimd::loadu_2f32 (&in_ptr [index + 2]);
			x  = fstb::ToolsSimd::interleave_2f32_low (x, y2);
			sb = b1 * x2 +  b2 * x1;
			sa = a1 * y2 +  a2 * y1;
			y1 = b0 * x  + (sb - sa);
			x1 = x;
			y  = fstb::ToolsSimd::Shift <2>::rotate (y1);
			fstb::ToolsSimd::storeu_2f32 (&out_ptr [index + 2], y);
			STP::step_z_eq (b0, b1, b2, a1, a2, b_inc, a_inc);

			index += 4;

			-- half_nbr_spl;
		}
		while (half_nbr_spl > 0);

		V128Par::store_f32 (data._mem_x [0], x1);
		V128Par::store_f32 (data._mem_x [1], x2);
		V128Par::store_f32 (data._mem_y [0], y1);
		V128Par::store_f32 (data._mem_y [1], y2);

		STP::store_result (data, b0, b1, b2, a1, a2);
	}

	// If number of samples was odd, there is one more to process.
	if ((nbr_spl & 1) > 0)
	{
		const auto     x = fstb::ToolsSimd::loadu_2f32 (&in_ptr [index]);
		const auto     y = process_sample_2x2_latency (data, x, b_inc, a_inc);
		fstb::ToolsSimd::storeu_2f32 (&out_ptr [index], y);
	}
}



template <class STP>
fstb::ToolsSimd::VectF32	Biquad4Simd_Proc <STP>::process_sample_2x2_latency (Biquad4SimdData &data, fstb::ToolsSimd::VectF32 x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	auto           b0 = V128Par::load_f32 (data._z_eq_b [0]);
	auto           b1 = V128Par::load_f32 (data._z_eq_b [1]);
	auto           b2 = V128Par::load_f32 (data._z_eq_b [2]);
	auto           a1 = V128Par::load_f32 (data._z_eq_a [1]);
	auto           a2 = V128Par::load_f32 (data._z_eq_a [2]);

	const int      alt_pos = 1 - data._mem_pos;

	const auto     x1 = V128Par::load_f32 (data._mem_x [data._mem_pos]);
	const auto     x2 = V128Par::load_f32 (data._mem_x [      alt_pos]);
	const auto     y1 = V128Par::load_f32 (data._mem_y [data._mem_pos]);
	const auto     y2 = V128Par::load_f32 (data._mem_y [      alt_pos]);

	x = fstb::ToolsSimd::interleave_2f32_low (x, y1);		// y1[1 0] x[1 0]

	auto           y  = b0 * x;
	const auto     sb = b1 * x1 + b2 * x2;
	const auto     sa = a1 * y1 + a2 * y2;
	y += sb - sa;

	V128Par::store_f32 (data._mem_x [alt_pos], x);
	V128Par::store_f32 (data._mem_y [alt_pos], y);
	data._mem_pos = alt_pos;

	y = fstb::ToolsSimd::Shift <2>::rotate (y);

	STP::step_z_eq_store_result (data, b0, b1, b2, a1, a2, b_inc, a_inc);

	return y;
}



template <class STP>
void	Biquad4Simd_Proc <STP>::process_block_2x2_immediate_post (Biquad4SimdData &data, float out_ptr [], const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	const int      alt_pos = 1 - data._mem_pos;
	float          x_s;

	x_s = data._mem_y [data._mem_pos] [0];
	out_ptr [0] = process_sample_single_stage (data, x_s, 2, data._mem_pos, alt_pos);
	x_s = data._mem_y [data._mem_pos] [1];
	out_ptr [1] = process_sample_single_stage (data, x_s, 3, data._mem_pos, alt_pos);
	STP::step_z_eq (data, b_inc, a_inc);
}



template <class STP>
float	Biquad4Simd_Proc <STP>::process_sample_single_stage (Biquad4SimdData &data, float x_s, int stage, int mem_pos, int alt_pos)
{
	assert (&data != 0);
	assert (stage >= 0);
	assert (stage < (Biquad4Simd <V128Dest, V128Src, V128Par>::_nbr_units));
	assert (mem_pos >= 0);
	assert (mem_pos < 2);
	assert (alt_pos >= 0);
	assert (alt_pos < 2);
	assert (alt_pos + mem_pos == 1);

	const float    x_1 = data._mem_x [mem_pos] [stage];
	const float    y_1 = data._mem_y [mem_pos] [stage];

	const float    y_s =
		  x_s                           * data._z_eq_b [0] [stage]
		+ x_1                           * data._z_eq_b [1] [stage]
		+ data._mem_x [alt_pos] [stage] * data._z_eq_b [2] [stage]
		- y_1                           * data._z_eq_a [1] [stage]
		- data._mem_y [alt_pos] [stage] * data._z_eq_a [2] [stage];

	data._mem_x [alt_pos] [stage] = x_1;
	data._mem_y [alt_pos] [stage] = y_1;
	data._mem_x [mem_pos] [stage] = x_s;
	data._mem_y [mem_pos] [stage] = y_s;

	return y_s;
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
Biquad4Simd <VD, VS, VP>::Biquad4Simd ()
// :	_data ()
{
	assert (V128Par::check_ptr (&_data._z_eq_a [0]));
	assert (V128Par::check_ptr (&_data._z_eq_b [0]));
	assert (V128Par::check_ptr (&_data._mem_x [0]));
	assert (V128Par::check_ptr (&_data._mem_y [0]));

	_data._mem_pos = 0;
	neutralise ();
	clear_buffers ();
}



template <class VD, class VS, class VP>
Biquad4Simd <VD, VS, VP>::Biquad4Simd (const Biquad4Simd <VD, VS, VP> &other)
:	_data (other._data)
{
	assert (&other != 0);
}



template <class VD, class VS, class VP>
Biquad4Simd <VD, VS, VP> &	Biquad4Simd <VD, VS, VP>::operator = (const Biquad4Simd <VD, VS, VP> &other)
{
	assert (&other != 0);

	if (this != &other)
	{
		_data = other._data;
	}

	return (*this);
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::neutralise ()
{
	V128Par::store_f32 (_data._z_eq_b [0], fstb::ToolsSimd::set1_f32 (1));
	V128Par::store_f32 (_data._z_eq_b [1], fstb::ToolsSimd::set_f32_zero ());
	V128Par::store_f32 (_data._z_eq_b [2], fstb::ToolsSimd::set_f32_zero ());

	V128Par::store_f32 (_data._z_eq_a [1], fstb::ToolsSimd::set_f32_zero ());
	V128Par::store_f32 (_data._z_eq_a [2], fstb::ToolsSimd::set_f32_zero ());
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::neutralise_one (int biq)
{
	assert (biq >= 0);
	assert (biq < _nbr_units);

	_data._z_eq_b [0] [biq] = 1;
	_data._z_eq_b [1] [biq] = 0;
	_data._z_eq_b [2] [biq] = 0;

	_data._z_eq_a [1] [biq] = 0;
	_data._z_eq_a [2] [biq] = 0;
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::set_z_eq (const VectFloat4 b [3], const VectFloat4 a [3])
{
	assert (b != 0);
	assert (a != 0);

	V128Par::store_f32 (_data._z_eq_b [0], V128Par::load (b [0]));
	V128Par::store_f32 (_data._z_eq_b [1], V128Par::load (b [1]));
	V128Par::store_f32 (_data._z_eq_b [2], V128Par::load (b [2]));

	V128Par::store_f32 (_data._z_eq_a [1], V128Par::load (a [1]));
	V128Par::store_f32 (_data._z_eq_a [2], V128Par::load (a [2]));
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::set_z_eq_same (const float b [3], const float a [3])
{
	assert (b != 0);
	assert (a != 0);

	V128Par::store_f32 (_data._z_eq_b [0], fstb::ToolsSimd::set1_f32 (b [0]));
	V128Par::store_f32 (_data._z_eq_b [1], fstb::ToolsSimd::set1_f32 (b [1]));
	V128Par::store_f32 (_data._z_eq_b [2], fstb::ToolsSimd::set1_f32 (b [2]));

	V128Par::store_f32 (_data._z_eq_a [1], fstb::ToolsSimd::set1_f32 (a [1]));
	V128Par::store_f32 (_data._z_eq_a [2], fstb::ToolsSimd::set1_f32 (a [2]));
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::set_z_eq_one (int biq, const float b [3], const float a [3])
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (b != 0);
	assert (a != 0);

	_data._z_eq_b [0] [biq] = b [0];
	_data._z_eq_b [1] [biq] = b [1];
	_data._z_eq_b [2] [biq] = b [2];

	_data._z_eq_a [1] [biq] = a [1];
	_data._z_eq_a [2] [biq] = a [2];
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::get_z_eq (VectFloat4 b [3], VectFloat4 a [3]) const
{
	assert (b != 0);
	assert (a != 0);

	V128Par::store_f32 (b [0], V128Par::load (_data._z_eq_b [0]));
	V128Par::store_f32 (b [1], V128Par::load (_data._z_eq_b [1]));
	V128Par::store_f32 (b [2], V128Par::load (_data._z_eq_b [2]));

	V128Par::store_f32 (a [1], V128Par::load (_data._z_eq_a [1]));
	V128Par::store_f32 (a [2], V128Par::load (_data._z_eq_a [2]));
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::get_z_eq_one (int biq, float b [3], float a [3]) const
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (b != 0);
	assert (a != 0);

	b [0] = _data._z_eq_b [0] [biq];
	b [1] = _data._z_eq_b [1] [biq];
	b [2] = _data._z_eq_b [2] [biq];

	a [1] = _data._z_eq_a [1] [biq];
	a [2] = _data._z_eq_a [2] [biq];
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::set_state_one (int biq, float const mem_x [2], const float mem_y [2])
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (mem_x != 0);
	assert (mem_y != 0);

	_data._mem_x [0] [biq] = mem_x [0];
	_data._mem_x [1] [biq] = mem_x [1];

	_data._mem_y [0] [biq] = mem_y [0];
	_data._mem_y [1] [biq] = mem_y [1];
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::get_state_one (int biq, float mem_x [2], float mem_y [2]) const
{
	assert (biq >= 0);
	assert (biq < _nbr_units);
	assert (mem_x != 0);
	assert (mem_y != 0);

	mem_x [0] = _data._mem_x [0] [biq];
	mem_x [1] = _data._mem_x [1] [biq];

	mem_y [0] = _data._mem_y [0] [biq];
	mem_y [1] = _data._mem_y [1] [biq];
}



// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_block_parallel (_data, out_ptr, in_ptr, nbr_spl, 0, 0);
}



// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

	Proc::process_block_parallel (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP, Biquad4Simd_LoaderSingle> >	Proc;

	Proc::process_block_parallel (_data, out_ptr, in_ptr, nbr_spl, 0, 0);
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP, Biquad4Simd_LoaderSingle> >	Proc;

	Proc::process_block_parallel (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



template <class VD, class VS, class VP>
fstb::ToolsSimd::VectF32	Biquad4Simd <VD, VS, VP>::process_sample_parallel (const fstb::ToolsSimd::VectF32 &x)
{
	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	return (Proc::process_sample_parallel (_data, x, 0, 0));
}



template <class VD, class VS, class VP>
fstb::ToolsSimd::VectF32	Biquad4Simd <VD, VS, VP>::process_sample_parallel (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

	return (Proc::process_sample_parallel (_data, x, b_inc, a_inc));
}



// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_serial_latency (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_block_serial_latency (_data, out_ptr, in_ptr, nbr_spl, 0, 0);
}



// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_serial_latency (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

	Proc::process_block_serial_latency (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



/*
==============================================================================
Name: process_sample_serial_latency
Description:

Processes the 4 biquads in serial, outputting the result with a latency of 4
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
a ---Biq0--> A ---Biq1--> A' ---Biq2--> A" ---Biq3--> A"'

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
float	Biquad4Simd <VD, VS, VP>::process_sample_serial_latency (float x_s)
{
	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	return (Proc::process_sample_serial_latency (_data, x_s, 0, 0));
}



template <class VD, class VS, class VP>
float	Biquad4Simd <VD, VS, VP>::process_sample_serial_latency (float x_s, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

	return (Proc::process_sample_serial_latency (_data, x_s, b_inc, a_inc));
}



// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_serial_immediate (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_SERIAL + 1.
	if (nbr_spl < _latency_serial + 1)
	{
		long				pos = 0;
		do
		{
			out_ptr [pos] = process_sample_serial_immediate (in_ptr [pos]);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

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
void	Biquad4Simd <VD, VS, VP>::process_block_serial_immediate (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_SERIAL + 1.
	if (nbr_spl < _latency_serial + 1)
	{
		long				pos = 0;
		do
		{
			out_ptr [pos] = process_sample_serial_immediate (in_ptr [pos], b_inc, a_inc);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

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
float	Biquad4Simd <VD, VS, VP>::process_sample_serial_immediate (float x_s)
{
	const int		alt_pos = 1 - _data._mem_pos;

	x_s = process_sample_single_stage_noswap (0, x_s, alt_pos);
	x_s = process_sample_single_stage_noswap (1, x_s, alt_pos);
	x_s = process_sample_single_stage_noswap (2, x_s, alt_pos);
	x_s = process_sample_single_stage_noswap (3, x_s, alt_pos);

	_data._mem_pos = alt_pos;

	return (x_s);
}



template <class VD, class VS, class VP>
float	Biquad4Simd <VD, VS, VP>::process_sample_serial_immediate (float x_s, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	const float		y_s = process_sample_serial_immediate (x_s);

	Biquad4Simd_StepOn <VD, VS, VP>::step_z_eq (_data, b_inc, a_inc);

	return (y_s);
}



// Input and output are stereo interlaced data
// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_2x2_latency (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	Proc::process_block_2x2_latency (_data, out_ptr, in_ptr, nbr_spl, 0, 0);
}



// Input and output are stereo interlaced data
// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_2x2_latency (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

	Proc::process_block_2x2_latency (_data, out_ptr, in_ptr, nbr_spl, b_inc, a_inc);
}



/*
==============================================================================
Name: process_sample_2x2_latency
Description:

Processes the 4 biquads arranged in two parallel paths made of two biquads in
serial. First path is made of biquads 0 and 2, second path is made of biquads
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
a ---Biq0--> A ---Biq1--> A'

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
fstb::ToolsSimd::VectF32	Biquad4Simd <VD, VS, VP>::process_sample_2x2_latency (const fstb::ToolsSimd::VectF32 &x)
{
	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	return (Proc::process_sample_2x2_latency (_data, x, 0, 0));
}



template <class VD, class VS, class VP>
fstb::ToolsSimd::VectF32	Biquad4Simd <VD, VS, VP>::process_sample_2x2_latency (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (&x != 0);
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

	return (Proc::process_sample_2x2_latency (_data, x, b_inc, a_inc));
}



// Can work in-place
template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_2x2_immediate (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_2X2 + 1.
	if (nbr_spl < _latency_2x2 + 1)
	{
		long           pos = 0;
		do
		{
			const auto     x = fstb::ToolsSimd::loadu_2f32 (in_ptr + pos * 2);
			const auto     y = process_sample_2x2_immediate (x);
			fstb::ToolsSimd::storeu_2f32 (out_ptr + pos * 2, y);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

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
void	Biquad4Simd <VD, VS, VP>::process_block_2x2_immediate (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_2X2 + 1.
	if (nbr_spl < _latency_2x2 + 1)
	{
		long           pos = 0;
		do
		{
			const auto     x = fstb::ToolsSimd::loadu_2f32 (in_ptr + pos * 2);
			const auto     y = process_sample_2x2_immediate (x, b_inc, a_inc);
			fstb::ToolsSimd::storeu_2f32 (out_ptr + pos * 2, y);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		typedef	Biquad4Simd_Proc <Biquad4Simd_StepOn <VD, VS, VP> >	Proc;

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
fstb::ToolsSimd::VectF32	Biquad4Simd <VD, VS, VP>::process_sample_2x2_immediate (const fstb::ToolsSimd::VectF32 &x)
{
	float          x_0 = fstb::ToolsSimd::Shift <0>::extract (x);
	float          x_1 = fstb::ToolsSimd::Shift <1>::extract (x);

	const int      alt_pos = 1 - _data._mem_pos;

	x_0 = process_sample_single_stage_noswap (0, x_0, alt_pos);
	x_1 = process_sample_single_stage_noswap (1, x_1, alt_pos);
	x_0 = process_sample_single_stage_noswap (2, x_0, alt_pos);
	x_1 = process_sample_single_stage_noswap (3, x_1, alt_pos);

	_data._mem_pos = alt_pos;

	const auto     y = fstb::ToolsSimd::set_2f32 (x_0, x_1);

	return (y);
}



template <class VD, class VS, class VP>
fstb::ToolsSimd::VectF32	Biquad4Simd <VD, VS, VP>::process_sample_2x2_immediate (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3])
{
	assert (V128Par::check_ptr (b_inc));
	assert (V128Par::check_ptr (a_inc));

	const auto     y = process_sample_2x2_immediate (x);

	Biquad4Simd_StepOn <VD, VS, VP>::step_z_eq (_data, b_inc, a_inc);

	return y;
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::clear_buffers ()
{
	V128Par::store_f32 (_data._mem_x [0], fstb::ToolsSimd::set_f32_zero ());
	V128Par::store_f32 (_data._mem_x [1], fstb::ToolsSimd::set_f32_zero ());
	V128Par::store_f32 (_data._mem_y [0], fstb::ToolsSimd::set_f32_zero ());
	V128Par::store_f32 (_data._mem_y [1], fstb::ToolsSimd::set_f32_zero ());
	_data._mem_pos = 0;
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::clear_buffers_one (int biq)
{
	assert (biq >= 0);
	assert (biq < _nbr_units);

	_data._mem_x [0] [biq] = 0;
	_data._mem_x [1] [biq] = 0;

	_data._mem_y [0] [biq] = 0;
	_data._mem_y [1] [biq] = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
float	Biquad4Simd <VD, VS, VP>::process_sample_single_stage_noswap (int stage, float x_s, int alt_pos)
{
	const float    y_s =
		   x_s                                  * _data._z_eq_b [0] [stage]
		+ _data._mem_x [_data._mem_pos] [stage] * _data._z_eq_b [1] [stage]
		+ _data._mem_x [       alt_pos] [stage] * _data._z_eq_b [2] [stage]
		- _data._mem_y [_data._mem_pos] [stage] * _data._z_eq_a [1] [stage]
		- _data._mem_y [       alt_pos] [stage] * _data._z_eq_a [2] [stage];

	_data._mem_x [alt_pos] [stage] = x_s;
	_data._mem_y [alt_pos] [stage] = y_s;

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
void	Biquad4Simd <VD, VS, VP>::process_block_serial_immediate_pre (const float in_ptr [])
{
	assert (in_ptr != 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	const int      alt_pos = 1 - _data._mem_pos;
	float          x_s;

	x_s = Proc::process_sample_single_stage (_data, in_ptr [0], 0, _data._mem_pos, alt_pos);
	x_s = Proc::process_sample_single_stage (_data, x_s, 1, _data._mem_pos, alt_pos);
	x_s = Proc::process_sample_single_stage (_data, x_s, 2, _data._mem_pos, alt_pos);
	Proc::process_sample_single_stage (_data, x_s, 3, _data._mem_pos, alt_pos);

	x_s = Proc::process_sample_single_stage (_data, in_ptr [1], 0, _data._mem_pos, alt_pos);
	x_s = Proc::process_sample_single_stage (_data, x_s, 1, _data._mem_pos, alt_pos);
	Proc::process_sample_single_stage (_data, x_s, 2, _data._mem_pos, alt_pos);

	x_s = Proc::process_sample_single_stage (_data, in_ptr [2], 0, _data._mem_pos, alt_pos);
	Proc::process_sample_single_stage (_data, x_s, 1, _data._mem_pos, alt_pos);

	Proc::process_sample_single_stage (_data, in_ptr [3], 0, _data._mem_pos, alt_pos);
}



template <class VD, class VS, class VP>
void	Biquad4Simd <VD, VS, VP>::process_block_2x2_immediate_pre (const float in_ptr [])
{
	assert (in_ptr != 0);

	typedef	Biquad4Simd_Proc <Biquad4Simd_StepOff <VD, VS, VP> >	Proc;

	const int      alt_pos = 1 - _data._mem_pos;

	Proc::process_sample_single_stage (_data, in_ptr [0], 0, _data._mem_pos, alt_pos);
	Proc::process_sample_single_stage (_data, in_ptr [1], 1, _data._mem_pos, alt_pos);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Biquad4Simd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
