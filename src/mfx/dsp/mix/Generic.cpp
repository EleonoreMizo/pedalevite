/*****************************************************************************

        Generic.cpp
        Author: Laurent de Soras, 2002

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

#include "mfx/dsp/mix/Fpu.h"
#include "mfx/dsp/mix/Generic.h"
#include "fstb/def.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace mix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Generic::is_ready ()
{
	return (use_instance ()._state == State_READY);
}



void	Generic::setup ()
{
	use_instance ().setup_internal ();
}



// Scale
void	(*Generic::scale_1_v) (float data_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::scale_1_vlr) (float data_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::scale_1_vlrauto) (float data_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::scale_2_v) (float data_1_ptr [], float data_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::scale_2_vlr) (float data_1_ptr [], float data_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::scale_2_vlrauto) (float data_1_ptr [], float data_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

// Copy
void	(*Generic::copy_1_1) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_1_1_v) (float out_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_1_1_vlr) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_1_1_vlrauto) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_2_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

// Spread copying
void	(*Generic::copy_spread_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol_l, float vol_r) = 0;
void	(*Generic::copy_spread_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;
void	(*Generic::copy_spread_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;

// Cross-fade copying
void	(*Generic::copy_xfade_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float xf) = 0;
void	(*Generic::copy_xfade_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_xf, float e_xf) = 0;
void	(*Generic::copy_xfade_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_xf, float e_xf) = 0;

void	(*Generic::copy_xfade_3_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], long nbr_spl) = 0;

// Matrix copying
void	(*Generic::copy_mat_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &vol) = 0;
void	(*Generic::copy_mat_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;
void	(*Generic::copy_mat_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;

// Copy and interleave
void	(*Generic::copy_1_2i) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_1_2i_v) (float out_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_1_2i_vlr) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_1_2i_vlrauto) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_2_2i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_2_2i_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_2_2i_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_2_2i_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_4_4i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], const float in_4_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_2_4i2) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;

void	(*Generic::copy_1_ni1) (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_out) = 0;

// Copy and deinterleave
void	(*Generic::copy_2i_1) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_2i_1_v) (float out_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_2i_1_vlr) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_2i_1_vlrauto) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_2i_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_2i_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::copy_2i_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::copy_2i_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::copy_4i_1) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_4i_4) (float out_1_ptr [], float out_2_ptr [], float out_3_ptr [], float out_4_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::copy_4i2_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl) = 0;

void	(*Generic::copy_ni1_1) (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_in) = 0;

// Copy and convert interleaving
void	(*Generic::copy_nip_mip) (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_out, int nbr_chn_in, int nbr_chn_copy) = 0;

// Mixing
void	(*Generic::mix_1_1) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_1_1_v) (float out_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_1_1_vlr) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_1_1_vlrauto) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::mix_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::mix_2_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_2_1_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_2_1_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_2_1_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::mix_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

// Spread mixing
void	(*Generic::mix_spread_1_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol_l, float vol_r) = 0;
void	(*Generic::mix_spread_1_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;
void	(*Generic::mix_spread_1_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) = 0;

// Matrix mixing
void	(*Generic::mix_mat_2_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &vol) = 0;
void	(*Generic::mix_mat_2_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;
void	(*Generic::mix_mat_2_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) = 0;

// Mix and interleave
void	(*Generic::mix_1_2i) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_1_2i_v) (float out_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_1_2i_vlr) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_1_2i_vlrauto) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::mix_2_2i) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_2_2i_v) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_2_2i_vlr) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_2_2i_vlrauto) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

// Mix and deinterleave
void	(*Generic::mix_2i_1) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_2i_1_v) (float out_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_2i_1_vlr) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_2i_1_vlrauto) (float out_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::mix_2i_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mix_2i_2_v) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float vol) = 0;
void	(*Generic::mix_2i_2_vlr) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;
void	(*Generic::mix_2i_2_vlrauto) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl, float s_vol, float e_vol) = 0;

void	(*Generic::mix_ni1_1) (float out_ptr [], const float in_ptr [], long nbr_spl, int nbr_chn_in) = 0;

// Multiply
void	(*Generic::mult_1_1) (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;

// Multiply, in-place
void	(*Generic::mult_ip_1_1) (float out_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mult_ip_1_2) (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], long nbr_spl) = 0;
void	(*Generic::mult_ip_2_2) (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], long nbr_spl) = 0;

// Misc
void	(*Generic::clear) (float out_ptr [], long nbr_spl) = 0;
void	(*Generic::clear_nim) (float out_ptr [], long nbr_spl, int clear_len, int skip_len) = 0;

void	(*Generic::fill) (float out_ptr [], long nbr_spl, float val) = 0;
void	(*Generic::fill_lr) (float out_ptr [], long nbr_spl, float s_val, float e_val) = 0;
void	(*Generic::fill_lrauto) (float out_ptr [], long nbr_spl, float s_val, float e_val) = 0;

void	(*Generic::add_cst_1_1) (float out_ptr [], long nbr_spl, float val) = 0;
void	(*Generic::add_cst_1_2) (float out_1_ptr [], float out_2_ptr [], long nbr_spl, float val) = 0;

void	(*Generic::linop_cst_1_1) (float out_ptr [], const float in_ptr [], long nbr_spl, float mul_val, float add_val) = 0;
void	(*Generic::linop_cst_ip_1) (float data_ptr [], long nbr_spl, float mul_val, float add_val) = 0;

void	(*Generic::add_sub_ip_2_2) (float out_1_ptr [], float out_2_ptr [], long nbr_spl) = 0;

void	(*Generic::sum_square_n_1) (float out_ptr [], const float * const src_ptr_arr [], long nbr_spl, int nbr_chn, float init_val) = 0;
void	(*Generic::sum_square_n_1_v) (float out_ptr [], const float * const src_ptr_arr [], long nbr_spl, int nbr_chn, float init_val, float vol) = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Generic::Generic ()
:	_state (Generic::State_UNBORN)
{
	setup_internal ();
}



void	Generic::setup_internal ()
{
	if (_state != State_READY)
	{
		setup_fpu ();

		_state = State_READY;
	}
}



void	Generic::setup_fpu ()
{
	// Scale
	scale_1_v               = Fpu::scale_1_v;
	scale_1_vlr             = Fpu::scale_1_vlr;
	scale_1_vlrauto         = Fpu::scale_1_vlrauto;

	scale_2_v               = Fpu::scale_2_v;
	scale_2_vlr             = Fpu::scale_2_vlr;
	scale_2_vlrauto         = Fpu::scale_2_vlrauto;

	// Copy
	copy_1_1                = Fpu::copy_1_1;
	copy_1_1_v              = Fpu::copy_1_1_v;
	copy_1_1_vlr            = Fpu::copy_1_1_vlr;
	copy_1_1_vlrauto        = Fpu::copy_1_1_vlrauto;

	copy_1_2                = Fpu::copy_1_2;
	copy_1_2_v              = Fpu::copy_1_2_v;
	copy_1_2_vlr            = Fpu::copy_1_2_vlr;
	copy_1_2_vlrauto        = Fpu::copy_1_2_vlrauto;

	copy_2_1                = Fpu::copy_2_1;
	copy_2_1_v              = Fpu::copy_2_1_v;
	copy_2_1_vlr            = Fpu::copy_2_1_vlr;
	copy_2_1_vlrauto        = Fpu::copy_2_1_vlrauto;

	copy_2_2                = Fpu::copy_2_2;
	copy_2_2_v              = Fpu::copy_2_2_v;
	copy_2_2_vlr            = Fpu::copy_2_2_vlr;
	copy_2_2_vlrauto        = Fpu::copy_2_2_vlrauto;

	// Spread copying
	copy_spread_1_2_v       = Fpu::copy_spread_1_2_v;
	copy_spread_1_2_vlr     = Fpu::copy_spread_1_2_vlr;
	copy_spread_1_2_vlrauto = Fpu::copy_spread_1_2_vlrauto;

	// Cross-fade copying
	copy_xfade_2_1_v        = Fpu::copy_xfade_2_1_v;
	copy_xfade_2_1_vlr      = Fpu::copy_xfade_2_1_vlr;
	copy_xfade_2_1_vlrauto  = Fpu::copy_xfade_2_1_vlrauto;

	copy_xfade_3_1          = Fpu::copy_xfade_3_1;

	// Matrix copying
	copy_mat_2_2_v          = Fpu::copy_mat_2_2_v;
	copy_mat_2_2_vlr        = Fpu::copy_mat_2_2_vlr;
	copy_mat_2_2_vlrauto    = Fpu::copy_mat_2_2_vlrauto;

	// Copy and interleave
	copy_1_2i               = Fpu::copy_1_2i;
	copy_1_2i_v             = Fpu::copy_1_2i_v;
	copy_1_2i_vlr           = Fpu::copy_1_2i_vlr;
	copy_1_2i_vlrauto       = Fpu::copy_1_2i_vlrauto;

	copy_2_2i               = Fpu::copy_2_2i;
	copy_2_2i_v             = Fpu::copy_2_2i_v;
	copy_2_2i_vlr           = Fpu::copy_2_2i_vlr;
	copy_2_2i_vlrauto       = Fpu::copy_2_2i_vlrauto;

	copy_4_4i               = Fpu::copy_4_4i;
	copy_2_4i2              = Fpu::copy_2_4i2;

	copy_1_ni1              = Fpu::copy_1_ni1;

	// Copy and deinterleave
	copy_2i_1               = Fpu::copy_2i_1;
	copy_2i_1_v             = Fpu::copy_2i_1_v;
	copy_2i_1_vlr           = Fpu::copy_2i_1_vlr;
	copy_2i_1_vlrauto       = Fpu::copy_2i_1_vlrauto;

	copy_2i_2               = Fpu::copy_2i_2;
	copy_2i_2_v             = Fpu::copy_2i_2_v;
	copy_2i_2_vlr           = Fpu::copy_2i_2_vlr;
	copy_2i_2_vlrauto       = Fpu::copy_2i_2_vlrauto;

	copy_4i_1               = Fpu::copy_4i_1;
	copy_4i_4               = Fpu::copy_4i_4;
	copy_4i2_2              = Fpu::copy_4i2_2;

	copy_ni1_1              = Fpu::copy_ni1_1;

	// Copy and convert interleaving
	copy_nip_mip            = Fpu::copy_nip_mip;

	// Mixing
	mix_1_1                 = Fpu::mix_1_1;
	mix_1_1_v               = Fpu::mix_1_1_v;
	mix_1_1_vlr             = Fpu::mix_1_1_vlr;
	mix_1_1_vlrauto         = Fpu::mix_1_1_vlrauto;

	mix_1_2                 = Fpu::mix_1_2;
	mix_1_2_v               = Fpu::mix_1_2_v;
	mix_1_2_vlr             = Fpu::mix_1_2_vlr;
	mix_1_2_vlrauto         = Fpu::mix_1_2_vlrauto;

	mix_2_1                 = Fpu::mix_2_1;
	mix_2_1_v               = Fpu::mix_2_1_v;
	mix_2_1_vlr             = Fpu::mix_2_1_vlr;
	mix_2_1_vlrauto         = Fpu::mix_2_1_vlrauto;

	mix_2_2                 = Fpu::mix_2_2;
	mix_2_2_v               = Fpu::mix_2_2_v;
	mix_2_2_vlr             = Fpu::mix_2_2_vlr;
	mix_2_2_vlrauto         = Fpu::mix_2_2_vlrauto;

	// Spread mixing
	mix_spread_1_2_v        = Fpu::mix_spread_1_2_v;
	mix_spread_1_2_vlr      = Fpu::mix_spread_1_2_vlr;
	mix_spread_1_2_vlrauto  = Fpu::mix_spread_1_2_vlrauto;

	// Matrix mixing
	mix_mat_2_2_v           = Fpu::mix_mat_2_2_v;
	mix_mat_2_2_vlr         = Fpu::mix_mat_2_2_vlr;
	mix_mat_2_2_vlrauto     = Fpu::mix_mat_2_2_vlrauto;

	// Mix and interleave
	mix_1_2i                = Fpu::mix_1_2i;
	mix_1_2i_v              = Fpu::mix_1_2i_v;
	mix_1_2i_vlr            = Fpu::mix_1_2i_vlr;
	mix_1_2i_vlrauto        = Fpu::mix_1_2i_vlrauto;

	mix_2_2i                = Fpu::mix_2_2i;
	mix_2_2i_v              = Fpu::mix_2_2i_v;
	mix_2_2i_vlr            = Fpu::mix_2_2i_vlr;
	mix_2_2i_vlrauto        = Fpu::mix_2_2i_vlrauto;

	// Mix and deinterleave
	mix_2i_1                = Fpu::mix_2i_1;
	mix_2i_1_v              = Fpu::mix_2i_1_v;
	mix_2i_1_vlr            = Fpu::mix_2i_1_vlr;
	mix_2i_1_vlrauto        = Fpu::mix_2i_1_vlrauto;

	mix_2i_2                = Fpu::mix_2i_2;
	mix_2i_2_v              = Fpu::mix_2i_2_v;
	mix_2i_2_vlr            = Fpu::mix_2i_2_vlr;
	mix_2i_2_vlrauto        = Fpu::mix_2i_2_vlrauto;

	mix_ni1_1               = Fpu::mix_ni1_1;

	// Multiply
	mult_1_1                = Fpu::mult_1_1;

	// Multiply, in-place
	mult_ip_1_1             = Fpu::mult_ip_1_1;
	mult_ip_1_2             = Fpu::mult_ip_1_2;
	mult_ip_2_2             = Fpu::mult_ip_2_2;

	// Misc
	clear                   = Fpu::clear;
	clear_nim               = Fpu::clear_nim;

	fill                    = Fpu::fill;
	fill_lr                 = Fpu::fill_lr;
	fill_lrauto             = Fpu::fill_lrauto;

	add_cst_1_1             = Fpu::add_cst_1_1;
	add_cst_1_2             = Fpu::add_cst_1_2;

	linop_cst_1_1           = Fpu::linop_cst_1_1;
	linop_cst_ip_1          = Fpu::linop_cst_ip_1;

	add_sub_ip_2_2          = Fpu::add_sub_ip_2_2;

	sum_square_n_1          = Fpu::sum_square_n_1;
	sum_square_n_1_v        = Fpu::sum_square_n_1_v;
}



Generic &	Generic::use_instance ()
{
	static Generic	instance;

	return (instance);
}



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
