/*****************************************************************************

        GpioPwm.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/bcm2837clk.h"
#include "mfx/hw/bcm2837dma.h"
#include "mfx/hw/GpioPwm.h"

#include <bcm_host.h>
#include <mailbox.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <wiringPi.h>

#include <memory>
#include <stdexcept>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <ctime>



namespace mfx
{
namespace hw
{




/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



GpioPwm::GpioPwm (int granularity)
:	_granularity (granularity)
,	_periph_base_addr (::bcm_host_get_peripheral_address ())
,	_reg_pwm (
		_periph_base_addr + bcm2837pwm::_pwm_ofs,
		bcm2837pwm::_pwm_len, "/dev/mem", O_RDWR | O_SYNC
	)
,	_reg_clk (
		_periph_base_addr + bcm2837clk::_clk_ofs,
		bcm2837clk::_clk_len, "/dev/mem", O_RDWR | O_SYNC
	)
,	_reg_gpio (
		_periph_base_addr + bcm2837gpio::_gpio_ofs,
		bcm2837gpio::_gpio_len, "/dev/mem", O_RDWR | O_SYNC
	)
{
	// RPi 4 (BCM 2711) has different PLLD clock values (750 MHz instead of 500 MHz)
	const bool     bcm2711_flag = (_periph_base_addr == bcm2837::_phys_base_2711);
	const int      plld_freq    =
		(bcm2711_flag) ? bcm2837::_plld_freq_2711 : bcm2837::_plld_freq_2835;

	// PWM divisor to obtain 10 MHz
	const int      target_freq  = 10 * 1000 * 1000; // 10 MHz
	const int      clk_div      = plld_freq / target_freq;

	_reg_pwm.at (bcm2837pwm::_ctl   ) = 0;
	::delayMicroseconds (10);
	_reg_clk.at (bcm2837clk::_pwmctl) = // Source = PLLD (500 or 750 MHz)
		bcm2837clk::_passwd | bcm2837clk::_src_plld;
	::delayMicroseconds (100);
	_reg_clk.at (bcm2837clk::_pwmdiv) =
		bcm2837clk::_passwd | (clk_div << bcm2837clk::_divi);
	::delayMicroseconds (100);
	_reg_clk.at (bcm2837clk::_pwmctl) =
		bcm2837clk::_passwd | bcm2837clk::_src_plld | bcm2837clk::_enab; // Source = PLLD and enable
	::delayMicroseconds (100);
	_reg_pwm.at (bcm2837pwm::_rng1  ) = _granularity * 10;
	::delayMicroseconds (10);
	_reg_pwm.at (bcm2837pwm::_dmac  ) =
		bcm2837pwm::_enab | (15 << bcm2837pwm::_panic) | (15 << bcm2837pwm::_dreq);
	::delayMicroseconds (10);
	_reg_pwm.at (bcm2837pwm::_ctl   ) = bcm2837pwm::_clrf1;
	::delayMicroseconds (10);
	_reg_pwm.at (bcm2837pwm::_ctl   ) = bcm2837pwm::_usef1 | bcm2837pwm::_pwen1;
	::delayMicroseconds (10);
}



// Setup a channel with a specific subcycle time in microseconds.
// After that pulse-widths can be added at any time.
int	GpioPwm::init_chn (int chn, int subcycle_time)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);
	assert (subcycle_time >= _min_subcycle_time);

	int            ret_val = Err_OK;

	assert (_chn_arr [chn].get () == nullptr);

	try
	{
		_chn_arr [chn] = ChannelSPtr (new Channel (
			chn, _periph_base_addr, subcycle_time, _granularity
		));
	}
	catch (const Err &err)
	{
		ret_val = int (err);
	}
	catch (MmapPtr::Error &err)
	{
		ret_val = Err_MMAP;
	}
	catch (...)
	{
		ret_val = Err_GENERIC;
	}

	return ret_val;
}



void	GpioPwm::clear (int chn)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);
	assert (_chn_arr [chn].get () != nullptr);

	Channel &      channel = *(_chn_arr [chn]);
	channel.clear ();
}



void	GpioPwm::clear (int chn, int pin)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);
	assert (_chn_arr [chn].get () != nullptr);

	Channel &      channel = *(_chn_arr [chn]);
	channel.clear (pin);
}



void	GpioPwm::add_pulse (int chn, int pin, int start, int width)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);
	assert (_chn_arr [chn].get () != nullptr);

	const int      start_spl = (start + (_granularity >> 1)) / _granularity;
	const int      width_spl = (width + (_granularity >> 1)) / _granularity;

	Channel &      channel   = *(_chn_arr [chn]);
	channel.add_pulse (pin, start_spl, width_spl);
}



void	GpioPwm::set_pulse (int chn, int pin, int start, int width)
{
	assert (chn >= 0);
	assert (chn < _nbr_dma_chn);
	assert (_chn_arr [chn].get () != nullptr);

	const int      start_spl = (start + (_granularity >> 1)) / _granularity;
	const int      width_spl = (width + (_granularity >> 1)) / _granularity;

	Channel &      channel   = *(_chn_arr [chn]);
	channel.set_pulse (pin, start_spl, width_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



GpioPwm::Channel::Channel (int index, uint32_t periph_base_addr, uint32_t subcycle_time, int granularity)
:	_index (index)
,	_dma_reg (
		periph_base_addr + bcm2837dma::_dma_ofs,
		bcm2837dma::_dma_chn_len, "/dev/mem", O_RDWR | O_SYNC
	)
,	_subcycle_time (subcycle_time)
,	_nbr_samples ((_subcycle_time + (granularity >> 1)) / granularity)
,	_nbr_cbs (_nbr_samples * 2)
,	_nbr_pages ((   (_nbr_cbs * 32 + _nbr_samples * 4 + PAGE_SIZE - 1)
	             >> PAGE_SHIFT))
,	_mbox (_nbr_pages << PAGE_SHIFT, MEM_FLAGS)
{
	assert (periph_base_addr != 0);
	assert (index >= 0);

	DmaCtrlBlock * cb0_ptr     = &use_cb ();
	uint32_t *     sample_ptr  = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

	// Reset complete per-sample gpio mask to 0
	memset (sample_ptr, 0, sizeof (_nbr_samples * sizeof (*sample_ptr)));

	// For each sample we add 2 control blocks:
	// - first: clear gpio and jump to second
	// - second: jump to next CB
	DmaCtrlBlock * cb_ptr = cb0_ptr;
	for (int i = 0; i < int (_nbr_samples); ++ i)
	{
		cb_ptr->info   =
			  bcm2837dma::_no_wide_b
			| bcm2837dma::_wait_resp;
		cb_ptr->src    = mem_virt_to_phys (sample_ptr + i);  // src contains mask of which gpios need change at this sample
		cb_ptr->dst    = _bus_gpclr0; // set each sample to clear set gpios by default
		cb_ptr->length = 4;
		cb_ptr->stride = 0;
		cb_ptr->next   = mem_virt_to_phys (cb_ptr + 1);
		++ cb_ptr;

		// Delay
		cb_ptr->info   =
			  bcm2837dma::_no_wide_b
			| bcm2837dma::_wait_resp
			| bcm2837dma::_dest_dreq
			| (bcm2837dma::Dreq_PWM << bcm2837dma::_permap);
		cb_ptr->src    = mem_virt_to_phys (sample_ptr); // Any data will do
		cb_ptr->dst    = _bus_fifo_adr;
		cb_ptr->length = 4;
		cb_ptr->stride = 0;
		cb_ptr->next   = mem_virt_to_phys (cb_ptr + 1);
		++ cb_ptr;
	}

	// The last control block links back to the first (= endless loop)
	-- cb_ptr;
	cb_ptr->next = mem_virt_to_phys (cb0_ptr);

	// Initialize the DMA channel 0 (p46, 47)
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_cs       ) =
		bcm2837dma::_reset;
	::delayMicroseconds (10);
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_cs       ) =
		bcm2837dma::_int | bcm2837dma::_end;
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_conblk_ad) =
		mem_virt_to_phys (cb0_ptr);
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_debug    ) =
		bcm2837dma::_all_errors; // Clears errors
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_cs       ) =
		  bcm2837dma::_waitfow
		| (8 << bcm2837dma::_panic_prio)
		| (8 << bcm2837dma::_priority)
		| bcm2837dma::_active;
}




GpioPwm::Channel::~Channel ()
{
	clear ();
	::delayMicroseconds (_subcycle_time);
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_cs) =
		bcm2837dma::_reset;
	::delayMicroseconds (10);
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
		cb_ptr [i * 2].dst = _bus_gpclr0;
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
			cb_ptr [pos * 2].dst = _bus_gpset0;
		}
		else
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				state_flag = (cb_ptr [pos * 2].dst == _bus_gpset0);
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
		cb_ptr [pos * 2].dst = _bus_gpclr0;
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
			cb_ptr [pos * 2].dst = _bus_gpset0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i == width && width > 0)
		{
			// Clear GPIO at end
			cb_ptr [pos * 2].dst = _bus_gpclr0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i > width)
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				cb_ptr [pos * 2].dst = _bus_gpclr0;
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

	DmaCtrlBlock * cb_ptr = &use_cb ();
	uint32_t *     d_ptr  = reinterpret_cast <uint32_t *> (_mbox._virt_ptr);

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
		uint32_t       set_or_clear = _bus_gpclr0;

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
				set_or_clear = _bus_gpset0;
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



int	GpioPwm::Channel::find_free_front_pos (int pin, int pos, bool up_flag, bool fwd_flag)
{
	const int      gpio = ::physPinToGpio (pin);
	const DmaCtrlBlock * cb_ptr = &use_cb ();
	const uint32_t *     d_ptr  =
		reinterpret_cast <const uint32_t *> (_mbox._virt_ptr);

	const int      dir   = fwd_flag ? 1 : -1;
	const uint32_t avoid = up_flag ? _bus_gpclr0 : _bus_gpset0;
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



// Use the mailbox interface to request memory from the VideoCore
// We specifiy (-1) for the handle rather than calling mbox_open()
// so multiple users can share the resource.
GpioPwm::Channel::MBox::MBox (int size, int mem_flag)
:	_handle (-1) // mbox_open()
,	_size (size)
,	_mem_ref (mem_alloc (_handle, _size, 4096, mem_flag))
,	_bus_adr (0)
,	_virt_ptr (0)
{
	try
	{
		if (_mem_ref == static_cast <unsigned int> (-1))
		{
			throw Err_MEM_ALLOC; // Failed to alloc memory from VideoCore
		}
		_bus_adr = mem_lock (_handle, _mem_ref);
		if (_bus_adr == static_cast <unsigned int> (~0))
		{
			throw Err_MEM_LOCK;  // Failed to lock memory
		}
		_virt_ptr = reinterpret_cast <uint8_t *> (
			mapmem (_bus_adr & 0x3FFFFFFF, _size, DEV_MEM)
		);
		if (_virt_ptr == 0)
		{
			throw Err_MAPMEM;   // Cannot use the mailbox interface
		}
	}
	catch (...)
	{
		cleanup ();
		throw;
	}
}



GpioPwm::Channel::MBox::~MBox ()
{
	cleanup ();
}



void	GpioPwm::Channel::MBox::cleanup ()
{
	if (_mem_ref != 0 && _mem_ref != static_cast <unsigned int> (-1))
	{
		if (_virt_ptr != 0)
		{
			unmapmem (_virt_ptr, _size);
			_virt_ptr = 0;
		}
		mem_unlock (_handle, _mem_ref);
		mem_free (_handle, _mem_ref);
		_mem_ref = 0;
	}

	if (_handle >= 0)
	{
		mbox_close (_handle);
		_handle = 0;
	}
}



uint32_t	GpioPwm::Channel::_gpio_init = 0;



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
