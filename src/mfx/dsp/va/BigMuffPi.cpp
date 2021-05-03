/*****************************************************************************

        BigMuffPi.cpp
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

#include "mfx/dsp/va/BigMuffPi.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BigMuffPi::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	if (_constructed_flag)
	{
		_dkm.update_sample_freq (sample_freq);
	}
	else
	{
		setup_circuit ();

		_dkm.prepare (sample_freq);
		_dkm.set_reordering_jacobian (
			{  0,  1,  2,  3,  4,  5,  6,  7,  9,  8 },
			{  0,  1,  2,  3,  4,  5,  6,  7,  9,  8 }
		);

		clear_buffers ();

		_constructed_flag = true;
	}
}



void	BigMuffPi::clear_buffers () noexcept
{
	_dkm.clear_buffers ();
}



void	BigMuffPi::set_pot (Pot pot, float val) noexcept
{
	assert (pot >= 0);
	assert (pot < Pot_NBR_ELT);
	assert (val >= 0);
	assert (val <= 1);

	const auto &   pot_idx_pair = _idx_pot_arr [pot];
	_dkm.set_pot (pot_idx_pair [0], val);
	_dkm.set_pot (pot_idx_pair [1], val);
}



float	BigMuffPi::process_sample (float x) noexcept
{
	_dkm.set_src_v (_idx_in, x);
	_dkm.process_sample ();
	x = float (_dkm.get_output (_idx_out));

	return x;
}



void	BigMuffPi::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



#if defined (mfx_dsp_va_dkm_Simulator_STATS)



void	BigMuffPi::reset_stats ()
{
	_dkm.reset_stats ();
}



void	BigMuffPi::get_stats (dkm::Simulator::Stats &st) const
{
	_dkm.get_stats (st);
}



#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BigMuffPi::setup_circuit ()
{
	// Node identifiers
	enum Node : dkm::Simulator::IdNode
	{
		nid_vcc = 1,
		nid_src,

		// Input buffer
		nid_r2c1,
		nid_q4b,
		nid_q4c,
		nid_q4e,

		// Sustain (gain) pot
		nid_sus3,
		nid_sus2,

		// Distortion stage 1
		nid_c5r19,
		nid_q3b,
		nid_q3c,
		nid_q3e,
		nid_s1di,

		// Distortion stage 2
		nid_c3r12,
		nid_q2b,
		nid_q2c,
		nid_q2e,
		nid_s2di,

		// Tone stack
		nid_tone1,
		nid_tone2,
		nid_tone3,

		// Output buffer
		nid_q1b,
		nid_q1c,
		nid_q1e,
		nid_vol2,
		nid_vol3
	};
	const auto     nid_gnd = dkm::Simulator::_nid_gnd;
	const auto     nid_dst = nid_vol2;

	// Default values
	constexpr float   v_psu    = 9.2f;
	constexpr float   pot_sus  = 1.f;
	constexpr float   pot_tone = 0.5f;
	constexpr float   pot_vol  = 0.25f;
	constexpr float   pot_val  = 150e3f;

	// Input buffer + sustain pot
	const float    r24 = pot_val;
	_dkm.add_resistor (nid_src, nid_r2c1, 39e3f); // R2
	_dkm.add_resistor (nid_q4b, nid_q4c, 470e3f); // R9
	_dkm.add_resistor (nid_vcc, nid_q4c, 12e3f); // R13
	_dkm.add_resistor (nid_q4b, nid_gnd, 102e3f); // R14 (orig: 100k)
	_dkm.add_resistor (nid_q4e, nid_gnd, 390.f); // R22
	_idx_pot_arr [Pot_SUS] [0] =
		_dkm.add_pot (nid_sus2, nid_gnd, 1e3f, 1e3f + r24, pot_sus); // R23 + R24CCW
	_idx_pot_arr [Pot_SUS] [1] =
		_dkm.add_pot (nid_sus2, nid_sus3, r24, 1.f, pot_sus); // R24CW
	_dkm.add_resistor (nid_sus2, nid_gnd, 1e9f); // Makes the matrix invertible
	_dkm.add_capacitor (nid_r2c1, nid_q4b, 140e-9f); // C1 (orig: 100n)
	_dkm.add_capacitor (nid_q4c, nid_sus3, 100e-9f); // C4
	_dkm.add_capacitor (nid_q4b, nid_q4c, 450e-12); // C10 (orig: 510p)
	_dkm.add_bjt_npn (nid_q4e, nid_q4b, nid_q4c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q4, 2N5089

	// Distortion stage 1
	_dkm.add_resistor (nid_q3b, nid_q3c, 500e3f); // R17 (orig: 470k)
	_dkm.add_resistor (nid_vcc, nid_q3c, 12e3f); // R18
	_dkm.add_resistor (nid_c5r19, nid_q3b, 12e3f); // R19 (orig: 10k)
	_dkm.add_resistor (nid_q3b, nid_gnd, 105e3f); // R20 (orig: 100k)
	_dkm.add_resistor (nid_q3e, nid_gnd, 390.f); // R21
	_dkm.add_capacitor (nid_sus2, nid_c5r19, 125e-9f); // C5 (orig: 100n)
	_dkm.add_capacitor (nid_q3b, nid_s1di, 55e-9f); // C6 (orig: 47n)
	_dkm.add_capacitor (nid_q3b, nid_q3c, 510e-12f); // C12
	_dkm.add_diode_antipar (nid_s1di, nid_q3c, 4.352e-9f, 1.906f, 4.352e-9f, 1.906f); // D3, D4, 2x 1N4148 (or 1N914)
	_dkm.add_bjt_npn (nid_q3e, nid_q3b, nid_q3c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q3, 2N5089

	// Distortion stage 2
	_dkm.add_resistor (nid_q2b, nid_q2c, 470e3f); // R15
	_dkm.add_resistor (nid_vcc, nid_q2c, 12e3f); // R11
	_dkm.add_resistor (nid_c3r12, nid_q2b, 10e3f); // R12
	_dkm.add_resistor (nid_q2b, nid_gnd, 90e3f); // R16 (orig: 100k)
	_dkm.add_resistor (nid_q2e, nid_gnd, 390.f); // R10
	_dkm.add_capacitor (nid_q3c, nid_c3r12, 115e-9f); // C13 (orig: 100n)
	_dkm.add_capacitor (nid_q2b, nid_s2di, 52e-9f); // C7 (orig: 47n)
	_dkm.add_capacitor (nid_q2b, nid_q2c, 510e-12f); // C11
	_dkm.add_diode_antipar (nid_s2di, nid_q2c, 4.352e-9f, 1.906f, 4.352e-9f, 1.906f); // D1, D2, 2x 1N4148 (or 1N914)
	_dkm.add_bjt_npn (nid_q2e, nid_q2b, nid_q2c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q2, 2N5089

	// Tone stack
	const float    r25 = pot_val;
	_dkm.add_resistor (nid_tone3, nid_gnd, 22e3f); // R5
	_dkm.add_resistor (nid_q2c, nid_tone1, 20e3f); // R8
	_idx_pot_arr [Pot_TONE] [0] =
		_dkm.add_pot (nid_tone1, nid_tone2, 1.f, r25, pot_tone); // R25CCW
	_idx_pot_arr [Pot_TONE] [1] =
		_dkm.add_pot (nid_tone2, nid_tone3, r25, 1.f, pot_tone); // R25CW
	_dkm.add_resistor (nid_tone1, nid_tone3, 1e9f); // Makes the matrix invertible
	_dkm.add_capacitor (nid_tone1, nid_gnd, 10e-9f); // C8
	_dkm.add_capacitor (nid_q2c, nid_tone3, 3.9e-9f); // C9

	// Output buffer
	const float    r26 = pot_val;
	_dkm.add_resistor (nid_q1b, nid_gnd, 100e3f); // R3
	_dkm.add_resistor (nid_q1e, nid_gnd, 2.7e3f); // R4
	_dkm.add_resistor (nid_vcc, nid_q1c, 10e3f); // R6
	_dkm.add_resistor (nid_vcc, nid_q1b, 470e3f); // R7
	_idx_pot_arr [Pot_VOL] [0] =
		_dkm.add_pot (nid_vol2, nid_gnd, 1.f, r26, pot_vol); // R26CCW
	_idx_pot_arr [Pot_VOL] [1] =
		_dkm.add_pot (nid_vol2, nid_vol3, r26, 1.f, pot_vol); // R26CW
	_dkm.add_resistor (nid_vol2, nid_gnd, 1e9f); // Makes the matrix invertible
	_dkm.add_capacitor (nid_q1c, nid_vol3, 100e-9f); // C2
	_dkm.add_capacitor (nid_tone2, nid_q1b, 100e-9f); // C3
	_dkm.add_bjt_npn (nid_q1e, nid_q1b, nid_q1c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q1, 2N5089

	// Misc setup
	_dkm.set_max_nbr_it (_max_it);
	_dkm.add_src_v (nid_vcc, nid_gnd, v_psu);
	_idx_in  = _dkm.add_src_v (nid_src, nid_gnd, 0.f);
	_idx_out = _dkm.add_output (nid_dst, nid_gnd);
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
