/*****************************************************************************

        DPvabI2s.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



// Enables use of SIMD instructions in the buffer conversion and copy
#define mfx_adrv_DPvabI2s_USE_SIMD



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#if defined (mfx_adrv_DPvabI2s_USE_SIMD)
	#include "fstb/ToolsSimd.h"
#endif
#include "mfx/adrv/CbInterface.h"
#include "mfx/adrv/DPvabI2s.h"
#include "mfx/hw/bcm2837pcm.h"
#include "mfx/hw/cs4272.h"
#include "mfx/hw/ThreadLinux.h"

#include <bcm_host.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <fcntl.h>

#include <chrono>
#include <stdexcept>
#include <thread>

#include <cassert>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DPvabI2s::DPvabI2s ()
:	_periph_base_addr (::bcm_host_get_peripheral_address ())
,	_pcm_mptr (
		_periph_base_addr + hw::bcm2837pcm::_pcm_ofs,
		hw::bcm2837pcm::_pcm_len,
		"/dev/mem",
		O_RDWR | O_SYNC
	)
,	_gpio ()
,	_i2c_hnd (::wiringPiI2CSetup (_i2c_addr))
,	_cb_ptr (nullptr)
,	_state (State_STOP)
,	_exit_flag (false)
,	_proc_ex_flag (false)
,	_cur_buf (0)
,	_buf_int_i (_block_size_a * _nbr_chn * 2)
,	_buf_int_o (_block_size_a * _nbr_chn * 2)
,	_buf_flt_i (_block_size_a * _nbr_chn)
,	_buf_flt_o (_block_size_a * _nbr_chn)
,	_thread_main ()
,	_blk_proc_mtx ()
,	_blk_proc_cv ()
,	_proc_now_flag (false)
{
	if (   ! _exit_flag.is_lock_free ()
	    || ! _proc_ex_flag.is_lock_free ()
	    || ! _cur_buf.is_lock_free ()
	    || ! _proc_now_flag.is_lock_free ())
	{
		close_i2c ();
		throw std::runtime_error (
			"std::atomic is not lock-free on this system."
		);
	}
}



DPvabI2s::~DPvabI2s ()
{
	close_i2c ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DPvabI2s::do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out)
{
	fstb::unused (driver_0, chn_idx_in, chn_idx_out);
	assert (chn_idx_in == 0);
	assert (chn_idx_out == 0);

	sample_freq    = (_fs_code != 0) ? 44100 : 48000;
	max_block_size = _block_size;
	_cb_ptr        = &callback;

	// Custom pins
	_gpio.set_fnc (_pin_rst , hw::GpioAccess::PinFnc_OUT  );
	_gpio.set_fnc (_pin_freq, hw::GpioAccess::PinFnc_OUT  );

	// I2S pins, mode ALT0 (p. 102)
	_gpio.set_fnc (_pin_bclk, hw::GpioAccess::PinFnc_ALT0);
	_gpio.set_fnc (_pin_lrck, hw::GpioAccess::PinFnc_ALT0);
	_gpio.set_fnc (_pin_din , hw::GpioAccess::PinFnc_ALT0);
	_gpio.set_fnc (_pin_dout, hw::GpioAccess::PinFnc_ALT0);

	_gpio.write (_pin_freq, _fs_code);

	// Puts the chip in reset state
	_gpio.write (_pin_rst, 0);

	return 0;
}



int	DPvabI2s::do_start ()
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

	// If MCLK is internally generated, waits for it
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

	// Initializes threads and stuff
	_exit_flag   = false;
	_thread_main = std::thread (&DPvabI2s::main_loop, this);
	hw::ThreadLinux::set_priority (_thread_main, 0, nullptr);

	if (ret_val == 0)
	{
		_state = State_RUN;
	}

	return ret_val;
}



int	DPvabI2s::do_stop ()
{
	int            ret_val = 0;

	if (_state == State_RUN)
	{
		_exit_flag = true;
		_thread_main.join ();
	}

	_state = State_STOP;

	return ret_val;
}



void	DPvabI2s::do_restart ()
{
	do_stop ();
	do_start ();
}



std::string	DPvabI2s::do_get_last_error () const
{
	return "";
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DPvabI2s::close_i2c ()
{
	if (_i2c_hnd != -1)
	{
		close (_i2c_hnd);
		_i2c_hnd = -1;
	}
}



void	DPvabI2s::main_loop ()
{
	_proc_ex_flag  = false;
	_proc_now_flag = false;
	_cur_buf       = 0;

	std::thread    thread_proc (&DPvabI2s::proc_loop, this);
	hw::ThreadLinux::set_priority (thread_proc, -1, nullptr);

	// Clears integer buffers
	memset (&_buf_int_i [0], 0, sizeof (_buf_int_i [0]) * _buf_int_i.size ());
	memset (&_buf_int_o [0], 0, sizeof (_buf_int_o [0]) * _buf_int_o.size ());

	using namespace hw::bcm2837pcm;

	// Sets up the I2S interface
	uint32_t       status_mask =
		  _cs_a_stby
		| _cs_a_rxsex
		| _cs_a_rxthr_one
		| _cs_a_txthr_ful1
		| _cs_a_en;
	_pcm_mptr.at (_mode_a) =
		  _mode_a_clkm
		| _mode_a_clki
		| _mode_a_fsm
		| _mode_a_fsi
		| ((_bits_per_chn * 2 - 1) << _mode_a_flen )
		| ( _bits_per_chn          << _mode_a_fslen);
	const uint32_t chn_conf_base =
		  _xc_a_en
		| (( (_resol - 8) & 0x0F) << _xc_a_wid)
		| ((((_resol - 8) & 0x10) != 0) ? _xc_a_wex : 0);
	const uint32_t chn_conf_l =
		chn_conf_base | ( _transfer_lag                  << _xc_a_pos);
	const uint32_t chn_conf_r =
		chn_conf_base | ((_transfer_lag + _bits_per_chn) << _xc_a_pos);
	_pcm_mptr.at (_rxc_a) =
		  (chn_conf_l << _xc_a_ch1)
		| (chn_conf_r << _xc_a_ch2);
	_pcm_mptr.at (_txc_a) =
		  (chn_conf_l << _xc_a_ch1)
		| (chn_conf_r << _xc_a_ch2);
	_pcm_mptr.at (_dreq_a) =
		  (0x10 << _dreq_a_tx_panic)
		| (0x30 << _dreq_a_rx_panic)
		| (0x30 << _dreq_a_tx      )
		| (0x20 << _dreq_a_rx      );
	_pcm_mptr.at (_inten_a) = 0;
	_pcm_mptr.at (_intstc_a) =
		  _intstc_a_rxerr
		| _intstc_a_txerr
		| _intstc_a_rxr
		| _intstc_a_txw;
	_pcm_mptr.at (_gray) = 0;

	// Clears the FIFOs and errors, enables the PCM clock
	_pcm_mptr.at (_cs_a) =
		  status_mask
		| _cs_a_sync
		| _cs_a_rxerr
		| _cs_a_txerr
		| _cs_a_rxclr
		| _cs_a_txclr;

	// The FIFO requires 2 clock cycles before being cleared.
#if 1
	// Wait for 1 ms, which should be much more than 2 clock cycles.
	std::this_thread::sleep_for (std::chrono::milliseconds (1));
#else /*** To do: this does not seem to work. Check what's wrong. ***/
	// Waits for the sync bit, so the FIFOs are actually cleared
	while ((_pcm_mptr.at (_cs_a) & _cs_a_sync) != 0 && ! _exit_flag)
	{
		continue;
	}
