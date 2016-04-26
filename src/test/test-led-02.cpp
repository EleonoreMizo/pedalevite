
#include "mailbox.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <wiringPi.h>

#include <memory>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>



// We'll make it a singleton later
class GpioPwm
{
public:

	static const int  _nbr_dma_chn       = 15;
	static const int  _min_subcycle_time = 3000; // Microseconds

	explicit       GpioPwm (int granularity = 10);
	virtual        ~GpioPwm ();
	int            init_chn (int chn, int subcycle_time);
	void           clear (int chn);
	void           clear (int chn, int pin);
	void           set_pulse (int chn, int pin, int width_start, int width);
	void           add_pulse (int chn, int pin, int width_start, int width);

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
		void           add_pulse (int pin, int width_start, int width);
		void           set_pulse (int pin, int width_start, int width);

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
};

GpioPwm::GpioPwm (int granularity)
:	_granularity (granularity)
,	_reg_pwm (map_periph (VIRT_BASE + PWM_OFS, PWM_LEN))
,	_reg_clk (map_periph (VIRT_BASE + CLK_OFS, CLK_LEN))
,	_reg_gpio (map_periph (VIRT_BASE + GPIO_OFS, GPIO_LEN))
{
	_reg_pwm [PWM_CTL    ] = 0;
	::delayMicroseconds (10);
	_reg_clk [PWMCLK_CNTL] = 0x5A000006;        // Source = PLLD (500MHz)
	::delayMicroseconds (100);
	_reg_clk [PWMCLK_DIV ] = 0x5A000000 | (50 << 12);    // set pwm div to 50, giving 10MHz
	::delayMicroseconds (100);
	_reg_clk [PWMCLK_CNTL] = 0x5A000016;        // Source = PLLD and enable
	::delayMicroseconds (100);
	_reg_pwm [PWM_RNG1   ] = _granularity * 10;
	::delayMicroseconds (10);
	_reg_pwm [PWM_DMAC   ] = PWMDMAC_ENAB | PWMDMAC_THRSHLD;
	::delayMicroseconds (10);
	_reg_pwm [PWM_CTL    ] = PWMCTL_CLRF;
	::delayMicroseconds (10);
	_reg_pwm [PWM_CTL    ] = PWMCTL_USEF1 | PWMCTL_PWEN1;
	::delayMicroseconds (10);
}

GpioPwm::~GpioPwm ()
{
	for (int chn = 0; chn < _nbr_dma_chn; ++chn)
	{
		Channel &      channel = _chn_arr [chn];
		if (channel._mbox._virt_ptr != 0 && channel._dma_reg_ptr != 0)
		{
			channel.clear ();
			::delayMicroseconds (channel._subcycle_time);
			channel._dma_reg_ptr [DMA_CS] = DMA_RESET;
			::delayMicroseconds (10);
		}
	}
}

// Setup a channel with a specific subcycle time in microseconds.
// After that pulse-widths can be added at any time.
int	GpioPwm::init_chn (int chn, int subcycle_time)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);
	assert (subcycle_time >= _min_subcycle_time);

	Channel &      channel = _chn_arr [chn];
	assert (channel._mbox._virt_ptr == 0);

	channel._index         = chn;
	channel._subcycle_time = subcycle_time;
	channel._nbr_samples   =
		(channel._subcycle_time + (_granularity >> 1)) / _granularity;
	channel._nbr_cbs       = channel._nbr_samples * 2;
	channel._nbr_pages     = 
	   (   (channel._nbr_cbs * 32 + channel._nbr_samples * 4 + PAGE_SIZE - 1)
       >> PAGE_SHIFT);

	int            ret_val =
		channel._mbox.init (channel._nbr_pages * 4096, MEM_FLAGS);
	if (ret_val == 0)
	{
		ret_val = channel.init_ctrl_data ();
	}
	
	return ret_val;
}

void	GpioPwm::clear (int chn)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);

	Channel &      channel = _chn_arr [chn];
	channel.clear ();
}

void	GpioPwm::clear (int chn, int pin)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);

	Channel &      channel = _chn_arr [chn];
	channel.clear (pin);
}

