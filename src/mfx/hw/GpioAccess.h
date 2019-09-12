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

#include "mfx/hw/MmapPtr.h"



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

	enum PinFnc
	{
		PinFnc_IN = 0,
		PinFnc_OUT,
		PinFnc_ALT_5,
		PinFnc_ALT_4,
		PinFnc_ALT_0,
		PinFnc_ALT_1,
		PinFnc_ALT_2,
		PinFnc_ALT_3,

		PinFnc_NBR_ELT,
	};
	enum Pull
	{
		Pull_NONE = 0,
		Pull_DOWN,
		Pull_UP,

		Pull_NBR_ELT
	};

	               GpioAccess ();
	               ~GpioAccess () = default;

	void           set_fnc (int gpio, PinFnc fnc) const;
	inline void    clear (int gpio) const;
	inline void    set (int gpio) const;
	inline void    write (int gpio, int val) const;
	inline int     read (int gpio) const;
	inline int     read_cached (int gpio) const;
	inline void    pull (int gpio, Pull p) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const uint32_t
	               _ofs_gpio     = 0x200000U;
	static const uint32_t                   // Bytes
	               _len_gpio     = 1024 * sizeof (uint32_t);
	static const uint32_t                   // In 32-bit words. W
	               _ofs_reg_fnc  =  0;
	static const uint32_t                   // In 32-bit words. W
	               _ofs_reg_set  =  7;
	static const uint32_t                   // In 32-bit words. W
	               _ofs_reg_clr  = 10;
	static const uint32_t                   // In 32-bit words. R
	               _ofs_reg_lvl  = 13;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_evt  = 16;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_ris  = 19;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_fal  = 22;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_hi   = 25;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_lo   = 28;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_aris = 31;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_afal = 34;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_pull = 37;
	static const uint32_t                   // In 32-bit words. R/W
	               _ofs_reg_pclk = 38;
	static const int  _fnc_field_size = 3;

	static inline void
	               find_addr_fnc (int &ofs_reg, int &shf_bit, int gpio);

	uint32_t       _periph_addr;            // 0x3F000000 on Pi 3, 0x20000000 on Pi 1
	MmapPtr        _gpio_mptr;
	volatile uint32_t *
	               _gpio_ptr;
	mutable uint32_t
	               _last_read;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               GpioAccess (const GpioAccess &other)        = delete;
	GpioAccess &   operator = (const GpioAccess &other)        = delete;
	bool           operator == (const GpioAccess &other) const = delete;
	bool           operator != (const GpioAccess &other) const = delete;

}; // class GpioAccess



}  // namespace hw
}  // namespace mfx



#include "mfx/hw/GpioAccess.hpp"



#endif   // mfx_hw_GpioAccess_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
