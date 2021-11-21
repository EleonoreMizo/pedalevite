/*****************************************************************************

        DPvabI2sDma.cpp
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
#define mfx_adrv_DPvabI2sDma_USE_SIMD



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#if defined (mfx_adrv_DPvabI2sDma_USE_SIMD)
	#include "fstb/ToolsSimd.h"
#endif
#include "mfx/adrv/CbInterface.h"
#include "mfx/adrv/DPvabI2sDma.h"
#include "mfx/hw/bcm2837.h"
#include "mfx/hw/bcm2837dma.h"
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
#include <cstring>
#include <ctime>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DPvabI2sDma::DPvabI2sDma ()
:	_periph_base_addr (::bcm_host_get_peripheral_address ())
,	_pcm_mptr (
		_periph_base_addr + hw::bcm2837pcm::_pcm_ofs,
		hw::bcm2837pcm::_pcm_len,
		"/dev/mem", O_RDWR | O_SYNC
	)
,	_dma_mptr (
		_periph_base_addr + hw::bcm2837dma::_dma_ofs,
		_dma_chn * hw::bcm2837dma::_dma_chn_inc + hw::bcm2837dma::_dma_chn_len,
		"/dev/mem", O_RDWR | O_SYNC
	)
,	_gpio ()
,	_i2c_hnd (::wiringPiI2CSetup (_i2c_addr))
,	_cb_ptr (nullptr)
,	_state (State_STOP)
,	_exit_flag (false)
,	_cur_buf (0)
,	_buf_int_i_ptr (nullptr)
,	_buf_int_o_ptr (nullptr)
,	_buf_flt_i (_block_size_a * _nbr_chn)
,	_buf_flt_o (_block_size_a * _nbr_chn)
,	_thread_main ()
,	_dma_uptr ()
,	_dma_buf_beg_arr {}
,	_spl_dur_ns (0)
,	_min_dur_ns (0)
{
	if (! _exit_flag.is_lock_free ())
	{
		close_i2c ();
		throw std::runtime_error (
			"std::atomic is not lock-free on this system."
		);
	}
}



DPvabI2sDma::~DPvabI2sDma ()
{
	assert (_state == State_STOP);
	close_i2c ();
}



// Call this only when the driver is running
DPvabI2sDma::PosIO	DPvabI2sDma::get_dma_pos () const noexcept
{
	using namespace hw::bcm2837dma;

	const int      dma_base = _dma_chn * _dma_chn_inc;
	const uint32_t cur_adr  = _dma_mptr.at (dma_base + _conblk_ad);

	// Finds the buffer index
	int            buf_idx = _nbr_buf;
	uint32_t       beg_adr = 0;
	do
	{
		-- buf_idx;
		beg_adr = _dma_buf_beg_arr [buf_idx];
	}
	while (buf_idx > 0 && cur_adr < beg_adr);
	const int      offset   = cur_adr - beg_adr;

	// Finds the frame index
	const int      frame_sz =
		sizeof (hw::bcm2837dma::CtrlBlock) * _nbr_chn * Dir_NBR_ELT;
	const int      spl_idx  = offset / frame_sz;

	// Channel index
	const int      chn_idx  = (offset / Dir_NBR_ELT) % _nbr_chn;

	return PosIO { buf_idx, spl_idx, chn_idx };
}



// Call this only when the driver is running
uint32_t	DPvabI2sDma::get_pcm_status () const noexcept
{
	return _pcm_mptr.at (hw::bcm2837pcm::_cs_a);
}



// Call this only when the driver is running
DPvabI2sDma::BufferDump	DPvabI2sDma::dump_buf_in () const noexcept
{
	BufferDump     content;
	for (int buf_idx = 0; buf_idx < _nbr_buf; ++buf_idx)
	{
		const SplType * src_ptr = _buf_int_i_ptr + _block_size_a * buf_idx;
		for (int frame_idx = 0; frame_idx < _block_size; ++frame_idx)
		{
			for (int chn_idx = 0; chn_idx < _nbr_chn; ++chn_idx)
			{
				const int pos = frame_idx * _nbr_chn + chn_idx;
				content [buf_idx] [pos] = src_ptr [pos];
			}
		}
	}

	return content;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DPvabI2sDma::do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) noexcept
{
	int            ret_val = 0;

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

	// Allocates the memory used by the DMA
	const int      nbr_blocks  = _nbr_buf * _block_size   * _nbr_chn * Dir_NBR_ELT;
	const int      buf_size_io = _nbr_buf * _block_size_a * _nbr_chn;
	const int      nbr_spl     = Dir_NBR_ELT * buf_size_io;
	try
	{
		_dma_uptr = std::make_unique <hw::RPiDmaBlocks> (
			nbr_blocks, nbr_spl * sizeof (SplType)
		);
		_buf_int_i_ptr = _dma_uptr->use_buf <SplType> ();
		_buf_int_o_ptr = _buf_int_i_ptr + buf_size_io;

		// Computes the minimum time we have to give to the system, per block
		const double   rt_ratio     = read_rt_ratio ();
		const double   spl_dur_ns   = 1e9 / sample_freq;
		_spl_dur_ns = fstb::round_int64 (spl_dur_ns);
		const double   block_dur_ns = 1e9 * _block_size / sample_freq;
		_min_dur_ns = fstb::round_int64 ((1 - rt_ratio) * block_dur_ns);
	}
	catch (...)
	{
		ret_val = -1;
	}

	return ret_val;
}



int	DPvabI2sDma::do_start () noexcept
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
	try
	{
		_thread_main = std::thread (&DPvabI2sDma::main_loop, this);
		hw::ThreadLinux::set_priority (_thread_main, 0, nullptr);
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



int	DPvabI2sDma::do_stop () noexcept
{
	int            ret_val = 0;

	if (_state == State_RUN)
	{
		_exit_flag = true;
		try
		{
			_thread_main.join ();
		}
		catch (...)
		{
			ret_val = -1;
		}
	}

	_state = State_STOP;

	return ret_val;
}



void	DPvabI2sDma::do_restart () noexcept
{
	do_stop ();
	do_start ();
}



std::string	DPvabI2sDma::do_get_last_error () const
{
	return "";
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DPvabI2sDma::close_i2c () noexcept
{
	if (_i2c_hnd != -1)
	{
		close (_i2c_hnd);
		_i2c_hnd = -1;
	}
}



void	DPvabI2sDma::main_loop () noexcept
{
	// Clears integer buffers
	const int      buf_size = _block_size_a * _nbr_chn * Dir_NBR_ELT;
	std::fill (_buf_int_i_ptr, _buf_int_i_ptr + buf_size, 0);
	std::fill (_buf_int_o_ptr, _buf_int_o_ptr + buf_size, 0);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Sets up the I2S interface

	using namespace hw::bcm2837pcm;

	uint32_t       status_mask =
		  _cs_a_stby
		| _cs_a_rxsex
		| _cs_a_dmaen
		| _cs_a_rxthr_one
		| _cs_a_txthr_zero
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

	// Clears the FIFOs and errors, enables the PCM clock, sets the SYNC bit
	// that will be echoed back in the read value after 2 clocks.
	_pcm_mptr.at (_cs_a) =
		  status_mask
		| _cs_a_sync
		| _cs_a_rxerr
		| _cs_a_txerr
		| _cs_a_rxclr
		| _cs_a_txclr;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Configures the DMA

	using namespace hw::bcm2837dma;

	build_dma_ctrl_block_list ();

	_dma_mptr.at (_enable) = _dma_mptr.at (_enable) | (1 << _dma_chn);
	const int      dma_base = _dma_chn * _dma_chn_inc;
	_dma_mptr.at (dma_base + _cs) = _reset;
	// We should probably wait a few us here but the next I2S operations
	// have to wait too, so we do them before continuing with the DMA.

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	// The FIFO requires 2 clock cycles before being cleared.
#if 1
	// Wait for 1 ms, which should be much more than 2 clock cycles.
	std::this_thread::sleep_for (std::chrono::milliseconds (1));
#else
	// Waits for the SYNC bit, so the FIFOs are actually cleared
	while ((_pcm_mptr.at (_cs_a) & _cs_a_sync) == 0 && ! _exit_flag) continue;
#endif

	// Writes a few samples in advance
	// Less samples = shorter latency
	// More samples = better protection against thread interruptions
	static_assert (
		_prefill * _nbr_chn <= _fifo_len,
		"Prefill should fit in the TX FIFO."
	);
	for (int k = 0; k < _prefill * _nbr_chn; ++k)
	{
		_pcm_mptr.at (_fifo_a) = 0;
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	// Starts the DMA
	constexpr int  priority = 12; // 0-15
	const auto     cb_adr   = _dma_uptr->virt_to_phys (&_dma_uptr->use_cb (0));
	_dma_mptr.at (dma_base + _cs       ) = _int | _end;
	_dma_mptr.at (dma_base + _conblk_ad) = cb_adr;
	_dma_mptr.at (dma_base + _debug    ) = _all_errors; // Clears errors
	_dma_mptr.at (dma_base + _cs       ) =
		  _waitfow
		| (priority << _panic_prio)
		| (priority << _priority)
		| _active;

	// Starts the PCM interface
	status_mask |= _cs_a_txon | _cs_a_rxon;
	_pcm_mptr.at (_cs_a) = status_mask;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	uint32_t       dummy = 0;
	_cur_buf = 0;
	while (! _exit_flag)
	{
		// Processes incoming data
		process_block (1 - _cur_buf);

		// Checks where we are in the I/O block
		auto           dma_pos = get_dma_pos ();

		// Checks if there are sync errors
		// For some unknown reason the L/R sync check works better when the DMA
		// is on channel 1
		bool           syncerr_flag = false;
		if (dma_pos._chn == 1)
		{
			uint32_t       status       = _pcm_mptr.at (_cs_a);
			if ((status & _cs_a_rxerr) != 0 || (status & _cs_a_txerr) != 0)
			{
				syncerr_flag = true;

				// Clears error at the PCM interface level
				_pcm_mptr.at (_cs_a) = status_mask | _cs_a_rxerr | _cs_a_txerr;

				/***
				To do: maybe we should brutally restart the DMA and I2S?
				The driver seems to recover gracefully anyway once the CPU load
				burst is over.
				***/
			}

			// Possible L/R sync errors, skips a frame to fix them
			if ((status & _cs_a_rxsync) == 0)
			{
				// Stores the result to make sure the read will not be
				// optimised out
				dummy += _pcm_mptr.at (_fifo_a);
				syncerr_flag = true;
			}
			if ((status & _cs_a_txsync) == 0)
			{
				_pcm_mptr.at (_fifo_a) = 0;
				syncerr_flag = true;
			}
		}

		if (dma_pos._buf != _cur_buf)
		{
			// We're already in another buffer
			syncerr_flag = true;

			// Makes it the current buffer
			_cur_buf = dma_pos._buf;
		}

		// Notifies the host that there was an error
		if (syncerr_flag)
		{
			_cb_ptr->notify_dropout ();
		}

		// The scheduler only gives a fraction of 1 s periods to the real-time
		// threads (default: 95 %). So if we don't want to get interrupted
		// for 50 ms every second for no real reason, we have to release the
		// CPU. This is not trivial because the system call to nanosleep()
		// has some overhead in tens of us, making it difficult to handle
		// very short blocks.

		// Estimates the remaining time before the next block, in nanoseconds
		const int      nbr_rem_frames = _block_size - dma_pos._frame;
		const int64_t  rem_time_ns    = nbr_rem_frames * _spl_dur_ns;

		// We won't sleep more than the amount of time that may make us miss
		// the ideal start for the next block
		const int64_t  sleep_ns = rem_time_ns - _nsleep_ovrhd_avg;

		if (sleep_ns > 0)
		{
			::timespec     slp;
			slp.tv_sec  = 0;
			slp.tv_nsec = long (sleep_ns);
			nanosleep (&slp, nullptr);
		}

		// Now waits for the next buffer (active polling)
		while (! _exit_flag && _cur_buf == dma_pos._buf)
		{
			dma_pos = get_dma_pos ();
		}

		// We're done, we can process the new block
		_cur_buf = dma_pos._buf;
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// The end

	// Disables the PCM interface
	_pcm_mptr.at (_cs_a) = 0 + int (dummy * 1e-300);

	// Stops the DMA channel
	_dma_mptr.at (dma_base + _cs) = _abort | _reset;
}