void	GpioPwm::add_pulse (int chn, int pin, int start, int width)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);

	const int      start_spl = (start + (_granularity >> 1)) / _granularity;
	const int      width_spl = (width + (_granularity >> 1)) / _granularity;

	Channel &      channel = _chn_arr [chn];
	channel.add_pulse (pin, start_spl, width_spl);
}

void	GpioPwm::set_pulse (int chn, int pin, int start, int width)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);

	const int      start_spl = (start + (_granularity >> 1)) / _granularity;
	const int      width_spl = (width + (_granularity >> 1)) / _granularity;

	Channel &      channel = _chn_arr [chn];
	channel.set_pulse (pin, start_spl, width_spl);
}

uint32_t *	GpioPwm::map_periph (uint32_t base, uint32_t len)
{
	int            fd = open ("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
	{
		throw std::runtime_error ("Cannot open /dev/mem.\n");
	}

	void *         v_ptr =
		mmap (0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
	close (fd);

	if (v_ptr == MAP_FAILED)
	{
		throw std::runtime_error ("Cannot map peripheral.");
	}

	return reinterpret_cast <uint32_t *> (v_ptr);
}

// Initialize control block for this channel
int	GpioPwm::Channel::init_ctrl_data ()
{
	int            ret_val = 0;

	DmaCtrlBlock * cb0_ptr     = &use_cb ();
	uint32_t *     sample_ptr  = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	_dma_reg_ptr = map_periph (VIRT_BASE + DMA_OFS, DMA_LEN);
	if (_dma_reg_ptr == 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		_dma_reg_ptr += _index * (DMA_CHN_INC / sizeof (*_dma_reg_ptr));

		// Reset complete per-sample gpio mask to 0
		memset (sample_ptr, 0, sizeof (_nbr_samples * sizeof (*sample_ptr)));

		// For each sample we add 2 control blocks:
		// - first: clear gpio and jump to second
		// - second: jump to next CB
		DmaCtrlBlock * cb_ptr = cb0_ptr;
		for (int i = 0; i < _nbr_samples; ++ i)
		{
			const uint32_t phys_gpclr0 = PHYS_BASE + GPIO_OFS + 0x28;
			cb_ptr->info   = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP;
			cb_ptr->src    = mem_virt_to_phys (sample_ptr + i);  // src contains mask of which gpios need change at this sample
			cb_ptr->dst    = phys_gpclr0;  // set each sample to clear set gpios by default
			cb_ptr->length = 4;
			cb_ptr->stride = 0;
			cb_ptr->next   = mem_virt_to_phys (cb_ptr + 1);
			++ cb_ptr;

			// Delay
			const uint32_t phys_fifo_adr = PHYS_BASE + PWM_OFS + 0x18;
			cb_ptr->info   = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP | DMA_D_DREQ | (5 * DMA_PER_MAP);
			cb_ptr->src    = mem_virt_to_phys (sample_ptr); // Any data will do
			cb_ptr->dst    = phys_fifo_adr;
			cb_ptr->length = 4;
			cb_ptr->stride = 0;
			cb_ptr->next   = mem_virt_to_phys (cb_ptr + 1);
			++ cb_ptr;
		}

		// The last control block links back to the first (= endless loop)
		-- cb_ptr;
		cb_ptr->next = mem_virt_to_phys (cb0_ptr);

		// Initialize the DMA channel 0 (p46, 47)
		_dma_reg_ptr [DMA_CS       ] = DMA_RESET;         // DMA channel reset
		::delayMicroseconds (10);
		_dma_reg_ptr [DMA_CS       ] = DMA_INT | DMA_END; // Interrupt status & DMA end flag
		_dma_reg_ptr [DMA_CONBLK_AD] = mem_virt_to_phys (cb0_ptr);  // initial CB
		_dma_reg_ptr [DMA_DEBUG    ] = 7;                 // clear debug error flags
		_dma_reg_ptr [DMA_CS       ] = 0x10880001;        // go, mid priority, wait for outstanding writes
	}

	return ret_val;
}

uint32_t	GpioPwm::Channel::mem_virt_to_phys (void *virt_ptr)
{
	const uint32_t offset =
		reinterpret_cast <uint8_t *> (virt_ptr) - _mbox._virt_ptr;

	return _mbox._bus_adr + offset;
}

GpioPwm::DmaCtrlBlock & GpioPwm::Channel::use_cb ()
{
	GpioPwm::DmaCtrlBlock *  ptr = reinterpret_cast <DmaCtrlBlock *> (
		_mbox._virt_ptr + ((_nbr_samples + 7) & -8) * sizeof (uint32_t)
	);
	assert ((reinterpret_cast <uint32_t> (ptr) & 0x1F) == 0);
	
	return *ptr;
}

void	GpioPwm::Channel::clear ()
{
	assert (_mbox._virt_ptr != 0);

	const uint32_t phys_gpclr0 = PHYS_BASE + GPIO_OFS + 0x28;
	DmaCtrlBlock * cb_ptr      = &use_cb ();
	uint32_t *     d_ptr       = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	// First we have to stop all currently enabled pulses
	for (int i = 0; i < _nbr_samples; ++i)
	{
		cb_ptr [i * 2].dst = phys_gpclr0;
	}

	// Let DMA do one cycle to actually clear them
	::delayMicroseconds (_subcycle_time);

	// Finally set all samples to 0 (instead of gpio_mask)
	for (int i = 0; i < _nbr_samples; i++)
	{
		d_ptr [i] = 0;
	}
}

void	GpioPwm::Channel::clear (int pin)
{
	assert (_mbox._virt_ptr != 0);

	const int      gpio        = ::physPinToGpio (pin);
	const uint32_t phys_gpclr0 = PHYS_BASE + GPIO_OFS + 0x28;
	DmaCtrlBlock * cb_ptr      = &use_cb ();
	uint32_t *     d_ptr       = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	// Remove this gpio from all samples
	for (int i = 0; i < _nbr_samples; i++)
	{
		d_ptr [i] &= ~(1 << gpio);
	}

	::digitalWrite (pin, 0);
}

// Update the channel with another pulse within one full cycle. Its possible to
// add more gpios to the same timeslots (width_start). width_start and width are
// multiplied with pulse_width_incr_us to get the pulse width in microseconds [us].
//
// Be careful: if you try to set one GPIO to high and another one to low at the same
// point in time, only the last added action (eg. set-to-low) will be executed on all pins.
// To create these kinds of inverted signals on two GPIOs, either offset them by 1 step, or
// use multiple DMA channels.
void	GpioPwm::Channel::add_pulse (int pin, int start, int width)
{
	assert (_mbox._virt_ptr != 0);
	assert (width <= _nbr_samples);
	assert (start >= 0);
	assert (start < _nbr_samples);

	const int      gpio        = ::physPinToGpio (pin);
	const uint32_t phys_gpclr0 = PHYS_BASE + GPIO_OFS + 0x28;
	const uint32_t phys_gpset0 = PHYS_BASE + GPIO_OFS + 0x1C;

	if (! is_gpio_ready (gpio))
	{
		init_gpio (pin, gpio);
	}

	int            pos    = start;
	DmaCtrlBlock * cb_ptr = &use_cb ();
	uint32_t *     d_ptr  = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	bool           state_flag = false;
	for (int i = 0; i < width; ++i)
	{
		if (i == 0)
		{
			// Enable or disable gpio at this point in the cycle
			d_ptr [pos] |= 1 << gpio;
			cb_ptr [pos * 2].dst = phys_gpset0;
		}
		else
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				state_flag = (cb_ptr [pos * 2].dst == phys_gpset0);
			}
			d_ptr [pos] &= ~(1 << gpio);  // Set just this gpio's bit to 0
		}

		++ pos;
		if (pos >= _nbr_samples)
		{
			pos -= _nbr_samples;
		}
	}

	// Clear GPIO at end
	if (width < _nbr_samples && ! state_flag)
	{
		d_ptr [pos] |= 1 << gpio;
		cb_ptr [pos * 2].dst = phys_gpclr0;
	}
}

