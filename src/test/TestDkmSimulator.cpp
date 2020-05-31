/*****************************************************************************

        TestDkmSimulator.cpp
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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "test/TestDkmSimulator.h"
#include "test/TimerAccurate.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"

#include <memory>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestDkmSimulator::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::va::dkm::Simulator...\n");

	const int      ovrspl      = 1;
	const double   sample_freq = 44100 * ovrspl;
	const int      ssin_len    = fstb::round_int (sample_freq * 10);

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

	mfx::dsp::va::dkm::Simulator  dkm;
	int            idx_src = -1;
	int            idx_dst = -1;

#if 0
	// Simple resistor bridge
	enum
	{
		no_src = 1,
		no_dst
	};
	idx_src = dkm.add_src_v (no_src, dkm._nid_gnd, 0);
	dkm.add_resistor (no_src, no_dst, 1000);
	dkm.add_resistor (no_dst, dkm._nid_gnd, 1000);
	float          gain = 1;
#elif 0
	// RC low-pass filter. Cutoff: 1.6 kHz
	enum
	{
		no_src = 1,
		no_dst
	};
	idx_src = dkm.add_src_v (no_src, dkm._nid_gnd, 0);
	dkm.add_resistor (no_src, no_dst, 1000.f);
	dkm.add_capacitor (no_dst, dkm._nid_gnd, 100e-9f);
	float          gain = 1;
#elif 0
	// RC low-pass filter + 2 diodes. Cutoff: 1.6 kHz
	enum
	{
		no_src = 1,
		no_dst
	};
	idx_src = dkm.add_src_v (no_src, dkm._nid_gnd, 0);
	dkm.add_resistor (no_src, no_dst, 1000.f);
	dkm.add_capacitor (no_dst, dkm._nid_gnd, 100e-9f);
	dkm.add_diode_antipar (no_dst, dkm._nid_gnd, 0.1e-15f, 1.f, 0.1e-6f, 4.f);
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
	idx_src = dkm.add_src_v (no_src, dkm._nid_gnd, 0);
	dkm.add_resistor (no_hpf, no_dst, 2200.f);
	dkm.add_resistor (no_sqw, no_dst, 100.f); // 100 - 10k
	// Increasing C9 makes harder basses, with stronger even harmonics
	// (octave-like tone)
	dkm.add_capacitor (no_src, no_hpf, 470e-9f);
	dkm.add_capacitor (no_dst, dkm._nid_gnd, 10e-9f);
	dkm.add_diode_antipar (no_hpf, no_sqw, 0.1e-15f, 4.f, 0.1e-15f, 1.f);
	dkm.add_diode_antipar (no_dst, dkm._nid_gnd, 0.1e-15f, 1.f, 0.1e-6f, 4.f);
	float          gain = 10;
#elif 0
	// Simple voltage follower (common collector BJT). No DC coupling
	enum
	{
		no_vcc = 1,
		no_src,
		no_dst
	};
	dkm.add_src_v (no_vcc, dkm._nid_gnd, 9.f);
	idx_src = dkm.add_src_v (no_src, dkm._nid_gnd, 0);
	dkm.add_resistor (no_dst, dkm._nid_gnd, 1000.f);
	dkm.add_bjt_npn (no_dst, no_src, no_vcc, 5.911e-15f, 1.f, 1122.f, 1.271f); // 2N5089
	float          gain = 2;
#else

	// EHX Big Muff Pi V7C Tall Font Green Russian
	// http://www.bigmuffpage.com/Big_Muff_Pi_versions_schematics_part3.html
	enum
	{
		no_vcc = 1,
		no_src,

		// Input buffer
		no_r2c1,
		no_q4b,
		no_q4c,
		no_q4e,

		// Sustain (gain) pot
		no_sus3,
		no_sus2,

		// Distortion stage 1
		no_c5r19,
		no_q3b,
		no_q3c,
		no_q3e,
		no_s1di,

		// Distortion stage 2
		no_c3r12,
		no_q2b,
		no_q2c,
		no_q2e,
		no_s2di,

		// Tone stack
		no_tone1,
		no_tone2,
		no_tone3,

		// Output buffer
		no_q1b,
		no_q1c,
		no_q1e,
		no_vol2,
		no_vol3
	};

	const float    pot_sus  = 1.f;
	const float    pot_tone = 0.5f;
	const float    pot_vol  = 0.25f;

	dkm.add_src_v (no_vcc, dkm._nid_gnd, 9.f);
	idx_src = dkm.add_src_v (no_src, dkm._nid_gnd, 0.f);

	// Input buffer + sustain pot
	const float    r24 = 100e3f;
	dkm.add_resistor (no_src, no_r2c1, 39e3f); // R2
	dkm.add_resistor (no_q4b, no_q4c, 470e3f); // R9
	dkm.add_resistor (no_vcc, no_q4c, 12e3f); // R13
	dkm.add_resistor (no_q4b, dkm._nid_gnd, 100e3f); // R14
	dkm.add_resistor (no_q4e, dkm._nid_gnd, 390.f); // R22
	dkm.add_pot (no_sus2, dkm._nid_gnd, 1e3f, 1e3f + r24, pot_sus); // R23 + R24CCW
	dkm.add_pot (no_sus2, no_sus3, r24, 1.f, pot_sus); // R24CW
	dkm.add_resistor (no_sus2, dkm._nid_gnd, 1e9f); // Makes the matrix invertible
	dkm.add_capacitor (no_r2c1, no_q4b, 100e-9f); // C1
	dkm.add_capacitor (no_q4c, no_sus3, 100e-9f); // C4
	dkm.add_capacitor (no_q4b, no_q4c, 500e-12f); // C10
	dkm.add_bjt_npn (no_q4e, no_q4b, no_q4c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q4, 2N5089

#if 0
	const int      no_dst = no_sus2;
#else
	// Distortion stage 1
	dkm.add_resistor (no_q3b, no_q3c, 470e3f); // R17
	dkm.add_resistor (no_vcc, no_q3c, 12e3f); // R18
	dkm.add_resistor (no_c5r19, no_q3b, 10e3f); // R19
	dkm.add_resistor (no_q3b, dkm._nid_gnd, 100e3f); // R20
	dkm.add_resistor (no_q3e, dkm._nid_gnd, 390.f); // R21
	dkm.add_capacitor (no_sus2, no_c5r19, 100e-9f); // C5
	dkm.add_capacitor (no_q3b, no_s1di, 47e-9f); // C6
	dkm.add_capacitor (no_q3b, no_q3c, 500e-12f); // C12
	dkm.add_diode_antipar (no_s1di, no_q3c, 4.352e-9f, 1.906f, 4.352e-9f, 1.906f); // D3, D4, 2x 1N4148 (or 1N914)
	dkm.add_bjt_npn (no_q3e, no_q3b, no_q3c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q3, 2N5089

#if 0
	const int      no_dst = no_q3c;
#else
	// Distortion stage 2
	dkm.add_resistor (no_q2b, no_q2c, 470e3f); // R15
	dkm.add_resistor (no_vcc, no_q2c, 12e3f); // R11
	dkm.add_resistor (no_c3r12, no_q2b, 10e3f); // R12
	dkm.add_resistor (no_q2b, dkm._nid_gnd, 100e3f); // R16
	dkm.add_resistor (no_q2e, dkm._nid_gnd, 390.f); // R10
	dkm.add_capacitor (no_q3c, no_c3r12, 100e-9f); // C13
	dkm.add_capacitor (no_q2b, no_s2di, 47e-9f); // C7
	dkm.add_capacitor (no_q2b, no_q2c, 500e-12f); // C11
	dkm.add_diode_antipar (no_s2di, no_q2c, 4.352e-9f, 1.906f, 4.352e-9f, 1.906f); // D1, D2, 2x 1N4148 (or 1N914)
	dkm.add_bjt_npn (no_q2e, no_q2b, no_q2c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q2, 2N5089

#if 0
	const int      no_dst = no_q2c;
#else
	// Tone stack
	const float    r25 = 100e3f;
	dkm.add_resistor (no_tone3, dkm._nid_gnd, 22e3f); // R5
	dkm.add_resistor (no_q2c, no_tone1, 20e3f); // R8
	dkm.add_pot (no_tone1, no_tone2, 1.f, r25, pot_tone); // R25CCW
	dkm.add_pot (no_tone2, no_tone3, r25, 1.f, pot_tone); // R25CW
	dkm.add_resistor (no_tone2, no_tone3, 1e9f); // Makes the matrix invertible
	dkm.add_capacitor (no_tone1, dkm._nid_gnd, 10e-9f); // C8
	dkm.add_capacitor (no_q2c, no_tone3, 3.9e-9f); // C9

#if 0
	const int      no_dst = no_tone2;
#else
	// Output buffer
	const float    r26 = 100e3f;
	dkm.add_resistor (no_q1b, dkm._nid_gnd, 100e3f); // R3
	dkm.add_resistor (no_q1e, dkm._nid_gnd, 2.7e3f); // R4
	dkm.add_resistor (no_vcc, no_q1c, 10e3f); // R6
	dkm.add_resistor (no_vcc, no_q1b, 470e3f); // R7
	dkm.add_pot (no_vol2, dkm._nid_gnd, 1.f, r26, pot_vol); // R26CCW
	dkm.add_pot (no_vol2, no_vol3, r26, 1.f, pot_vol); // R26CW
	dkm.add_resistor (no_vol2, dkm._nid_gnd, 1e9f); // Makes the matrix invertible
	dkm.add_capacitor (no_q1c, no_vol3, 100e-9f); // C2
	dkm.add_capacitor (no_tone2, no_q1b, 100e-9f); // C3
	dkm.add_bjt_npn (no_q1e, no_q1b, no_q1c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q1, 2N5089
	const int      no_dst = no_vol2;
#endif // + output buffer
#endif // + tone stack
#endif // + disto stage 2
#endif // + disto stage 1

	float          gain = 1;

#endif

	idx_dst = dkm.add_output (no_dst, dkm._nid_gnd);
	dkm.prepare (sample_freq);
#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	dkm.reset_stats ();
#endif // mfx_dsp_va_dkm_Simulator_STATS

	const int      nbr_nodes = dkm.get_nbr_nodes ();
	const int      nbr_src_v = dkm.get_nbr_src_v ();
	printf ("Nodes      : %3d\n", nbr_nodes);
	printf ("Voltage src: %3d\n", nbr_src_v);

	printf ("Simulating...\n");
	fflush (stdout);

	TimerAccurate  tim;
	tim.reset ();
	tim.start ();

	for (int pos = 0; pos < len; ++pos)
	{
#if 0
		if (pos >= 500 && pos <= 500)
		{
			printf ("*** Before %d ***\n", pos);
			dkm.print_all ();
#if ! defined (NDEBUG)
			__debugbreak ();
#endif
		}
#endif

		float           x = src [pos] * gain;
#if 0 // Audio-rate modulation
		const int per = 337;
		dkm.set_pot (1, 0.5f + 0.5f * fstb::Approx::sin_nick_2pi (
			float (pos % per) * (1.f / float (per)) - 0.5f
		));
#endif
		dkm.set_src_v (idx_src, x);
		dkm.process_sample ();
		x = float (dkm.get_output (idx_dst));
		dst [pos] = x;
	}

	tim.stop ();
	const double      spl_per_s = tim.get_best_rate (len);

	mfx::FileOpWav::save ("results/simuldkm1.wav", dst, sample_freq, 0.5f);

	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	print_stats (dkm);
	dkm.reset_stats ();
#endif // mfx_dsp_va_dkm_Simulator_STATS

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestDkmSimulator::gen_saw (std::vector <float> &data, double sample_freq, double freq, int len)
{
	const int      per = fstb::round_int (sample_freq / freq);
	for (int pos = 0; pos < len; ++pos)
	{
		const float    val = (pos % per) * (2.f / per) - 1.f;
		data.push_back (val);
	}
}



#if defined (mfx_dsp_va_dkm_Simulator_STATS)

void	TestDkmSimulator::print_stats (mfx::dsp::va::dkm::Simulator &dkm)
{
	mfx::dsp::va::dkm::Simulator::Stats  stats;
	dkm.get_stats (stats);

	printf ("=== Iterations ===\n");
	print_histo (
		stats._hist_it.data (),
		int (stats._hist_it.size ()),
		stats._nbr_spl_proc
	);

	printf ("\n");
}



void	TestDkmSimulator::print_histo (int hist_arr [], int nbr_bars, int nbr_spl)
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

#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
