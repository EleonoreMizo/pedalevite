/*****************************************************************************

        bcm2837.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_bcm2837_HEADER_INCLUDED)
#define mfx_hw_bcm2837_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{
namespace bcm2837
{



// Bus base address for the peripherals
static const uint32_t   _bus_base = 0x7E000000;

// Physical addresses. Depends on the SoC model.
// https://www.raspberrypi.org/documentation/hardware/raspberrypi/peripheral_addresses.md
static const uint32_t   _phys_base_2835 = 0x20000000;
static const uint32_t   _phys_base_2836 = 0x3F000000;
static const uint32_t   _phys_base_2837 = 0x3F000000;
static const uint32_t   _phys_base_2711 = 0xFE000000;

// PLLD clock frequency, in Hz
static const int        _plld_freq_2835 = 500 * 1000 * 1000; // 500 MHz
static const int        _plld_freq_2711 = 750 * 1000 * 1000; // 750 MHz

// Oscillator frequency, in Hz
static const int        _osc_freq_2835  = 19200 * 1000; // 19.2 MHz
static const int        _osc_freq_2711  = 54000 * 1000; // 54 MHz (experimental data)



}  // namespace bcm2837
}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_bcm2837_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
