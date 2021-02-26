/*****************************************************************************

        SvfCore.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SvfCore_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SvfCore_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class MX>
void	SvfCore <MX>::neutralise ()
{
	_g0  = 0;
	_g1  = 0;
	_g2  = 0;
	_v0m = 1;
	_v1m = 0;
	_v2m = 0;
}



template <class MX>
void	SvfCore <MX>::set_coefs (float g0, float g1, float g2)
{
	_g0 = g0;
	_g1 = g1;
	_g2 = g2;
}



template <class MX>
void	SvfCore <MX>::get_coefs (float &g0, float &g1, float &g2) const
{
	g0 = _g0;
	g1 = _g1;
	g2 = _g2;
}



template <class MX>
void	SvfCore <MX>::set_mix (float v0m, float v1m, float v2m)
{
	_v0m = v0m;
	_v1m = v1m;
	_v2m = v2m;
}



template <class MX>
void	SvfCore <MX>::get_mix (float &v0m, float &v1m, float &v2m) const
{
	v0m = _v0m;
	v1m = _v1m;
	v2m = _v2m;
}



template <class MX>
void	SvfCore <MX>::copy_z_eq (const SvfCore <MX> &other)
{
	_g0  = other._g0;
	_g1  = other._g1;
	_g2  = other._g2;
	_v0m = other._v0m;
	_v1m = other._v1m;
	_v2m = other._v2m;
}



template <class MX>
void	SvfCore <MX>::clear_buffers ()
{
	_ic1eq = 0;
	_ic2eq = 0;
}



template <class MX>
float	SvfCore <MX>::process_sample (float x)
{
	return process_sample (x, _g0, _g1, _g2);
}



template <class MX>
float	SvfCore <MX>::process_sample (float x, float g0, float g1, float g2)
{
	return process_sample (x, g0, g1, g2, _v0m, _v1m, _v2m);
}



template <class MX>
float	SvfCore <MX>::process_sample (float x, float g0, float g1, float g2, float v0m, float v1m, float v2m)
{
	float          v1;
	float          v2;
	iterate (x, v1, v2, g0, g1, g2);

	return Mixer::mix (x, v1, v2, v0m, v1m, v2m);
}



template <class MX>
float	SvfCore <MX>::process_sample_inc (float x, float g0i, float g1i, float g2i, float v0mi, float v1mi, float v2mi)
{
	float          v1;
	float          v2;
	iterate (x, v1, v2, _g0, _g1, _g2);
	const float    y = Mixer::mix (x, v1, v2, _v0m, _v1m, _v2m);
	increment (_g0, _g1, _g2, _v0m, _v1m, _v2m, g0i, g1i, g2i, v0mi, v1mi, v2mi);

	return y;
}



// Multi-mode output. Returns v1 (band) as first and v2 (low) as second.
// Mixer is not taken into account
template <class MX>
std::array <float, 2>	SvfCore <MX>::process_sample_mm (float x)
{
	return process_sample_mm (x, _g0, _g1, _g2);
}



template <class MX>
std::array <float, 2>	SvfCore <MX>::process_sample_mm (float x, float g0, float g1, float g2)
{
	float          v1;
	float          v2;
	iterate (x, v1, v2, g0, g1, g2);

	return std::array <float, 2> {{ v1, v2 }};
}



template <class MX>
std::array <float, 2>	SvfCore <MX>::process_sample_mm_inc (float x, float g0i, float g1i, float g2i)
{
	const auto     v1v2 { process_sample_mm (x) };
	increment (_g0, _g1, _g2, g0i, g1i, g2i);

	return v1v2;
}



// Can work in-place
template <class MX>
void	SvfCore <MX>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



// Can work in-place
template <class MX>
void	SvfCore <MX>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float g0_ptr [], const float g1_ptr [], const float g2_ptr [])
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (g0_ptr != nullptr);
	assert (g1_ptr != nullptr);
	assert (g2_ptr != nullptr);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (
			src_ptr [pos],
			g0_ptr [pos], g1_ptr [pos], g2_ptr [pos]
		);
	}
}



// Can work in-place
template <class MX>
void	SvfCore <MX>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float g0_ptr [], const float g1_ptr [], const float g2_ptr [], const float v0m_ptr [], const float v1m_ptr [], const float v2m_ptr [])
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (g0_ptr  != nullptr);
	assert (g1_ptr  != nullptr);
	assert (g2_ptr  != nullptr);
	assert (v0m_ptr != nullptr);
	assert (v1m_ptr != nullptr);
	assert (v2m_ptr != nullptr);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (
			src_ptr [pos],
			g0_ptr [pos], g1_ptr [pos], g2_ptr [pos],
			v0m_ptr [pos], v1m_ptr [pos], v2m_ptr [pos]
		);
	}
}



// Can work in-place
template <class MX>
void	SvfCore <MX>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, float g0i, float g1i, float g2i, float v0mi, float v1mi, float v2mi)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
		increment (
			_g0, _g1, _g2, _v0m, _v1m, _v2m,
			g0i, g1i, g2i, v0mi, v1mi, v2mi
		);
	}
}



template <class MX>
void	SvfCore <MX>::process_block_mm (float v1_ptr [], float v2_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (v1_ptr  != nullptr);
	assert (v2_ptr  != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x = src_ptr [pos];
		float          v1;
		float          v2;
		iterate (x, v1, v2, _g0, _g1, _g2);
		v1_ptr [pos] = v1;
		v2_ptr [pos] = v2;
	}
}



template <class MX>
void	SvfCore <MX>::process_block_mm (float v1_ptr [], float v2_ptr [], const float src_ptr [], int nbr_spl, const float g0_ptr [], const float g1_ptr [], const float g2_ptr [])
{
	assert (v1_ptr  != nullptr);
	assert (v2_ptr  != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (g0_ptr  != nullptr);
	assert (g1_ptr  != nullptr);
	assert (g2_ptr  != nullptr);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x  = src_ptr [pos];
		const float    g0 = g0_ptr [pos];
		const float    g1 = g1_ptr [pos];
		const float    g2 = g2_ptr [pos];
		float          v1;
		float          v2;
		iterate (x, v1, v2, g0, g1, g2);
		v1_ptr [pos] = v1;
		v2_ptr [pos] = v2;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class MX>
void	SvfCore <MX>::iterate (float v0, float &v1, float &v2, float g0, float g1, float g2)
{
	const float    t0 = v0 - _ic2eq;
	const float    t1 = g0 * t0 + g1 * _ic1eq;
	const float    t2 = g2 * t0 + g0 * _ic1eq;
	v1      = t1 + _ic1eq;
	v2      = t2 + _ic2eq;
	_ic1eq += t1 + t1;
	_ic2eq += t2 + t2;
}



template <class MX>
void	SvfCore <MX>::increment (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, float g0i, float g1i, float g2i, float v0mi, float v1mi, float v2mi)
{
	increment (g0, g1, g2, g0i, g1i, g2i);
	Mixer::inc (v0m, v1m, v2m, v0mi, v1mi, v2mi);
}



template <class MX>
void	SvfCore <MX>::increment (float &g0, float &g1, float &g2, float g0i, float g1i, float g2i)
{
	g0 += g0i;
	g1 += g1i;
	g2 += g2i;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SvfCore_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
