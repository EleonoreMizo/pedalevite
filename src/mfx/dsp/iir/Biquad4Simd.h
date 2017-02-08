/*****************************************************************************

        Biquad4Simd.h
        Author: Laurent de Soras, 2016

Objects of this class should be aligned on 16-bytes boudaries.
This class can be inherited but is not polymorph.

Template parameters:

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectF32 VD::load_f32 (const void *ptr);
	static void VD::store_f32 (void *ptr, fstb::ToolsSimd::VectF32 val);

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectF32 VS::load_f32 (const void *ptr);

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
#include "fstb/ToolsSimd.h"
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

	               Biquad4Simd ();
	               Biquad4Simd (const Biquad4Simd <VD, VS, VP> &other);
	               ~Biquad4Simd () = default;

	Biquad4Simd <VD, VS, VP> &
	               operator = (const Biquad4Simd <VD, VS, VP> &other);

	void           neutralise ();
	void           neutralise_one (int biq);
	void           set_z_eq (const VectFloat4 b [3], const VectFloat4 a [3]);
	void           set_z_eq_same (const float b [3], const float a [3]);
	void           set_z_eq_one (int biq, const float b [3], const float a [3]);

	void           get_z_eq (VectFloat4 b [3], VectFloat4 a [3]) const;
	void           get_z_eq_one (int biq, float b [3], float a [3]) const;

	void           set_state_one (int biq, float const mem_x [2], const float mem_y [2]);
	void           get_state_one (int biq, float mem_x [2], float mem_y [2]) const;

	void           process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl);
	void           process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	void           process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const float in_ptr [], long nbr_spl);
	void           process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_parallel (const fstb::ToolsSimd::VectF32 &x);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_parallel (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	void				process_block_serial_latency (float out_ptr [], const float in_ptr [], long nbr_spl);
	void				process_block_serial_latency (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	fstb_FORCEINLINE float
						process_sample_serial_latency (float x_s);
	fstb_FORCEINLINE float
						process_sample_serial_latency (float x_s, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	void				process_block_serial_immediate (float out_ptr [], const float in_ptr [], long nbr_spl);
	void				process_block_serial_immediate (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	fstb_FORCEINLINE float
						process_sample_serial_immediate (float x_s);
	fstb_FORCEINLINE float
						process_sample_serial_immediate (float x_s, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	void				process_block_2x2_latency (float out_ptr [], const float in_ptr [], long nbr_spl);
	void				process_block_2x2_latency (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_2x2_latency (const fstb::ToolsSimd::VectF32 &x);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_2x2_latency (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	void				process_block_2x2_immediate (float out_ptr [], const float in_ptr [], long nbr_spl);
	void				process_block_2x2_immediate (float out_ptr [], const float in_ptr [], long nbr_spl, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_2x2_immediate (const fstb::ToolsSimd::VectF32 &x);
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
						process_sample_2x2_immediate (const fstb::ToolsSimd::VectF32 &x, const fstb::ToolsSimd::VectF32 b_inc [3], const fstb::ToolsSimd::VectF32 a_inc [3]);

	void				clear_buffers ();
	void				clear_buffers_one (int biq);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE float
						process_sample_single_stage_noswap (int stage, float x_s, int alt_pos);

	void				process_block_serial_immediate_pre (const float in_ptr []);
	void				process_block_2x2_immediate_pre (const float in_ptr []);

	Biquad4SimdData
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
