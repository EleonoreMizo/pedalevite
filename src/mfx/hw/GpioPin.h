/*****************************************************************************

        GpioPin.h
        Author: Laurent de Soras, 2023

References all the GPIO pins used in the program.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_GpioPin_HEADER_INCLUDED)
#define mfx_hw_GpioPin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace hw
{



class GpioPin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// BCM numbering (GPIO)

	// Shared
	static constexpr int _reset      = 24; // GPIO 24 / phys 18

	// Switches
	static constexpr int _nav_ok     =  4; // GPIO  4 / phys  7
	static constexpr int _nav_cancel = 25; // GPIO 25 / phys 22

	// LEDs
	static constexpr int _led_0      = 17; // GPIO 17 / phys 11
	static constexpr int _led_1      = 27; // GPIO 27 / phys 13
	static constexpr int _led_2      = 22; // GPIO 22 / phys 15

	// Small Nokia display (never acually used)
	static constexpr int _pcd8544_dc = 18; // GPIO 18 / phys 12
	static constexpr int _pcd8544_cs = 23; // GPIO 23 / phys 16

	// 128x64 display (SPI)
	static constexpr int _st7920_cs  = 23; // GPIO 23 / phys 16

	// Audio interface (I2S)
	static constexpr int _snd_reset  =  5; // GPIO  5 / phys 29
	static constexpr int _snd_sfreq  =  6; // GPIO  6 / phys 31
	static constexpr int _snd_bclk   = 18; // GPIO 18 / phys 12
	static constexpr int _snd_lrck   = 19; // GPIO 19 / phys 35
	static constexpr int _snd_din    = 20; // GPIO 20 / phys 38
	static constexpr int _snd_dout   = 21; // GPIO 21 / phys 40



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               GpioPin ()                               = delete;
	               ~GpioPin ()                              = delete;
	               GpioPin (const GpioPin &other)           = delete;
	               GpioPin (GpioPin &&other)                = delete;
	GpioPin &      operator = (const GpioPin &other)        = delete;
	GpioPin &      operator = (GpioPin &&other)             = delete;
	bool           operator == (const GpioPin &other) const = delete;
	bool           operator != (const GpioPin &other) const = delete;

}; // class GpioPin



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/GpioPin.hpp"



#endif   // mfx_hw_GpioPin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
