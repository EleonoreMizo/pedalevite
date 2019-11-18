/*****************************************************************************

        GpioPwm.h
        Author: Laurent de Soras, 2016

"Avoid channels 0, 1, 2, 3, 6, 7. The GPU uses 1, 3, 6, 7.
The frame buffer uses 0 and the SD card uses 2."
"You can see the channels reserved for GPU use by
cat /sys/module/dma/parameters/dmachans
Mine shows 32565 or 0111 1111 0011 0101
If bit n is 0 it is reserved for GPU use.
In addition channels 0 and 2 are used by the ARM software."
joan, https://www.raspberrypi.org/forums/viewtopic.php?f=32&t=86339

Update 2019-07-20:
- Channel 4 does not work anymore.
- /sys/module/dma/parameters/dmachans does not exist any more.
These changes are probably related to the Pi 4 release. This needs more
investigation.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_GpioPwm_HEADER_INCLUDED)
#define mfx_hw_GpioPwm_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/bcm2837.h"
#include "mfx/hw/bcm2837gpio.h"
#include "mfx/hw/bcm2837pwm.h"
#include "mfx/hw/MmapPtr.h"

#include <array>
#include <memory>

#include <cstdint>



namespace mfx
{
namespace hw
{



// We'll make it a singleton later
class GpioPwm
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_dma_chn       = 15;
	static const int  _min_subcycle_time = 3000; // Microseconds

	explicit       GpioPwm (int granularity);
	virtual        ~GpioPwm () = default;
	int            init_chn (int chn, int subcycle_time);
	void           clear (int chn);
	void           clear (int chn, int pin);
	void           set_pulse (int chn, int pin, int start, int width);
	void           add_pulse (int chn, int pin, int start, int width);
	float          set_multilevel (int chn, int pin, int nbr_cycles, int nbr_phases, int phase, float level);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class DmaCtrlBlock
	{
	public:
		uint32_t info;    // TI: transfer information
		uint32_t src;     // SOURCE_AD
		uint32_t dst;     // DEST_AD
		uint32_t length;  // TXFR_LEN: transfer length
		uint32_t stride;  // 2D stride mode
		uint32_t next;    // NEXTCONBK
		uint32_t pad [2]; // _reserved_
	};

	class Channel
	{
	public:

		class MBox
		{
		public:
			explicit       MBox (int size, int mem_flag);
			virtual        ~MBox ();
			void           cleanup ();
			int            _handle   = 0; // From mbox_open()
			uint32_t       _size     = 0; // Required size
			unsigned int   _mem_ref  = 0; // From mem_alloc()
			unsigned int   _bus_adr  = 0; // From mem_lock()
			uint8_t *      _virt_ptr = 0; // From mapmem()
		};

		explicit       Channel (int index, uint32_t periph_base_addr, uint32_t subcycle_time, int granularity);
		virtual        ~Channel ();

		DmaCtrlBlock & use_cb ();
		uint32_t       mem_virt_to_phys (void *virt_ptr);

		void           clear ();
		void           clear (int pin);
		void           add_pulse (int pin, int start, int width);
		void           set_pulse (int pin, int start, int width);
		float          set_multilevel (int pin, int nbr_cycles, int nbr_phases, int phase, float level);
		int            find_free_front_pos (int pin, int pos, bool up_flag, bool fwd_flag);

		static bool    is_gpio_ready (int gpio);
		static void    init_gpio (int pin, int gpio);

		int            _index;
		MmapPtr        _dma_reg;

		// Set by user
		uint32_t       _subcycle_time;

		// Set by system
		uint32_t       _nbr_samples;
		uint32_t       _nbr_cbs;
		uint32_t       _nbr_pages;

		MBox           _mbox;

		static uint32_t
		               _gpio_init;
	};
	typedef std::shared_ptr <Channel> ChannelSPtr;
	typedef std::array <ChannelSPtr, _nbr_dma_chn> ChannelArray;

	int            _granularity;   // Granularity in microseconds
	uint32_t       _periph_base_addr;  // Value depends on the Pi version
	MmapPtr        _reg_pwm;
	MmapPtr        _reg_clk;
	MmapPtr        _reg_gpio;
	ChannelArray   _chn_arr;

	// Standard page sizes
	static const int  PAGE_SHIFT = 12;
	static const int  PAGE_SIZE  = 1 << PAGE_SHIFT;
	static const int  PAGE_MASK  = PAGE_SIZE - 1;

	static const int  MEM_FLAGS  = 0x04;               // 0x0C for Pi 1.

	// Bus addresses
	static const uint32_t   _bus_gpclr0   =
		bcm2837::_bus_base + bcm2837gpio::_gpio_ofs + bcm2837gpio::_gpclr;
	static const uint32_t   _bus_gpset0   =
		bcm2837::_bus_base + bcm2837gpio::_gpio_ofs + bcm2837gpio::_gpset;
	static const uint32_t	_bus_fifo_adr =
		bcm2837::_bus_base + bcm2837pwm::_pwm_ofs   + bcm2837pwm::_fif1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               GpioPwm ()                               = delete;
	               GpioPwm (const GpioPwm &other)           = delete;
	GpioPwm &      operator = (const GpioPwm &other)        = delete;
	bool           operator == (const GpioPwm &other) const = delete;
	bool           operator != (const GpioPwm &other) const = delete;

}; // class GpioPwm



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/GpioPwm.hpp"



#endif   // mfx_hw_GpioPwm_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
