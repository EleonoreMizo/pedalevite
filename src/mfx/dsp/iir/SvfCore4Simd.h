/*****************************************************************************

        SvfCore4Simd.h
        Author: Laurent de Soras, 2016

Objects of this class should be aligned on 16-bytes boudaries if VP requires
alignment.
This class can be inherited but is not polymorph.

Template parameters:

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectF32 VD::load_f32 (const void *ptr);
	static void VD::store_f32 (void *ptr, const fstb::ToolsSimd::VectF32 val);

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectF32 VS::load_f32 (const void *ptr);

- VP: same as VD, but for parametering and internal data.
	Requires: same as VD.

- MX: output mixer, creating the final output from the input (v0) and the raw
	outputs (v1 and v2).
	Requires:
	static fstb::ToolsSimd::VectF32 MX::mix (const fstb::ToolsSimd::VectF32 &v0,
		const fstb::ToolsSimd::VectF32 &v1, const fstb::ToolsSimd::VectF32 &v2,
		const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m,
		const fstb::ToolsSimd::VectF32 &v2m);
	static float MX::mix (float v0, float v1, float v2,
		float v0m, float v1m, float v2m);
	static void MX::inc (fstb::ToolsSimd::VectF32 &v0m, fstb::ToolsSimd::VectF32 &v1m,
		fstb::ToolsSimd::VectF32 &v2m, const fstb::ToolsSimd::VectF32 &v0mi,
		const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

Implements:

v0     = input
t0     = v0 - ic2eq
t1     = g0 * t0 + g1 * ic1eq
t2     = g2 * t0 + g0 * ic1eq
v1     = t1 + ic1eq
v2     = t2 + ic2eq
ic1eq  = 2 * t1 + ic1eq
ic2eq  = 2 * t2 + ic2eq
output = v0 * v0m + v1 * v1m + v2 * v2m

Specifically, for the output:

low   =                v2
band  =           v1
high  = v0 -  k * v1 - v2
notch = v0 -  k * v1      = low + high
bell  = v0 + gi * v1      = input + gi * band

Coefficients are not calculated in this class and should follow:

k   = 1 / Q
w   = PI * f0 / fs
s1  = sin (    w)
s2  = sin (2 * w)
nrm = 1 / (2 + k * s2)
g0  =                     s2  * nrm
g1  = (-2 * s1 * s1 - k * s2) * nrm
g2  =   2 * s1 * s1           * nrm

Reference:
Andrew Simper,
Linear Trapezoidal State Variable Filter (SVF) in state increment form: state += val,
2014-06-07, updated 2014-07-03,
http://www.cytomic.com/technical-papers

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SvfCore4Simd_HEADER_INCLUDED)
#define mfx_dsp_iir_SvfCore4Simd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/SvfCore4SimdData.h"
#include "mfx/dsp/iir/SvfMixerDefault.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



template <class VD, class VS, class VP, class MX = SvfMixerDefault>
class SvfCore4Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units      = SvfCore4SimdData::_nbr_units; // Number of processing units
	static const int  _latency_2x2    = _nbr_units / 2 - 1;  // Latency for 2-way processing, samples
	static const int  _latency_serial = _nbr_units     - 1;  // Latency for serial processing, samples

	typedef VD V128Dst;
	typedef VS V128Src;
	typedef VP V128Par;
	typedef MX Mixer;

	typedef SvfCore4SimdData::VectFloat4 VectFloat4;

	               SvfCore4Simd ();
	               SvfCore4Simd (const SvfCore4Simd <VD, VS, VP, MX> &other);
	               ~SvfCore4Simd () = default;

	SvfCore4Simd <VD, VS, VP, MX> &
	               operator = (const SvfCore4Simd <VD, VS, VP, MX> &other);

	void           neutralise ();
	void           neutralise_one (int unit);
	void           set_coefs (const VectFloat4 g0, const VectFloat4 g1, const VectFloat4 g2);
	void           set_coefs_one (int unit, float g0, float g1, float g2);
	void           get_coefs_one (int unit, float &g0, float &g1, float &g2) const;
	void           set_mix (const VectFloat4 v0m, const VectFloat4 v1m, const VectFloat4 v2m);
	void           set_mix_one (int unit, float v0m, float v1m, float v2m);
	void           get_mix_one (int unit, float &v0m, float &v1m, float &v2m) const;

	void           clear_buffers ();
	void           clear_buffers_one (int unit);

	// Parallel
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_par (const fstb::ToolsSimd::VectF32 &x);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_par (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_par (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2, const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m, const fstb::ToolsSimd::VectF32 &v2m);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_par_inc (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	void           process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], long nbr_spl);
	void           process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr []);
	void           process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [], const fstb::ToolsSimd::VectF32 v0m_ptr [], const fstb::ToolsSimd::VectF32 v1m_ptr [], const fstb::ToolsSimd::VectF32 v2m_ptr []);
	void           process_block_par (fstb::ToolsSimd::VectF32 dst_ptr [], const fstb::ToolsSimd::VectF32 src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	// 2x2 with latency
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_2x2_lat (const fstb::ToolsSimd::VectF32 &x);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_2x2_lat (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_2x2_lat (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2, const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m, const fstb::ToolsSimd::VectF32 &v2m);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_2x2_lat_inc (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	void           process_block_2x2_lat (float dst_ptr [], const float src_ptr [], long nbr_spl);
	void           process_block_2x2_lat (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr []);
	void           process_block_2x2_lat (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [], const fstb::ToolsSimd::VectF32 v0m_ptr [], const fstb::ToolsSimd::VectF32 v1m_ptr [], const fstb::ToolsSimd::VectF32 v2m_ptr []);
	void           process_block_2x2_lat (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	// 2x2, immediate
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_2x2_imm (const fstb::ToolsSimd::VectF32 &x);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               process_sample_2x2_imm (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	void           process_block_2x2_imm (float dst_ptr [], const float src_ptr [], long nbr_spl);
	void           process_block_2x2_imm (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	// Serial with latency
	fstb_FORCEINLINE float
	               process_sample_ser_lat (float x_s);
	fstb_FORCEINLINE float
	               process_sample_ser_lat (float x_s, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2);
	fstb_FORCEINLINE float
	               process_sample_ser_lat (float x_s, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2, const fstb::ToolsSimd::VectF32 &v0m, const fstb::ToolsSimd::VectF32 &v1m, const fstb::ToolsSimd::VectF32 &v2m);
	fstb_FORCEINLINE float
	               process_sample_ser_lat_inc (float x_s, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	void           process_block_ser_lat (float dst_ptr [], const float src_ptr [], long nbr_spl);
	void           process_block_ser_lat (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr []);
	void           process_block_ser_lat (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 g0_ptr [], const fstb::ToolsSimd::VectF32 g1_ptr [], const fstb::ToolsSimd::VectF32 g2_ptr [], const fstb::ToolsSimd::VectF32 v0m_ptr [], const fstb::ToolsSimd::VectF32 v1m_ptr [], const fstb::ToolsSimd::VectF32 v2m_ptr []);
	void           process_block_ser_lat (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	// Serial, immediate
	fstb_FORCEINLINE float
	               process_sample_ser_imm (float x_s);
	fstb_FORCEINLINE float
	               process_sample_ser_imm (float x_s, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	void           process_block_ser_imm (float dst_ptr [], const float src_ptr [], long nbr_spl);
	void           process_block_ser_imm (float dst_ptr [], const float src_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               process_block_ser_imm_pre (const float src_ptr []);
	fstb_FORCEINLINE void
	               process_block_2x2_imm_pre (const float src_ptr []);

	fstb_FORCEINLINE void
	               process_block_ser_imm_post (float dst_ptr []);
	fstb_FORCEINLINE void
	               process_block_ser_imm_post (float dst_ptr [], const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);
	fstb_FORCEINLINE void
	               process_block_2x2_imm_post (float dst_ptr []);
	fstb_FORCEINLINE void
	               process_block_2x2_imm_post (float dst_ptr [], const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	static fstb_FORCEINLINE float
	               process_sample_single_stage (SvfCore4SimdData &data, float x_s, int stage, int stage_y);
	static fstb_FORCEINLINE void
	               iterate_parallel (const fstb::ToolsSimd::VectF32 &v0, fstb::ToolsSimd::VectF32 &v1, fstb::ToolsSimd::VectF32 &v2, fstb::ToolsSimd::VectF32 &ic1eq, fstb::ToolsSimd::VectF32 &ic2eq, const fstb::ToolsSimd::VectF32 &g0, const fstb::ToolsSimd::VectF32 &g1, const fstb::ToolsSimd::VectF32 &g2);
	static fstb_FORCEINLINE void
	               increment (SvfCore4SimdData &data, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);
	static fstb_FORCEINLINE void
	               increment (fstb::ToolsSimd::VectF32 &g0, fstb::ToolsSimd::VectF32 &g1, fstb::ToolsSimd::VectF32 &g2, fstb::ToolsSimd::VectF32 &v0m, fstb::ToolsSimd::VectF32 &v1m, fstb::ToolsSimd::VectF32 &v2m, const fstb::ToolsSimd::VectF32 &g0i, const fstb::ToolsSimd::VectF32 &g1i, const fstb::ToolsSimd::VectF32 &g2i, const fstb::ToolsSimd::VectF32 &v0mi, const fstb::ToolsSimd::VectF32 &v1mi, const fstb::ToolsSimd::VectF32 &v2mi);

	static fstb_FORCEINLINE void
	               save_sample_pair (float out_ptr [2], const fstb::ToolsSimd::VectF32 &x);

	SvfCore4SimdData
	               _data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SvfCore4Simd <VD, VS, VP, MX> &other) const = delete;
	bool           operator != (const SvfCore4Simd <VD, VS, VP, MX> &other) const = delete;

}; // class SvfCore4Simd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SvfCore4Simd.hpp"



#endif   // mfx_dsp_iir_SvfCore4Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