#endif

	// Writes a few samples in advance
	// Less samples = shorter latency
	// More samples = better protection against thread interruptions
	for (int k = 0; k < _prefill; ++k)
	{
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			_pcm_mptr.at (_fifo_a) = 0;
		}
	}

	// Start
	status_mask |= _cs_a_txon | _cs_a_rxon;
	_pcm_mptr.at (_cs_a) = status_mask;

	int            buf_idx       = _cur_buf;
	int            buf_pos       = 0;
	int            chn_pos       = 0;
#if 0
	uint32_t       dummy         = 0;
#endif
	bool           sync_err_flag = false;
	while (! _exit_flag)
	{
		uint32_t       status = _pcm_mptr.at (_cs_a);

		if ((status & _cs_a_rxerr) != 0 || (status & _cs_a_txerr) != 0)
		{
			sync_err_flag = true;

			// Clears error at the PCM interface level
			_pcm_mptr.at (_cs_a) = status_mask | _cs_a_rxerr | _cs_a_txerr;
		}

#if 0 /*** To do: this test does not work, we have to check exactly why. ***/
		// Possible L/R sync errors, skips a frame to fix them
		if (chn_pos == 0)
		{
			if ((status & _cs_a_rxsync) == 0)
			{
				dummy += _pcm_mptr.at (_fifo_a);
				sync_err_flag = true;
			}
			if ((status & _cs_a_txsync) == 0)
			{
				_pcm_mptr.at (_fifo_a) = 0;
				sync_err_flag = true;
			}
		}
#endif

		// Can we read/write something ?
		if (   (status & _cs_a_rxr) != 0
		    && (status & _cs_a_txw) != 0)
		{
			const int      pos =
				(buf_idx * _block_size_a + buf_pos) * _nbr_chn + chn_pos;

			// Read to the input buffer
			_buf_int_i [pos]       = _pcm_mptr.at (_fifo_a);

			// Write from the output buffer
			_pcm_mptr.at (_fifo_a) = _buf_int_o [pos];

			// Next sample
			++ chn_pos;
			if (chn_pos >= _nbr_chn)
			{
				chn_pos = 0;
				++ buf_pos;

				if (buf_pos >= _block_size)
				{
					buf_pos  = 0;
					buf_idx  = 1 - buf_idx;
					_cur_buf = buf_idx;

					// Buffer done, signals the processing thread
					_proc_now_flag = true;
					_blk_proc_cv.notify_one ();
				}
			}
		}

		else if (sync_err_flag)
		{
			_cb_ptr->notify_dropout ();
		}
	}

	_proc_ex_flag = true;
	_blk_proc_cv.notify_one ();

	// Disables the PCM interface
	_pcm_mptr.at (_cs_a) = 0;

	// Waits for the processing thread to terminate
	thread_proc.join ();
}