void	GpioPwm::Channel::set_pulse (int pin, int start, int width)
{
	assert (_mbox._virt_ptr != 0);
	assert (width <= _nbr_samples);
	assert (start >= 0);
	assert (start < _nbr_samples);

	const int      gpio        = ::physPinToGpio (pin);
	const uint32_t phys_gpclr0 = PHYS_BASE + GPIO_OFS + 0x28;
	const uint32_t phys_gpset0 = PHYS_BASE + GPIO_OFS + 0x1C;

	if (! is_gpio_ready (gpio))
	{
		init_gpio (pin, gpio);
	}

	int            pos    = start;
	DmaCtrlBlock * cb_ptr = &use_cb ();
	uint32_t *     d_ptr  = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	for (int i = 0; i < _nbr_samples; ++i)
	{
		if (i == 0 && width > 0)
		{
			// Enable or disable gpio at this point in the cycle
			cb_ptr [pos * 2].dst = phys_gpset0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i == width)
		{
			// Clear GPIO at end
			cb_ptr [pos * 2].dst = phys_gpclr0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i > width)
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				cb_ptr [pos * 2].dst = phys_gpclr0;
			}
		}
		else
		{
			d_ptr [pos] &= ~(1 << gpio);  // Set just this gpio's bit to 0
		}

		++ pos;
		if (pos >= _nbr_samples)
		{
			pos -= _nbr_samples;
		}
	}
}

