/*****************************************************************************

        bcm2837clk.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_bcm2837clk_HEADER_INCLUDED)
#define mfx_hw_bcm2837clk_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{
namespace bcm2837clk
{



// Clock registers relative to the base address (p. 107)
static const uint32_t   _clk_ofs   = 0x00101000;
static const uint32_t   _clk_len   = 0xA8;

// Register map
static const uint32_t   _gp0ctl    = 0x70;
static const uint32_t   _gp0div    = 0x74;
static const uint32_t   _gp1ctl    = 0x78;
static const uint32_t   _gp1div    = 0x7C;
static const uint32_t   _gp2ctl    = 0x80;
static const uint32_t   _gp2div    = 0x84;

static const uint32_t   _pcmctl    = 0x98;
static const uint32_t   _pcmdiv    = 0x9C;

static const uint32_t   _pwmctl    = 0xA0;
static const uint32_t   _pwmdiv    = 0xA4;

// Common to all registers
static const uint32_t   _passwd    = 0x5A << 24;

// Control register fields
static const uint32_t   _mash_idiv = 0 <<  9;
static const uint32_t   _mash_1    = 1 <<  9;
static const uint32_t   _mash_2    = 2 <<  9;
static const uint32_t   _mash_3    = 3 <<  9;
static const uint32_t   _flip      = 1 <<  8;
static const uint32_t   _busy      = 1 <<  7;
static const uint32_t   _kill      = 1 <<  5;
static const uint32_t   _enab      = 1 <<  4;
static const uint32_t   _src_gnd   = 0 <<  0;
static const uint32_t   _src_osc   = 1 <<  0;
static const uint32_t   _src_dbg0  = 2 <<  0;
static const uint32_t   _src_dbg1  = 3 <<  0;
static const uint32_t   _src_plla  = 4 <<  0;
static const uint32_t   _src_pllc  = 5 <<  0;
static const uint32_t   _src_plld  = 6 <<  0;
static const uint32_t   _src_hdmi  = 7 <<  0;

// Divisor register fields
static const int        _divi      = 12; // 12 bits
static const int        _divf      =  0; // 12 bits



}  // namespace bcm2837clk
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_bcm2837clk_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
