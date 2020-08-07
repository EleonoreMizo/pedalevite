/*****************************************************************************

        Align.h
        Author: Laurent de Soras, 2003

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_mix_Align_HEADER_INCLUDED)
#define mfx_dsp_mix_Align_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if ! defined (fstb_HAS_SIMD)
#include "mfx/dsp/mix/Fpu.h"
#endif



namespace mfx
{
namespace dsp
{

class StereoLevel;

namespace mix
{



#if ! defined (fstb_HAS_SIMD)

using Align = Fpu;

#else // fstb_HAS_SIMD

class Align
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum State
	{
		State_UNBORN = 0,
		State_READY,

		State_NBR_ELT
	};

	static bool    is_ready ();
	static void    setup ();

	// Scale
	static void    (*scale_1_v) (float data_ptr [], int nbr_spl, float vol);
	static void    (*scale_1_vlr) (float data_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*scale_1_vlrauto) (float data_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*scale_2_v) (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float vol);
	static void    (*scale_2_vlr) (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*scale_2_vlrauto) (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	// Copy
	static void    (*copy_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_1_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*copy_1_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_1_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*copy_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_2_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);
	static void    (*copy_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol);
	static void    (*copy_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);
	static void    (*copy_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol);
	static void    (*copy_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	// Spread copying
	static void    (*copy_spread_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r);
	static void    (*copy_spread_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);
	static void    (*copy_spread_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);

	// Cross-fade copying
	static void    (*copy_xfade_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float xf);
	static void    (*copy_xfade_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf);
	static void    (*copy_xfade_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf);

	static void    (*copy_xfade_3_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], int nbr_spl);

	// Matrix copying
	static void    (*copy_mat_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol);
	static void    (*copy_mat_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);
	static void    (*copy_mat_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);

	// Copy and interleave
	static void    (*copy_1_2i) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_1_2i_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*copy_1_2i_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_1_2i_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_2_2i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);
	static void    (*copy_2_2i_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol);
	static void    (*copy_2_2i_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_2_2i_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_4_4i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], const float in_4_ptr [], int nbr_spl);
	static void    (*copy_2_4i2) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);

	static void    (*copy_1_ni1) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_out);

	// Copy and deinterleave
	static void    (*copy_2i_1) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_2i_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*copy_2i_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_2i_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_2i_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_2i_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*copy_2i_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*copy_2i_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*copy_4i_1) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_4i_4) (float out_1_ptr [], float out_2_ptr [], float out_3_ptr [], float out_4_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*copy_4i2_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl);

	static void    (*copy_ni1_1) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_in);

	// Copy and convert interleaving
	static void    (*copy_nip_mip) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_out, int nbr_chn_in, int nbr_chn_copy);

	// Mixing
	static void    (*mix_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mix_1_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*mix_1_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_1_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*mix_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mix_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*mix_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*mix_2_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);
	static void    (*mix_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol);
	static void    (*mix_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*mix_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);
	static void    (*mix_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol);
	static void    (*mix_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	// Spread mixing
	static void    (*mix_spread_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r);
	static void    (*mix_spread_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);
	static void    (*mix_spread_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r);

	// Matrix mixing
	static void    (*mix_mat_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol);
	static void    (*mix_mat_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);
	static void    (*mix_mat_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol);

	// Mix and interleave
	static void    (*mix_1_2i) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mix_1_2i_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*mix_1_2i_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_1_2i_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*mix_2_2i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);
	static void    (*mix_2_2i_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol);
	static void    (*mix_2_2i_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_2_2i_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol);

	// Mix and deinterleave
	static void    (*mix_2i_1) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mix_2i_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*mix_2i_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_2i_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*mix_2i_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mix_2i_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol);
	static void    (*mix_2i_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);
	static void    (*mix_2i_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol);

	static void    (*mix_ni1_1) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_in);

	// Multiply
	static void    (*mult_1_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);

	// Multiply, in-place
	static void    (*mult_ip_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mult_ip_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl);
	static void    (*mult_ip_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl);

	// Misc
	static void    (*clear) (float out_ptr [], int nbr_spl);
	static void    (*clear_nim) (float out_ptr [], int nbr_spl, int clear_len, int skip_len);

	static void    (*fill) (float out_ptr [], int nbr_spl, float val);
	static void    (*fill_lr) (float out_ptr [], int nbr_spl, float s_val, float e_val);
	static void    (*fill_lrauto) (float out_ptr [], int nbr_spl, float s_val, float e_val);

	static void    (*add_cst_1_1) (float out_ptr [], int nbr_spl, float val);
	static void    (*add_cst_1_2) (float out_1_ptr [], float out_2_ptr [], int nbr_spl, float val);

	static void    (*linop_cst_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl, float mul_val, float add_val);
	static void    (*linop_cst_ip_1) (float data_ptr [], int nbr_spl, float mul_val, float add_val);

	static void    (*add_sub_ip_2_2) (float out_1_ptr [], float out_2_ptr [], int nbr_spl);

	static void    (*sum_square_n_1) (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val);
	static void    (*sum_square_n_1_v) (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val, float vol);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Align ();
	               ~Align () = default;

	void           setup_internal ();

	static void    setup_unaligned ();
#if fstb_IS (ARCHI, X86) || fstb_IS (ARCHI, ARM)
	static void    setup_simd ();
#endif

	static Align & use_instance ();

	State          _state;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Align (const Align &other)             = delete;
	               Align (Align &&other)                  = delete;
	Align &        operator = (const Align &other)        = delete;
	Align &        operator = (Align &&other)             = delete;
	bool           operator == (const Align &other) const = delete;
	bool           operator != (const Align &other) const = delete;

}; // class Align

#endif // fstb_HAS_SIMD



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/mix/Align.hpp"



#endif   // mfx_dsp_mix_Align_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