// Builds the DMA control block list.
// Each single block reads or write a single (mono) samples.
// We interleave read and write for each sample of a stereo pair, it seems
// that's the best way to keep everything in sync.
// Sample data input and output buffers should be ready before the call.
void	DPvabI2sDma::build_dma_ctrl_block_list () noexcept
{
	assert (_buf_int_i_ptr != nullptr);
	assert (_buf_int_o_ptr != nullptr);

	// DMA uses "bus" (not physical) addresses for peripherals, so we have
	// to convert our FIFO address into a bus address.
	const uint32_t fifo_bus_adr =
		  hw::bcm2837::_bus_base
		+ hw::bcm2837pcm::_pcm_ofs
		+ hw::bcm2837pcm::_fifo_a;
	const int      nbr_blocks = _nbr_buf * _block_size * _nbr_chn * Dir_NBR_ELT;

	int            blk_idx    = 0;
	for (int buf_idx = 0; buf_idx < _nbr_buf; ++buf_idx)
	{
		const auto &   cb_first = _dma_uptr->use_cb (blk_idx);
		_dma_buf_beg_arr [buf_idx] = _dma_uptr->virt_to_phys (&cb_first);

		for (int buf_pos = 0; buf_pos < _block_size; ++buf_pos)
		{
			for (int chn_idx = 0; chn_idx < _nbr_chn; ++chn_idx)
			{
				// Position in mono samples within the in/out buffers
				const auto     pos = (buf_idx * _block_size_a + buf_pos) * _nbr_chn + chn_idx;

				// 0 = read, 1 = write
				for (int dir_idx = 0; dir_idx < Dir_NBR_ELT; ++dir_idx)
				{
					auto &         cb          = _dma_uptr->use_cb (blk_idx    );

					const auto     blk_idx_nxt = (blk_idx + 1) % nbr_blocks;
					auto &         cb_nxt      = _dma_uptr->use_cb (blk_idx_nxt);

					using namespace hw::bcm2837dma;
					uint32_t       ti_base     = _no_wide_b | _wait_resp;
					if (dir_idx == Dir_R)
					{
						const auto     pm      = Dreq_PCM_RX << _permap;
						const auto     buf_adr =
							_dma_uptr->virt_to_phys (_buf_int_i_ptr + pos);
						cb._info = ti_base | pm | _src_dreq; // TI: transfer information
						cb._src  = fifo_bus_adr; // SOURCE_AD
						cb._dst  = buf_adr;      // DEST_AD
					}
					else
					{
						const auto     pm      = Dreq_PCM_TX << _permap;
						const auto     buf_adr =
							_dma_uptr->virt_to_phys (_buf_int_o_ptr + pos);
						cb._info = ti_base | pm | _dest_dreq; // TI: transfer information
						cb._src  = buf_adr;      // SOURCE_AD
						cb._dst  = fifo_bus_adr; // DEST_AD
					}
					cb._length = sizeof (SplType); // TXFR_LEN: transfer length
					cb._stride = 0;  // 2D stride mode
					cb._next   = _dma_uptr->virt_to_phys (&cb_nxt); // NEXTCONBK
					memset (cb._pad, 0, sizeof (cb._pad));

					++ blk_idx;
				}
			}
		}
	}
	assert (blk_idx == _dma_uptr->get_nbr_blocks ());
}



