/*****************************************************************************

        bcm2837pcm.h
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
#if ! defined (mfx_hw_bcm2837pcm_HEADER_INCLUDED)
#define mfx_hw_bcm2837pcm_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{
namespace bcm2837pcm
{



// Physical base address for the peripherals
static const uint32_t   _phys_base = 0x7E000000;

// PCM/I2S registers relative to the base address (p. 125)
static const uint32_t   _pcm_ofs   = 0x00203000;
static const uint32_t   _pcm_len   = 0x24;

// PCM register map
static const uint32_t   _cs_a      = 0x00;
static const uint32_t   _fifo_a    = 0x04;
static const uint32_t   _mode_a    = 0x08;
static const uint32_t   _rxc_a     = 0x0C;
static const uint32_t   _txc_a     = 0x10;
static const uint32_t   _dreq_a    = 0x14;
static const uint32_t   _inten_a   = 0x18;
static const uint32_t   _intstc_a  = 0x1C;
static const uint32_t   _gray      = 0x20;

// CS_A (p. 126)
static const uint32_t   _cs_a_stby   = 1 << 25; // RAM Standby
static const uint32_t   _cs_a_sync   = 1 << 24; // PCM Clock sync helper
static const uint32_t   _cs_a_rxsex  = 1 << 23; // RX Sign Extend
static const uint32_t   _cs_a_rxf    = 1 << 22; // RX FIFO is Full
static const uint32_t   _cs_a_txe    = 1 << 21; // TX FIFO is Empty
static const uint32_t   _cs_a_rxd    = 1 << 20; // Indicates that the RX FIFO contains data
static const uint32_t   _cs_a_txd    = 1 << 19; // Indicates that the TX FIFO can accept data
static const uint32_t   _cs_a_rxr    = 1 << 18; // Indicates that the RX FIFO needs reading
static const uint32_t   _cs_a_txw    = 1 << 17; // Indicates that the TX FIFO needs Writing
static const uint32_t   _cs_a_rxerr  = 1 << 16; // RX FIFO Error
static const uint32_t   _cs_a_txerr  = 1 << 15; // TX FIFO Error
static const uint32_t   _cs_a_rxsync = 1 << 14; // RX FIFO Sync
static const uint32_t   _cs_a_txsync = 1 << 13; // TX FIFO Sync
static const uint32_t   _cs_a_dmaen  = 1 <<  9; // DMA DREQ Enable
static const int        _cs_a_rxthr  =       7; // 2 bits - Sets the RX FIFO threshold at which point the RXR flag is set
static const uint32_t   _cs_a_rxthr_one  = 0 <<  7; // Single sample in the RX FIFO
static const uint32_t   _cs_a_rxthr_mid1 = 1 <<  7; // At least full
static const uint32_t   _cs_a_rxthr_mid2 = 2 <<  7; // At least full
static const uint32_t   _cs_a_rxthr_full = 3 <<  7; // Full
static const int        _cs_a_txthr  =       5; // 2 bits - Sets the TX FIFO threshold at which point the TXW flag is set
static const uint32_t   _cs_a_txthr_zero = 0 <<  5; // TX FIFO is empty
static const uint32_t   _cs_a_txthr_mid1 = 1 <<  5; // Less than full
static const uint32_t   _cs_a_txthr_mid2 = 2 <<  5; // Less than full
static const uint32_t   _cs_a_txthr_ful1 = 3 <<  5; // Full but for one sample
static const uint32_t   _cs_a_rxclr  = 1 <<  4; // Clear the RX FIFO
static const uint32_t   _cs_a_txclr  = 1 <<  3; // Clear the TX FIFO
static const uint32_t   _cs_a_txon   = 1 <<  2; // Enable transmission
static const uint32_t   _cs_a_rxon   = 1 <<  1; // Enable reception
static const uint32_t   _cs_a_en     = 1 <<  0; // Enable the PCM Audio Interface

// MODE_A (p. 130)
static const uint32_t   _mode_a_clk_dis = 1 << 28; // PCM Clock Disable
static const uint32_t   _mode_a_pdmn    = 1 << 27; // PDM Decimation Factor (N)
static const uint32_t   _mode_a_pdme    = 1 << 26; // PDM Input Mode Enable
static const uint32_t   _mode_a_frxp    = 1 << 25; // Receive Frame Packed Mode
static const uint32_t   _mode_a_ftxp    = 1 << 24; // Transmit Frame Packed Mode
static const uint32_t   _mode_a_clkm    = 1 << 23; // PCM Clock Mode
static const uint32_t   _mode_a_clki    = 1 << 22; // Clock Invert
static const uint32_t   _mode_a_fsm     = 1 << 21; // Frame Sync Mode
static const uint32_t   _mode_a_fsi     = 1 << 20; // Frame Sync Invert
static const int        _mode_a_flen    =      10; // 10 bits - Frame Length
static const int        _mode_a_fslen   =       0; // 10 bits - Frame Sync Length

// RXC_A, TXC_A (p. 132)
static const int        _xc_a_ch1 =      16; // Channel 1
static const int        _xc_a_ch2 =       0; // Channel 2

static const uint32_t   _xc_a_wex = 1 << 15; // Width Extension Bit
static const uint32_t   _xc_a_en  = 1 << 14; // Enable
static const int        _xc_a_pos =       4; // 10 bits - Position
static const int        _xc_a_wid =       0; //  4 bits - Width

// DREQ_A (p. 134)
static const int        _dreq_a_tx_panic =      24; // 7 bits - TX Panic Level
static const int        _dreq_a_rx_panic =      16; // 7 bits - RX Panic Level
static const int        _dreq_a_tx       =       8; // 7 bits - TX Request Level
static const int        _dreq_a_rx       =       0; // 7 bits - RX Request Level

// INTEN_A (p. 135)
static const uint32_t   _inten_a_rxerr = 1 <<  3; // RX Error Interrupt
static const uint32_t   _inten_a_rxerr = 1 <<  2; // TX Error Interrupt
static const uint32_t   _inten_a_rxr   = 1 <<  1; // RX Read Interrupt Enable
static const uint32_t   _inten_a_rxw   = 1 <<  0; // TX Write Interrupt Enable

// INTSTC_A (p. 136)
static const uint32_t   _intstc_a_rxerr = 1 <<  3; // RX Error Interrupt Status/Clear
static const uint32_t   _intstc_a_rxerr = 1 <<  2; // TX Error Interrupt Status/Clear
static const uint32_t   _intstc_a_rxr   = 1 <<  1; // RX Read Interrupt Status/Clear
static const uint32_t   _intstc_a_rxw   = 1 <<  0; // TX Write Interrupt Status/Clear

// GRAY (p. 136)
static const int        _gray_rxfifolevel =      16; // 6 bits - The Current level of the RXFIFO
static const int        _gray_flushed     =      10; // 6 bits - The Number of bits that were flushed into the RXFIFO
static const int        _gray_rxlevel     =       4; // 6 bits - The Current fill level of the RX Buffer
static const uint32_t   _gray_flush       = 1 <<  2; // Flush the RX Buffer into the RX FIFO
static const uint32_t   _gray_clr         = 1 <<  1; // Clear the GRAY Mode Logic
static const uint32_t   _gray_en          = 1 <<  0; // Enable GRAY Mode



}  // namespace bcm2837pcm
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_bcm2837pcm_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
