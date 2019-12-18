/*****************************************************************************

        Align.cpp
        Author: Laurent de Soras, 2003

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/mix/Simd.h"
#include "fstb/CpuId.h"
#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace mix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Align::is_ready ()
{
	return (use_instance ()._state == State_READY);
}



void	Align::setup ()
{
	use_instance ().setup_internal ();
}



// Scale
void	(*Align::scale_1_v) (float data_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::scale_1_vlr) (float data_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::scale_1_vlrauto) (float data_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::scale_2_v) (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::scale_2_vlr) (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::scale_2_vlrauto) (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

// Copy
void	(*Align::copy_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_1_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_1_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_1_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_2_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;
void	(*Align::copy_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;
void	(*Align::copy_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

// Spread copying
void	(*Align::copy_spread_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r) = 0;
void	(*Align::copy_spread_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;
void	(*Align::copy_spread_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;

// Cross-fade copying
void	(*Align::copy_xfade_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float xf) = 0;
void	(*Align::copy_xfade_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf) = 0;
void	(*Align::copy_xfade_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf) = 0;

void	(*Align::copy_xfade_3_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], int nbr_spl) = 0;

// Matrix copying
void	(*Align::copy_mat_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol) = 0;
void	(*Align::copy_mat_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;
void	(*Align::copy_mat_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;

// Copy and interleave
void	(*Align::copy_1_2i) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_1_2i_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_1_2i_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_1_2i_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_2_2i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;
void	(*Align::copy_2_2i_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_2_2i_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_2_2i_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_4_4i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], const float in_4_ptr [], int nbr_spl) = 0;
void	(*Align::copy_2_4i2) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;

void	(*Align::copy_1_ni1) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_out) = 0;

// Copy and deinterleave
void	(*Align::copy_2i_1) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_2i_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_2i_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_2i_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_2i_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_2i_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::copy_2i_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::copy_2i_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::copy_4i_1) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_4i_4) (float out_1_ptr [], float out_2_ptr [], float out_3_ptr [], float out_4_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::copy_4i2_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) = 0;

void	(*Align::copy_ni1_1) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_in) = 0;

// Copy and convert interleaving
void	(*Align::copy_nip_mip) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_out, int nbr_chn_in, int nbr_chn_copy) = 0;

// Mixing
void	(*Align::mix_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mix_1_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_1_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_1_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::mix_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mix_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::mix_2_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;
void	(*Align::mix_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::mix_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;
void	(*Align::mix_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

// Spread mixing
void	(*Align::mix_spread_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r) = 0;
void	(*Align::mix_spread_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;
void	(*Align::mix_spread_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;

// Matrix mixing
void	(*Align::mix_mat_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol) = 0;
void	(*Align::mix_mat_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;
void	(*Align::mix_mat_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;

// Mix and interleave
void	(*Align::mix_1_2i) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mix_1_2i_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_1_2i_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_1_2i_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::mix_2_2i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;
void	(*Align::mix_2_2i_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_2_2i_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_2_2i_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

// Mix and deinterleave
void	(*Align::mix_2i_1) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mix_2i_1_v) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_2i_1_vlr) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_2i_1_vlrauto) (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::mix_2i_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mix_2i_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) = 0;
void	(*Align::mix_2i_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;
void	(*Align::mix_2i_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) = 0;

void	(*Align::mix_ni1_1) (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_in) = 0;

// Multiply
void	(*Align::mult_1_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;

// Multiply, in-place
void	(*Align::mult_ip_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mult_ip_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) = 0;
void	(*Align::mult_ip_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) = 0;

// Misc
void	(*Align::clear) (float out_ptr [], int nbr_spl) = 0;
void	(*Align::clear_nim) (float out_ptr [], int nbr_spl, int clear_len, int skip_len) = 0;

void	(*Align::fill) (float out_ptr [], int nbr_spl, float val) = 0;
void	(*Align::fill_lr) (float out_ptr [], int nbr_spl, float s_val, float e_val) = 0;
void	(*Align::fill_lrauto) (float out_ptr [], int nbr_spl, float s_val, float e_val) = 0;

void	(*Align::add_cst_1_1) (float out_ptr [], int nbr_spl, float val) = 0;
void	(*Align::add_cst_1_2) (float out_1_ptr [], float out_2_ptr [], int nbr_spl, float val) = 0;

void	(*Align::linop_cst_1_1) (float out_ptr [], const float in_ptr [], int nbr_spl, float mul_val, float add_val) = 0;
void	(*Align::linop_cst_ip_1) (float data_ptr [], int nbr_spl, float mul_val, float add_val) = 0;

void	(*Align::add_sub_ip_2_2) (float out_1_ptr [], float out_2_ptr [], int nbr_spl) = 0;

void	(*Align::sum_square_n_1) (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val) = 0;
void	(*Align::sum_square_n_1_v) (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val, float vol) = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Align::Align ()
:	_state (Align::State_UNBORN)
{
	setup_internal ();
}



void	Align::setup_internal ()
{
	if (_state != State_READY)
	{
		setup_unaligned ();

		fstb::CpuId    cpuid;
#if fstb_IS (ARCHI, ARM)
		setup_simd ();
#elif fstb_IS (ARCHI, X86)
		if (cpuid._sse_flag)
		{
			setup_simd ();
		}
#endif
		
		_state = State_READY;
	}
}



void	Align::setup_unaligned ()
{
	Generic::setup ();

	// Scale
	scale_1_v               = Generic::scale_1_v;
	scale_1_vlr             = Generic::scale_1_vlr;
	scale_1_vlrauto         = Generic::scale_1_vlrauto;

	scale_2_v               = Generic::scale_2_v;
	scale_2_vlr             = Generic::scale_2_vlr;
	scale_2_vlrauto         = Generic::scale_2_vlrauto;

	// Copy
	copy_1_1                = Generic::copy_1_1;
	copy_1_1_v              = Generic::copy_1_1_v;
	copy_1_1_vlr            = Generic::copy_1_1_vlr;
	copy_1_1_vlrauto        = Generic::copy_1_1_vlrauto;

	copy_1_2                = Generic::copy_1_2;
	copy_1_2_v              = Generic::copy_1_2_v;
	copy_1_2_vlr            = Generic::copy_1_2_vlr;
	copy_1_2_vlrauto        = Generic::copy_1_2_vlrauto;

	copy_2_1                = Generic::copy_2_1;
	copy_2_1_v              = Generic::copy_2_1_v;
	copy_2_1_vlr            = Generic::copy_2_1_vlr;
	copy_2_1_vlrauto        = Generic::copy_2_1_vlrauto;

	copy_2_2                = Generic::copy_2_2;
	copy_2_2_v              = Generic::copy_2_2_v;
	copy_2_2_vlr            = Generic::copy_2_2_vlr;
	copy_2_2_vlrauto        = Generic::copy_2_2_vlrauto;

	// Spread copying
	copy_spread_1_2_v       = Generic::copy_spread_1_2_v;
	copy_spread_1_2_vlr     = Generic::copy_spread_1_2_vlr;
	copy_spread_1_2_vlrauto = Generic::copy_spread_1_2_vlrauto;

	// Cross-fade copying
	copy_xfade_2_1_v        = Generic::copy_xfade_2_1_v;
	copy_xfade_2_1_vlr      = Generic::copy_xfade_2_1_vlr;
	copy_xfade_2_1_vlrauto  = Generic::copy_xfade_2_1_vlrauto;

	copy_xfade_3_1          = Generic::copy_xfade_3_1;

	// Matrix copying
	copy_mat_2_2_v          = Generic::copy_mat_2_2_v;
	copy_mat_2_2_vlr        = Generic::copy_mat_2_2_vlr;
	copy_mat_2_2_vlrauto    = Generic::copy_mat_2_2_vlrauto;

	// Copy and interleave
	copy_1_2i               = Generic::copy_1_2i;
	copy_1_2i_v             = Generic::copy_1_2i_v;
	copy_1_2i_vlr           = Generic::copy_1_2i_vlr;
	copy_1_2i_vlrauto       = Generic::copy_1_2i_vlrauto;

	copy_2_2i               = Generic::copy_2_2i;
	copy_2_2i_v             = Generic::copy_2_2i_v;
	copy_2_2i_vlr           = Generic::copy_2_2i_vlr;
	copy_2_2i_vlrauto       = Generic::copy_2_2i_vlrauto;

	copy_4_4i               = Generic::copy_4_4i;
	copy_2_4i2              = Generic::copy_2_4i2;

	copy_1_ni1              = Generic::copy_1_ni1;

	// Copy and deinterleave
	copy_2i_1               = Generic::copy_2i_1;
	copy_2i_1_v             = Generic::copy_2i_1_v;
	copy_2i_1_vlr           = Generic::copy_2i_1_vlr;
	copy_2i_1_vlrauto       = Generic::copy_2i_1_vlrauto;

	copy_2i_2               = Generic::copy_2i_2;
	copy_2i_2_v             = Generic::copy_2i_2_v;
	copy_2i_2_vlr           = Generic::copy_2i_2_vlr;
	copy_2i_2_vlrauto       = Generic::copy_2i_2_vlrauto;

	copy_4i_1               = Generic::copy_4i_1;
	copy_4i_4               = Generic::copy_4i_4;
	copy_4i2_2              = Generic::copy_4i2_2;

	copy_ni1_1              = Generic::copy_ni1_1;

	// Copy and convert interleaving
	copy_nip_mip            = Generic::copy_nip_mip;

	// Mixing
	mix_1_1                 = Generic::mix_1_1;
	mix_1_1_v               = Generic::mix_1_1_v;
	mix_1_1_vlr             = Generic::mix_1_1_vlr;
	mix_1_1_vlrauto         = Generic::mix_1_1_vlrauto;

	mix_1_2                 = Generic::mix_1_2;
	mix_1_2_v               = Generic::mix_1_2_v;
	mix_1_2_vlr             = Generic::mix_1_2_vlr;
	mix_1_2_vlrauto         = Generic::mix_1_2_vlrauto;

	mix_2_1                 = Generic::mix_2_1;
	mix_2_1_v               = Generic::mix_2_1_v;
	mix_2_1_vlr             = Generic::mix_2_1_vlr;
	mix_2_1_vlrauto         = Generic::mix_2_1_vlrauto;

	mix_2_2                 = Generic::mix_2_2;
	mix_2_2_v               = Generic::mix_2_2_v;
	mix_2_2_vlr             = Generic::mix_2_2_vlr;
	mix_2_2_vlrauto         = Generic::mix_2_2_vlrauto;

	// Spread mixing
	mix_spread_1_2_v        = Generic::mix_spread_1_2_v;
	mix_spread_1_2_vlr      = Generic::mix_spread_1_2_vlr;
	mix_spread_1_2_vlrauto  = Generic::mix_spread_1_2_vlrauto;

	// Matrix mixing
	mix_mat_2_2_v           = Generic::mix_mat_2_2_v;
	mix_mat_2_2_vlr         = Generic::mix_mat_2_2_vlr;
	mix_mat_2_2_vlrauto     = Generic::mix_mat_2_2_vlrauto;

	// Mix and interleave
	mix_1_2i                = Generic::mix_1_2i;
	mix_1_2i_v              = Generic::mix_1_2i_v;
	mix_1_2i_vlr            = Generic::mix_1_2i_vlr;
	mix_1_2i_vlrauto        = Generic::mix_1_2i_vlrauto;

	mix_2_2i                = Generic::mix_2_2i;
	mix_2_2i_v              = Generic::mix_2_2i_v;
	mix_2_2i_vlr            = Generic::mix_2_2i_vlr;
	mix_2_2i_vlrauto        = Generic::mix_2_2i_vlrauto;

	// Mix and deinterleave
	mix_2i_1                = Generic::mix_2i_1;
	mix_2i_1_v              = Generic::mix_2i_1_v;
	mix_2i_1_vlr            = Generic::mix_2i_1_vlr;
	mix_2i_1_vlrauto        = Generic::mix_2i_1_vlrauto;

	mix_2i_2                = Generic::mix_2i_2;
	mix_2i_2_v              = Generic::mix_2i_2_v;
	mix_2i_2_vlr            = Generic::mix_2i_2_vlr;
	mix_2i_2_vlrauto        = Generic::mix_2i_2_vlrauto;

	mix_ni1_1               = Generic::mix_ni1_1;

	// Multiply
	mult_1_1                = Generic::mult_1_1;

	// Multiply, in-place
	mult_ip_1_1             = Generic::mult_ip_1_1;
	mult_ip_1_2             = Generic::mult_ip_1_2;
	mult_ip_2_2             = Generic::mult_ip_2_2;

	// Misc
	clear                   = Generic::clear;
	clear_nim               = Generic::clear_nim;

	fill                    = Generic::fill;
	fill_lr                 = Generic::fill_lr;
	fill_lrauto             = Generic::fill_lrauto;

	add_cst_1_1             = Generic::add_cst_1_1;
	add_cst_1_2             = Generic::add_cst_1_2;

	linop_cst_1_1           = Generic::linop_cst_1_1;
	linop_cst_ip_1          = Generic::linop_cst_ip_1;

	add_sub_ip_2_2          = Generic::add_sub_ip_2_2;

	sum_square_n_1          = Generic::sum_square_n_1;
	sum_square_n_1_v        = Generic::sum_square_n_1_v;
}



#if fstb_IS (ARCHI, X86) || fstb_IS (ARCHI, ARM)

void	Align::setup_simd ()
{
	typedef	Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	>	SimdA;

	// Scale
	scale_1_v               = SimdA::scale_1_v;
	scale_1_vlr             = SimdA::scale_1_vlr;
	scale_1_vlrauto         = SimdA::scale_1_vlrauto;

	scale_2_v               = SimdA::scale_2_v;
	scale_2_vlr             = SimdA::scale_2_vlr;
	scale_2_vlrauto         = SimdA::scale_2_vlrauto;

	// Copy
	copy_1_1                = SimdA::copy_1_1;
	copy_1_1_v              = SimdA::copy_1_1_v;
	copy_1_1_vlr            = SimdA::copy_1_1_vlr;
	copy_1_1_vlrauto        = SimdA::copy_1_1_vlrauto;

	copy_1_2                = SimdA::copy_1_2;
	copy_1_2_v              = SimdA::copy_1_2_v;
	copy_1_2_vlr            = SimdA::copy_1_2_vlr;
	copy_1_2_vlrauto        = SimdA::copy_1_2_vlrauto;

	copy_2_1                = SimdA::copy_2_1;
	copy_2_1_v              = SimdA::copy_2_1_v;
	copy_2_1_vlr            = SimdA::copy_2_1_vlr;
	copy_2_1_vlrauto        = SimdA::copy_2_1_vlrauto;

	copy_2_2                = SimdA::copy_2_2;
	copy_2_2_v              = SimdA::copy_2_2_v;
	copy_2_2_vlr            = SimdA::copy_2_2_vlr;
	copy_2_2_vlrauto        = SimdA::copy_2_2_vlrauto;

	// Spread copying
	copy_spread_1_2_v       = SimdA::copy_spread_1_2_v;
	copy_spread_1_2_vlr     = SimdA::copy_spread_1_2_vlr;
	copy_spread_1_2_vlrauto = SimdA::copy_spread_1_2_vlrauto;

	// Cross-fade copying
	copy_xfade_2_1_v        = SimdA::copy_xfade_2_1_v;
	copy_xfade_2_1_vlr      = SimdA::copy_xfade_2_1_vlr;
	copy_xfade_2_1_vlrauto  = SimdA::copy_xfade_2_1_vlrauto;

	copy_xfade_3_1          = SimdA::copy_xfade_3_1;

	// Matrix copying
	copy_mat_2_2_v          = SimdA::copy_mat_2_2_v;
	copy_mat_2_2_vlr        = SimdA::copy_mat_2_2_vlr;
	copy_mat_2_2_vlrauto    = SimdA::copy_mat_2_2_vlrauto;

	// Copy and interleave
	copy_1_2i               = SimdA::copy_1_2i;
	copy_1_2i_v             = SimdA::copy_1_2i_v;
	copy_1_2i_vlr           = SimdA::copy_1_2i_vlr;
	copy_1_2i_vlrauto       = SimdA::copy_1_2i_vlrauto;

	copy_2_2i               = SimdA::copy_2_2i;
	copy_2_2i_v             = SimdA::copy_2_2i_v;
	copy_2_2i_vlr           = SimdA::copy_2_2i_vlr;
	copy_2_2i_vlrauto       = SimdA::copy_2_2i_vlrauto;

	copy_4_4i               = SimdA::copy_4_4i;
	copy_2_4i2              = SimdA::copy_2_4i2;

//	copy_1_ni1              = SimdA::copy_1_ni1;

	// Copy and deinterleave
	copy_2i_1               = SimdA::copy_2i_1;
	copy_2i_1_v             = SimdA::copy_2i_1_v;
	copy_2i_1_vlr           = SimdA::copy_2i_1_vlr;
	copy_2i_1_vlrauto       = SimdA::copy_2i_1_vlrauto;

	copy_2i_2               = SimdA::copy_2i_2;
	copy_2i_2_v             = SimdA::copy_2i_2_v;
	copy_2i_2_vlr           = SimdA::copy_2i_2_vlr;
	copy_2i_2_vlrauto       = SimdA::copy_2i_2_vlrauto;

	copy_4i_1               = SimdA::copy_4i_1;
	copy_4i_4               = SimdA::copy_4i_4;
	copy_4i2_2              = SimdA::copy_4i2_2;

//	copy_ni1_1              = SimdA::copy_ni1_1;

	// Copy and convert interleaving
//	copy_nip_mip            = SimdA::copy_nip_mip;

	// Mixing
	mix_1_1                 = SimdA::mix_1_1;
	mix_1_1_v               = SimdA::mix_1_1_v;
	mix_1_1_vlr             = SimdA::mix_1_1_vlr;
	mix_1_1_vlrauto         = SimdA::mix_1_1_vlrauto;

	mix_1_2                 = SimdA::mix_1_2;
	mix_1_2_v               = SimdA::mix_1_2_v;
	mix_1_2_vlr             = SimdA::mix_1_2_vlr;
	mix_1_2_vlrauto         = SimdA::mix_1_2_vlrauto;

	mix_2_1                 = SimdA::mix_2_1;
	mix_2_1_v               = SimdA::mix_2_1_v;
	mix_2_1_vlr             = SimdA::mix_2_1_vlr;
	mix_2_1_vlrauto         = SimdA::mix_2_1_vlrauto;

	mix_2_2                 = SimdA::mix_2_2;
	mix_2_2_v               = SimdA::mix_2_2_v;
	mix_2_2_vlr             = SimdA::mix_2_2_vlr;
	mix_2_2_vlrauto         = SimdA::mix_2_2_vlrauto;

	// Spread mixing
	mix_spread_1_2_v        = SimdA::mix_spread_1_2_v;
	mix_spread_1_2_vlr      = SimdA::mix_spread_1_2_vlr;
	mix_spread_1_2_vlrauto  = SimdA::mix_spread_1_2_vlrauto;

	// Matrix mixing
	mix_mat_2_2_v           = SimdA::mix_mat_2_2_v;
	mix_mat_2_2_vlr         = SimdA::mix_mat_2_2_vlr;
	mix_mat_2_2_vlrauto     = SimdA::mix_mat_2_2_vlrauto;

	// Mix and interleave
	mix_1_2i                = SimdA::mix_1_2i;
	mix_1_2i_v              = SimdA::mix_1_2i_v;
	mix_1_2i_vlr            = SimdA::mix_1_2i_vlr;
	mix_1_2i_vlrauto        = SimdA::mix_1_2i_vlrauto;

	mix_2_2i                = SimdA::mix_2_2i;
	mix_2_2i_v              = SimdA::mix_2_2i_v;
	mix_2_2i_vlr            = SimdA::mix_2_2i_vlr;
	mix_2_2i_vlrauto        = SimdA::mix_2_2i_vlrauto;

	// Mix and deinterleave
	mix_2i_1                = SimdA::mix_2i_1;
	mix_2i_1_v              = SimdA::mix_2i_1_v;
	mix_2i_1_vlr            = SimdA::mix_2i_1_vlr;
	mix_2i_1_vlrauto        = SimdA::mix_2i_1_vlrauto;

	mix_2i_2                = SimdA::mix_2i_2;
	mix_2i_2_v              = SimdA::mix_2i_2_v;
	mix_2i_2_vlr            = SimdA::mix_2i_2_vlr;
	mix_2i_2_vlrauto        = SimdA::mix_2i_2_vlrauto;

//	mix_ni1_1               = SimdA::mix_ni1_1;

	// Multiply
	mult_1_1                = SimdA::mult_1_1;

	// Multiply, in-place
	mult_ip_1_1             = SimdA::mult_ip_1_1;
	mult_ip_1_2             = SimdA::mult_ip_1_2;
	mult_ip_2_2             = SimdA::mult_ip_2_2;

	// Misc
	clear                   = SimdA::clear;
//	clear_nim               = SimdA::clear_nim;

	fill                    = SimdA::fill;
	fill_lr                 = SimdA::fill_lr;
	fill_lrauto             = SimdA::fill_lrauto;

	add_cst_1_1             = SimdA::add_cst_1_1;
	add_cst_1_2             = SimdA::add_cst_1_2;
	
	linop_cst_1_1           = SimdA::linop_cst_1_1;
	linop_cst_ip_1          = SimdA::linop_cst_ip_1;

	add_sub_ip_2_2          = SimdA::add_sub_ip_2_2;

	sum_square_n_1          = SimdA::sum_square_n_1;
	sum_square_n_1_v        = SimdA::sum_square_n_1_v;
}

#endif // X86, ARM



Align &	Align::use_instance ()
{
	static Align   instance;

	return (instance);
}



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