void	DPvabI2sDma::process_block (int buf_idx) noexcept
{
	assert (buf_idx >= 0);
	assert (buf_idx < _nbr_buf);

	std::array <float *, _nbr_buf>   dst_arr = 
	{
		&_buf_flt_o [0            ],
		&_buf_flt_o [_block_size_a]
	};
	std::array <const float *, _nbr_buf>   src_arr =
	{
		&_buf_flt_i [0            ],
		&_buf_flt_i [_block_size_a]
	};

	constexpr float   scale_o = float ((1U << (_resol - 1)) - 1);
	constexpr float   scale_i = 1.0f / scale_o;
	constexpr float   min_flt = -1.0f;
	constexpr float   max_flt = +1.0f;

#if defined (mfx_adrv_DPvabI2sDma_USE_SIMD)
	const auto     sc_o_v = fstb::ToolsSimd::set1_f32 (scale_o);
	const auto     sc_i_v = fstb::ToolsSimd::set1_f32 (scale_i);
	const auto     maxf_v = fstb::ToolsSimd::set1_f32 (max_flt);
	const auto     minf_v = fstb::ToolsSimd::set1_f32 (min_flt);
#endif // mfx_adrv_DPvabI2sDma_USE_SIMD

	float *        buf_flt_i_ptr = &_buf_flt_i [0];
	float *        buf_flt_o_ptr = &_buf_flt_o [0];

	const int      ofs     = buf_idx * _block_size_a * _nbr_chn;
	const int32_t* buf_int_i_ptr = _buf_int_i_ptr + ofs;
	int32_t *      buf_int_o_ptr = _buf_int_o_ptr + ofs;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Copies acquired data to input buffer

#if defined (mfx_adrv_DPvabI2sDma_USE_SIMD)
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
#else // mfx_adrv_DPvabI2sDma_USE_SIMD
	for (int pos = 0; pos < _block_size; ++pos)
	{
		const int32_t  xl_int = buf_int_i_ptr [pos * 2    ];
		const int32_t  xr_int = buf_int_i_ptr [pos * 2 + 1];
		const float    xl_flt = xl_int * scale_i;
		const float    xr_flt = xr_int * scale_i;
		buf_flt_i_ptr [                pos] = xl_flt;
		buf_flt_i_ptr [_block_size_a + pos] = xr_flt;
	}
#endif // mfx_adrv_DPvabI2sDma_USE_SIMD

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Processing

	_cb_ptr->process_block (dst_arr.data (), src_arr.data (), _block_size);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Copies produced data to the output buffer

#if defined (mfx_adrv_DPvabI2sDma_USE_SIMD)
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
#else // mfx_adrv_DPvabI2sDma_USE_SIMD
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
#endif // mfx_adrv_DPvabI2sDma_USE_SIMD
}



