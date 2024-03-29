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
#include "test/TimerAccurate.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/va/mna/PartBjt.h"
#include "mfx/dsp/va/mna/PartBjtNpn.h"
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
#elif 0
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
	auto  bjt_sptr = std::make_shared <mfx::dsp::va::mna::PartBjtNpn> (
		no_dst, no_src, no_vcc,
		5.911e-15f, 1122.f, 1.271f // 2N5089
	);
	mna.add_part (vcc_sptr);
	mna.add_part (src_v_sptr);
	mna.add_part (re_sptr);
	mna.add_part (bjt_sptr);
	float          gain = 2;
#else

	// EHX Big Muff Pi V7C Tall Font Green Russian
	// http://www.bigmuffpage.com/Big_Muff_Pi_versions_schematics_part3.html
	// 40 s of simulation in release mode
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
		no_vol3,

		no_dst = no_vol2
	};

	const float    pot_sus  = 1.f;
	const float    pot_tone = 0.5f;
	const float    pot_vol  = 0.25f;

	auto  vcc_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_vcc, mfx::dsp::va::mna::PartInterface::_nid_gnd, 9.f
	);
	auto  src_v_sptr = std::make_shared <mfx::dsp::va::mna::PartSrcVoltage> (
		no_src, mfx::dsp::va::mna::PartInterface::_nid_gnd, 0.f
	);

	// Input buffer + sustain pot
	const float    r24 = 100e3f;
	auto  r2_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_src, no_r2c1, 39e3f
	);
	auto  r9_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q4b, no_q4c, 470e3f
	);
	auto  r13_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vcc, no_q4c, 12e3f
	);
	auto  r14_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q4b, mfx::dsp::va::mna::PartInterface::_nid_gnd, 102e3f
	);
	auto  r22_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q4e, mfx::dsp::va::mna::PartInterface::_nid_gnd, 390.f
	);
	auto  r23r24ccw_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_sus2, mfx::dsp::va::mna::PartInterface::_nid_gnd, 1e3f + r24 * pot_sus
	);
	auto  r24cw_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_sus2, no_sus3, std::max (r24 * (1 - pot_sus), 1.f)
	);
	auto  c1_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_r2c1, no_q4b, 140e-9f
	);
	auto  c4_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q4c, no_sus3, 100e-9f
	);
	auto  c10_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q4b, no_q4c, 450e-12f
	);
	auto  q4_sptr = std::make_shared <mfx::dsp::va::mna::PartBjtNpn> (
		no_q4e, no_q4b, no_q4c,
		5.911e-15f, 1122.f, 1.271f // 2N5089
	);

	// Distortion stage 1
	auto  r17_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q3b, no_q3c, 500e3f
	);
	auto  r18_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vcc, no_q3c, 12e3f
	);
	auto  r19_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_c5r19, no_q3b, 12e3f
	);
	auto  r20_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q3b, mfx::dsp::va::mna::PartInterface::_nid_gnd, 100e3f
	);
	auto  r21_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q3e, mfx::dsp::va::mna::PartInterface::_nid_gnd, 390.f
	);
	auto  c5_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_sus2, no_c5r19, 125e-9f
	);
	auto  c6_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q3b, no_s1di, 55e-9f
	);
	auto  c12_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q3b, no_q3c, 510e-12f
	);
	auto  d3d4_sptr = std::make_shared <mfx::dsp::va::mna::PartDiodeAntipar> (
		no_s1di, no_q3c,
		4.352e-9f, 1.906f, 4.352e-9f, 1.906f // 2x 1N4148 (or 1N914)
	);
	auto  q3_sptr = std::make_shared <mfx::dsp::va::mna::PartBjtNpn> (
		no_q3e, no_q3b, no_q3c,
		5.911e-15f, 1122.f, 1.271f // 2N5089
	);

	// Distortion stage 2
	auto  r15_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q2b, no_q2c, 470e3f
	);
	auto  r11_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vcc, no_q2c, 12e3f
	);
	auto  r12_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_c3r12, no_q2b, 10e3f
	);
	auto  r16_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q2b, mfx::dsp::va::mna::PartInterface::_nid_gnd, 90e3f
	);
	auto  r10_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q2e, mfx::dsp::va::mna::PartInterface::_nid_gnd, 390.f
	);
	auto  c13_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q3c, no_c3r12, 115e-9f
	);
	auto  c7_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q2b, no_s2di, 52e-9f
	);
	auto  c11_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q2b, no_q2c, 510e-12f
	);
	auto  d1d2_sptr = std::make_shared <mfx::dsp::va::mna::PartDiodeAntipar> (
		no_s2di, no_q2c,
		4.352e-9f, 1.906f, 4.352e-9f, 1.906f // 2x 1N4148 (or 1N914)
	);
	auto  q2_sptr = std::make_shared <mfx::dsp::va::mna::PartBjtNpn> (
		no_q2e, no_q2b, no_q2c,
		5.911e-15f, 1122.f, 1.271f // 2N5089
	);

	// Tone stack
	const float    r25 = 100e3f;
	auto  r5_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_tone3, mfx::dsp::va::mna::PartInterface::_nid_gnd, 22e3f
	);
	auto  r8_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q2c, no_tone1, 20e3f
	);
	auto  r25ccw_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_tone1, no_tone2, std::max (r25 * pot_tone, 1.f)
	);
	auto  r25cw_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_tone2, no_tone3, std::max (r25 * (1 - pot_tone), 1.f)
	);
	auto  c8_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_tone1, mfx::dsp::va::mna::PartInterface::_nid_gnd, 10e-9f
	);
	auto  c9_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q2c, no_tone3, 3.9e-9f
	);

	// Output buffer
	const float    r26 = 100e3f;
	auto  r3_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q1b, mfx::dsp::va::mna::PartInterface::_nid_gnd, 100e3f
	);
	auto  r4_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_q1e, mfx::dsp::va::mna::PartInterface::_nid_gnd, 2.7e3f
	);
	auto  r6_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vcc, no_q1c, 10e3f
	);
	auto  r7_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vcc, no_q1b, 470e3f
	);
	auto  r26ccw_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vol2, mfx::dsp::va::mna::PartInterface::_nid_gnd,
		std::max (r26 * pot_vol, 1.f)
	);
	auto  r26cw_sptr = std::make_shared <mfx::dsp::va::mna::PartResistor> (
		no_vol2, no_vol3, std::max (r26 * (1 - pot_vol), 1.f)
	);
	auto  c2_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_q1c, no_vol3, 100e-9f
	);
	auto  c3_sptr = std::make_shared <mfx::dsp::va::mna::PartCapacitor> (
		no_tone2, no_q1b, 100e-9f
	);
	auto  q1_sptr = std::make_shared <mfx::dsp::va::mna::PartBjtNpn> (
		no_q1e, no_q1b, no_q1c,
		5.911e-15f, 1122.f, 1.271f // 2N5089
	);

	mna.add_part (vcc_sptr);
	mna.add_part (src_v_sptr);

	mna.add_part (r2_sptr);
	mna.add_part (r9_sptr);
	mna.add_part (r13_sptr);
	mna.add_part (r14_sptr);
	mna.add_part (r22_sptr);
	mna.add_part (r23r24ccw_sptr);
	mna.add_part (r24cw_sptr);
	mna.add_part (c1_sptr);
	mna.add_part (c4_sptr);
	mna.add_part (c10_sptr);
	mna.add_part (q4_sptr);

	mna.add_part (r17_sptr);
	mna.add_part (r18_sptr);
	mna.add_part (r19_sptr);
	mna.add_part (r20_sptr);
	mna.add_part (r21_sptr);
	mna.add_part (c5_sptr);
	mna.add_part (c6_sptr);
	mna.add_part (c12_sptr);
	mna.add_part (d3d4_sptr);
	mna.add_part (q3_sptr);

	mna.add_part (r15_sptr);
	mna.add_part (r11_sptr);
	mna.add_part (r12_sptr);
	mna.add_part (r16_sptr);
	mna.add_part (r10_sptr);
	mna.add_part (c13_sptr);
	mna.add_part (c7_sptr);
	mna.add_part (c11_sptr);
	mna.add_part (d1d2_sptr);
	mna.add_part (q2_sptr);

	mna.add_part (r5_sptr);
	mna.add_part (r8_sptr);
	mna.add_part (r25ccw_sptr);
	mna.add_part (r25cw_sptr);
	mna.add_part (c8_sptr);
	mna.add_part (c9_sptr);

	mna.add_part (r3_sptr);
	mna.add_part (r4_sptr);
	mna.add_part (r6_sptr);
	mna.add_part (r7_sptr);
	mna.add_part (r26ccw_sptr);
	mna.add_part (r26cw_sptr);
	mna.add_part (c2_sptr);
	mna.add_part (c3_sptr);
	mna.add_part (q1_sptr);

	float          gain = 1;

