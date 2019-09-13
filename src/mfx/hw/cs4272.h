/*****************************************************************************

        cs4272.h
        Author: Laurent de Soras, 2019

Register bitfields for the CS4272 codec.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_cs4272_HEADER_INCLUDED)
#define mfx_hw_cs4272_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace hw
{
namespace cs4272
{



// 01: Mode Control 1
static const int  _mc1_single       = 0 << 6;
static const int  _mc1_double       = 2 << 6;
static const int  _mc1_quad         = 3 << 6;
static const int  _mc1_r1           = 1 << 5;
static const int  _mc1_r0           = 1 << 4;
static const int  _mc1_master       = 1 << 3;
static const int  _mc1_fmt_left     = 0;
static const int  _mc1_fmt_i2s      = 1;
static const int  _mc1_fmt_r16      = 2;
static const int  _mc1_fmt_r24      = 3;
static const int  _mc1_fmt_r20      = 4;
static const int  _mc1_fmt_r18      = 5;

// 02: DAC Control
static const int  _dacc_amute       = 1 << 7;
static const int  _dacc_flt_slow    = 1 << 6;
static const int  _dacc_deemph_none = 0 << 4;
static const int  _dacc_deemph_44_1 = 1 << 4;
static const int  _dacc_deemph_48   = 2 << 4;
static const int  _dacc_deemph_32   = 3 << 4;
static const int  _dacc_volramp_up  = 1 << 3;
static const int  _dacc_volramp_dw  = 1 << 2;
static const int  _dacc_polarity_a  = 1 << 1;
static const int  _dacc_polarity_b  = 1 << 0;

// 03: DAC Volume & Mixing Control
static const int  _mix_b_eq_a       = 1 << 6;
static const int  _mix_soft_r       = 1 << 5;
static const int  _mix_zero_x       = 1 << 4;
static const int  _mix_atapi_l_to_l = 1 << 3;
static const int  _mix_atapi_r_to_l = 1 << 2;
static const int  _mix_atapi_l_to_r = 1 << 1;
static const int  _mix_atapi_r_to_r = 1 << 0;

// 04: DAC Channel A Volume Control
static const int  _vol_a_mute       = 1 << 7;

// 05: DAC Channel B Volume Control
static const int  _vol_b_mute       = 1 << 7;

// 06: ADC Control
static const int  _adcc_dither16    = 1 << 5;
static const int  _adcc_fmt_left    = 0 << 4;
static const int  _adcc_fmt_i2s     = 1 << 4;
static const int  _adcc_mute_a      = 1 << 3;
static const int  _adcc_mute_b      = 1 << 2;
static const int  _adcc_no_hpf_a    = 1 << 1;
static const int  _adcc_no_hpf_b    = 1 << 0;

// 07: Mode Control 2
static const int  _mc2_loopback     = 1 << 4;
static const int  _mc2_sync_mute_ab = 1 << 3;
static const int  _mc2_freeze       = 1 << 2;
static const int  _mc2_ctrl_port    = 1 << 1;
static const int  _mc2_power_down   = 0 << 1;



}  // namespace cs4272
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_cs4272_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
