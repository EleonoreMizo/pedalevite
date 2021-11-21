/*****************************************************************************

        Biquad4Simd.h
        Author: Laurent de Soras, 2016

Objects of this class should be aligned on 16-bytes boudaries.
This class can be inherited but is not polymorph.

Template parameters:

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr) noexcept;
	static fstb::Vf32 VD::load_f32 (const void *ptr) noexcept;
	static void VD::store_f32 (void *ptr, fstb::Vf32 val) noexcept;

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr) noexcept;
	static fstb::Vf32 VS::load_f32 (const void *ptr) noexcept;

- VP: same as VD, but for parametering and internal data.
	Requires: same as VD.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Biquad4Simd_HEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4Simd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"
#include "mfx/dsp/iir/Biquad4SimdData.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



template <class VD, class VS, class VP>
class Biquad4Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units      = Biquad4SimdData::_nbr_units;  // Number of processing units
	static const int  _latency_serial = _nbr_units;          // Latency for serial processing, samples
	static const int  _latency_2x2    = _nbr_units / 2 - 1;  // Latency for 2-way processing, samples

	typedef VD V128Dest;
	typedef VS V128Src;
	typedef VP V128Par;

	typedef Biquad4SimdData::VectFloat4 VectFloat4;

	               Biquad4Simd () noexcept;
	               Biquad4Simd (const Biquad4Simd <VD, VS, VP> &other) noexcept;
	               Biquad4Simd (Biquad4Simd <VD, VS, VP> &&other) noexcept;

	               ~Biquad4Simd () = default;

	Biquad4Simd <VD, VS, VP> &
	               operator = (const Biquad4Simd <VD, VS, VP> &other) noexcept;
	Biquad4Simd <VD, VS, VP> &
	               operator = (Biquad4Simd <VD, VS, VP> &&other) noexcept;

	void           neutralise () noexcept;
	void           neutralise_one (int biq) noexcept;
	void           set_z_eq (const VectFloat4 b [3], const VectFloat4 a [3]) noexcept;
	void           set_z_eq_same (const float b [3], const float a [3]) noexcept;
	void           set_z_eq_one (int biq, const float b [3], const float a [3]) noexcept;

	void           get_z_eq (VectFloat4 b [3], VectFloat4 a [3]) const noexcept;
	void           get_z_eq_one (int biq, float b [3], float a [3]) const noexcept;

	void           copy_z_eq (const Biquad4Simd <VD, VS, VP> &other) noexcept;

	void           set_state_one (int biq, float const mem_x [2], const float mem_y [2]) noexcept;
	void           get_state_one (int biq, float mem_x [2], float mem_y [2]) const noexcept;

	void           process_block_parallel (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl) noexcept;
	void           process_block_parallel (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;
	void           process_block_parallel (fstb::Vf32 out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_parallel (fstb::Vf32 out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_parallel (const fstb::Vf32 &x) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_parallel (const fstb::Vf32 &x, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;

	void           process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_latency (float x_s) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_latency (float x_s, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;

	void           process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_immediate (float x_s) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_immediate (float x_s, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;

	void           process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_latency (const fstb::Vf32 &x) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_latency (const fstb::Vf32 &x, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;

	void           process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_immediate (const fstb::Vf32 &x) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_immediate (const fstb::Vf32 &x, const fstb::Vf32 b_inc [3], const fstb::Vf32 a_inc [3]) noexcept;

	void           clear_buffers () noexcept;
	void           clear_buffers_one (int biq) noexcept;

	fstb_FORCEINLINE static bool
	               check_stability (float a1, float a2) noexcept;
	fstb_FORCEINLINE static bool
	               check_stability (fstb::Vf32 a1, fstb::Vf32 a2) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE float
						process_sample_single_stage_noswap (int stage, float x_s, int alt_pos) noexcept;

	void				process_block_serial_immediate_pre (const float in_ptr []) noexcept;
	void				process_block_2x2_immediate_pre (const float in_ptr []) noexcept;

	alignas (16) Biquad4SimdData
	               _data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Biquad4Simd &other) const = delete;
	bool           operator != (const Biquad4Simd &other) const = delete;

}; // class Biquad4Simd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Biquad4Simd.hpp"



#endif   // mfx_dsp_iir_Biquad4Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