#endif

	mna.prepare (sample_freq);
#if defined (mfx_dsp_va_mna_Simulator_STATS)
	mna.reset_stats ();
#endif // mfx_dsp_va_mna_Simulator_STATS

	const int      msize     = mna.get_matrix_size ();
	const int      nbr_nodes = mna.get_nbr_nodes ();
	const int      nbr_src_v = mna.get_nbr_src_v ();
	printf ("Matrix size: %3d\n", msize);
	printf ("Nodes      : %3d\n", nbr_nodes);
	printf ("Voltage src: %3d\n", nbr_src_v);

	printf ("Simulating...\n");
	fflush (stdout);

	TimerAccurate  tim;
	tim.reset ();
	tim.start ();

	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
		src_v_sptr->set_voltage (x);
		mna.process_sample ();
		x = float (mna.get_node_voltage (no_dst));
		dst [pos] = x;

#if 0 // Displays the matrix in the middle of the run
		if (pos == 1000)
		{
			const auto     mat = mna.get_matrix ();
			const auto     vec = mna.get_vector ();

			for (int r = 0; r < msize; ++r)
			{
				for (int c = 0; c < msize; ++c)
				{
					printf ("%-10g ", mat [r * msize + c]);
				}
				printf ("| %-10g\n", vec [r]);
			}
		}
#endif
	}

	tim.stop ();
	const double      spl_per_s = tim.get_best_rate (len);

	mfx::FileOpWav::save ("results/simulmna1.wav", dst, sample_freq, 0.5f);

	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

#if defined (mfx_dsp_va_mna_Simulator_STATS)
	print_stats (mna);
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
		const float    val = float (pos % per) * (2.f / float (per)) - 1.f;
		data.push_back (val);
	}
}



#if defined (mfx_dsp_va_mna_Simulator_STATS)

void	TestMnaSimulator::print_stats (const mfx::dsp::va::mna::Simulator &mna)
{
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



void	TestMnaSimulator::print_histo (const int hist_arr [], int nbr_bars, int nbr_spl)
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
