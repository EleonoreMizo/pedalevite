/*****************************************************************************

        DPvabDirect.h
        Author: Laurent de Soras, 2019

Driver for the Pedale Vite Audio Board
Uses explicit pin reading/writing

This driver doesn't work satisfactorily enough. The thread is often inter-
rupted and loses synchronisation, causing glitches in input and output audio
signals. Moreover, the GPIO timings read on a scope look too tight to be
really safe.

It would be necessary to use a dedicated interface on the SoC, like the
PCM/I2S one (BCM2835 doc, chap. 8, p. 119).

It may be possible to use raw DMA transfers, but synchronisation looks
complicated.

However this driver would probably work on a faster system and a real-time
kernel.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DPvabDirect_HEADER_INCLUDED)
#define mfx_adrv_DPvabDirect_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



#undef mfx_adrv_DPvabDirect_TEST

// It's better to make the Control Port Mode as default. Stand-alone mode
// has issues because some stuffs cannot be changed once the codec is started.
#define mfx_adrv_DPvabDirect_CTRL_PORT_MODE



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/adrv/DriverInterface.h"
#include "mfx/hw/GpioPin.h"

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



class DPvabDirect final
:	public DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_chn      =  2;
	static const int  _bits_per_chn = 32; // Transmitted bits (meaningful + padding)
	static const int  _resol        = 24; // Number of meaningful bits.
	static const int  _transfer_lag =  1; // I2S data is one clock cycle after the LRCLK edge. >= 0
#if defined (mfx_adrv_DPvabDirect_TEST)
	static const int  _block_size   =  4; // Buffer size in samples, for processing
#else
	static const int  _block_size   = 64; // Buffer size in samples, for processing
#endif
	static const int  _fs_code      =  1; // Sampling rate: 0 = 48 kHz, 1 = 44.1 kHz
#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE)
	static const int  _i2c_addr     = 0x10 + 0;
#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE

	// GPIO pins (BCM numbering, not WiringPi)
	static const int  _pin_rst      = hw::GpioPin::_snd_reset; // W - Reset pin (0 = reset, 1 = working)
	static const int  _pin_freq     = hw::GpioPin::_snd_sfreq; // W - Frequency selection (0 = 48 kHz, 1 = 44.1 kHz)
	static const int  _pin_bclk     = hw::GpioPin::_snd_bclk;  // R - I2S bit clock
	static const int  _pin_lrck     = hw::GpioPin::_snd_lrck;  // R - I2S word selection (0 = L, 1 = R)
	static const int  _pin_din      = hw::GpioPin::_snd_din;   // R - I2S data input (codec to cpu)
	static const int  _pin_dout     = hw::GpioPin::_snd_dout;  // W - I2S data output (cpu to codec)

	               DPvabDirect ();
	virtual        ~DPvabDirect ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::adrv::DriverInterface
	int            do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) noexcept final;
	int            do_start () noexcept final;
	int            do_stop () noexcept final;
	void           do_restart () noexcept final;
	std::string    do_get_last_error () const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <int32_t, fstb::AllocAlign <int32_t, 16> > BufIntAlign;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufFltAlign;

	class GpioAccess
	{
	public:
		// Actually it is 54, but we use only the 32 first ones, and it
		// simplifies addressing.
		static const int  _nbr_gpio = 32;

		enum PinFnc
		{
			PinFnc_IN = 0,
			PinFnc_OUT,
			PinFnc_ALT_5,
			PinFnc_ALT_4,
			PinFnc_ALT_0,
			PinFnc_ALT_1,
			PinFnc_ALT_2,
			PinFnc_ALT_3,

			PinFnc_NBR_ELT,
		};
		enum Pull
		{
			Pull_NONE = 0,
			Pull_DOWN,
			Pull_UP,

			Pull_NBR_ELT
		};
		               GpioAccess ();
		virtual        ~GpioAccess () = default;
		inline void    set_fnc (int gpio, PinFnc fnc) const noexcept;
		inline void    clear (int gpio) const noexcept;
		inline void    set (int gpio) const noexcept;
		inline void    write (int gpio, int val) const noexcept;
		inline int     read (int gpio) const noexcept;
		inline int     read_cached (int gpio) const noexcept;
		inline void    pull (int gpio, Pull p) const noexcept;
#if defined (mfx_adrv_DPvabDirect_TEST)
		void           run ();
		void           stop ();
#endif
	private:
#if defined (mfx_adrv_DPvabDirect_TEST)
		void           fake_data_loop () noexcept;
		void           set_fake_bit (int gpio, int val) noexcept;
		void           print_gpio () const noexcept;
#else
		volatile uint32_t *
		               map_periph (uint32_t base, uint32_t len);
#endif

		static const uint32_t
		               _ofs_gpio  =   0x200000U;
		static const uint32_t                   // Bytes
		               _len_gpio  = 1024 * sizeof (uint32_t);
		static const uint32_t                   // In 32-bit words. W
		               _ofs_reg_fnc  =  0;
		static const uint32_t                   // In 32-bit words. W
		               _ofs_reg_set  =  7;
		static const uint32_t                   // In 32-bit words. W
		               _ofs_reg_clr  = 10;
		static const uint32_t                   // In 32-bit words. R
		               _ofs_reg_lvl  = 13;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_evt  = 16;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_ris  = 19;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_fal  = 22;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_hi   = 25;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_lo   = 28;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_aris = 31;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_afal = 34;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_pull = 37;
		static const uint32_t                   // In 32-bit words. R/W
		               _ofs_reg_pclk = 38;
		static const int  _fnc_field_size = 3;

#if defined (mfx_adrv_DPvabDirect_TEST)
		std::atomic <bool>
		               _quit_flag = { false };
		mutable std::atomic <uint32_t>
		               _gpio_state = { 0 };
		mutable std::atomic <uint32_t>          // GPIO ports open for reading (from the CPU PoV)
		               _gpio_read = { 0 };
		mutable std::atomic <uint32_t>          // GPIO ports open for writing (from the CPU PoV)
		               _gpio_write = { 0 };
		const int      _hclk_dur = 100;         // Half-clock time, ms
		std::thread    _test_thread;
#else
		uint32_t       _periph_addr;            // 0x3F000000 on Pi 3, 0x20000000 on Pi 1
		volatile uint32_t *
		               _gpio_ptr;
#endif
		mutable uint32_t
		               _last_read;

		static inline void
		               find_addr_fnc (int &ofs_reg, int &shf_bit, int gpio) noexcept;
	}; // class GpioAccess

	enum State
	{
		State_STOP = 0,
		State_RUN,

		State_NBR_ELT
	};

	static const int  _bclk_timeout = 1'000'000'000;          // Number of loops
	static const int  _block_size_a = (_block_size + 3) & ~3; // Aligned block size

	void           main_loop () noexcept;
	inline void    sync_to_bclk_edge (int dir) noexcept;
	void           proc_loop ();

#if ! defined (mfx_adrv_DPvabDirect_TEST)
	#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE)
	void           write_reg (uint8_t reg, uint8_t val) noexcept;
	#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE
#endif

	GpioAccess     _gpio;
	CbInterface *  _cb_ptr;       // 0 = not set

	// Main loop private data
	int            _lrclk_cur;    // Channel, current state of the LRCK pin
	int            _btclk_cur;    // Current state of the BCLK pin
	int            _clk_cnt;      // Current clock index after LRCK transition,
	                              // starting from 0. Actual content lags _transfer_lag
	                              // clocks behind the LRCK edge.
	int            _bit_pos;      // Position of the bit being written or read. Should be < _resol. Negative = done.
	int            _buf_pos;      // Word position within the buffers (samples)
	int32_t        _content_r;    // Content being read. Bits are introduced
	                              // from the right up to _resol.
	int32_t        _content_w;    // Content being written.
	bool           _timeout_flag; // Cannot get signal from the soundchip
	bool           _resync_flag;  // Engine just started (need to sync all clocks)
	// End of main loop private data

	std::atomic <bool>            // Main loop is required to exit ASAP.
	               _exit_flag;
	std::atomic <bool>            // Processing loop is required to exit ASAP.
	               _proc_ex_flag;
	std::atomic <bool>            // Something went wrong (sync error), but it looks recoverable
	               _syncerr_flag;
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

	State          _state;

#if defined (mfx_adrv_DPvabDirect_CTRL_PORT_MODE) && ! defined (mfx_adrv_DPvabDirect_TEST)
	int            _i2c_hnd;
#endif // mfx_adrv_DPvabDirect_CTRL_PORT_MODE



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DPvabDirect (const DPvabDirect &other)       = delete;
	               DPvabDirect (DPvabDirect &&other)            = delete;
	DPvabDirect &  operator = (const DPvabDirect &other)        = delete;
	DPvabDirect &  operator = (DPvabDirect &&other)             = delete;
	bool           operator == (const DPvabDirect &other) const = delete;
	bool           operator != (const DPvabDirect &other) const = delete;

}; // class DPvabDirect



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DPvabDirect.hpp"



#endif   // mfx_adrv_DPvabDirect_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