bool	GpioPwm::Channel::is_gpio_ready (int gpio)
{
	return ((_gpio_init & (1 << gpio)) != 0);
}

void	GpioPwm::Channel::init_gpio (int pin, int gpio)
{
	::pinMode (pin, OUTPUT);
	::digitalWrite (pin, 0);
	_gpio_init |= 1 << gpio;
}

GpioPwm::Channel::MBox::~MBox ()
{
	if (_virt_ptr != 0)
	{
		unmapmem (_virt_ptr, _size);
		mem_unlock (_handle, _mem_ref);
		mem_free (_handle, _mem_ref);
		if (_handle >= 0)
		{
			mbox_close (_handle);
		}
	}
}

int	GpioPwm::Channel::MBox::init (int size, int mem_flag)
{
	int            ret_val = 0;

	// Use the mailbox interface to request memory from the VideoCore
	// We specifiy (-1) for the handle rather than calling mbox_open()
	// so multiple users can share the resource.
	_handle = -1; // mbox_open();
	_size = size;
	_mem_ref = mem_alloc (_handle, _size, 4096, mem_flag);
	if (_mem_ref < 0)
	{
		printf ("Failed to alloc memory from VideoCore\n");
		ret_val = -1;
	}
	_bus_adr = mem_lock (_handle, _mem_ref);
	if (_bus_adr == ~0)
	{
		mem_free (_handle, _size);
		printf ("Failed to lock memory\n");
		ret_val = -1;
	}
	_virt_ptr = reinterpret_cast <uint8_t *> (
		mapmem (_bus_adr & 0x3FFFFFFF, _size)
	);
	
	return ret_val;
}

uint32_t	GpioPwm::Channel::_gpio_init = 0;



static std::unique_ptr <GpioPwm> MAIN_pwm_uptr;

static const int  MAIN_nbr_led = 3;
static const int  MAIN_led_pin_arr [MAIN_nbr_led] = { 11, 13, 15};



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



int main (int argc, char *argv [])
{
	::wiringPiSetupPhys ();

	for (int i = 0; i < MAIN_nbr_led; ++i)
	{
		::pinMode  (MAIN_led_pin_arr [i], OUTPUT);
		::digitalWrite (MAIN_led_pin_arr [i], LOW);
	}

	GpioPwm        pwm (100); // Resolution: 0.1 ms
	int            ret_val = pwm.init_chn (0, 10 * 1000); // Cycle: 10 ms
	if (ret_val != 0)
	{
		printf ("Error while initializing the PWM\n");
	}

	int            active = 0;
	for (int i = 0; i < 5 && ret_val == 0; ++i)
	{
		for (int k = 0; k < 200; ++k)
		{
			const int       w = 100 - std::abs (k - 100); // Width in 0.1 ms steps
			pwm.set_pulse (0, MAIN_led_pin_arr [active], 0, w * 100);
			::delayMicroseconds (10 * 1000);
		}
		pwm.clear (0, MAIN_led_pin_arr [active]);

		active = (active + 1) % MAIN_nbr_led;
	}

	return ret_val;
}
