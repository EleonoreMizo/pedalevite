/*****************************************************************************

        Testadrv.cpp
        Author: Laurent de Soras, 2021

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



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#if fstb_SYS == fstb_SYS_WIN
	#include "mfx/adrv/DAsio.h"
	#define Testadrv_DRV_T mfx::adrv::DAsio
	constexpr int Testadrv_chn_idx_i = 2;
	constexpr int Testadrv_chn_idx_o = 0;
#elif fstb_SYS == fstb_SYS_LINUX
	#include "mfx/adrv/DPvabI2sDma.h"
	#include "mfx/hw/bcm2837dma.h"
	#include "mfx/hw/bcm2837pcm.h"
	#include "mfx/hw/ThreadLinux.h"
	#define Testadrv_DRV_T mfx::adrv::DPvabI2sDma
	constexpr int Testadrv_chn_idx_i = 0;
	constexpr int Testadrv_chn_idx_o = 0;
#else
	#error unsupported system
#endif
#include "test/Testadrv.h"

#if fstb_SYS == fstb_SYS_LINUX
	#include <sys/time.h>
#endif

#include <array>
#include <algorithm>
#include <chrono>
#include <thread>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <ctime>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Testadrv::perform_test ()
{
	int            ret_val = 0;

#define Testadrv_EXPAND_MACRO( x) #x
#define Testadrv_PRINT_MACRO( x) Testadrv_EXPAND_MACRO (x)
	printf ("Testing " Testadrv_PRINT_MACRO (Testadrv_DRV_T) "...\n");
#undef Testadrv_PRINT_MACRO
#undef Testadrv_EXPAND_MACRO

#if 0 && fstb_SYS == fstb_SYS_LINUX
	printf ("\nPreliminary test: nanosleep() resolution.\n");

	std::thread nt (&test_nanosleep);
	mfx::hw::ThreadLinux::set_priority (nt, 0, nullptr);
	nt.join ();

	printf ("\nDriver test beginning now.\n");
#endif

	// Filters out repeated errors flooding the output
	constexpr bool filter_diarrhea_flag = false;

	Testadrv_DRV_T snd_drv;
	AdrvCallback   callback;
	auto           sample_freq    = double {};
	auto           max_block_size = int {};
	ret_val = snd_drv.init (
		sample_freq,
		max_block_size,
		callback,
		nullptr,
		Testadrv_chn_idx_i,
		Testadrv_chn_idx_o
	);

	if (ret_val == 0)
	{
		ret_val = snd_drv.start ();
	}

	if (ret_val == 0)
	{
		std::chrono::steady_clock clk;
		const auto     t_beg = clk.now ();
		auto           t_cur = t_beg;
		const auto     d_max = std::chrono::duration <double> { 10.0 }; // s
		std::chrono::duration <double> t_dif = t_cur - t_beg;
		int            dropout_count = 0;
#if 1 && fstb_SYS == fstb_SYS_LINUX
		int            pcmerr_count  = 0;
#endif
		while (! callback.is_exit_requested () && t_dif < d_max)
		{
#if 0 && fstb_SYS == fstb_SYS_LINUX
			const auto     dma_pos = snd_drv.get_dma_pos ();
			printf ("DMA: buf %d, spl %3d\n", dma_pos._buf, dma_pos._frame);
#endif

#if 1 && fstb_SYS == fstb_SYS_LINUX
			const auto     cs_a    = snd_drv.get_pcm_status ();
			using namespace mfx::hw::bcm2837pcm;
			const bool     rxerr_flag = ((cs_a & _cs_a_rxerr) != 0);
			const bool     txerr_flag = ((cs_a & _cs_a_txerr) != 0);
			if (rxerr_flag || txerr_flag)
			{
				const bool     rxd = ((cs_a & _cs_a_rxd) != 0);
				const bool     txd = ((cs_a & _cs_a_txd) != 0);
				const bool     rxr = ((cs_a & _cs_a_rxr) != 0);
				const bool     txw = ((cs_a & _cs_a_txw) != 0);
				++ pcmerr_count;
				if (! filter_diarrhea_flag || fstb::is_pow_2 (pcmerr_count))
				{
					printf (
						"PCM errors: %s %s %d %s %s %s %s\n",
						rxerr_flag ? "RX" : "  ",
						txerr_flag ? "TX" : "  ",
						pcmerr_count,
						rxd ? "rxd" : "   ",
						txd ? "txd" : "   ",
						rxr ? "rxr" : "   ",
						txw ? "txw" : "   "
					);

#if 0
					const int buf_proc_cnt = snd_drv.get_buf_proc_cnt ();
					printf ("Buffers processed: %d\n", buf_proc_cnt);

					const auto content = snd_drv.dump_buf_in ();
					for (const auto &buf : content)
					{
						for (int frame_idx = 0; frame_idx < snd_drv._block_size; ++frame_idx)
						{
							for (int chn_idx = 0; chn_idx < snd_drv._nbr_chn; ++chn_idx)
							{
								const int      pos =
									frame_idx * snd_drv._nbr_chn + chn_idx;
								const auto     spl = buf [pos];
								printf ("%06X ", spl & 0xFFFFFF);
							}
							printf ("  ");
						}
						printf ("\n");
					}
#endif
				}
			}
#endif // fstb_SYS_LINUX

			if (callback.check_dropout ())
			{
				++ dropout_count;
				if (! filter_diarrhea_flag || fstb::is_pow_2 (dropout_count))
				{
					printf ("Dropout! %d\n", dropout_count);
				}
			}

			std::this_thread::sleep_for (std::chrono::milliseconds { 1 });
			t_cur = clk.now ();
			t_dif = t_cur - t_beg;
		}
	}

	snd_drv.stop ();

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Testadrv::AdrvCallback::is_exit_requested () const
{
	return _request_exit_flag;
}



bool	Testadrv::AdrvCallback::check_dropout ()
{
	return _dropout_flag.exchange (false);
}



void	Testadrv::AdrvCallback::do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl) noexcept
{
	assert (dst_arr != nullptr);
	assert (src_arr != nullptr);
	assert (nbr_spl > 0);

	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		std::copy (
			src_arr [chn_cnt],
			src_arr [chn_cnt] + nbr_spl,
			dst_arr [chn_cnt]
		);
	}
}



void	Testadrv::AdrvCallback::do_notify_dropout () noexcept
{
	_dropout_flag.store (true);
}



void	Testadrv::AdrvCallback::do_request_exit () noexcept
{
	_request_exit_flag.store (true);
}



void	Testadrv::test_nanosleep ()
{
#if fstb_SYS == fstb_SYS_LINUX
	const int nbr_sec = 300;
	printf ("Please wait, %d seconds per test series. Time results in microseconds.\n", nbr_sec);
	constexpr std::array <uint64_t, 7> us_tgt_arr {
		1, 25, 50, 75, 100, 150, 200
	};
	for (auto us_tgt : us_tgt_arr)
	{
		uint64_t  us_max = 0;
		uint64_t  us_min = UINT64_MAX;
		uint64_t  us_sum = 0;
		uint64_t  us_sum2 = 0;
		timespec slp;
		slp.tv_sec  = 0;
		slp.tv_nsec = us_tgt * 1000;
		timeval t_ref;
		timeval t_end;
		gettimeofday (&t_ref, nullptr);
		uint64_t nbr_iter = 0;
		do
		{
			constexpr int nbr_iter_one_loop = 10000;
			for (int iter_cnt = 0; iter_cnt < nbr_iter_one_loop; ++iter_cnt)
			{
				timeval t_beg;
				gettimeofday (&t_beg, nullptr);

				nanosleep (&slp, nullptr);

				gettimeofday (&t_end, nullptr);

				const uint64_t us_dif = (t_end.tv_sec - t_beg.tv_sec) * 1'000'000 + t_end.tv_usec - t_beg.tv_usec;
				us_sum  += us_dif;
				us_sum2 += us_dif * us_dif;
				us_min   = std::min (us_min, us_dif);
				us_max   = std::max (us_max, us_dif);
			}
			nbr_iter += nbr_iter_one_loop;
		}
		while (t_end.tv_sec - t_ref.tv_sec < nbr_sec);

		const double us_avg = double (us_sum) / double (nbr_iter);
		const double us_dev = sqrt (double (us_sum2) / double (nbr_iter) - us_avg * us_avg);
		const double us_dif = us_avg - double (us_tgt);

		printf (
			"Target: %4lld, avg: %6.1f, min: %4lld, max: %4lld, stddev: %5.1f, ovrhd: %5.1f\n",
			static_cast <long long> (us_tgt),
			us_avg,
			static_cast <long long> (us_min),
			static_cast <long long> (us_max),
			us_dev,
			us_dif
		);
	}
#endif
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
