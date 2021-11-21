/*****************************************************************************

        DPvabDirect.cpp
        Author: Laurent de Soras, 2019

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



// Enables use of SIMD instructions in the buffer conversion and copy
#define mfx_adrv_DPvabDirect_USE_SIMD



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#if defined (mfx_adrv_DPvabDirect_USE_SIMD)
	#include "fstb/ToolsSimd.h"
#endif
#include "mfx/adrv/CbInterface.h"
#include "mfx/adrv/DPvabDirect.h"
#if ! defined (mfx_adrv_DPvabDirect_TEST)
	#include "mfx/hw/ThreadLinux.h"
	#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE)
		#include "mfx/hw/cs4272.h"
	#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE
#endif

#if ! defined (mfx_adrv_DPvabDirect_TEST)

	#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE)
		#include <wiringPi.h>
		#include <wiringPiI2C.h>
	#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE

	#include <bcm_host.h>

	#include <sys/mman.h>
	#include <fcntl.h>
	#include <unistd.h>

#endif // mfx_adrv_DPvabDirect_TEST

#include <chrono>
#include <stdexcept>

#include <cassert>
#include <cstdio>
#include <cstring>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DPvabDirect::DPvabDirect ()
:	_gpio ()
,	_cb_ptr (nullptr)
,	_lrclk_cur (0)
,	_btclk_cur (0)
,	_clk_cnt (0)
,	_bit_pos (-1)
,	_content_r (0)
,	_content_w (0)
,	_timeout_flag (0)
,	_resync_flag (false)
,	_exit_flag (false)
,	_proc_ex_flag (false)
,	_syncerr_flag (false)
,	_cur_buf (0)
,	_buf_int_i (_block_size_a * _nbr_chn * 2)
,	_buf_int_o (_block_size_a * _nbr_chn * 2)
,	_buf_flt_i (_block_size_a * _nbr_chn)
,	_buf_flt_o (_block_size_a * _nbr_chn)
,	_thread_main ()
,	_blk_proc_mtx ()
,	_blk_proc_cv ()
,	_proc_now_flag (false)
,	_state (State_STOP)
#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE) && ! defined (mfx_adrv_DPvabDirect_TEST)
,	_i2c_hnd (::wiringPiI2CSetup (_i2c_addr))
#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE, mfx_adrv_DPvabDirect_TEST
{
	// Nothing
}



DPvabDirect::~DPvabDirect ()
{
	if (_thread_main.joinable ())
	{
		_exit_flag = true;
		_thread_main.join ();
	}

#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE) && ! defined (mfx_adrv_DPvabDirect_TEST)
	if (_i2c_hnd != -1)
	{
		close (_i2c_hnd);
		_i2c_hnd = -1;
	}
#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE, mfx_adrv_DPvabDirect_TEST
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DPvabDirect::do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) noexcept
{
	fstb::unused (driver_0, chn_idx_in, chn_idx_out);
	assert (chn_idx_in == 0);
	assert (chn_idx_out == 0);

	sample_freq    = (_fs_code != 0) ? 44100 : 48000;
	max_block_size = _block_size;
	_cb_ptr        = &callback;

	// Prepares the chip for master mode
	_gpio.pull (_pin_dout, GpioAccess::Pull_UP);

	_gpio.set_fnc (_pin_rst , GpioAccess::PinFnc_OUT);
	_gpio.set_fnc (_pin_freq, GpioAccess::PinFnc_OUT);
	_gpio.set_fnc (_pin_bclk, GpioAccess::PinFnc_IN );
	_gpio.set_fnc (_pin_lrck, GpioAccess::PinFnc_IN );
	_gpio.set_fnc (_pin_din , GpioAccess::PinFnc_IN );
	_gpio.set_fnc (_pin_dout, GpioAccess::PinFnc_OUT);

	_gpio.write (_pin_freq, _fs_code);

	// Puts the chip in reset state
	_gpio.write (_pin_rst, 0);

	return 0;
}



int	DPvabDirect::do_start () noexcept
{
	int            ret_val = 0;

	if (_state != State_STOP)
	{
		do_stop ();
	}

	// Puts the chip in reset state
	_gpio.write (_pin_rst, 0);

	// Waits a few ms for all clocks to be stable
	std::this_thread::sleep_for (std::chrono::milliseconds (5));

	_gpio.write (_pin_rst, 1);

#if ! defined (mfx_adrv_DPvabDirect_TEST)

	#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE)

	// If MCLK is internally generated, waits for for it
	std::this_thread::sleep_for (std::chrono::milliseconds (1));

	using namespace hw::cs4272;

	// Mode control 2: sets CPEN and PDN
	write_reg (0x07, _mc2_ctrl_port | _mc2_power_down);

	// Setup -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Master mode and I2S for DAC
	// Table 9, p. 29:  MCLK=Input, Master Mode, Single Speed,
	// MCLK/LRCK = 256, SCLK/LRCK = 64: ratio1 = 0, ratio0 = 0
	write_reg (0x01, _mc1_single | _mc1_master | _mc1_fmt_i2s);

	write_reg (0x02, _dacc_deemph_none);

	write_reg (0x03, _mix_soft_r | _mix_atapi_l_to_l | _mix_atapi_r_to_r);

	write_reg (0x04, 0);
	write_reg (0x05, 0);

	// I2S for ADC
	write_reg (0x06, _adcc_fmt_i2s);
	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	// Clears the PDN bit for startup
	write_reg (0x07, _mc2_ctrl_port);

	// Actually only 85 us are required
	std::this_thread::sleep_for (std::chrono::milliseconds (1));

	#else  // mfx_adrv_DPvabDirect_CTRL_PORT_MODE

	// Waits 10 ms so we make sure we are in stand-alone mode
	std::this_thread::sleep_for (std::chrono::milliseconds (10));

	#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE

#endif // mfx_adrv_DPvabDirect_TEST

	// Initializes threads and stuff
	_exit_flag   = false;
	try
	{
		_thread_main = std::thread (&DPvabDirect::main_loop, this);
#if defined (mfx_adrv_DPvabDirect_TEST)
		_gpio.run ();
#else // mfx_adrv_DPvabDirect_TEST
		hw::ThreadLinux::set_priority (_thread_main, 0, nullptr);
#endif // mfx_adrv_DPvabDirect_TEST
	}
	catch (...)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		_state = State_RUN;
	}

	return ret_val;
}



int	DPvabDirect::do_stop () noexcept
{
	int            ret_val = 0;

	if (_state == State_RUN)
	{
		_exit_flag = true;
		try
		{
			_thread_main.join ();
#if defined (mfx_adrv_DPvabDirect_TEST)
			_gpio.stop ();
#endif // mfx_adrv_DPvabDirect_TEST
		}
		catch (...)
		{
			ret_val = -1;
		}
	}

	_state = State_STOP;

	return ret_val;
}



void	DPvabDirect::do_restart () noexcept
{
	do_stop ();
	do_start ();
}



std::string	DPvabDirect::do_get_last_error () const
{
	return "";
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DPvabDirect::GpioAccess::GpioAccess ()
#if ! defined (mfx_adrv_DPvabDirect_TEST)
:	_periph_addr (::bcm_host_get_peripheral_address ())
,	_gpio_ptr (map_periph (_periph_addr + _ofs_gpio, _len_gpio))
,	_last_read (0)
#endif // mfx_adrv_DPvabDirect_TEST
{
	// Nothing
}



// This function requires atomic compare-and-swap to be thread-safe.
void	DPvabDirect::GpioAccess::set_fnc (int gpio, PinFnc fnc) const noexcept
{
	assert (fnc >= 0);
	assert (fnc < PinFnc_NBR_ELT);

#if defined (mfx_adrv_DPvabDirect_TEST)
	const uint32_t mask = uint32_t (1) << gpio;
	_gpio_read  = (_gpio_read  & ~mask) | ((fnc == PinFnc_IN ) ? mask : 0);
	_gpio_write = (_gpio_write & ~mask) | ((fnc == PinFnc_OUT) ? mask : 0);
#else // mfx_adrv_DPvabDirect_TEST
	int            ofs_reg;
	int            shf_bit;
	find_addr_fnc (ofs_reg, shf_bit, gpio);

	const uint32_t msk_base = (1 << _fnc_field_size) - 1;

	uint32_t       val = _gpio_ptr [ofs_reg];
	val &= ~(msk_base << shf_bit);
	val |= uint32_t (fnc) << shf_bit;
	_gpio_ptr [ofs_reg] = val;
#endif // mfx_adrv_DPvabDirect_TEST
}



void	DPvabDirect::GpioAccess::clear (int gpio) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	const uint32_t mask = uint32_t (1) << gpio;
#if defined (mfx_adrv_DPvabDirect_TEST)
	if ((_gpio_write & mask) != 0)
	{
		_gpio_state.fetch_and (~mask);
	}
#else // mfx_adrv_DPvabDirect_TEST
	_gpio_ptr [_ofs_reg_clr] = mask;
#endif // mfx_adrv_DPvabDirect_TEST
}



void	DPvabDirect::GpioAccess::set (int gpio) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	const uint32_t mask = uint32_t (1) << gpio;
#if defined (mfx_adrv_DPvabDirect_TEST)
	if ((_gpio_write & mask) != 0)
	{
		_gpio_state.fetch_or (mask);
	}
#else  // mfx_adrv_DPvabDirect_TEST
	_gpio_ptr [_ofs_reg_set] = mask;
#endif // mfx_adrv_DPvabDirect_TEST
}



void	DPvabDirect::GpioAccess::write (int gpio, int val) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	if (val == 0)
	{
		clear (gpio);
	}
	else
	{
		set (gpio);
	}
}



int	DPvabDirect::GpioAccess::read (int gpio) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

#if defined (mfx_adrv_DPvabDirect_TEST)
	_last_read = _gpio_state;
#else  // mfx_adrv_DPvabDirect_TEST
	_last_read = _gpio_ptr [_ofs_reg_lvl];
#endif // mfx_adrv_DPvabDirect_TEST

	return read_cached (gpio);
}



int	DPvabDirect::GpioAccess::read_cached (int gpio) const noexcept
{
	return (_last_read >> gpio) & 1;
}



void	DPvabDirect::GpioAccess::pull (int gpio, Pull p) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);
	assert (p >= 0);
	assert (p < Pull_NBR_ELT);

	// At least 150 cycles
	const std::chrono::microseconds  wait_time (10);

#if ! defined (mfx_adrv_DPvabDirect_TEST)
	const uint32_t mask = uint32_t (1) << gpio;
	_gpio_ptr [_ofs_reg_pull] = uint32_t (p);
	std::this_thread::sleep_for (wait_time);
	_gpio_ptr [_ofs_reg_pclk] = _gpio_ptr [_ofs_reg_pclk] |  mask;
	std::this_thread::sleep_for (wait_time);
	_gpio_ptr [_ofs_reg_pull] = uint32_t (Pull_NONE);
	_gpio_ptr [_ofs_reg_pclk] = _gpio_ptr [_ofs_reg_pclk] & ~mask;
#endif // mfx_adrv_DPvabDirect_TEST
}



#if defined (mfx_adrv_DPvabDirect_TEST)

void	DPvabDirect::GpioAccess::run ()
{
	_quit_flag = false;
	_test_thread = std::thread (&DPvabDirect::GpioAccess::fake_data_loop, this);
}

void	DPvabDirect::GpioAccess::stop ()
{
	_quit_flag = true;
	_test_thread.join ();
}

void	DPvabDirect::GpioAccess::fake_data_loop () noexcept
{
	printf ("Device Event   Pos Chn Data\n");

	_gpio_state.fetch_and (~_gpio_read);

	int            pos_sig  = 0;
	int32_t        val_sent = 0;
	int            pos_bit  = -1;

	while (! _quit_flag)
	{
		for (int chn = 0
		;	chn < DPvabDirect::_nbr_chn && ! _quit_flag
		;	++ chn)
		{
			set_fake_bit (DPvabDirect::_pin_lrck, chn);

			// Fake signal generation
			const int      d   = 5;
			const int      p   = pos_sig + chn * d;
			const int      u   = p % (4 * d);
			const int      tri = std::abs (2 * d - std::abs (d - u)) - d;
			const int32_t  val_24 = tri * 0x111111;

			printf (
				"BOARD  Send         %d  %s0x%06X\n",
				chn,
				(chn == 1) ? "         " : "",
				val_24
			);


			for (int pos_clk = 0
			;	pos_clk < DPvabDirect::_bits_per_chn && ! _quit_flag
			;	++ pos_clk)
			{
				if (pos_clk == 1)
				{
					val_sent = val_24;
					pos_bit  = DPvabDirect::_resol - 1;
				}

				int            data_bit = 0;
				if (pos_bit >= 0)
				{
					data_bit = (val_sent >> pos_bit) & 1;
				}

				set_fake_bit (DPvabDirect::_pin_bclk, 0);
				set_fake_bit (DPvabDirect::_pin_din, data_bit);

				print_gpio ();
				std::this_thread::sleep_for (std::chrono::milliseconds (_hclk_dur));

				set_fake_bit (DPvabDirect::_pin_bclk, 1);

				print_gpio ();
				std::this_thread::sleep_for (std::chrono::milliseconds (_hclk_dur));

				-- pos_bit;
			}
		}

		++ pos_sig;
	}
}

void	DPvabDirect::GpioAccess::set_fake_bit (int gpio, int val) noexcept
{
	const uint32_t mask = uint32_t (1) << gpio;
	if ((_gpio_read & mask) != 0)
	{
		if (val != 0)
		{
			_gpio_state.fetch_or (mask);
		}
		else
		{
			_gpio_state.fetch_and (~mask);
		}
	}
}

void	DPvabDirect::GpioAccess::print_gpio () const noexcept
{
#if 0 // Very verbose...
	const uint32_t state = _gpio_state;
	printf ("GPIO  - ");
	for (int pos = 0; pos < 32; ++pos)
	{
		printf (
			"%01d%s",
			(state >> pos) & 1,
			((pos % 10) == 9) ? "  " : " "
		);
	}
	printf ("\n");
#endif
}



#else // mfx_adrv_DPvabDirect_TEST



// Duplicated from GpioPwm::map_periph ()
volatile uint32_t *	DPvabDirect::GpioAccess::map_periph (uint32_t base, uint32_t len)
{
	int            fd = open ("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
	{
		throw std::runtime_error ("Cannot open /dev/mem.\n");
	}

	void *         v_ptr =
		mmap (nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
	close (fd);

	if (v_ptr == MAP_FAILED)
	{
		throw std::runtime_error ("Cannot map peripheral.");
	}

	return reinterpret_cast <volatile uint32_t *> (v_ptr);
}



#endif // mfx_adrv_DPvabDirect_TEST



void	DPvabDirect::GpioAccess::find_addr_fnc (int &ofs_reg, int &shf_bit, int gpio) noexcept
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	const int      nbr_gpio_per_reg = 10;

	ofs_reg =  gpio / nbr_gpio_per_reg;
	shf_bit = (gpio - (ofs_reg * nbr_gpio_per_reg)) * _fnc_field_size;
}



void	DPvabDirect::main_loop () noexcept
{
	try
	{
	_lrclk_cur     = 0;
	_btclk_cur     = 0;
	_clk_cnt       = 0;
	_bit_pos       = -1;
	_buf_pos       = 0;
	_content_r     = 0;
	_content_w     = 0;
	_timeout_flag  = false;
	_resync_flag   = true;
	_syncerr_flag  = false;

	_proc_ex_flag  = false;
	_proc_now_flag = false;
	std::thread    thread_proc (&DPvabDirect::proc_loop, this);
#if ! defined (mfx_adrv_DPvabDirect_TEST)
	hw::ThreadLinux::set_priority (thread_proc, -4, nullptr);
#endif // mfx_adrv_DPvabDirect_TEST

	// Clears integer buffers
	memset (&_buf_int_i [0], 0, sizeof (_buf_int_i [0]) * _buf_int_i.size ());
	memset (&_buf_int_o [0], 0, sizeof (_buf_int_o [0]) * _buf_int_o.size ());

	while (! _exit_flag && ! _timeout_flag)
	{
		// Rising edge
		sync_to_bclk_edge (+1);
#if defined (mfx_adrv_DPvabDirect_TEST)
		std::this_thread::sleep_for (std::chrono::milliseconds (5));
//		printf ("CPU   - Rising edge\n");
#endif // mfx_adrv_DPvabDirect_TEST

		const int      lrck = _gpio.read_cached (_pin_lrck);
		const int      val  = _gpio.read_cached (_pin_din );

		// Checks LRCK and updates counters
		if (lrck != _lrclk_cur)
		{
			// Sanity check
			if (_clk_cnt != _bits_per_chn - 1 && ! _resync_flag)
			{
				_syncerr_flag = true;
			}

			_clk_cnt   = 0;
			_lrclk_cur = lrck;
			_bit_pos = _resol - 1 - _clk_cnt + _transfer_lag;
			if (_bit_pos >= _resol)
			{
				_bit_pos -= _bits_per_chn;
			}

			if (_resync_flag && _lrclk_cur == 0)
			{
				// Sync done, real stuff can begin
				_resync_flag = false;
				if (_transfer_lag > 0)
				{
					// Buffer position has not been updated yet so we have to set
					// the current buffer position to its "previous" place, the
					// R channel.
					_buf_pos |= 1;
				}
				else
				{
					_buf_pos &= ~1;
				}

#if defined (mfx_adrv_DPvabDirect_TEST)
				printf ("CPU    Synchronized\n");
#endif // mfx_adrv_DPvabDirect_TEST
			}
		}
		else
		{
			++ _clk_cnt;

			// Sanity check
			if (_clk_cnt >= _bits_per_chn && ! _resync_flag)
			{
				_syncerr_flag = true;
			}
		}

		// Reads data
		if (_bit_pos == _resol - 1)
		{
			// If MSB is 1, fills _content_r with 1s to ensure correct sign.
			_content_r = -val;
		}
		else if (_bit_pos < _resol - 1 && _bit_pos >= 0)
		{
			_content_r = (_content_r << 1) + val;
		}

		// Next bit
		-- _bit_pos;
		if (_bit_pos < _resol - _bits_per_chn)
		{
			_bit_pos += _bits_per_chn;
		}
		assert (_bit_pos < _resol);

		// Buffer stuff and signaling
		if (_bit_pos == _resol - 1 && ! _resync_flag)
		{
			int            buf_idx = _cur_buf;

			// Writes acquired sample
			const int      pos_i   =
				buf_idx * _block_size_a * _nbr_chn + _buf_pos;
			_buf_int_i [pos_i] = _content_r;
#if defined (mfx_adrv_DPvabDirect_TEST)
			printf (
				"CPU    Receive %03d  %d                    %s0x%06X\n",
				_buf_pos >> 1,
				_buf_pos & 1,
				((_buf_pos & 1) == 1) ? "         " : "",
				_content_r & ((1 << _resol) - 1)
			);

#endif // mfx_adrv_DPvabDirect_TEST

			// Updates buffer-related counters
			++ _buf_pos;
			if (_buf_pos >= _block_size * _nbr_chn)
			{
				if (_transfer_lag < 1)
				{
					// The LRCLK signal will be updated at the next clock, so we
					// have to consider its cached value as the previous channel
					// state.
					_buf_pos = 1 - _lrclk_cur;
				}
				else
				{
					_buf_pos = _lrclk_cur;
				}
				buf_idx  = 1 - buf_idx;
				_cur_buf = buf_idx;

				// Buffer done, signals the processing thread
				_proc_now_flag = true;
				_blk_proc_cv.notify_one ();
			}
			assert ((_buf_pos & 1) == (_lrclk_cur ^ ((_transfer_lag < 1) ? 1 : 0)));

			// Reads the sample to be sent
			const int      pos_o   =
				buf_idx * _block_size_a * _nbr_chn + _buf_pos;
			_content_w = _buf_int_o [pos_o];
#if defined (mfx_adrv_DPvabDirect_TEST)
			printf (
				"CPU    Send    %03d  %d                                      %s0x%06X\n",
				_buf_pos >> 1,
				_buf_pos & 1,
				((_buf_pos & 1) == 1) ? "         " : "",
				_content_w & ((1 << _resol) - 1)
			);
#endif // mfx_adrv_DPvabDirect_TEST
		}

		// Data to be written after the falling edge
		int            bit_w   = 0;
		if (_bit_pos >= 0 && _bit_pos < _resol)
		{
			bit_w = (_content_w >> _bit_pos) & 1;
		}

		// Falling edge
		sync_to_bclk_edge (-1);
#if defined (mfx_adrv_DPvabDirect_TEST)
		std::this_thread::sleep_for (std::chrono::milliseconds (5));
//		printf ("CPU   - Falling edge\n");
#endif // mfx_adrv_DPvabDirect_TEST

		// Writes data
		_gpio.write (_pin_dout, bit_w);

		if (_syncerr_flag)
		{
			_cb_ptr->notify_dropout ();
			_syncerr_flag = false;
		}
	}

	_proc_ex_flag = true;
	_blk_proc_cv.notify_one ();

	// Waits for the processing thread to terminate
	thread_proc.join ();
	}
	catch (...)
	{
		// Nothing
		assert (false);
	}
}



// dir:
// +1: rising edge
// -1: falling edge
void	DPvabDirect::sync_to_bclk_edge (int dir) noexcept
{
	assert (dir == 1 || dir == -1);

	int            loop_cnt  = 0;
	int            val       = _btclk_cur;
	int            val_prev  = val;
	do
	{
		val_prev = val;
		val = _gpio.read (_pin_bclk);
		++ loop_cnt;
	}
	while (   (val - val_prev) != dir
	       && loop_cnt < _bclk_timeout
	       && ! _exit_flag);

	if (loop_cnt >= _bclk_timeout)
	{
		_timeout_flag = true;
	}

	_btclk_cur = val;
}



void	DPvabDirect::proc_loop ()
{
	std::unique_lock <std::mutex> lock (_blk_proc_mtx);

	float *        dst_arr [2] =
	{
		&_buf_flt_o [0],
		&_buf_flt_o [_block_size_a]
	};
	const float *  src_arr [2] =
	{
		&_buf_flt_i [0],
		&_buf_flt_i [_block_size_a]
	};

	const float    scale_o = float ((1U << (_resol - 1)) - 1);
	const float    scale_i = 1.0f / scale_o;
	const float    min_flt = -1.0f;
	const float    max_flt = +1.0f;

#if defined (mfx_adrv_DPvabDirect_USE_SIMD)
	const auto     sc_o_v = fstb::ToolsSimd::set1_f32 (scale_o);
	const auto     sc_i_v = fstb::ToolsSimd::set1_f32 (scale_i);
	const auto     maxf_v = fstb::ToolsSimd::set1_f32 (max_flt);
	const auto     minf_v = fstb::ToolsSimd::set1_f32 (min_flt);
#endif // mfx_adrv_DPvabDirect_USE_SIMD

	float *        buf_flt_i_ptr = &_buf_flt_i [0];
	float *        buf_flt_o_ptr = &_buf_flt_o [0];

	while (! _proc_ex_flag)
	{
		while (! _proc_now_flag && ! _proc_ex_flag)
		{
			_blk_proc_cv.wait (lock);
		}
		_proc_now_flag = false;

		if (_proc_ex_flag)
		{
			break;
		}

		// _cur_buf is sampled now and its value will be blocked for the whole
		// processing duration.
		const int      buf_idx = 1 - _cur_buf;

		const int      ofs     = buf_idx * _block_size_a * _nbr_chn;
		const int32_t* buf_int_i_ptr = &_buf_int_i [ofs];
		int32_t *      buf_int_o_ptr = &_buf_int_o [ofs];

		// Copies acquired data to input buffer
#if defined (mfx_adrv_DPvabDirect_USE_SIMD)
		for (int pos = 0; pos < _block_size; pos += 4)
		{
			const auto     x0_int =
				fstb::ToolsSimd::load_s32 (buf_int_i_ptr + pos * 2    );
			const auto     x1_int =
				fstb::ToolsSimd::load_s32 (buf_int_i_ptr + pos * 2 + 4);
			auto           x0_flt = fstb::ToolsSimd::conv_s32_to_f32 (x0_int);
			auto           x1_flt = fstb::ToolsSimd::conv_s32_to_f32 (x1_int);
			x0_flt *= sc_i_v;
			x1_flt *= sc_i_v;
			fstb::Vf32     xl_flt;
			fstb::Vf32     xr_flt;
			fstb::ToolsSimd::deinterleave_f32 (xl_flt, xr_flt, x0_flt, x1_flt);
			fstb::ToolsSimd::store_f32 (
				buf_flt_i_ptr +                 pos, xl_flt
			);
			fstb::ToolsSimd::store_f32 (
				buf_flt_i_ptr + _block_size_a + pos, xr_flt
			);
		}
#else // mfx_adrv_DPvabDirect_USE_SIMD
		for (int pos = 0; pos < _block_size; ++pos)
		{
			const int32_t  xl_int = buf_int_i_ptr [pos * 2    ];
			const int32_t  xr_int = buf_int_i_ptr [pos * 2 + 1];
			const float    xl_flt = xl_int * scale_i;
			const float    xr_flt = xr_int * scale_i;
			buf_flt_i_ptr [                pos] = xl_flt;
			buf_flt_i_ptr [_block_size_a + pos] = xr_flt;
		}
#endif // mfx_adrv_DPvabDirect_USE_SIMD

		// Processing
		_cb_ptr->process_block (dst_arr, src_arr, _block_size);

		// Copies produced data to the output buffer
#if defined (mfx_adrv_DPvabDirect_USE_SIMD)
		for (int pos = 0; pos < _block_size; pos += 4)
		{
			auto           xl_flt =
				fstb::ToolsSimd::load_f32 (buf_flt_o_ptr +                 pos);
			auto           xr_flt =
				fstb::ToolsSimd::load_f32 (buf_flt_o_ptr + _block_size_a + pos);
			xl_flt  = fstb::ToolsSimd::max_f32 (xl_flt, minf_v);
			xr_flt  = fstb::ToolsSimd::max_f32 (xr_flt, minf_v);
			xl_flt  = fstb::ToolsSimd::min_f32 (xl_flt, maxf_v);
			xr_flt  = fstb::ToolsSimd::min_f32 (xr_flt, maxf_v);
			xl_flt *= sc_o_v;
			xr_flt *= sc_o_v;
			fstb::Vf32     x0_flt;
			fstb::Vf32     x1_flt;
			fstb::ToolsSimd::interleave_f32 (x0_flt, x1_flt, xl_flt, xr_flt);
			const auto  x0_int = fstb::ToolsSimd::conv_f32_to_s32 (x0_flt);
			const auto  x1_int = fstb::ToolsSimd::conv_f32_to_s32 (x1_flt);
			fstb::ToolsSimd::store_s32 (buf_int_o_ptr + pos * 2    , x0_int);
			fstb::ToolsSimd::store_s32 (buf_int_o_ptr + pos * 2 + 4, x1_int);
		}
#else // mfx_adrv_DPvabDirect_USE_SIMD
		for (int pos = 0; pos < _block_size; ++pos)
		{
			float          xl_flt = buf_flt_o_ptr [                pos];
			float          xr_flt = buf_flt_o_ptr [_block_size_a + pos];
			xl_flt  = fstb::limit (xl_flt, min_flt, max_flt);
			xr_flt  = fstb::limit (xr_flt, min_flt, max_flt);
			xl_flt *= scale_o;
			xr_flt *= scale_o;
			const int32_t  xl_int = fstb::conv_int_fast (xl_flt);
			const int32_t  xr_int = fstb::conv_int_fast (xr_flt);
			buf_int_o_ptr [pos * 2    ] = xl_int;
			buf_int_o_ptr [pos * 2 + 1] = xr_int;
		}
#endif // mfx_adrv_DPvabDirect_USE_SIMD
	}
}



#if ! defined (mfx_adrv_DPvabDirect_TEST)

	#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE)



void	DPvabDirect::write_reg (uint8_t reg, uint8_t val) noexcept
{
	assert (_i2c_hnd != -1);

	::wiringPiI2CWriteReg8 (_i2c_hnd, reg, val);
}



	#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE



#endif // mfx_adrv_DPvabDirect_TEST



}  // namespace adrv
}  // namespace mfx



#undef mfx_adrv_DPvabDirect_USE_SIMD



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
