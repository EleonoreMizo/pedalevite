/*****************************************************************************

        TestSpectralFreeze.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/pi/sfreeze/Param.h"
#include "mfx/pi/sfreeze/SpectralFreeze.h"
#include "mfx/pi/sfreeze/SpectralFreezeDesc.h"
#include "mfx/FileOpWav.h"
#include "mfx/PluginPoolHostMini.h"
#include "test/PiProc.h"
#include "test/TestSpectralFreeze.h"

#include <memory>
#include <vector>

#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSpectralFreeze::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::pi::sfreeze::SpectralFreeze...\n");

	static const int  nbr_chn = 1;
	double         sample_freq = 44100;

	constexpr int  len        = 100000;
	constexpr int  pos_auto_1 = len / 4;
	constexpr auto f0         = 220; // Hz
	std::vector <std::vector <float> >  chn_arr (nbr_chn);
	chn_arr.resize (nbr_chn);
	chn_arr [0].resize (len);
	for (int pos = 0; pos < len; ++pos)
	{
		chn_arr [0] [pos] = 0.5f * sinf (float (pos * 2 * fstb::PI * f0 / sample_freq));
	}
	for (int chn_idx = 1; chn_idx < nbr_chn; ++chn_idx)
	{
		chn_arr [chn_idx] = chn_arr [1];
	}

	mfx::PluginPoolHostMini host;
	mfx::pi::sfreeze::SpectralFreeze plugin (host);
	const int      max_block_size = 64;
	int            latency = 0;
	PiProc         pi_proc;
	pi_proc.set_desc (
		std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
			std::make_shared <mfx::pi::sfreeze::SpectralFreezeDesc> ()
		)
	);
	pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
	size_t         pos = 0;
	float * const* dst_arr = pi_proc.use_buf_list_dst ();
	float * const* src_arr = pi_proc.use_buf_list_src ();
	mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
	pi_proc.reset_param ();
	constexpr auto base = int (mfx::pi::sfreeze::Param_SLOT_BASE);
	pi_proc.set_param_nat (base + mfx::pi::sfreeze::ParamSlot_FREEZE, 0.0);
	pi_proc.set_param_nat (base + mfx::pi::sfreeze::ParamSlot_GAIN  , 1.0);
	pi_proc.set_param_nat (mfx::pi::sfreeze::Param_DMODE, double (mfx::pi::sfreeze::DMode_CUT));

	do
	{
		if (pos >= pos_auto_1 && pos < pos_auto_1 + max_block_size)
		{
			pi_proc.set_param_nat (base + mfx::pi::sfreeze::ParamSlot_FREEZE, 1.0);
		}

		const int      block_len =
			int (std::min (len - pos, size_t (max_block_size)));

		proc_info._nbr_spl = block_len;

		for (int chn = 0; chn < nbr_chn; ++chn)
		{
			memcpy (
				src_arr [chn],
				&chn_arr [chn] [pos],
				block_len * sizeof (src_arr [chn] [0])
			);
		}

		plugin.process_block (proc_info);

		for (int chn = 0; chn < nbr_chn; ++chn)
		{
			memcpy (
				&chn_arr [chn] [pos],
				dst_arr [chn],
				block_len * sizeof (chn_arr [chn] [pos])
			);
		}

		pi_proc.reset_param ();
		pos += block_len;
	}
	while (pos < len);

	ret_val = mfx::FileOpWav::save (
		"results/spectralfreeze0.wav", chn_arr, sample_freq, 1
	);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