void	DPvabI2s::proc_loop ()
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

#if defined (mfx_adrv_DPvabI2s_USE_SIMD)
	const auto     sc_o_v = fstb::ToolsSimd::set1_f32 (scale_o);
	const auto     sc_i_v = fstb::ToolsSimd::set1_f32 (scale_i);
	const auto     maxf_v = fstb::ToolsSimd::set1_f32 (max_flt);
	const auto     minf_v = fstb::ToolsSimd::set1_f32 (min_flt);
#endif // mfx_adrv_DPvabI2s_USE_SIMD

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
#if defined (mfx_adrv_DPvabI2s_USE_SIMD)
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
			fstb::ToolsSimd::VectF32   xl_flt;
			fstb::ToolsSimd::VectF32   xr_flt;
			fstb::ToolsSimd::deinterleave_f32 (xl_flt, xr_flt, x0_flt, x1_flt);
			fstb::ToolsSimd::store_f32 (
				buf_flt_i_ptr +                 pos, xl_flt
			);
			fstb::ToolsSimd::store_f32 (
				buf_flt_i_ptr + _block_size_a + pos, xr_flt
			);
		}
#else // mfx_adrv_DPvabI2s_USE_SIMD
		for (int pos = 0; pos < _block_size; ++pos)
		{
			const int32_t  xl_int = buf_int_i_ptr [pos * 2    ];
			const int32_t  xr_int = buf_int_i_ptr [pos * 2 + 1];
			const float    xl_flt = xl_int * scale_i;
			const float    xr_flt = xr_int * scale_i;
			buf_flt_i_ptr [                pos] = xl_flt;
			buf_flt_i_ptr [_block_size_a + pos] = xr_flt;
		}
#endif // mfx_adrv_DPvabI2s_USE_SIMD

		// Processing
		_cb_ptr->process_block (dst_arr, src_arr, _block_size);

		// Copies produced data to the output buffer
#if defined (mfx_adrv_DPvabI2s_USE_SIMD)
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
			fstb::ToolsSimd::VectF32   x0_flt;
			fstb::ToolsSimd::VectF32   x1_flt;
			fstb::ToolsSimd::interleave_f32 (x0_flt, x1_flt, xl_flt, xr_flt);
			const auto  x0_int = fstb::ToolsSimd::conv_f32_to_s32 (x0_flt);
			const auto  x1_int = fstb::ToolsSimd::conv_f32_to_s32 (x1_flt);
			fstb::ToolsSimd::store_s32 (buf_int_o_ptr + pos * 2    , x0_int);
			fstb::ToolsSimd::store_s32 (buf_int_o_ptr + pos * 2 + 4, x1_int);
		}
#else // mfx_adrv_DPvabI2s_USE_SIMD
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
#endif // mfx_adrv_DPvabI2s_USE_SIMD

		if (_proc_now_flag)
		{
			// We must be late
			_cb_ptr->notify_dropout ();
		}
	}
}



void	DPvabI2s::write_reg (uint8_t reg, uint8_t val)
{
	assert (_i2c_hnd != -1);

	::wiringPiI2CWriteReg8 (_i2c_hnd, reg, val);
}



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
