/*****************************************************************************

        DPvabI2sDma.h
        Author: Laurent de Soras, 2019

PCM/I2S interface in DMA mode, but without interrupts.
This driver specifically targets the Pedale Vite audio board, using a
Cirrus Logic CS4272 codec and a custom master clock providing 44.1 and 48 kHz
sampling rates.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DPvabI2sDma_HEADER_INCLUDED)
#define mfx_adrv_DPvabI2sDma_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/adrv/DriverInterface.h"
#include "mfx/hw/GpioAccess.h"
#include "mfx/hw/GpioPin.h"
#include "mfx/hw/Higepio.h"
#include "mfx/hw/MmapPtr.h"
#include "mfx/hw/RPiDmaBlocks.h"

#include <array>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace adrv
{



class DPvabI2sDma final
:	public DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Number of input and output channels. We don't actually support anything
	// but stereo streams.
	static constexpr int _nbr_chn      =  2;

	// Number of meaningful bits per sample
	static constexpr int _resol        = 24;

	// Transmitted bits per sample in I2S frames (meaningful + padding)
	static constexpr int _bits_per_chn = 32;

	// I2S data is one clock cycle after the LRCLK edge. >= 0
	static constexpr int _transfer_lag =  1;

	// Buffer size in samples, for processing. Could be set as a variable
	// instead of a constant.
	static constexpr int _block_size   = 64;

	// Custom setting of sampling rate: 0 = 48 kHz, 1 = 44.1 kHz
	static constexpr int _fs_code      =  1;

	// I2C address to drive the CS4272 codec
	static constexpr int _i2c_addr     = 0x10 + 0;

	// DMA channel. 8 is a DMA lite, which is enough
	static constexpr int _dma_chn      =  8;

	// Double-buffering. Do not change this value, the constant just makes the
	// calculations meaningful.
	static constexpr int _nbr_buf      =  2;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// GPIO pins (BCM numbering, not WiringPi)

	// W - Reset pin (0 = reset, 1 = working)
	static constexpr int _pin_rst      = hw::GpioPin::_snd_reset;

	// W - Sampling rate selection (see _fs_code)
	static constexpr int _pin_freq     = hw::GpioPin::_snd_sfreq;

	// R - I2S bit clock
	static constexpr int _pin_bclk     = hw::GpioPin::_snd_bclk;

	// R - I2S word selection (0 = L, 1 = R)
	static constexpr int _pin_lrck     = hw::GpioPin::_snd_lrck;

	// R - I2S data input (codec to CPU)
	static constexpr int _pin_din      = hw::GpioPin::_snd_din;

	// W - I2S data output (CPU to codec)
	static constexpr int _pin_dout     = hw::GpioPin::_snd_dout;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	explicit       DPvabI2sDma (hw::Higepio &io);
	virtual        ~DPvabI2sDma ();

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Debugging stuff

	class PosIO
	{
	public:
		int            _buf   = 0;
		int            _frame = 0;
		int            _chn   = 0;
	};

	typedef std::array <
		std::array <int32_t, _block_size * _nbr_chn>,
		_nbr_buf
	> BufferDump;

	PosIO          get_dma_pos () const noexcept;
	uint32_t       get_pcm_status () const noexcept;
	BufferDump     dump_buf_in () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// DriverInterface
	int            do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) noexcept final;
	int            do_start () noexcept final;
	int            do_stop () noexcept final;
	void           do_restart () noexcept final;
	std::string    do_get_last_error () const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef int32_t SplType;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufFltAlign;

	static constexpr int _block_size_a = (_block_size + 3) & ~3; // Aligned block size

	// Overhead for nanosleep(), in ns. This is a result from tests.
	// The maximum is much higher, especially as we don't run on a RT kernel,
	// but the important thing here is the averge value + standard deviation,
	// because the positive deviation from a block may be recovered with the
	// next blocks.
	static constexpr int64_t   _nsleep_ovrhd_avg = 63'000;

	enum Dir
	{
		Dir_R = 0, // Read / receive
		Dir_W,     // Write / transmit
		Dir_NBR_ELT
	};

	// Number of sample frames written in advance in the I2S TX queue. >= 1
	// After initial filling, we sync on the RX queue (we try to keep it always
	// empty) and write the same number of read samples on the TX queue.
	static constexpr int _prefill = 1;

	enum State
	{
		State_STOP = 0,
		State_RUN,

		State_NBR_ELT
	};

	void           main_loop () noexcept;
	void           build_dma_ctrl_block_list () noexcept;
	void           process_block (int buf_idx) noexcept;
	inline void    write_reg (uint8_t reg, uint8_t val) noexcept;

	static double  read_rt_ratio () noexcept;
	static int     read_value_from_file (long long &val, const char *filename_0) noexcept;

	// Virtual base address for the peripherals
	uint32_t       _periph_base_addr;

	// Virtual base address for the PCM registers
	hw::MmapPtr    _pcm_mptr;

	// Virtual base address for the DMA registers
	hw::MmapPtr    _dma_mptr;

	// Object to read and write the GPIO pins
	hw::GpioAccess _gpio;

	// Handle on I2C communications
	hw::Higepio::I2c
	               _i2c;

	// Audio processing callback. 0 = not set
	CbInterface *  _cb_ptr;

	// Current state of the driver
	State          _state;

	// Main loop is required to exit ASAP.
	std::atomic <bool>
	               _exit_flag;

	// Current buffer for I2S transfer, 0 or 1. The other buffer is for processing
	int            _cur_buf;

	// Double input and output buffers (interleaved stereo), integer data.
	// 0 = not set
	SplType *      _buf_int_i_ptr;
	SplType *      _buf_int_o_ptr;

	// Input and output buffer (soundchip to software processing), dual mono
	BufFltAlign    _buf_flt_i;
	BufFltAlign    _buf_flt_o;

	// Processing thread, running at a real-time priority
	std::thread    _thread_main;

	// Control blocks for the DMA and integer buffers for sample input/output
	// The buffer (extra-bytes) layout is: In_0, In_1, Out_0, Out_1.
	// Each sub-buffer is 16-byte aligned and contains interleaved-stereo
	// sample frames
	std::unique_ptr <hw::RPiDmaBlocks>
	               _dma_uptr;

	// Physical address of the DMA block for the beginning of each buffer.
	// This helps to find where we are when the DMA is running.
	std::array <uint32_t, _nbr_buf>
	               _dma_buf_beg_arr;

	// Duration of a sample frame, in ns. We use it to calculate how long we
	// can sleep before the next block.
	int64_t        _spl_dur_ns;

	// Minimum time we have to give back to the system per block, in ns.
	// This value is not used at the moment.
	int64_t        _min_dur_ns;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DPvabI2sDma ()                               = delete;
	               DPvabI2sDma (const DPvabI2sDma &other)       = delete;
	               DPvabI2sDma (DPvabI2sDma &&other)            = delete;
	DPvabI2sDma &  operator = (const DPvabI2sDma &other)        = delete;
	DPvabI2sDma &  operator = (DPvabI2sDma &&other)             = delete;
	bool           operator == (const DPvabI2sDma &other) const = delete;
	bool           operator != (const DPvabI2sDma &other) const = delete;

}; // class DPvabI2sDma



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DPvabI2sDma.hpp"



#endif   // mfx_adrv_DPvabI2sDma_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
