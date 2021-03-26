/*****************************************************************************

        DPvabI2sDma.h
        Author: Laurent de Soras, 2019

PCM/I2S interface in DMA mode

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

	static const int  _nbr_chn      =  2;
	static const int  _bits_per_chn = 32; // Transmitted bits (meaningful + padding)
	static const int  _resol        = 24; // Number of meaningful bits.
	static const int  _transfer_lag =  1; // I2S data is one clock cycle after the LRCLK edge. >= 0
	static const int  _block_size   = 64; // Buffer size in samples, for processing
	static const int  _fs_code      =  1; // Sampling rate: 0 = 48 kHz, 1 = 44.1 kHz
	static const int  _i2c_addr     = 0x10 + 0;
	static const int  _dma_chn      =  8; // DMA channel. 8 is a DMA lite, which is enough

	// GPIO pins (BCM numbering, not WiringPi)
	static const int  _pin_rst      =  5; // W - Reset pin (0 = reset, 1 = working)
	static const int  _pin_freq     =  6; // W - Frequency selection (0 = 48 kHz, 1 = 44.1 kHz)
	static const int  _pin_bclk     = 18; // R - I2S bit clock
	static const int  _pin_lrck     = 19; // R - I2S word selection (0 = L, 1 = R)
	static const int  _pin_din      = 20; // R - I2S data input (codec to cpu)
	static const int  _pin_dout     = 21; // W - I2S data output (cpu to codec)

	static const int  _nbr_buf      =  2; // Double-buffering. Do not change this value, the constant just makes the calculations meaningful

	class PosIO
	{
	public:
		int            _buf   = 0;
		int            _frame = 0;
		int            _chn   = 0;
	};

	               DPvabI2sDma ();
	virtual        ~DPvabI2sDma ();

	// Debugging functions
	PosIO          get_dma_pos () const;
	uint32_t       get_pcm_status () const;
	std::array <std::array <int32_t, _block_size * _nbr_chn>, _nbr_buf>
	               dump_buf_in () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// DriverInterface
	int            do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) final;
	int            do_start () final;
	int            do_stop () final;
	void           do_restart () final;
	std::string    do_get_last_error () const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef int32_t SplType;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufFltAlign;

	static const int  _block_size_a = (_block_size + 3) & ~3; // Aligned block size
	static const int64_t _nsleep_ovrhd_min =  6'000; // Minimum overhead for nanosleep(), in ns
	static const int64_t _nsleep_ovrhd_max = 50'000; // Maximum overhead for nanosleep(), in ns

	enum Dir
	{
		Dir_R = 0, // Read / receive
		Dir_W,     // Write / transmit
		Dir_NBR_ELT
	};

	// Number of sample frames written in advance in the I2S TX queue. >= 1
	// After initial filling, we sync on the RX queue (we try to keep it always
	// empty) and write the same number of read samples on the TX queue.
	static const int  _prefill = 10;
	static_assert (
		_prefill * _nbr_chn <= 64,
		"Prefill should fit in the TX FIFO."
	);

	enum State
	{
		State_STOP = 0,
		State_RUN,

		State_NBR_ELT
	};

	void           close_i2c ();
	void           main_loop ();
	void           build_dma_ctrl_block_list ();
	void           process_block (int buf_idx);
	inline void    write_reg (uint8_t reg, uint8_t val);

	static double  read_rt_ratio ();
	static int     read_value_from_file (long long &val, const char *filename_0);

	uint32_t       _periph_base_addr;   // Virtual base address for the peripherals
	hw::MmapPtr    _pcm_mptr;           // Virtual base address for the PCM registers
	hw::GpioAccess _gpio;
	int            _i2c_hnd;
	CbInterface *  _cb_ptr;       // 0 = not set
	State          _state;

	std::atomic <bool>            // Main loop is required to exit ASAP.
	               _exit_flag;
	int            _cur_buf;      // Current buffer for I2S transfer, 0 or 1. The other buffer is for processing
	SplType *      _buf_int_i_ptr; // Double input buffer (interleaved stereo), integer data. 0 = not set
	SplType *      _buf_int_o_ptr; // Double output buffer (interleaved stereo), integer data. 0 = not set
	BufFltAlign    _buf_flt_i;    // Input buffer (soundchip to software processing), dual mono
	BufFltAlign    _buf_flt_o;    // Output buffer (software processing to soundchip), dual mono
	std::thread    _thread_main;
	std::condition_variable
	               _blk_proc_cv;

	// Control blocks for the DMA and integer buffers for sample input/output
	// The buffer (extra-bytes) layout is: In_0, In_1, Out_0, Out_1.
	// Each sub-buffer is 16-byte aligned and contains interleaved-stereo
	// sample frames
	std::unique_ptr <hw::RPiDmaBlocks>
	               _dma_uptr;
	hw::MmapPtr    _dma_reg;
	std::array <uint32_t, _nbr_buf> // Physical address of the DMA block for the beginning of each buffer
	               _dma_buf_beg_arr;

	int64_t        _spl_dur_ns;     // Duration of a sample frame, in ns
	int64_t        _min_dur_ns;     // Minimum time we have to give back to the system per block, in ns. This value is not used at the moment.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

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
