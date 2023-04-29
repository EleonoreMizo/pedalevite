/*****************************************************************************

        GpioAccess.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_GpioAccess_HEADER_INCLUDED)
#define mfx_hw_GpioAccess_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/bcm2837gpio.h"
#include "mfx/hw/MmapPtr.h"

#include <cstdint>



namespace mfx
{
namespace hw
{



class GpioAccess
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Actually it is 54, but we use only the 32 first ones, and it
	// simplifies addressing.
	static const int  _nbr_gpio = 32;

	               GpioAccess ();
	               ~GpioAccess () = default;

	void           set_fnc (int gpio, bcm2837gpio::PinFnc fnc) const;
	inline void    clear (int gpio) const;
	inline void    set (int gpio) const;
	inline void    write (int gpio, int val) const;
	inline int     read (int gpio) const;
	void           pull (int gpio, bcm2837gpio::Pull p) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Offsets in 32-bit words
	static constexpr uint32_t  _reg_fnc  = bcm2837gpio::_gpfsel   >> 2; //  0
	static constexpr uint32_t  _reg_set  = bcm2837gpio::_gpset    >> 2; //  7
	static constexpr uint32_t  _reg_clr  = bcm2837gpio::_gpclr    >> 2; // 10
	static constexpr uint32_t  _reg_lvl  = bcm2837gpio::_gplev    >> 2; // 13
	static constexpr uint32_t  _reg_evt  = bcm2837gpio::_gpeds    >> 2; // 16
	static constexpr uint32_t  _reg_ris  = bcm2837gpio::_gpren    >> 2; // 19
	static constexpr uint32_t  _reg_fal  = bcm2837gpio::_gpfen    >> 2; // 22
	static constexpr uint32_t  _reg_hi   = bcm2837gpio::_gphen    >> 2; // 25
	static constexpr uint32_t  _reg_lo   = bcm2837gpio::_gplen    >> 2; // 28
	static constexpr uint32_t  _reg_aris = bcm2837gpio::_gparen   >> 2; // 31
	static constexpr uint32_t  _reg_afal = bcm2837gpio::_gpafen   >> 2; // 34
	static constexpr uint32_t  _reg_pull = bcm2837gpio::_gppud    >> 2; // 37
	static constexpr uint32_t  _reg_pclk = bcm2837gpio::_gppudclk >> 2; // 38
	static constexpr int  _fnc_field_size = 3;

	static inline void
	               find_addr_fnc (int &ofs_reg, int &shf_bit, int gpio);

	uint32_t       _periph_addr;            // 0x3F000000 on Pi 3, 0x20000000 on Pi 1
	MmapPtr        _gpio_mptr;
	volatile uint32_t *
	               _gpio_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               GpioAccess (const GpioAccess &other)        = delete;
	               GpioAccess (GpioAccess &&other)             = delete;
	GpioAccess &   operator = (const GpioAccess &other)        = delete;
	GpioAccess &   operator = (GpioAccess &&other)             = delete;
	bool           operator == (const GpioAccess &other) const = delete;
	bool           operator != (const GpioAccess &other) const = delete;

}; // class GpioAccess



}  // namespace hw
}  // namespace mfx



#include "mfx/hw/GpioAccess.hpp"



#endif   // mfx_hw_GpioAccess_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
