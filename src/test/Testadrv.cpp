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
#if fstb_SYS == fstb_SYS_WIN
	#include "mfx/adrv/DAsio.h"
	#define Testadrv_DRV_T mfx::adrv::DAsio
	constexpr int Testadrv_chn_idx_i = 2;
	constexpr int Testadrv_chn_idx_o = 0;
#elif fstb_SYS == fstb_SYS_LINUX
	#include "mfx/adrv/DPvabI2s.h"
	#define Testadrv_DRV_T mfx::adrv::DPvabI2s
	constexpr int Testadrv_chn_idx_i = 0;
	constexpr int Testadrv_chn_idx_o = 0;
#else
	#error unsupported system
#endif
#include "test/Testadrv.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Testadrv::perform_test ()
{
	int            ret_val = 0;

#define Testadrv_PRINT_MACRO( x) #x
	printf ("Testing " Testadrv_PRINT_MACRO (Testadrv_DRV_T) "...\n");
#undef Testadrv_PRINT_MACRO

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
		while (! callback.is_exit_requested () && t_dif < d_max)
		{
			if (callback.check_dropout ())
			{
				printf ("Dropout!\n");
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



void	Testadrv::AdrvCallback::do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
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



void	Testadrv::AdrvCallback::do_notify_dropout ()
{
	_dropout_flag.store (true);
}



void	Testadrv::AdrvCallback::do_request_exit ()
{
	_request_exit_flag.store (true);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
