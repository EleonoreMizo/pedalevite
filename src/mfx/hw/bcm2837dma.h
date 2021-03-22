/*****************************************************************************

        bcm2837dma.h
        Author: Laurent de Soras, 2019

Register map and bitfields for the PCM/I2S part of the BCM2837 SoC

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_bcm2837dma_HEADER_INCLUDED)
#define mfx_hw_bcm2837dma_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{
namespace bcm2837dma
{



// DMA 0-14 registers relative to the base address (p. 39)
static const uint32_t   _dma_ofs     = 0x00007000;
static const uint32_t   _dma_len     = 0x1000;
static const uint32_t   _dma_chn_len = 0x24;        // Bytes
static const uint32_t   _dma_chn_inc = 0x100;       // Bytes

static const uint32_t   _dma_chn_15  = 0x00E05000;

// Register map per DMA channel (p. 41)
static const uint32_t   _cs          = 0x00; // Control and Status
static const uint32_t   _conblk_ad   = 0x04; // Control Block Address
static const uint32_t   _ti          = 0x08; // CB Word 0 (Transfer Information)
static const uint32_t   _source_ad   = 0x0C; // CB Word 1 (Source Address)
static const uint32_t   _dest_ad     = 0x10; // CB Word 2 (Destination Address)
static const uint32_t   _txfr_len    = 0x14; // CB Word 3 (Transfer Length)
static const uint32_t   _stride      = 0x18; // CB Word 4 (2D Stride)
static const uint32_t   _nextconbk   = 0x1C; // CB Word 5 (Next CB Address)
static const uint32_t   _debug       = 0x20; // Debug

// Other registers
static const uint32_t   _int_status  = 0xFE0;   // Interrupt status of each DMA channel
static const uint32_t   _enable      = 0xFF0;   // Global enable bits for each DMA channel

// Control and Status (p. 47)
static const uint32_t   _reset       = 1 << 31; // DMA Channel Reset
static const uint32_t   _abort       = 1 << 30; // Abort DMA
static const uint32_t   _disdebug    = 1 << 29; // Disable debug pause signal
static const uint32_t   _waitfow     = 1 << 28; // Wait for outstanding writes
static const int        _panic_prio  =      20; // 4 bits - AXI Panic Priority Level
static const int        _priority    =      16; // 4 bits - AXI Priority Level
static const uint32_t   _error       = 1 <<  8; // DMA Error
static const uint32_t   _waitingfow  = 1 <<  6; // DMA is Waiting for the Last Write to be Received
static const uint32_t   _dreq_stops  = 1 <<  5; // DMA Paused by DREQ State
static const uint32_t   _paused      = 1 <<  4; // DMA Paused State
static const uint32_t   _dreq        = 1 <<  3; // DREQ State
static const uint32_t   _int         = 1 <<  2; // Interrupt Status
static const uint32_t   _end         = 1 <<  1; // DMA End Flag
static const uint32_t   _active      = 1 <<  0; // Activate the DMA

// Transfer Information (p. 51, p. 56)
static const uint32_t   _no_wide_b   = 1 << 26; // Don't Do wide writes as a 2 beat burst
static const int        _wait        =      21; // 5 bits - Add Wait Cycles
static const int        _permap      =      16; // 5 bits - Peripheral Mapping
static const int        _burst_len   =      12; // 4 bits - Burst Transfer Length
static const uint32_t   _src_ignore  = 1 << 11; // Ignore Reads
static const uint32_t   _src_dreq    = 1 << 10; // Control Source Reads with DREQ
static const uint32_t   _src_width   = 1 <<  9; // Source Transfer Width
static const uint32_t   _src_inc     = 1 <<  8; // Source Address Increment
static const uint32_t   _dest_ignore = 1 <<  7; // Ignore Writes
static const uint32_t   _dest_dreq   = 1 <<  6; // Control Destination Writes with DREQ
static const uint32_t   _dest_width  = 1 <<  5; // Destination Transfer Width
static const uint32_t   _dest_inc    = 1 <<  4; // Destination Address Increment
static const uint32_t   _wait_resp   = 1 <<  3; // Wait for a Write Response
static const uint32_t   _tdmode      = 1 <<  1; // 2D Mode
static const uint32_t   _inten       = 1 <<  0; // Interrupt Enable

// Transfer Length (p. 53, p. 57)
static const int        _ylength     =      16; // 14 bits - Y transfer length in 2D mode
static const int        _wlength     =       0; // 16 bits - Transfer Length in bytes

// 2D Stride (p. 54)
static const int        _d_stride    =      16; // 16 bits - Destination Stride (2D Mode)
static const int        _s_stride    =       0; // 16 bits - Source Stride (2D Mode)

// Debug (p. 55, p. 58)
static const uint32_t   _lite        = 1 << 28; // DMA Lite
static const int        _version     =      25; // 3 bits - DMA Version
static const int        _dma_state   =      16; // 9 bits - DMA State Machine State
static const int        _dma_id      =       8; // 8 bits - DMA ID
static const int        _outstnd_wr  =       4; // 4 bits - DMA Outstanding Writes Counter
static const uint32_t   _read_error  = 1 <<  2; // Slave Read Response Error
static const uint32_t   _fifo_error  = 1 <<  1; // Fifo Error
static const uint32_t   _rlns_error  = 1 <<  0; // Read Last Not Set Error
static const uint32_t   _all_errors  = _read_error | _fifo_error | _rlns_error;

// Peripheral DREQ Signals (p. 61)
enum Dreq
{
	Dreq_ON = 0,
	Dreq_DSI,
	Dreq_PCM_TX,
	Dreq_PCM_RX,
	Dreq_SMI,
	Dreq_PWM,
	Dreq_SPI_TX,
	Dreq_SPI_RX,
	Dreq_BSC_SPI_TX,
	Dreq_BSC_SPI_RX,
	Dreq_UNUSED,
	Dreq_EMMC,
	Dreq_UART_TX,
	Dreq_SD_HOST,
	Dreq_UART_RX,
	Dreq_DSI_2,
	Dreq_SLIMBUS_MCTX,
	Dreq_HDMI,
	Dreq_SLIMBUX_MCRX,
	Dreq_SLIMBUX_DC0,
	Dreq_SLIMBUX_DC1,
	Dreq_SLIMBUX_DC2,
	Dreq_SLIMBUX_DC3,
	Dreq_SLIMBUX_DC4,
	Dreq_SCALER_FIFO0_SMI,
	Dreq_SCALER_FIFO1_SMI,
	Dreq_SCALER_FIFO2_SMI,
	Dreq_SLIMBUX_DC5,
	Dreq_SLIMBUX_DC6,
	Dreq_SLIMBUX_DC7,
	Dreq_SLIMBUX_DC8,
	Dreq_SLIMBUX_DC9,

	Dreq_NBR_ELT
};

class CtrlBlock
{
public:
	uint32_t       _info;    // TI: transfer information
	uint32_t       _src;     // SOURCE_AD
	uint32_t       _dst;     // DEST_AD
	uint32_t       _length;  // TXFR_LEN: transfer length
	uint32_t       _stride;  // 2D stride mode
	uint32_t       _next;    // NEXTCONBK
	uint32_t       _pad [2]; // Reserved
};



}  // namespace bcm2837dma
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_bcm2837dma_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
