/*****************************************************************************

        bcm2837pwm.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_bcm2837pwm_HEADER_INCLUDED)
#define mfx_hw_bcm2837pwm_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{
namespace bcm2837pwm
{



// PWM registers relative to the base address (p. 141)
static const uint32_t   _pwm_ofs   = 0x0020C000;
static const uint32_t   _pwm_len   = 0x30;

// Register map
static const uint32_t   _ctl  = 0x00;
static const uint32_t   _sta  = 0x04;
static const uint32_t   _dmac = 0x08;
static const uint32_t   _rng1 = 0x10;
static const uint32_t   _dat1 = 0x14;
static const uint32_t   _fif1 = 0x18;
static const uint32_t   _rng2 = 0x20;
static const uint32_t   _dat2 = 0x24;

// CTL (p. 142)
static const uint32_t   _msen2       = 1 << 15; // Channel 2 M/S Enable
static const uint32_t   _usef2       = 1 << 13; // Channel 2 Use Fifo
static const uint32_t   _pola2       = 1 << 12; // Channel 2 Polarity
static const uint32_t   _sbit2       = 1 << 11; // Channel 2 Silence Bit
static const uint32_t   _rptl2       = 1 << 10; // Channel 2 Repeat Last Data
static const uint32_t   _mode2       = 1 <<  9; // Channel 2 Mode
static const uint32_t   _pwen2       = 1 <<  8; // Channel 2 Enable
static const uint32_t   _msen1       = 1 <<  7; // Channel 1 M/S Enable
static const uint32_t   _clrf1       = 1 <<  6; // Clear Fifo
static const uint32_t   _usef1       = 1 <<  5; // Channel 1 Use Fifo
static const uint32_t   _pola1       = 1 <<  4; // Channel 1 Polarity
static const uint32_t   _sbit1       = 1 <<  3; // Channel 1 Silence Bit
static const uint32_t   _rptl1       = 1 <<  2; // Channel 1 Repeat Last Data
static const uint32_t   _mode1       = 1 <<  1; // Channel 1 Mode
static const uint32_t   _pwen1       = 1 <<  0; // Channel 1 Enable

// STA (p. 144)
static const uint32_t   _sta4        = 1 << 12; // Channel 4 State
static const uint32_t   _sta3        = 1 << 11; // Channel 3 State
static const uint32_t   _sta2        = 1 << 10; // Channel 2 State
static const uint32_t   _sta1        = 1 <<  9; // Channel 1 State
static const uint32_t   _berr        = 1 <<  8; // Bus Error Flag
static const uint32_t   _gapo4       = 1 <<  7; // Channel 4 Gap Occurred Flag
static const uint32_t   _gapo3       = 1 <<  6; // Channel 3 Gap Occurred Flag
static const uint32_t   _gapo2       = 1 <<  5; // Channel 2 Gap Occurred Flag
static const uint32_t   _gapo1       = 1 <<  4; // Channel 1 Gap Occurred Flag
static const uint32_t   _rerr1       = 1 <<  3; // Fifo Read Error Flag
static const uint32_t   _werr1       = 1 <<  2; // Fifo Write Error Flag
static const uint32_t   _empt1       = 1 <<  1; // Fifo Empty Flag
static const uint32_t   _full1       = 1 <<  0; // Fifo Full Flag 

// DMAC (p. 145)
static const uint32_t   _enab        = 1 << 31; // DMA Enable
static const int        _panic       =       8; // 8 bits - DMA Threshold for PANIC signal
static const int        _dreq        =       0; // 8 bits - DMA Threshold for DREQ signal



}  // namespace bcm2837pwm
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_bcm2837pwm_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
