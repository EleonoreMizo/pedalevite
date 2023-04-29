/*****************************************************************************

        DPvabI2s.h
        Author: Laurent de Soras, 2019

PCM/I2S interface in polled mode

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DPvabI2s_HEADER_INCLUDED)
#define mfx_adrv_DPvabI2s_HEADER_INCLUDED

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

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace adrv
{



class DPvabI2s final
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

	// GPIO pins (BCM numbering, not WiringPi)
	static const int  _pin_rst      = hw::GpioPin::_snd_reset; // W - Reset pin (0 = reset, 1 = working)
	static const int  _pin_freq     = hw::GpioPin::_snd_sfreq; // W - Frequency selection (0 = 48 kHz, 1 = 44.1 kHz)
	static const int  _pin_bclk     = hw::GpioPin::_snd_bclk;  // R - I2S bit clock
	static const int  _pin_lrck     = hw::GpioPin::_snd_lrck;  // R - I2S word selection (0 = L, 1 = R)
	static const int  _pin_din      = hw::GpioPin::_snd_din;   // R - I2S data input (codec to cpu)
	static const int  _pin_dout     = hw::GpioPin::_snd_dout;  // W - I2S data output (cpu to codec)

	explicit       DPvabI2s (hw::Higepio &io);
	               ~DPvabI2s () = default;



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

	typedef std::vector <int32_t, fstb::AllocAlign <int32_t, 16> > BufIntAlign;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufFltAlign;

	static const int  _block_size_a = (_block_size + 3) & ~3; // Aligned block size

	// Number of sample frames written in advance in the I2S TX queue. >= 1
	// After initial filling, we sync on the RX queue (we try to keep it always
	// empty) and write the same number of read samples on the TX queue.
	static const int  _prefill = 4;
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

	void           main_loop () noexcept;
	void           proc_loop ();
	inline void    write_reg (uint8_t reg, uint8_t val) noexcept;

	uint32_t       _periph_base_addr;   // Virtual base address for the peripherals
	hw::MmapPtr    _pcm_mptr;           // Virtual base address for the PCM registers
	hw::GpioAccess _gpio;
	hw::Higepio::I2c
	               _i2c;
	CbInterface *  _cb_ptr;       // 0 = not set
	State          _state;

	std::atomic <bool>            // Main loop is required to exit ASAP.
	               _exit_flag;
	std::atomic <bool>            // Processing loop is required to exit ASAP.
	               _proc_ex_flag;
	std::atomic <int>             // Current buffer for I2S transfer, 0 or 1. The other buffer is for processing
	               _cur_buf;
	BufIntAlign    _buf_int_i;    // Double input buffer (interleaved stereo), integer data
	BufIntAlign    _buf_int_o;    // Double output buffer (interleaved stereo), integer data
	BufFltAlign    _buf_flt_i;    // Input buffer (soundchip to software processing), dual mono
	BufFltAlign    _buf_flt_o;    // Output buffer (software processing to soundchip), dual mono
	std::thread    _thread_main;
	std::mutex     _blk_proc_mtx;
	std::condition_variable
	               _blk_proc_cv;
	std::atomic <bool>
	               _proc_now_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DPvabI2s ()                               = delete;
	               DPvabI2s (const DPvabI2s &other)          = delete;
	               DPvabI2s (DPvabI2s &&other)               = delete;
	DPvabI2s &     operator = (const DPvabI2s &other)        = delete;
	DPvabI2s &     operator = (DPvabI2s &&other)             = delete;
	bool           operator == (const DPvabI2s &other) const = delete;
	bool           operator != (const DPvabI2s &other) const = delete;

}; // class DPvabI2s



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DPvabI2s.hpp"



#endif   // mfx_adrv_DPvabI2s_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
