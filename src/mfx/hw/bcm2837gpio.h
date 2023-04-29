/*****************************************************************************

        bcm2837gpio.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_bcm2837gpio_bcm2837gpio_HEADER_INCLUDED)
#define mfx_hw_bcm2837gpio_bcm2837gpio_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{
namespace bcm2837gpio
{



static constexpr int       _nbr_gpio = 54;    // Number of GPIO pins (0-based)

// GPIO registers relative to the base address (p. 90)
static constexpr uint32_t  _gpio_ofs = 0x00200000;
static constexpr uint32_t  _gpio_len = 0x1000; // Bytes

// Register map
static constexpr uint32_t  _gpfsel   = 0x00;  // W, 6 words, 10 pins per word
static constexpr uint32_t  _gpset    = 0x1C;  // W, 2 words
static constexpr uint32_t  _gpclr    = 0x28;  // W, 2 words
static constexpr uint32_t  _gplev    = 0x34;  // R, 2 words
static constexpr uint32_t  _gpeds    = 0x40;  // R/W, 2 words
static constexpr uint32_t  _gpren    = 0x4C;  // R/W, 2 words
static constexpr uint32_t  _gpfen    = 0x58;  // R/W, 2 words
static constexpr uint32_t  _gphen    = 0x64;  // R/W, 2 words
static constexpr uint32_t  _gplen    = 0x70;  // R/W, 2 words
static constexpr uint32_t  _gparen   = 0x7C;  // R/W, 2 words
static constexpr uint32_t  _gpafen   = 0x88;  // R/W, 2 words
static constexpr uint32_t  _gppud    = 0x94;  // R/W
static constexpr uint32_t  _gppudclk = 0x98;  // R/W, 2 words

// Registers specific to BCM2711
static constexpr uint32_t  _gppuppdn = 0xE4;  // R/W, 4 words, 16 pins per word

enum PinFnc
{
	PinFnc_IN = 0,
	PinFnc_OUT,
	PinFnc_ALT5,
	PinFnc_ALT4,
	PinFnc_ALT0,
	PinFnc_ALT1,
	PinFnc_ALT2,
	PinFnc_ALT3,

	PinFnc_NBR_ELT,
};
static constexpr int       _fnc_field_size = 3; // Bits

enum Pull
{
	Pull_NONE = 0,
	Pull_DOWN,
	Pull_UP,

	Pull_NBR_ELT
};



}  // namespace bcm2837gpio
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_bcm2837gpio_bcm2837gpio_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
