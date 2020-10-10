/*****************************************************************************

        Biquad4SimdMorph.h
        Author: Laurent de Soras, 2016

Objects of this class should be aligned on 16-bytes boudaries.
This class can be inherited but is not polymorph.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Biquad4SimdMorph_HEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4SimdMorph_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/Biquad4Simd.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



template <class VD, class VS, class VP>
class Biquad4SimdMorph
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	VD	V128Dest;
	typedef	VS	V128Src;
	typedef	VP	V128Par;

	typedef	Biquad4Simd <V128Dest, V128Src, V128Par>	BiqSimd;
	typedef	Biquad4SimdData::VectFloat4	VectFlt4;

	               Biquad4SimdMorph () = default;
	               Biquad4SimdMorph (const Biquad4SimdMorph <VD, VS, VP> &other);
	               Biquad4SimdMorph (Biquad4SimdMorph <VD, VS, VP> &&other);

	               ~Biquad4SimdMorph () = default;

	Biquad4SimdMorph <VD, VS, VP> &
	               operator = (const Biquad4SimdMorph <VD, VS, VP> &other);
	Biquad4SimdMorph <VD, VS, VP> &
	               operator = (Biquad4SimdMorph <VD, VS, VP> &&other);

	void           copy_vectors (const Biquad4SimdMorph <VD, VS, VP> &other);

	void           set_ramp_time (int nbr_spl);
	inline int     get_ramp_time () const;

	void           set_z_eq (const VectFlt4 b [3], const VectFlt4 a [3], bool ramp_flag = false);
	void           set_z_eq_same (const float b [3], const float a [3], bool ramp_flag = false);
	void           set_z_eq_one (int biq, const float b [3], const float a [3], bool ramp_flag = false);
	void           neutralise (bool ramp_flag = false);
	void           neutralise_one (int biq, bool ramp_flag = false);

	void           get_z_eq (VectFlt4 b [3], VectFlt4 a [3]) const;
	void           get_z_eq_one (int biq, float b [3], float a [3]) const;
	void           get_z_eq_one_ramp (int biq, float b [3], float a [3]) const;
	void           get_z_eq_one_final (int biq, float b [3], float a [3]) const;

	void           set_state_one (int biq, float const mem_x [2], const float mem_y [2]);
	void           get_state_one (int biq, float mem_x [2], float mem_y [2]) const;

	inline bool    is_ramping () const;
	void           process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], int nbr_spl);
	void           process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const float in_ptr [], int nbr_spl);
	void           process_block_serial_latency (float out_ptr [], const float in_ptr [], int nbr_spl);
	void           process_block_serial_immediate (float out_ptr [], const float in_ptr [], int nbr_spl);
	void           process_block_2x2_latency (float out_ptr [], const float in_ptr [], int nbr_spl);
	void           process_block_2x2_immediate (float out_ptr [], const float in_ptr [], int nbr_spl);

	void           clear_buffers ();
	void           clear_buffers_one (int biq);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           handle_ramp_post (int nbr_spl);

	BiqSimd        _biq;

	alignas (16) VectFlt4
	               _step_b [3];   // Step for the current ramp
	alignas (16) VectFlt4
	               _step_a [3];
	alignas (16) VectFlt4
	               _target_b [3]; // Target Z-equation for the current ramp.
	alignas (16) VectFlt4
	               _target_a [3];

	alignas (16) VectFlt4
	               _prog_b [3];   // Target Z-equation for the subsequent ramp.
	alignas (16) VectFlt4
	               _prog_a [3];

	alignas (16) VectFlt4
	               _tmp_b [3];
	alignas (16) VectFlt4
	               _tmp_a [3];

	int            _nbr_rem_spl = 0;     // Number of remaining samples for the current ramp. 0 = no ramping.
	int            _ramp_len    = 64;    // > 0
	bool           _prog_flag   = false; // A ramp is programmed, will follow the current one.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Biquad4SimdMorph <VD, VS, VP> &other) const = delete;
	bool           operator != (const Biquad4SimdMorph <VD, VS, VP> &other) const = delete;

}; // class Biquad4SimdMorph



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Biquad4SimdMorph.hpp"



#endif   // mfx_dsp_iir_Biquad4SimdMorph_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
