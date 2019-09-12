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

#include <array>

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
	virtual        ~GpioPwm ();
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
			virtual        ~MBox ();
			int            init (int size, int mem_flag);
			int            _handle   = 0; // From mbox_open()
			uint32_t       _size     = 0; // Required size
			unsigned int   _mem_ref  = 0; // From mem_alloc()
			unsigned int   _bus_adr  = 0; // From mem_lock()
			uint8_t *      _virt_ptr = 0; // From mapmem()
		};

		int            init_virtbase ();
		int            init_ctrl_data (uint32_t periph_base_addr);
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

		int            _index         = -1;
		MBox           _mbox;
		uint32_t *     _sample_ptr    = 0;
		DmaCtrlBlock * _cb_ptr        = 0;
		volatile uint32_t *
		               _dma_reg_ptr   = 0;

		// Set by user
		uint32_t       _subcycle_time = 0;

		// Set by system
		uint32_t       _nbr_samples   = 0;
		uint32_t       _nbr_cbs       = 0;
		uint32_t       _nbr_pages     = 0;

		static uint32_t
		               _gpio_init;
	};

	static uint32_t *
	                map_periph (uint32_t base, uint32_t len);

	int            _granularity = 10;   // Granularity in microseconds
	uint32_t       _periph_base_addr = 0x3F000000;  // 0x20000000 on Pi 1.
	volatile uint32_t *
						_reg_pwm = 0;
	volatile uint32_t *
						_reg_clk = 0;
	volatile uint32_t *
	               _reg_gpio = 0;
	std::array <Channel, _nbr_dma_chn>
	               _chn_arr;

	// Standard page sizes
	static const int  PAGE_SHIFT = 12;
	static const int  PAGE_SIZE  = 1 << PAGE_SHIFT;
	static const int  PAGE_MASK  = PAGE_SIZE - 1;

	static const int  MEM_FLAGS  = 0x04;               // 0x0C for Pi 1.

	// Base addresses
	static const uint32_t   PHYS_BASE   = 0x7E000000;

	// Memory Addresses
	static const uint32_t   DMA_OFS     = 0x00007000;
	static const int        DMA_LEN     = 0x24;
	static const int        DMA_CHN_INC = 0x100;
	static const uint32_t   PWM_OFS     = 0x0020C000;
	static const int        PWM_LEN     = 0x28;
	static const uint32_t   CLK_OFS     = 0x00101000;
	static const int        CLK_LEN     = 0xA8;
	static const int        GPIO_OFS    = 0x00200000;
	static const int        GPIO_LEN    = 0x100;

	static const uint32_t   _phys_gpclr0   = PHYS_BASE + GPIO_OFS + 0x28;
	static const uint32_t   _phys_gpset0   = PHYS_BASE + GPIO_OFS + 0x1C;
	static const uint32_t	_phys_fifo_adr = PHYS_BASE + PWM_OFS  + 0x18;

	// Datasheet p. 51:
	static const uint32_t   DMA_NO_WIDE_BURSTS = 1 << 26;
	static const uint32_t   DMA_WAIT_RESP      = 1 <<  3;
	static const uint32_t   DMA_D_DREQ         = 1 <<  6;
	static const uint32_t   DMA_PER_MAP        = 1 << 16;
	static const uint32_t   DMA_END            = 1 <<  1;
	static const uint32_t   DMA_RESET          = 1 << 31;
	static const uint32_t   DMA_INT            = 1 <<  2;

	// Each DMA channel has 3 writeable registers:
	static const int  DMA_CS        = 0x00 / sizeof (uint32_t);
	static const int  DMA_CONBLK_AD = 0x04 / sizeof (uint32_t);
	static const int  DMA_SOURCE_AD = 0x0C / sizeof (uint32_t);
	static const int  DMA_DEBUG     = 0x20 / sizeof (uint32_t);

	// PWM Memory Addresses
	static const int  PWM_CTL       = 0x00 / sizeof (uint32_t);
	static const int  PWM_DMAC      = 0x08 / sizeof (uint32_t);
	static const int  PWM_RNG1      = 0x10 / sizeof (uint32_t);
	static const int  PWM_FIFO      = 0x18 / sizeof (uint32_t);

	static const int  PWMCLK_CNTL   = 40;
	static const int  PWMCLK_DIV    = 41;

	static const uint32_t   PWMCTL_MODE1    = 1 << 1;
	static const uint32_t   PWMCTL_PWEN1    = 1 << 0;
	static const uint32_t   PWMCTL_CLRF     = 1 << 6;
	static const uint32_t   PWMCTL_USEF1    = 1 << 5;

	static const uint32_t   PWMDMAC_ENAB    = 1 << 31;
	static const uint32_t   PWMDMAC_THRSHLD = (15 << 8) | (15 << 0);



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
