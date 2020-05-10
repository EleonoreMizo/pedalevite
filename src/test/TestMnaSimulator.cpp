/*****************************************************************************

        TestMnaSimulator.cpp
        Author: Laurent de Soras, 2020

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

#include "fstb/fnc.h"
#include "test/TestMnaSimulator.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/va/mna/PartBjt.h"
#include "mfx/dsp/va/mna/PartCapacitor.h"
#include "mfx/dsp/va/mna/PartDiodeAntipar.h"
#include "mfx/dsp/va/mna/PartResistor.h"
#include "mfx/dsp/va/mna/PartSrcVoltage.h"
#include "mfx/dsp/va/mna/PartVccs.h"
#include "mfx/dsp/va/mna/PartVcvs.h"
#include "mfx/FileOpWav.h"

#include <memory>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestMnaSimulator::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::va::mna::Simulator...\n");

	const double   sample_freq = 44100;
	const int      ssin_len = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (ssin_len);

	// Sweeping sine
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), ssin_len);

	// Sawtooth
	const int      saw_len = fstb::round_int (sample_freq * 1);
	for (int o = -3; o < 7; ++o)
	{
		const double   freq = 220 * pow (2.0, o + 3 / 12.0);
		gen_saw (src, sample_freq, freq, saw_len);
	}

	const int      len = int (src.size ());
	std::vector <float>  dst (len);

	mfx::dsp::va::mna::Simulator  mna;

#if 0
	// Simple resistor bridge
	enum
	{
		no_src = 1,
		no_dst
	};
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0
	);
	auto  r1_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_src, no_dst, 1000
	);
	auto  r2_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd, 1000
	);
	mna.add_part (src_v_sptr);
	mna.add_part (r1_sptr);
	mna.add_part (r2_sptr);
	float          gain = 1;
#elif 0
	// RC low-pass filter. Cutoff: 1.6 kHz
	enum
	{
		no_src = 1,
		no_dst
	};
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0.f
	);
	auto  r1_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_src, no_dst, 1000.f
	);
	auto  c1_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd, 100e-9f
	);
	mna.add_part (src_v_sptr);
	mna.add_part (r1_sptr);
	mna.add_part (c1_sptr);
	float          gain = 1;
#elif 0
	// RC low-pass filter + output buffer (attn = 0.5f). Cutoff: 1.6 kHz
	enum
	{
		no_src = 1,
		no_buf,
		no_dst
	};
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0.f
	);
	auto  r1_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_src, no_buf, 1000.f
	);
	auto  c1_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_buf, mfx::dsp::va::mna::PartInterface::_nid_gnd, 100e-9f
	);
	auto  buf_sptr = std::make_shared <mfx::dsp::va::mna::PartVcvs> (
		no_buf, mfx::dsp::va::mna::PartInterface::_nid_gnd,
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd,
		0.5f
	);
	mna.add_part (src_v_sptr);
	mna.add_part (r1_sptr);
	mna.add_part (c1_sptr);
	mna.add_part (buf_sptr);
	float          gain = 1;
#elif 0
	// RC low-pass filter + 2 diodes. Cutoff: 1.6 kHz
	enum
	{
		no_src = 1,
		no_dst
	};
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0.f
	);
	auto  r1_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_src, no_dst, 1000.f
	);
	auto  c1_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd, 100e-9f
	);
	auto  d1_sptr = std::make_shared <mfx::dsp::va::mna::PartDiodeAntipar> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd,
		0.1e-15f, 1.f, 0.1e-6f, 4.f
	);
	mna.add_part (src_v_sptr);
	mna.add_part (r1_sptr);
	mna.add_part (c1_sptr);
	mna.add_part (d1_sptr);
	float          gain = 10;
#elif 0
	// Distopia clipping section
	enum
	{
		no_src = 1,
		no_hpf,     // Between C9 and R14
		no_sqw,     // Between D2//D3 and R46+RV3
		no_dst
	};
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0.f
	);
	auto  r14_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_hpf, no_dst, 2200.f
	);
	auto  r46rv3_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_sqw, no_dst, 100.f // 100 - 10k
	);
	// Increasing C9 makes harder basses, with stronger even harmonics
	// (octave-like tone)
	auto  c9_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_src, no_hpf, 470e-9f
	);
	auto  c10_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd, 10e-9f
	);
	auto  d2_d3_sptr = std::make_shared <mfx::dsp::va::mna::PartDiodeAntipar> (
		no_hpf, no_sqw, 0.1e-15f, 4.f, 0.1e-15f, 1.f
	);
	auto  d4_d5_sptr = std::make_shared <mfx::dsp::va::mna::PartDiodeAntipar> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd,
		0.1e-15f, 1.f, 0.1e-6f, 4.f
	);
	mna.add_part (src_v_sptr);
	mna.add_part (r14_sptr);
	mna.add_part (r46rv3_sptr);
	mna.add_part (c9_sptr);
	mna.add_part (c10_sptr);
	mna.add_part (d2_d3_sptr);
	mna.add_part (d4_d5_sptr);
	float          gain = 10;
#else
	// Simple voltage follower (common collector BJT). No DC coupling
	enum
	{
		no_vcc = 1,
		no_src,
		no_dst
	};
	auto  vcc_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_vcc, mfx::dsp::va::mna::PartInterface::_nid_gnd, 9.f
	);
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0.f
	);
	auto  re_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_dst, mfx::dsp::va::mna::PartInterface::_nid_gnd, 1000.f
	);
	auto  bjt_sptr = std::make_shared <mfx::dsp::va::mna::PartBjt> (
		no_dst, no_src, no_vcc,
		false, 5.911e-15f, 1122.f, 1.271f // 2N5089
	);
	mna.add_part (vcc_sptr);
	mna.add_part (src_v_sptr);
	mna.add_part (re_sptr);
	mna.add_part (bjt_sptr);
	float          gain = 2;
#endif

	mna.prepare (sample_freq);
#if defined (mfx_dsp_va_mna_Simulator_STATS)
	mna.reset_stats ();
#endif // mfx_dsp_va_mna_Simulator_STATS

	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
		src_v_sptr->set_voltage (x);
		mna.process_sample ();
		x = mna.get_node_voltage (no_dst);
		dst [pos] = x;
	}

	mfx::FileOpWav::save ("results/simulmna1.wav", dst, sample_freq, 0.5f);

#if defined (mfx_dsp_va_mna_Simulator_STATS)
	print_stats (mna, "Standard");
	mna.reset_stats ();
#endif // mfx_dsp_va_mna_Simulator_STATS

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestMnaSimulator::gen_saw (std::vector <float> &data, double sample_freq, double freq, int len)
{
	const int      per = fstb::round_int (sample_freq / freq);
	for (int pos = 0; pos < len; ++pos)
	{
		const float    val = (pos % per) * (2.f / per) - 1.f;
		data.push_back (val);
	}
}



#if defined (mfx_dsp_va_mna_Simulator_STATS)

void	TestMnaSimulator::print_stats (mfx::dsp::va::mna::Simulator &mna, const char msg_0 [])
{
	printf ("%s\n", msg_0);
	
	mfx::dsp::va::mna::Simulator::Stats  stats;
	mna.get_stats (stats);

	printf ("=== Iterations ===\n");
	print_histo (
		stats._hist_it.data (),
		int (stats._hist_it.size ()),
		stats._nbr_spl_proc
	);

	printf ("\n");
}



void	TestMnaSimulator::print_histo (int hist_arr [], int nbr_bars, int nbr_spl)
{
	int            bar_max = 0;
	int            total   = 0;
	for (int k = 0; k < nbr_bars; ++k)
	{
		const int       val = hist_arr [k];
		bar_max = std::max (bar_max, val);
		total  += val * k;
	}

	const int      bar_size = 64;
	char           bar_0 [bar_size+1];
	for (int k = 0; k < bar_size; ++k)
	{
		bar_0 [k] = '#';
	}
	bar_0 [bar_size] = '\0';

	const double   nbr_spl_inv = 1.0 / double (nbr_spl);
	const double   bar_scale   = double (bar_size) / double (bar_max);
	printf ("Average: %.2f\n", double (total) * nbr_spl_inv);
	for (int k = 0; k < nbr_bars; ++k)
	{
		const int      val = hist_arr [k];
		if (val > 0)
		{
			const double   prop = double (val) * nbr_spl_inv;
			printf ("%3d: %10d, %5.1f %% ", k, val, prop * 100);
			const int      bar_len = fstb::round_int (val * bar_scale);
			printf ("%s\n", bar_0 + bar_size - bar_len);
		}
	}
}

#endif // mfx_dsp_va_mna_Simulator_STATS



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
