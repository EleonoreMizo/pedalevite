/*****************************************************************************

        Simd.h
        Author: Laurent de Soras, 2001

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

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_mix_Simd_HEADER_INCLUDED)
#define mfx_dsp_mix_Simd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{

class StereoLevel;

namespace mix
{



template <class VD, class VS>
class Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	VD		V128Dst;
	typedef	VS		V128Src;

	// Scale
	static void		scale_1_v (float data_ptr [], long nbr_spl, float vol);
	static void		scale_1_vlr (float data_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		scale_1_vlrauto (float data_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		scale_2_v (float data_1_ptr [], float data_2_ptr [], long nbr_spl, float vol);
	static void		scale_2_vlr (float data_1_ptr [], float data_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		scale_2_vlrauto (float data_1_ptr [], float data_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	// Copy
	static void		copy_1_1 (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_1_1_v (float out_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		copy_1_1_vlr (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_1_1_vlrauto (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		copy_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_2_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);
	static void		copy_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol);
	static void		copy_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);
	static void		copy_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol);
	static void		copy_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	// Spread copying
	static void		copy_spread_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol_l, float vol_r);
	static void		copy_spread_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);
	static void		copy_spread_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);

	// Cross-fade copying
	static void		copy_xfade_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float xf);
	static void		copy_xfade_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_xf, float e_xf);
	static void		copy_xfade_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_xf, float e_xf);

	static void    copy_xfade_3_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], long nbr_spl);

	// Matrix copying
	static void		copy_mat_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &vol);
	static void		copy_mat_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);
	static void		copy_mat_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);

	// Copy and interleave
	static void		copy_1_2i (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_1_2i_v (float out_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		copy_1_2i_vlr (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_1_2i_vlrauto (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_2_2i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);
	static void		copy_2_2i_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol);
	static void		copy_2_2i_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_2_2i_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_4_4i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], const float in_4_ptr [], long nbr_spl);
	static void		copy_2_4i2 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);

//	static void		copy_1_ni1 (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_out);

	// Copy and deinterleave
	static void		copy_2i_1 (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_2i_1_v (float out_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		copy_2i_1_vlr (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_2i_1_vlrauto (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_2i_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_2i_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		copy_2i_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		copy_2i_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		copy_4i_1 (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_4i_4 (float out_1_ptr [], float out_2_ptr [], float out_3_ptr [], float out_4_ptr [], const float in_ptr [], long nbr_spl);
	static void		copy_4i2_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl);

//	static void		copy_ni1_1 (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_in);

	// Copy and convert interleaving
//	static void		copy_nip_mip (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_out, int nbr_chn_in, int nbr_chn_copy);

	// Mixing
	static void		mix_1_1 (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		mix_1_1_v (float out_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		mix_1_1_vlr (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_1_1_vlrauto (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		mix_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl);
	static void		mix_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		mix_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		mix_2_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);
	static void		mix_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol);
	static void		mix_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		mix_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);
	static void		mix_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol);
	static void		mix_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	// Spread mixing
	static void		mix_spread_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol_l, float vol_r);
	static void		mix_spread_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);
	static void		mix_spread_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);

	// Matrix mixing
	static void		mix_mat_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &vol);
	static void		mix_mat_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);
	static void		mix_mat_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);

	// Mix and interleave
	static void		mix_1_2i (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		mix_1_2i_v (float out_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		mix_1_2i_vlr (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_1_2i_vlrauto (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		mix_2_2i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);
	static void		mix_2_2i_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol);
	static void		mix_2_2i_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_2_2i_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol);

	// Mix and deinterleave
	static void		mix_2i_1 (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		mix_2i_1_v (float out_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		mix_2i_1_vlr (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_2i_1_vlrauto (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

	static void		mix_2i_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl);
	static void		mix_2i_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol);
	static void		mix_2i_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);
	static void		mix_2i_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol);

//	static void		mix_ni1_1 (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_in);

	// Multiply
	static void		mult_1_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);

	// Multiply, in-place
	static void		mult_ip_1_1 (float out_ptr [], const float in_ptr [], long nbr_spl);
	static void		mult_ip_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl);
	static void		mult_ip_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl);

	// Misc
	static void		clear (float out_ptr [], long nbr_spl);
//	static void		clear_nim (float out_ptr [], long nbr_spl, int clear_len, int skip_len);

	static void		fill (float out_ptr [], long nbr_spl, float val);
	static void		fill_lr (float out_ptr [], long nbr_spl, float s_val, float e_val);
	static void		fill_lrauto (float out_ptr [], long nbr_spl, float s_val, float e_val);

	static void		add_cst_1_1 (float out_ptr [], long nbr_spl, float val);
	static void		add_cst_1_2 (float out_1_ptr [], float out_2_ptr [], long nbr_spl, float val);

	static void		linop_cst_1_1 (float out_ptr [], const float in_ptr [], long nbr_spl, float mul_val, float add_val);
	static void		linop_cst_ip_1 (float data_ptr [], long nbr_spl, float mul_val, float add_val);

	static void		add_sub_ip_2_2 (float out_1_ptr [], float out_2_ptr [], long nbr_spl);

	static void    sum_square_n_1 (float out_ptr [], const float * const src_ptr_arr [], long nbr_spl, int nbr_chn, float init_val);
	static void    sum_square_n_1_v (float out_ptr [], const float * const src_ptr_arr [], long nbr_spl, int nbr_chn, float init_val, float vol);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	virtual        ~Simd ()                              = delete;
	               Simd ()                               = delete;
	               Simd (const Simd &other)              = delete;
	Simd &         operator = (const Simd &other)        = delete;
	bool           operator == (const Simd &other) const = delete;
	bool           operator != (const Simd &other) const = delete;

}; // class Simd



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/mix/Simd.hpp"



#endif   // mfx_dsp_mix_Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