void	DPvabI2sDma::write_reg (uint8_t reg, uint8_t val) noexcept
{
	assert (_i2c_hnd != -1);

	::wiringPiI2CWriteReg8 (_i2c_hnd, reg, val);
}



double	DPvabI2sDma::read_rt_ratio () noexcept
{
	double         ratio   = 1; // Default or error
	int            ret_val = 0;

	long long      runtime = 0;
	if (ret_val == 0)
	{
		ret_val = read_value_from_file (
			runtime, "/proc/sys/kernel/sched_rt_runtime_us"
		);
	}

	// Negative runtime values mean 100 %
	if (ret_val == 0 && runtime >= 0)
	{
		long long      period = 0;
		ret_val = read_value_from_file (
			period, "/proc/sys/kernel/sched_rt_period_us"
		);
		if (ret_val == 0)
		{
			ratio = double (runtime) / double (period);
			ratio = fstb::limit (ratio, 0.0, 1.0);
		}
	}

	return ratio;
}



int	DPvabI2sDma::read_value_from_file (long long &val, const char *filename_0) noexcept
{
	assert (filename_0 != nullptr);

	int            ret_val = 0;

	FILE *         f_ptr = fstb::fopen_utf8 (filename_0, "r");
	if (f_ptr == nullptr)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		const int      nbr_read = fscanf (f_ptr, "%lld", &val);
		if (nbr_read != 1)
		{
			ret_val = -1;
		}
	}

	if (f_ptr != nullptr)
	{
		fclose (f_ptr);
		f_ptr = nullptr;
	}

	return ret_val;
}



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
