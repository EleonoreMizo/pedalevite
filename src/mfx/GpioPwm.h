/*****************************************************************************

        GpioPwm.h
        Copyright (c) 2016 Ohm Force

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_GpioPwm_HEADER_INCLUDED)
#define mfx_GpioPwm_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>

#include <cstdint>



namespace mfx
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
		int            init_ctrl_data ();
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

	// Peripheral base addresses
	static const uint32_t   VIRT_BASE   = 0x3F000000;  // 0x20000000 for Pi 1.
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



}  // namespace mfx



//#include "mfx/GpioPwm.hpp"



#endif   // mfx_GpioPwm_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
