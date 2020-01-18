/*****************************************************************************

        SvfCore4Simd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SvfCore4Simd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SvfCore4Simd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP, class MX>
SvfCore4Simd <VD, VS, VP, MX>::SvfCore4Simd ()
// :	_data ()
{
	neutralise ();
	clear_buffers ();
}



template <class VD, class VS, class VP, class MX>
SvfCore4Simd <VD, VS, VP, MX>::SvfCore4Simd (const SvfCore4Simd <VD, VS, VP, MX> &other)
:	_data (other._data)
{
	// Nothing
}



template <class VD, class VS, class VP, class MX>
SvfCore4Simd <VD, VS, VP, MX> &	SvfCore4Simd <VD, VS, VP, MX>::operator = (const SvfCore4Simd <VD, VS, VP, MX> &other)
{
	if (this != &other)
	{
		_data = other._data;
	}

	return *this;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::neutralise ()
{
	const auto     zero = fstb::ToolsSimd::set_f32_zero ();
	V128Par::store_f32 (_data._g0 , zero);
	V128Par::store_f32 (_data._g1 , zero);
	V128Par::store_f32 (_data._g2 , zero);
	V128Par::store_f32 (_data._v0m, fstb::ToolsSimd::set1_f32 (1));
	V128Par::store_f32 (_data._v1m, zero);
	V128Par::store_f32 (_data._v2m, zero);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::neutralise_one (int unit)
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	_data._g0  [unit] = 0;
	_data._g1  [unit] = 0;
	_data._g2  [unit] = 0;
	_data._v0m [unit] = 1;
	_data._v1m [unit] = 0;
	_data._v2m [unit] = 0;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::set_coefs (const VectFloat4 g0, const VectFloat4 g1, const VectFloat4 g2)
{
	assert (g0 != 0);
	assert (g1 != 0);
	assert (g2 != 0);

	V128Par::store_f32 (_data._g0, V128Par::load_f32 (g0));
	V128Par::store_f32 (_data._g1, V128Par::load_f32 (g1));
	V128Par::store_f32 (_data._g2, V128Par::load_f32 (g2));
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::set_coefs_one (int unit, float g0, float g1, float g2)
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	_data._g0 [unit] = g0;
	_data._g1 [unit] = g1;
	_data._g2 [unit] = g2;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::get_coefs_one (int unit, float &g0, float &g1, float &g2) const
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	g0 = _data._g0 [unit];
	g1 = _data._g1 [unit];
	g2 = _data._g2 [unit];
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::set_mix (const VectFloat4 v0m, const VectFloat4 v1m, const VectFloat4 v2m)
{
	assert (v0m != 0);
	assert (v1m != 0);
	assert (v2m != 0);

	V128Par::store_f32 (_data._v0m, V128Par::load_f32 (v0m));
	V128Par::store_f32 (_data._v1m, V128Par::load_f32 (v1m));
	V128Par::store_f32 (_data._v2m, V128Par::load_f32 (v2m));
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::set_mix_one (int unit, float v0m, float v1m, float v2m)
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	_data._v0m [unit] = v0m;
	_data._v1m [unit] = v1m;
	_data._v2m [unit] = v2m;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::get_mix_one (int unit, float &v0m, float &v1m, float &v2m) const
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	v0m = _data._v0m [unit];
	v1m = _data._v1m [unit];
	v2m = _data._v2m [unit];
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::copy_z_eq (const SvfCore4Simd <VD, VS, VP, MX> &other)
{
	V128Par::store_f32 (_data._g0 , V128Par::load_f32 (other._data.g0 ));
	V128Par::store_f32 (_data._g1 , V128Par::load_f32 (other._data.g1 ));
	V128Par::store_f32 (_data._g2 , V128Par::load_f32 (other._data.g2 ));
	V128Par::store_f32 (_data._v0m, V128Par::load_f32 (other._data._v0m));
	V128Par::store_f32 (_data._v1m, V128Par::load_f32 (other._data._v1m));
	V128Par::store_f32 (_data._v2m, V128Par::load_f32 (other._data._v2m));
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::clear_buffers ()
{
	const auto     zero = fstb::ToolsSimd::set_f32_zero ();
	V128Par::store_f32 (_data._ic1eq, zero);
	V128Par::store_f32 (_data._ic2eq, zero);
	V128Par::store_f32 (_data._y    , zero);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::clear_buffers_one (int unit)
{
	assert (unit >= 0);
	assert (unit < _nbr_units);

	_data._ic1eq [unit] = 0;
	_data._ic2eq [unit] = 0;
	_data._y     [unit] = 0;
}



/*****************************************************************************
*
*       PARALLEL
*
*****************************************************************************/



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_par (const fstb::ToolsSimd::VectF32 &x)
{
	const auto     g0    = V128Par::load_f32 (_data._g0   );
	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );

	return process_sample_par (x, g0, g1, g2);
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_par (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2)
{
	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	return process_sample_par (x, g0, g1, g2, v0m, v1m, v2m);
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_par (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2, const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m, const fstb::ToolsSimd::VectF32 &v2m)
{
	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);

	fstb::ToolsSimd::VectF32   v1;
	fstb::ToolsSimd::VectF32   v2;
	iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);

	return Mixer::mix (x, v1, v2, v0m, v1m, v2m);
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_par_inc (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	auto           g0    = V128Par::load_f32 (_data._g0   );
	auto           g1    = V128Par::load_f32 (_data._g1   );
	auto           g2    = V128Par::load_f32 (_data._g2   );
	auto           v0m   = V128Par::load_f32 (_data._v0m  );
	auto           v1m   = V128Par::load_f32 (_data._v1m  );
	auto           v2m   = V128Par::load_f32 (_data._v2m  );
	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);

	fstb::ToolsSimd::VectF32   v1;
	fstb::ToolsSimd::VectF32   v2;
	iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
	const auto     y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
	increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	V128Par::store_f32 (_data._g0   , g0   );
	V128Par::store_f32 (_data._g1   , g1   );
	V128Par::store_f32 (_data._g2   , g2   );
	V128Par::store_f32 (_data._v0m  , v0m  );
	V128Par::store_f32 (_data._v1m  , v1m  );
	V128Par::store_f32 (_data._v2m  , v2m  );
	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);

	return (y);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], int nbr_spl)
{
	assert (V128Dst::check_ptr (dst_ptr));
	assert (V128Src::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	const auto     g0    = V128Par::load_f32 (_data._g0   );
	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);

	int            pos   = 0;
	do
	{
		const auto     x = V128Src::load (src_ptr + pos);
		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		const auto     y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		V128Dst::store_f32 (dst_ptr + pos, y);

		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [])
{
	assert (V128Dst::check_ptr (dst_ptr));
	assert (V128Src::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert (V128Src::check_ptr (g0_ptr));
	assert (V128Src::check_ptr (g1_ptr));
	assert (V128Src::check_ptr (g2_ptr));

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);

	int            pos   = 0;
	do
	{
		const auto     x  = V128Src::load_f32 (src_ptr + pos);
		const auto     g0 = V128Src::load_f32 (g0_ptr  + pos);
		const auto     g1 = V128Src::load_f32 (g1_ptr  + pos);
		const auto     g2 = V128Src::load_f32 (g2_ptr  + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		const auto     y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		V128Dst::store_f32 (dst_ptr + pos, y);

		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [], const fstb::ToolsSimd::VectF32 v0m_ptr [], const fstb::ToolsSimd::VectF32 v1m_ptr [], const fstb::ToolsSimd::VectF32 v2m_ptr [])
{
	assert (V128Dst::check_ptr (dst_ptr));
	assert (V128Src::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert (V128Src::check_ptr (g0_ptr));
	assert (V128Src::check_ptr (g1_ptr));
	assert (V128Src::check_ptr (g2_ptr));
	assert (V128Src::check_ptr (v0m_ptr));
	assert (V128Src::check_ptr (v1m_ptr));
	assert (V128Src::check_ptr (v2m_ptr));

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);

	int            pos   = 0;
	do
	{
		const auto     x  = V128Src::load_f32 (src_ptr + pos);
		const auto     g0 = V128Src::load_f32 (g0_ptr  + pos);
		const auto     g1 = V128Src::load_f32 (g1_ptr  + pos);
		const auto     g2 = V128Src::load_f32 (g2_ptr  + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);

		const auto     v0m = V128Src::load_f32 (v0m_ptr + pos);
		const auto     v1m = V128Src::load_f32 (v1m_ptr + pos);
		const auto     v2m = V128Src::load_f32 (v2m_ptr + pos);

		const auto     y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		V128Dst::store_f32 (dst_ptr + pos, y);

		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (V128Dst::check_ptr (dst_ptr));
	assert (V128Src::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	const auto     g0    = V128Par::load_f32 (_data._g0   );
	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	const auto     ic1eq = V128Par::load_f32 (_data._ic1eq);
	const auto     ic2eq = V128Par::load_f32 (_data._ic2eq);

	int            pos   = 0;
	do
	{
		const auto     x  = V128Src::load_f32 (src_ptr + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		const auto     y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);
		V128Dst::store_f32 (dst_ptr + pos, y);

		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._g0   , g0   );
	V128Par::store_f32 (_data._g1   , g1   );
	V128Par::store_f32 (_data._g2   , g2   );
	V128Par::store_f32 (_data._v0m  , v0m  );
	V128Par::store_f32 (_data._v1m  , v1m  );
	V128Par::store_f32 (_data._v2m  , v2m  );
	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
}



/*****************************************************************************
*
*       2X2 WITH LATENCY
*
*****************************************************************************/



// Only the two lower words are taken into account.
// As result, the two upper words may contain garbage and must be discarded.
template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_2x2_lat (const fstb::ToolsSimd::VectF32 &x)
{
	const auto     g0    = V128Par::load_f32 (_data._g0   );
	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );

	return process_sample_2x2_lat (x, g0, g1, g2);
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_2x2_lat (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2)
{
	const auto     v0m   = V128Par::load_f32 (_data._v0m);
	const auto     v1m   = V128Par::load_f32 (_data._v1m);
	const auto     v2m   = V128Par::load_f32 (_data._v2m);

	return process_sample_2x2_lat (x, g0, g1, g2, v0m, v1m, v2m);
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_2x2_lat (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2, const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m, const fstb::ToolsSimd::VectF32 &v2m)
{
	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	const auto     xx = fstb::ToolsSimd::interleave_2f32_lo (x, y);		// y[1 0] x[1 0]

	fstb::ToolsSimd::VectF32   v1;
	fstb::ToolsSimd::VectF32   v2;
	iterate_parallel (xx, v1, v2, ic1eq, ic2eq, g0, g1, g2);
	y = Mixer::mix (xx, v1, v2, v0m, v1m, v2m);

	const auto     ys     = fstb::ToolsSimd::interleave_2f32_hi (y, y);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );

	return ys;
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_2x2_lat_inc (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	auto           g0    = V128Par::load_f32 (_data._g0   );
	auto           g1    = V128Par::load_f32 (_data._g1   );
	auto           g2    = V128Par::load_f32 (_data._g2   );
	auto           v0m   = V128Par::load_f32 (_data._v0m  );
	auto           v1m   = V128Par::load_f32 (_data._v1m  );
	auto           v2m   = V128Par::load_f32 (_data._v2m  );
	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	const auto     xx = fstb::ToolsSimd::interleave_2f32_lo (x, y);		// y[1 0] x[1 0]

	fstb::ToolsSimd::VectF32   v1;
	fstb::ToolsSimd::VectF32   v2;
	iterate_parallel (xx, v1, v2, ic1eq, ic2eq, g0, g1, g2);
	y = Mixer::mix (xx, v1, v2, v0m, v1m, v2m);
	increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	const auto     ys     = fstb::ToolsSimd::interleave_2f32_hi (y, y);

	V128Par::store_f32 (_data._g0   , g0   );
	V128Par::store_f32 (_data._g1   , g1   );
	V128Par::store_f32 (_data._g2   , g2   );
	V128Par::store_f32 (_data._v0m  , v0m  );
	V128Par::store_f32 (_data._v1m  , v1m  );
	V128Par::store_f32 (_data._v2m  , v2m  );
	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );

	return ys;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_lat (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );
	const auto     g0    = V128Par::load_f32 (_data._g0   );

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );


	int            pos = 0;
	do
	{
		auto           x = fstb::ToolsSimd::loadu_2f32 (src_ptr + pos * 2);
		x = fstb::ToolsSimd::interleave_2f32_lo (x, y);		// y[1 0] x[1 0]

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);

		const auto      ys  = fstb::ToolsSimd::interleave_2f32_hi (y, y);
		fstb::ToolsSimd::storeu_2f32 (dst_ptr + pos * 2, ys);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_lat (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [])
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (V128Src::check_ptr (g0_ptr));
	assert (V128Src::check_ptr (g1_ptr));
	assert (V128Src::check_ptr (g2_ptr));

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );


	int            pos   = 0;
	do
	{
		auto           x = fstb::ToolsSimd::loadu_2f32 (src_ptr + pos * 2);
		x = fstb::ToolsSimd::interleave_2f32_lo (x, y);		// y[1 0] x[1 0]

		const auto     g0 = V128Src::load_f32 (g0_ptr + pos);
		const auto     g1 = V128Src::load_f32 (g1_ptr + pos);
		const auto     g2 = V128Src::load_f32 (g2_ptr + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);

		const auto     ys = fstb::ToolsSimd::interleave_2f32_hi (y, y);
		fstb::ToolsSimd::storeu_2f32 (dst_ptr + pos * 2, ys);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_lat (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [], const fstb::ToolsSimd::VectF32 v0m_ptr [], const fstb::ToolsSimd::VectF32 v1m_ptr [], const fstb::ToolsSimd::VectF32 v2m_ptr [])
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (V128Src::check_ptr (g0_ptr));
	assert (V128Src::check_ptr (g1_ptr));
	assert (V128Src::check_ptr (g2_ptr));
	assert (V128Src::check_ptr (v0m_ptr));
	assert (V128Src::check_ptr (v1m_ptr));
	assert (V128Src::check_ptr (v2m_ptr));

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	int            pos   = 0;
	do
	{
		auto           x = fstb::ToolsSimd::loadu_2f32 (src_ptr + pos * 2);
		x = fstb::ToolsSimd::interleave_2f32_lo (x, y);		// y[1 0] x[1 0]

		const auto     g0  = V128Src::load_f32 (g0_ptr  + pos);
		const auto     g1  = V128Src::load_f32 (g1_ptr  + pos);
		const auto     g2  = V128Src::load_f32 (g2_ptr  + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);

		const auto     v0m = V128Src::load_f32 (v0m_ptr + pos);
		const auto     v1m = V128Src::load_f32 (v1m_ptr + pos);
		const auto     v2m = V128Src::load_f32 (v2m_ptr + pos);

		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);

		const auto     ys  = fstb::ToolsSimd::interleave_2f32_hi (y, y);
		fstb::ToolsSimd::storeu_2f32 (dst_ptr + pos * 2, ys);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_lat (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	auto           g0    = V128Par::load_f32 (_data._g0   );
	auto           g1    = V128Par::load_f32 (_data._g1   );
	auto           g2    = V128Par::load_f32 (_data._g2   );

	auto           v0m   = V128Par::load_f32 (_data._v0m  );
	auto           v1m   = V128Par::load_f32 (_data._v1m  );
	auto           v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	int            pos   = 0;
	do
	{
		auto           x = fstb::ToolsSimd::loadu_2f32 (src_ptr + pos * 2);
		x = fstb::ToolsSimd::interleave_2f32_lo (x, y);		// y[1 0] x[1 0]

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

		const auto     ys  = fstb::ToolsSimd::interleave_2f32_hi (y, y);
		fstb::ToolsSimd::storeu_2f32 (dst_ptr + pos * 2, ys);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._g0   , g0   );
	V128Par::store_f32 (_data._g1   , g1   );
	V128Par::store_f32 (_data._g2   , g2   );
	V128Par::store_f32 (_data._v0m  , v0m  );
	V128Par::store_f32 (_data._v1m  , v1m  );
	V128Par::store_f32 (_data._v2m  , v2m  );
	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



/*****************************************************************************
*
*       2X2, IMMEDIATE
*
*****************************************************************************/



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_2x2_imm (const fstb::ToolsSimd::VectF32 &x)
{
	float          x_0 = fstb::ToolsSimd::Shift <0>::extract (x);
	float          x_1 = fstb::ToolsSimd::Shift <1>::extract (x);

	x_0 = process_sample_single_stage (_data, x_0, 0, 0);
	x_1 = process_sample_single_stage (_data, x_1, 1, 1);
	x_0 = process_sample_single_stage (_data, x_0, 2, 2);
	x_1 = process_sample_single_stage (_data, x_1, 3, 3);

	return fstb::ToolsSimd::set_2f32 (x_0, x_1);
}



template <class VD, class VS, class VP, class MX>
fstb::ToolsSimd::VectF32	SvfCore4Simd <VD, VS, VP, MX>::process_sample_2x2_imm (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	const auto     y = process_sample_2x2_imm (x);
	increment (_data, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	return y;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_imm (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is _latency_2x2 + 1.
	if (nbr_spl < _latency_2x2 + 1)
	{
		int            pos = 0;
		do
		{
			const auto     x = fstb::ToolsSimd::loadu_2f32 (src_ptr + pos * 2);
			const auto     y = process_sample_2x2_imm (x);
			fstb::ToolsSimd::storeu_2f32 (dst_ptr + pos * 2, y);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		process_block_2x2_imm_pre (src_ptr);
		process_block_2x2_lat (
			&dst_ptr [0],
			&src_ptr [_latency_2x2 * 2],
			nbr_spl - _latency_2x2
		);
		process_block_2x2_imm_post (&dst_ptr [(nbr_spl - _latency_2x2) * 2]);
	}
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_imm (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is _latency_2x2 + 1.
	if (nbr_spl < _latency_2x2 + 1)
	{
		int            pos = 0;
		do
		{
			const auto     x = fstb::ToolsSimd::loadu_2f32 (src_ptr + pos * 2);
			const auto     y = process_sample_2x2_imm (
				x,
				g0i, g1i, g2i, v0mi, v1mi, v2mi
			);
			fstb::ToolsSimd::storeu_2f32 (dst_ptr + pos * 2, y);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		process_block_2x2_imm_pre (src_ptr);
		process_block_2x2_lat (
			&dst_ptr [0],
			&src_ptr [_latency_2x2 * 2],
			nbr_spl - _latency_2x2,
			g0i, g1i, g2i, v0mi, v1mi, v2mi
		);
		process_block_2x2_imm_post (
			&dst_ptr [(nbr_spl - _latency_2x2) * 2],
			g0i, g1i, g2i, v0mi, v1mi, v2mi
		);
	}
}



/*****************************************************************************
*
*       SERIAL WITH LATENCY
*
*****************************************************************************/



// Only the lower words is taken into account.
// As result, the three upper words may contain garbage and are to be discarded.
template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_ser_lat (float x_s)
{
	const auto     g0    = V128Par::load_f32 (_data._g0   );
	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );

	return process_sample_ser_lat (x_s, g0, g1, g2);
}



template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_ser_lat (float x_s, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2)
{
	const auto     v0m   = V128Par::load (_data._v0m);
	const auto     v1m   = V128Par::load (_data._v1m);
	const auto     v2m   = V128Par::load (_data._v2m);

	return process_sample_ser_lat (x_s, g0, g1, g2, v0m, v1m, v2m);
}



template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_ser_lat (float x_s, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2, const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m, const fstb::ToolsSimd::VectF32 &v2m)
{
	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	const auto     x     = fstb::ToolsSimd::Shift <0>::insert (y, x_s);

	fstb::ToolsSimd::VectF32   v1;
	fstb::ToolsSimd::VectF32   v2;
	iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
	y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
	y = fstb::ToolsSimd::Shift <1>::rotate (y);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );

	const float    y_s = fstb::ToolsSimd::Shift <0>::extract (y);

	return y_s;
}



template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_ser_lat_inc (float x_s, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	auto           g0    = V128Par::load_f32 (_data._g0   );
	auto           g1    = V128Par::load_f32 (_data._g1   );
	auto           g2    = V128Par::load_f32 (_data._g2   );
	auto           v0m   = V128Par::load_f32 (_data._v0m  );
	auto           v1m   = V128Par::load_f32 (_data._v1m  );
	auto           v2m   = V128Par::load_f32 (_data._v2m  );
	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	const auto     x     = fstb::ToolsSimd::Shift <0>::insert (y, x_s);

	fstb::ToolsSimd::VectF32   v1;
	fstb::ToolsSimd::VectF32   v2;
	iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
	y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
	y = fstb::ToolsSimd::Shift <1>::rotate (y);
	increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	V128Par::store_f32 (_data._g0   , g0   );
	V128Par::store_f32 (_data._g1   , g1   );
	V128Par::store_f32 (_data._g2   , g2   );
	V128Par::store_f32 (_data._v0m  , v0m  );
	V128Par::store_f32 (_data._v1m  , v1m  );
	V128Par::store_f32 (_data._v2m  , v2m  );
	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );

	const float    y_s = fstb::ToolsSimd::Shift <0>::extract (y);

	return y_s;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_lat (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const auto     g1    = V128Par::load_f32 (_data._g1   );
	const auto     g2    = V128Par::load_f32 (_data._g2   );
	const auto     g0    = V128Par::load_f32 (_data._g0   );

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	int            pos = 0;
	do
	{
		const auto     x = fstb::ToolsSimd::Shift <0>::insert (y, src_ptr [pos]);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		y = fstb::ToolsSimd::Shift <1>::rotate (y);

		fstb::ToolsSimd::storeu_1f32 (dst_ptr + pos, y);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_lat (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [])
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (V128Src::check_ptr (g0_ptr));
	assert (V128Src::check_ptr (g1_ptr));
	assert (V128Src::check_ptr (g2_ptr));

	const auto     v0m   = V128Par::load_f32 (_data._v0m  );
	const auto     v1m   = V128Par::load_f32 (_data._v1m  );
	const auto     v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	int            pos = 0;
	do
	{
		const auto     x = fstb::ToolsSimd::Shift <0>::insert (y, src_ptr [pos]);

		const auto     g0 = V128Src::load (g0_ptr + pos);
		const auto     g1 = V128Src::load (g1_ptr + pos);
		const auto     g2 = V128Src::load (g2_ptr + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		y = fstb::ToolsSimd::Shift <1>::rotate (y);

		fstb::ToolsSimd::storeu_1f32 (dst_ptr + pos, y);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_lat (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [], const fstb::ToolsSimd::VectF32 v0m_ptr [], const fstb::ToolsSimd::VectF32 v1m_ptr [], const fstb::ToolsSimd::VectF32 v2m_ptr [])
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (V128Src::check_ptr (g0_ptr));
	assert (V128Src::check_ptr (g1_ptr));
	assert (V128Src::check_ptr (g2_ptr));
	assert (V128Src::check_ptr (v0m_ptr));
	assert (V128Src::check_ptr (v1m_ptr));
	assert (V128Src::check_ptr (v2m_ptr));

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	int            pos = 0;
	do
	{
		const auto     x = fstb::ToolsSimd::Shift <0>::insert (y, src_ptr [pos]);

		const auto     g0  = V128Src::load_f32 (g0_ptr  + pos);
		const auto     g1  = V128Src::load_f32 (g1_ptr  + pos);
		const auto     g2  = V128Src::load_f32 (g2_ptr  + pos);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);

		const auto     v0m = V128Src::load_f32 (v0m_ptr + pos);
		const auto     v1m = V128Src::load_f32 (v1m_ptr + pos);
		const auto     v2m = V128Src::load_f32 (v2m_ptr + pos);

		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		y = fstb::ToolsSimd::Shift <1>::rotate (y);

		fstb::ToolsSimd::storeu_1f32 (dst_ptr + pos, y);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_lat (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	auto           g0    = V128Par::load_f32 (_data._g0   );
	auto           g1    = V128Par::load_f32 (_data._g1   );
	auto           g2    = V128Par::load_f32 (_data._g2   );

	auto           v0m   = V128Par::load_f32 (_data._v0m  );
	auto           v1m   = V128Par::load_f32 (_data._v1m  );
	auto           v2m   = V128Par::load_f32 (_data._v2m  );

	auto           ic1eq = V128Par::load_f32 (_data._ic1eq);
	auto           ic2eq = V128Par::load_f32 (_data._ic2eq);
	auto           y     = V128Par::load_f32 (_data._y    );

	int            pos = 0;
	do
	{
		const auto     x = fstb::ToolsSimd::Shift <0>::insert (y, src_ptr [pos]);

		fstb::ToolsSimd::VectF32   v1;
		fstb::ToolsSimd::VectF32   v2;
		iterate_parallel (x, v1, v2, ic1eq, ic2eq, g0, g1, g2);
		y = Mixer::mix (x, v1, v2, v0m, v1m, v2m);
		y = fstb::ToolsSimd::Shift <1>::rotate (y);
		increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

		fstb::ToolsSimd::storeu_1f32 (dst_ptr + pos, y);
		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_data._g0   , g0   );
	V128Par::store_f32 (_data._g1   , g1   );
	V128Par::store_f32 (_data._g2   , g2   );
	V128Par::store_f32 (_data._v0m  , v0m  );
	V128Par::store_f32 (_data._v1m  , v1m  );
	V128Par::store_f32 (_data._v2m  , v2m  );
	V128Par::store_f32 (_data._ic1eq, ic1eq);
	V128Par::store_f32 (_data._ic2eq, ic2eq);
	V128Par::store_f32 (_data._y    , y    );
}



/*****************************************************************************
*
*       SERIAL, IMMEDIATE
*
*****************************************************************************/



template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_ser_imm (float x_s)
{
	x_s = process_sample_single_stage (_data, x_s, 0, 1);
	x_s = process_sample_single_stage (_data, x_s, 1, 2);
	x_s = process_sample_single_stage (_data, x_s, 2, 3);
	x_s = process_sample_single_stage (_data, x_s, 3, 0);

	return x_s;
}



template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_ser_imm (float x_s, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	const float    y = process_sample_ser_imm (x_s);
	increment (_data, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	return y;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_imm (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_SERIAL + 1.
	if (nbr_spl < _latency_serial + 1)
	{
		int            pos = 0;
		do
		{
			dst_ptr [pos] = process_sample_ser_imm (src_ptr [pos]);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		process_block_ser_imm_pre (src_ptr);
		process_block_ser_lat (
			&dst_ptr [0],
			&src_ptr [_latency_serial],
			nbr_spl - _latency_serial
		);
		process_block_ser_imm_post (&dst_ptr [nbr_spl - _latency_serial]);
	}
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_imm (float dst_ptr [], const float src_ptr [], int nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	// We could tune this value. Lower bound is LATENCY_SERIAL + 1.
	if (nbr_spl < _latency_serial + 1)
	{
		int            pos = 0;
		do
		{
			dst_ptr [pos] = process_sample_ser_imm (
				src_ptr [pos],
				g0i, g1i, g2i, v0mi, v1mi, v2mi
			);
			++ pos;
		}
		while (pos < nbr_spl);
	}

	else
	{
		process_block_ser_imm_pre (src_ptr);
		process_block_ser_lat (
			&dst_ptr [0],
			&src_ptr [_latency_serial],
			nbr_spl - _latency_serial,
			g0i, g1i, g2i, v0mi, v1mi, v2mi
		);
		process_block_ser_imm_post 
			(&dst_ptr [nbr_spl - _latency_serial],
			g0i, g1i, g2i, v0mi, v1mi, v2mi
		);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
Changes the states of the filters in order to accomodate them to serial/latency
processing.

Filter   B1        B2        B3        B4
Before   y1 [-1]   y2 [-1]   y3 [-1]   y4 [-1]
After    y1 [ 2]   y2 [ 1]   y3 [ 0]   y4 [-1]

Where yi [k] is the memory of the i-th filter containing a sample
correspounding to final index k within the processing block.

This function must be called before serial/latency-processing the block,
starting at the 4th sample. src_ptr points on the first sample.
*/

template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_imm_pre (const float src_ptr [])
{
	assert (src_ptr != nullptr);

	float				x_s;

	x_s = process_sample_single_stage (_data, src_ptr [0], 0, 1);
	x_s = process_sample_single_stage (_data, x_s        , 1, 2);
	      process_sample_single_stage (_data, x_s        , 2, 3);

	x_s = process_sample_single_stage (_data, src_ptr [1], 0, 1);
	      process_sample_single_stage (_data, x_s        , 1, 2);

	      process_sample_single_stage (_data, src_ptr [2], 0, 1);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_imm_pre (const float src_ptr [])
{
	assert (src_ptr != 0);

	process_sample_single_stage (_data, src_ptr [0], 0, 0);
	process_sample_single_stage (_data, src_ptr [1], 1, 1);
}



/*
Flushes data and adapt the state of the filter from serial/latency processing
to direct processing.

Filter   B1        B2        B3        B4
Before   y1 [N-4]  y2 [N-3]  y3 [N-2]  y4 [N-1]
After    y1 [N-1]  y2 [N-1]  y3 [N-1]  y4 [N-1]

Where N is the block length.

This function must be called after having serial/latency-processed the last
sample of the block. out_ptr points on sample N-3.
*/

template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_imm_post (float dst_ptr [])
{
	assert (dst_ptr != nullptr);


	float          x_s;

	x_s         = _data._y [3];
	dst_ptr [0] = process_sample_single_stage (_data, x_s, 3, 0);

	x_s         = _data._y [2];
	x_s         = process_sample_single_stage (_data, x_s, 2, 3);
	dst_ptr [1] = process_sample_single_stage (_data, x_s, 3, 0);

	x_s         = _data._y [1];
	x_s         = process_sample_single_stage (_data, x_s, 1, 2);
	x_s         = process_sample_single_stage (_data, x_s, 2, 3);
	dst_ptr [2] = process_sample_single_stage (_data, x_s, 3, 0);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_ser_imm_post (float dst_ptr [], const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (dst_ptr != 0);

	float          x_s;

	x_s         = _data._y [3];
	dst_ptr [0] = process_sample_single_stage (_data, x_s, 3, 0);
	increment (_data, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	x_s         = _data._y [2];
	x_s         = process_sample_single_stage (_data, x_s, 2, 3);
	dst_ptr [1] = process_sample_single_stage (_data, x_s, 3, 0);
	increment (_data, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	x_s         = _data._y [1];
	x_s         = process_sample_single_stage (_data, x_s, 1, 2);
	x_s         = process_sample_single_stage (_data, x_s, 2, 3);
	dst_ptr [2] = process_sample_single_stage (_data, x_s, 3, 0);
	increment (_data, g0i, g1i, g2i, v0mi, v1mi, v2mi);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_imm_post (float dst_ptr [])
{
	assert (dst_ptr != 0);

	float          x_s;

	x_s = _data._y [0];
	dst_ptr [0] = process_sample_single_stage (_data, x_s, 2, 2);
	x_s = _data._y [1];
	dst_ptr [1] = process_sample_single_stage (_data, x_s, 3, 3);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::process_block_2x2_imm_post (float dst_ptr [], const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	assert (dst_ptr != 0);

	process_block_2x2_imm_post (dst_ptr);
	increment (_data, g0i, g1i, g2i, v0mi, v1mi, v2mi);
}



template <class VD, class VS, class VP, class MX>
float	SvfCore4Simd <VD, VS, VP, MX>::process_sample_single_stage (SvfCore4SimdData &data, float x_s, int stage, int stage_y)
{
	assert (stage >= 0);
	assert (stage < SvfCore4SimdData::_nbr_units);
	assert (stage_y >= 0);
	assert (stage_y < SvfCore4SimdData::_nbr_units);

	const float    t0 = x_s - data._ic2eq [stage];
	const float    t1 = data._g0 [stage] * t0 + data._g1 [stage] * data._ic1eq [stage];
	const float    t2 = data._g2 [stage] * t0 + data._g0 [stage] * data._ic1eq [stage];
	const float    v1 = t1 + data._ic1eq [stage];
	const float    v2 = t2 + data._ic2eq [stage];
	data._ic1eq [stage] += 2 * t1;
	data._ic2eq [stage] += 2 * t2;

	data._y [stage_y] = Mixer::mix (
		x_s, v1, v2,
		data._v0m [stage], data._v1m [stage], data._v2m [stage]
	);

	return data._y [stage_y];
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::iterate_parallel (const fstb::ToolsSimd::VectF32 &v0, fstb::ToolsSimd::VectF32 &v1, fstb::ToolsSimd::VectF32 &v2, fstb::ToolsSimd::VectF32 &ic1eq, fstb::ToolsSimd::VectF32 &ic2eq, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2)
{
	const auto     t0 = v0 - ic2eq;
	const auto     t1 = g0 * t0 + g1 * ic1eq;
	const auto     t2 = g2 * t0 + g0 * ic1eq;
	v1     = t1 + ic1eq;
	v2     = t2 + ic2eq;
	ic1eq += t1 + t1;
	ic2eq += t2 + t2;
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::increment (SvfCore4SimdData &data, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	auto           g0    = V128Par::load_f32 (data._g0 );
	auto           g1    = V128Par::load_f32 (data._g1 );
	auto           g2    = V128Par::load_f32 (data._g2 );
	auto           v0m   = V128Par::load_f32 (data._v0m);
	auto           v1m   = V128Par::load_f32 (data._v1m);
	auto           v2m   = V128Par::load_f32 (data._v2m);

	increment (g0, g1, g2, v0m, v1m, v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	V128Par::store_f32 (data._g0 , g0 );
	V128Par::store_f32 (data._g1 , g1 );
	V128Par::store_f32 (data._g2 , g2 );
	V128Par::store_f32 (data._v0m, v0m);
	V128Par::store_f32 (data._v1m, v1m);
	V128Par::store_f32 (data._v2m, v2m);
}



template <class VD, class VS, class VP, class MX>
void	SvfCore4Simd <VD, VS, VP, MX>::increment (fstb::ToolsSimd::VectF32 &g0, fstb::ToolsSimd::VectF32 &g1, fstb::ToolsSimd::VectF32 &g2, fstb::ToolsSimd::VectF32 &v0m, fstb::ToolsSimd::VectF32 &v1m, fstb::ToolsSimd::VectF32 &v2m, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi)
{
	g0 += g0i;
	g1 += g1i;
	g2 += g2i;
	Mixer::inc (v0m, v1m, v2m, v0mi, v1mi, v2mi);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SvfCore4Simd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
