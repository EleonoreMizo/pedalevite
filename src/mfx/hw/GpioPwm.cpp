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
#include "mfx/hw/GpioPwm.h"

#include <bcm_host.h>
#include <mailbox.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <wiringPi.h>

#include <algorithm>
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
	catch (MBox::Error &err)
	{
		ret_val = Err_MAILBOX;
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
		index * bcm2837dma::_dma_chn_inc + bcm2837dma::_dma_chn_len,
		"/dev/mem", O_RDWR | O_SYNC
	)
,	_subcycle_time (subcycle_time)
,	_nbr_samples ((_subcycle_time + (granularity >> 1)) / granularity)
,	_dma (_nbr_samples * _nbr_blk_per_spl, _nbr_samples * sizeof (uint32_t))
{
	assert (periph_base_addr != 0);
	assert (index >= 0);

	uint32_t *     sample_ptr  = _dma.use_buf <uint32_t> ();

	// Reset complete per-sample gpio mask to 0
	std::fill (sample_ptr, sample_ptr + _nbr_samples, 0);

	// For each sample we add 2 control blocks:
	// - first: clear gpio and jump to second
	// - second: jump to next CB
	for (int i = 0; i < int (_nbr_samples); ++ i)
	{
		DmaCtrlBlock & cb0 = _dma.use_cb (i * _nbr_blk_per_spl);
		cb0._info   =
		     bcm2837dma::_no_wide_b
			| bcm2837dma::_wait_resp;
		cb0._src    = _dma.virt_to_phys (sample_ptr + i);  // src contains mask of which gpios need change at this sample
		cb0._dst    = _bus_gpclr0; // set each sample to clear set gpios by default
		cb0._length = 4;
		cb0._stride = 0;
		memset (cb0._pad, 0, sizeof (cb0._pad));

		cb0._next   =
			_dma.virt_to_phys (&_dma.use_cb (i * _nbr_blk_per_spl + 1));

		// Delay
		DmaCtrlBlock & cb1 = _dma.use_cb (i * _nbr_blk_per_spl + 1);
		cb1._info   =
		     bcm2837dma::_no_wide_b
			| bcm2837dma::_wait_resp
			| bcm2837dma::_dest_dreq
			| (bcm2837dma::Dreq_PWM << bcm2837dma::_permap);
		cb1._src    = _dma.virt_to_phys (sample_ptr); // Any data will do
		cb1._dst    = _bus_fifo_adr;
		cb1._length = 4;
		cb1._stride = 0;
		memset (cb1._pad, 0, sizeof (cb1._pad));

		// The last control block links back to the first (= endless loop)
		cb1._next   = _dma.virt_to_phys (
			&_dma.use_cb (((i + 1) % _nbr_samples) * _nbr_blk_per_spl)
		);
	}

	// Initialize the DMA channel (p46, 47)
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_cs       ) =
		bcm2837dma::_reset;
	::delayMicroseconds (10);
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_cs       ) =
		bcm2837dma::_int | bcm2837dma::_end;
	_dma_reg.at (_index * bcm2837dma::_dma_chn_inc + bcm2837dma::_conblk_ad) =
		_dma.virt_to_phys (&_dma.use_cb (0));
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



void	GpioPwm::Channel::clear ()
{
	DmaCtrlBlock * cb_ptr = &_dma.use_cb (0);
	uint32_t *     d_ptr  = _dma.use_buf <uint32_t> ();

	// First we have to stop all currently enabled pulses
	for (int i = 0; i < int (_nbr_samples); ++i)
	{
		cb_ptr [i * _nbr_blk_per_spl]._dst = _bus_gpclr0;
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
	const int      gpio  = ::physPinToGpio (pin);
	uint32_t *     d_ptr = _dma.use_buf <uint32_t> ();

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
	assert (width <= int (_nbr_samples));
	assert (start >= 0);
	assert (start < int (_nbr_samples));

	const int      gpio = ::physPinToGpio (pin);
	if (! is_gpio_ready (gpio))
	{
		init_gpio (pin, gpio);
	}

	int            pos    = start;
	DmaCtrlBlock * cb_ptr = &_dma.use_cb (0);
	uint32_t *     d_ptr  = _dma.use_buf <uint32_t> ();

	bool           state_flag = false;
	for (int i = 0; i < width; ++i)
	{
		if (i == 0)
		{
			// Enable or disable gpio at this point in the cycle
			d_ptr [pos] |= 1 << gpio;
			cb_ptr [pos * _nbr_blk_per_spl]._dst = _bus_gpset0;
		}
		else
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				state_flag = (cb_ptr [pos * _nbr_blk_per_spl]._dst == _bus_gpset0);
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
		cb_ptr [pos * _nbr_blk_per_spl]._dst = _bus_gpclr0;
	}
}



void	GpioPwm::Channel::set_pulse (int pin, int start, int width)
{
	assert (width <= int (_nbr_samples));
	assert (start >= 0);
	assert (start < int (_nbr_samples));

	const int      gpio = ::physPinToGpio (pin);
	if (! is_gpio_ready (gpio))
	{
		init_gpio (pin, gpio);
	}

	int            pos    = start;
	DmaCtrlBlock * cb_ptr = &_dma.use_cb (0);
	uint32_t *     d_ptr  = _dma.use_buf <uint32_t> ();

	for (int i = 0; i < int (_nbr_samples); ++i)
	{
		if (i == 0 && width > 0)
		{
			// Enable or disable gpio at this point in the cycle
			cb_ptr [pos * _nbr_blk_per_spl]._dst = _bus_gpset0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i == width && width > 0)
		{
			// Clear GPIO at end
			cb_ptr [pos * _nbr_blk_per_spl]._dst = _bus_gpclr0;
			d_ptr [pos] |= 1 << gpio;
		}
		else if (i > width)
		{
			if ((d_ptr [pos] & (1 << gpio)) != 0)
			{
				cb_ptr [pos * _nbr_blk_per_spl]._dst = _bus_gpclr0;
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

	DmaCtrlBlock * cb_ptr = &_dma.use_cb (0);
	uint32_t *     d_ptr  = _dma.use_buf <uint32_t> ();

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

		cb_ptr [pos * _nbr_blk_per_spl]._dst = set_or_clear;

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
	const DmaCtrlBlock * cb_ptr = &_dma.use_cb (0);
	const uint32_t *     d_ptr  = _dma.use_buf <const uint32_t> ();

	const int      dir   = fwd_flag ? 1 : -1;
	const uint32_t avoid = up_flag ? _bus_gpclr0 : _bus_gpset0;
	while (   cb_ptr [pos * _nbr_blk_per_spl]._dst == avoid
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



uint32_t	GpioPwm::Channel::_gpio_init = 0;



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
