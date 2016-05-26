/*****************************************************************************

        GpioPwm.cpp
        Copyright (c) 2016 Ohm Force

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/GpioPwm.h"

#include <mailbox.h>

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



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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
		for (int i = 0; i < int (_nbr_samples); ++ i)
		{
			cb_ptr->info   = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP;
			cb_ptr->src    = mem_virt_to_phys (sample_ptr + i);  // src contains mask of which gpios need change at this sample
			cb_ptr->dst    = _phys_gpclr0; // set each sample to clear set gpios by default
			cb_ptr->length = 4;
			cb_ptr->stride = 0;
			cb_ptr->next   = mem_virt_to_phys (cb_ptr + 1);
			++ cb_ptr;

			// Delay
			cb_ptr->info   = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP | DMA_D_DREQ | (5 * DMA_PER_MAP);
			cb_ptr->src    = mem_virt_to_phys (sample_ptr); // Any data will do
			cb_ptr->dst    = _phys_fifo_adr;
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

	DmaCtrlBlock * cb_ptr      = &use_cb ();
	uint32_t *     d_ptr       = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	// First we have to stop all currently enabled pulses
	for (int i = 0; i < int (_nbr_samples); ++i)
	{
		cb_ptr [i * 2].dst = _phys_gpclr0;
	}

	// Let DMA do one cycle to actually clear them
	::delayMicroseconds (_subcycle_time);

	// Finally set all samples to 0 (instead of gpio_mask)
	for (int i = 0; i < int (_nbr_samples); i++)
	{
		d_ptr [i] = 0;
	}
}



void	GpioPwm::Channel::clear (int pin)
{
	assert (_mbox._virt_ptr != 0);

	const int      gpio  = ::physPinToGpio (pin);
	uint32_t *     d_ptr = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	// Remove this gpio from all samples
	for (int i = 0; i < int (_nbr_samples); i++)
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
	assert (width <= int (_nbr_samples));
	assert (start >= 0);
	assert (start < int (_nbr_samples));

	const int      gpio = ::physPinToGpio (pin);
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
			cb_ptr [pos * 2].dst = _phys_gpset0;
		}
		else
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				state_flag = (cb_ptr [pos * 2].dst == _phys_gpset0);
			}
			d_ptr [pos] &= ~(1 << gpio);  // Set just this gpio's bit to 0
		}

		++ pos;
		if (pos >= int (_nbr_samples))
		{
			pos -= _nbr_samples;
		}
	}

	// Clear GPIO at end
	if (width < int (_nbr_samples) && ! state_flag)
	{
		d_ptr [pos] |= 1 << gpio;
		cb_ptr [pos * 2].dst = _phys_gpclr0;
	}
}



void	GpioPwm::Channel::set_pulse (int pin, int start, int width)
{
	assert (_mbox._virt_ptr != 0);
	assert (width <= int (_nbr_samples));
	assert (start >= 0);
	assert (start < int (_nbr_samples));

	const int      gpio = ::physPinToGpio (pin);
	if (! is_gpio_ready (gpio))
	{
		init_gpio (pin, gpio);
	}

	int            pos    = start;
	DmaCtrlBlock * cb_ptr = &use_cb ();
	uint32_t *     d_ptr  = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	for (int i = 0; i < int (_nbr_samples); ++i)
	{
		if (i == 0 && width > 0)
		{
			// Enable or disable gpio at this point in the cycle
			cb_ptr [pos * 2].dst = _phys_gpset0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i == width && width > 0)
		{
			// Clear GPIO at end
			cb_ptr [pos * 2].dst = _phys_gpclr0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i > width)
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				cb_ptr [pos * 2].dst = _phys_gpclr0;
			}
		}
		else
		{
			d_ptr [pos] &= ~(1 << gpio);  // Set just this gpio's bit to 0
		}

		++ pos;
		if (pos >= int (_nbr_samples))
		{
			pos -= _nbr_samples;
		}
	}

	if (width == 0)
	{
		::digitalWrite (pin, 0);
	}
}



// === Function not tested ===
//
// Generates non-overlapping waveforms
// Maximum duty cycle of indidividual pins depends on the number of
// phases (below 1 / nbr_phase)
// level is related to the maximum duty cycle achievable.
// Some phases may have a larger maximum duty cycle than others.
// It is possible to generate multiple pulses per DMA cycle, but
// the more pulses, the shorter the maximum duty cycle (because
// rising and falling fronts cannot occur at the same time), and
// the greater the resulting error.
// This function is not compatible with the set_pulse() function.
// For a given DMA channel, this function must be called with the
// same "physical" parameters (nbr_cycles, nbr_phases).
// Returns the level error.
float	GpioPwm::Channel::set_multilevel (int pin, int nbr_cycles, int nbr_phases, int phase, float level)
{
	assert (_mbox._virt_ptr != 0);
	assert (nbr_cycles > 0);
	assert (nbr_cycles * nbr_phases * 2 <= int (_nbr_samples));
	assert (nbr_phases > 0);
	assert (phase >= 0);
	assert (phase < nbr_phases);
	assert (level >= 0);
	assert (level <= 1);

	const int      gpio = ::physPinToGpio (pin);
	if (! is_gpio_ready (gpio))
	{
		init_gpio (pin, gpio);
	}

	const int      nbr_pulses     = nbr_cycles * nbr_phases;
	const int      max_duty_cycle = _nbr_samples - nbr_pulses;
	const float    duty_per_spl   = float (nbr_phases) / max_duty_cycle;

	int            pulse_val   = 0;
	int            phase_cur   = 0;
	int            pos         = 0;
	float          duty        = 0;
	bool           active_flag = false;
	bool				pulse_slot_end_flag = false;
	while (pos < int (_nbr_samples))
	{
		const bool     need_active_flag = (duty + duty_per_spl * 0.5 < level);
		uint32_t       set_or_clear = _phys_gpclr0;

		// Beginning or end of a pulse slot?
		const bool     trans_flag = (pulse_val < nbr_pulses);
		if (trans_flag)
		{
			// End
			if (pulse_slot_end_flag)
			{
				d_ptr [pos] |= 1 << gpio;
				pulse_slot_end_flag = false;
				if (active_flag)
				{
					duty -= level;
					active_flag = false;
				}
			}

			// Beginning
			else
			{
				set_or_clear = _phys_gpset0;
				if (phase_cur == phase && need_active_flag)
				{
					d_ptr [pos] |= 1 << gpio;
					active_flag = true;
				}
				else
				{
					d_ptr [pos] &= ~(1 << gpio);
					active_flag = false;
				}
			}
		}
		else
		{
			if (active_flag)
			{
				if (need_active_flag)
				{
					d_ptr [pos] &= ~(1 << gpio);
					duty += duty_per_spl;
				}
				else
				{
					d_ptr [pos] |= 1 << gpio;
					duty -= level;
					active_flag = false;
				}
			}
			else
			{
				d_ptr [pos] |= 1 << gpio;
			}
		}

		cb_ptr [pos * 2].dst = set_or_clear;

		// Next sample
		if (! pulse_slot_end_flag)
		{
			pulse_val += nbr_pulses;
			if (pulse_val >= max_duty_cycle)
			{
				pulse_val -= max_duty_cycle;
				pulse_slot_end_flag = true;
				++ phase_cur;
				if (phase_cur >= nbr_phases)
				{
					phase_cur = 0;
				}
			}
		}
		++ pos;
	}

	if (level == 0)
	{
		::digitalWrite (pin, 0);
	}

	return duty / nbr_cycles;
}



void	GpioPwm::Channel::find_free_front_pos (int pin, int pos, bool up_flag, bool fwd_flag) const
{
	const int      gpio = ::physPinToGpio (pin);
	const DmaCtrlBlock * cb_ptr = &use_cb ();
	const uint32_t *     d_ptr  =
		reinterpret_cast <const uint32_t *> (_mbox._virt_ptr);

	const int      dir   = fwd_flag ? 1 : -1;
	const int      avoid = up_flag ? _phys_gpclr0 : _phys_gpset0;
	while (   cb_ptr [pos * 2].dst == avoid
		    && (d_ptr [pos] & ~(1 << gpio)) != 0)
	{
		pos += dir;
		if (pos >= int (_nbr_samples))
		{
			pos -= _nbr_samples;
		}
		else if (pos < 0)
		{
			pos += _nbr_samples;
		}
	}

	return pos;
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
	if (_mem_ref == static_cast <unsigned int> (-1))
	{
		printf ("Failed to alloc memory from VideoCore\n");
		ret_val = -1;
	}
	_bus_adr = mem_lock (_handle, _mem_ref);
	if (_bus_adr == static_cast <unsigned int> (~0))
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



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
