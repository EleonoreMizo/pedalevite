/*****************************************************************************

        OnePole4Simd.h
        Author: Laurent de Soras, 2021

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
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_OnePole4Simd_HEADER_INCLUDED)
#define mfx_dsp_iir_OnePole4Simd_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"
#include "mfx/dsp/iir/OnePole4SimdData.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



template <class VD, class VS, class VP>
class OnePole4Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units      = OnePole4SimdData::_nbr_units; // Number of processing units
	static const int  _latency_serial = _nbr_units;          // Latency for serial processing, samples
	static const int  _latency_2x2    = _nbr_units / 2 - 1;  // Latency for 2-way processing, samples

	typedef VD V128Dest;
	typedef VS V128Src;
	typedef VP V128Par;

	typedef OnePole4SimdData::VectFloat4 VectFloat4;

	               OnePole4Simd () noexcept;
	               OnePole4Simd (const OnePole4Simd <VD, VS, VP> &other) noexcept;
	               OnePole4Simd (OnePole4Simd <VD, VS, VP> &&other) noexcept;

	               ~OnePole4Simd () = default;

	OnePole4Simd <VD, VS, VP> &
	               operator = (const OnePole4Simd <VD, VS, VP> &other) noexcept;
	OnePole4Simd <VD, VS, VP> &
	               operator = (OnePole4Simd <VD, VS, VP> &&other) noexcept;

	void           neutralise () noexcept;
	void           neutralise_one (int idx) noexcept;
	void           set_z_eq (const VectFloat4 b [2], const VectFloat4 a [2]) noexcept;
	void           set_z_eq_same (const float b [2], const float a [2]) noexcept;
	void           set_z_eq_one (int idx, const float b [2], const float a [2]) noexcept;

	void           get_z_eq (VectFloat4 b [2], VectFloat4 a [2]) const noexcept;
	void           get_z_eq_one (int idx, float b [2], float a [2]) const noexcept;

	void           copy_z_eq (const OnePole4Simd <VD, VS, VP> &other) noexcept;

	void           set_state_one (int idx, float mem_x, float mem_y) noexcept;
	void           get_state_one (int idx, float &mem_x, float &mem_y) const noexcept;

	void           process_block_parallel (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl) noexcept;
	void           process_block_parallel (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	void           process_block_parallel (fstb::Vf32 out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_parallel (fstb::Vf32 out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_parallel (const fstb::Vf32 &x) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_parallel (const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	void           process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_latency (float x_s) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_latency (float x_s, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	void           process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_immediate (float x_s) noexcept;
	fstb_FORCEINLINE float
	               process_sample_serial_immediate (float x_s, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	void           process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_latency (const fstb::Vf32 &x) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_latency (const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	void           process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	void           process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_immediate (const fstb::Vf32 &x) noexcept;
	fstb_FORCEINLINE fstb::Vf32
	               process_sample_2x2_immediate (const fstb::Vf32 &x, const fstb::Vf32 b_inc [2], const fstb::Vf32 a_inc [2]) noexcept;

	void           clear_buffers () noexcept;
	void           clear_buffers_one (int idx) noexcept;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE float
						process_sample_single_stage_noswap (int stage, float x_s) noexcept;

	void				process_block_serial_immediate_pre (const float in_ptr []) noexcept;
	void				process_block_2x2_immediate_pre (const float in_ptr []) noexcept;

	alignas (fstb_SIMD128_ALIGN) OnePole4SimdData
	               _data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OnePole4Simd &other) const = delete;
	bool           operator != (const OnePole4Simd &other) const = delete;

}; // class OnePole4Simd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/OnePole4Simd.hpp"



#endif   // mfx_dsp_iir_OnePole4Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
