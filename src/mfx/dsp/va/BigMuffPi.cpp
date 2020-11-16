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

	if (! _constructed_flag)
	{
		setup_circuit ();
	}

	_buf_in.prepare (sample_freq);
	for (auto &stage : _dist_arr)
	{
		stage._dist.prepare (sample_freq);
	}
	_buf_out.prepare (sample_freq);

	clear_buffers ();
}



void	BigMuffPi::clear_buffers ()
{
	_buf_in.clear_buffers ();
	for (auto &stage : _dist_arr)
	{
		stage._dist.clear_buffers ();
	}
	_buf_out.clear_buffers ();
}



void	BigMuffPi::set_pot (Pot pot, float val)
{
	assert (pot >= 0);
	assert (pot < Pot_NBR_ELT);
	assert (val >= 0);
	assert (val <= 1);

	switch (pot)
	{
	case Pot_SUS:
		_buf_in.set_pot (0, val);
		_buf_in.set_pot (1, val);
		break;
	case Pot_TONE:
		_dist_arr.back ()._dist.set_pot (0, val);
		_dist_arr.back ()._dist.set_pot (1, val);
		break;
	case Pot_VOL:
		_buf_out.set_pot (0, val);
		_buf_out.set_pot (1, val);
		break;
	default:
		assert (false);
		break;
	}
}



float	BigMuffPi::process_sample (float x)
{
	_buf_in.set_src_v (_idx_buf_in_i, x);
	_buf_in.process_sample ();
	x = float (_buf_in.get_output (_idx_buf_in_o));

	for (auto &stage : _dist_arr)
	{
		stage._dist.set_src_v (stage._idx_i, x);
		stage._dist.process_sample ();
		x = float (stage._dist.get_output (stage._idx_o));
	}

	_buf_out.set_src_v (_idx_buf_out_i, x);
	_buf_out.process_sample ();
	x = float (_buf_out.get_output (_idx_buf_out_o));

	return x;
}



void	BigMuffPi::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
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
	_buf_in.reset_stats ();
	for (auto &stage : _dist_arr)
	{
		stage._dist.reset_stats ();
	}
	_buf_out.reset_stats ();
}



void	BigMuffPi::get_stats (dkm::Simulator::Stats &st_in, dkm::Simulator::Stats &st_dfirst, dkm::Simulator::Stats &st_dlast, dkm::Simulator::Stats &st_out) const
{
	_buf_in.get_stats (st_in);
	_dist_arr.front ()._dist.get_stats (st_dfirst);
	_dist_arr.back ()._dist.get_stats (st_dlast);
	_buf_out.get_stats (st_out);
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

		// Input buffer
		nid_src,
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
/* Not needed here
		nid_c3r12,
		nid_q2b,
		nid_q2c,
		nid_q2e,
		nid_s2di,
*/

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

	// Default values
	const float    v_psu    = 9.2f;
	const float    pot_sus  = 1.f;
	const float    pot_tone = 0.5f;
	const float    pot_vol  = 0.25f;
	const float    pot_val  = 150e3f;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Input buffer + sustain pot

	_buf_in.set_max_nbr_it (_max_it);
	_buf_in.add_src_v (nid_vcc, nid_gnd, v_psu);
	_idx_buf_in_i = _buf_in.add_src_v (nid_src, nid_gnd, 0.f);
	_idx_buf_in_o = _buf_in.add_output (nid_sus2, nid_gnd);

	const float    r24 = pot_val;
	_buf_in.add_resistor (nid_src, nid_r2c1, 39e3f); // R2
	_buf_in.add_resistor (nid_q4b, nid_q4c, 470e3f); // R9
	_buf_in.add_resistor (nid_vcc, nid_q4c, 12e3f); // R13
	_buf_in.add_resistor (nid_q4b, nid_gnd, 102e3f); // R14 (orig: 100k)
	_buf_in.add_resistor (nid_q4e, nid_gnd, 390.f); // R22
	_buf_in.add_pot (nid_sus2, nid_gnd, 1e3f, 1e3f + r24, pot_sus); // R23 + R24CCW
	_buf_in.add_pot (nid_sus2, nid_sus3, r24, 1.f, pot_sus); // R24CW
	_buf_in.add_resistor (nid_sus2, nid_gnd, 1e9f); // Makes the S0 matrix invertible
	_buf_in.add_capacitor (nid_r2c1, nid_q4b, 140e-9f); // C1 (orig: 100n)
	_buf_in.add_capacitor (nid_q4c, nid_sus3, 100e-9f); // C4
	_buf_in.add_capacitor (nid_q4b, nid_q4c, 450e-12f); // C10 (orig: 510p)
	_buf_in.add_bjt_npn (nid_q4e, nid_q4b, nid_q4c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q4, 2N5089

	// Fake load
	_buf_in.add_capacitor (nid_sus2, nid_tone3, 100e-9f); // C99
	_buf_in.add_resistor (nid_tone3, nid_gnd, 27e3f); // R99
	_buf_in.add_resistor (nid_tone3, nid_tone1, 18e3f); // R98
	_buf_in.add_capacitor (nid_tone1, nid_gnd, 3e-9f); // C98

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Distortion stages

	for (int stg_cnt = 0; stg_cnt < _nbr_dist_stages; ++stg_cnt)
	{
		auto &         stage = _dist_arr [stg_cnt];
		auto &         dkm   = stage._dist;

		dkm.set_max_nbr_it (_max_it);
		dkm.add_src_v (nid_vcc, nid_gnd, v_psu);
		stage._idx_i = dkm.add_src_v (nid_sus2, nid_gnd, 0.f);

		dkm.add_resistor (nid_q3b, nid_q3c, (stg_cnt == 0) ? 500e3f : 470e3f); // R17
		dkm.add_resistor (nid_vcc, nid_q3c, 12e3f); // R18
		dkm.add_resistor (nid_c5r19, nid_q3b, (stg_cnt == 0) ? 12e3f : 10e3f); // R19
		dkm.add_resistor (nid_q3b, nid_gnd, (stg_cnt == 0) ? 105e3f : (stg_cnt == 1) ? 90e3f : 100e3f); // R20
		dkm.add_resistor (nid_q3e, nid_gnd, 390.f); // R21
		dkm.add_capacitor (nid_sus2, nid_c5r19, (stg_cnt == 0) ? 125e-9f : (stg_cnt == 1) ? 115e-9f : 100e-9f); // C5
		dkm.add_capacitor (nid_q3b, nid_s1di, (stg_cnt == 0) ? 55e-9f : (stg_cnt == 1) ? 52e-9f : 47e-9f); // C6
		dkm.add_capacitor (nid_q3b, nid_q3c, 510e-12f); // C12
		dkm.add_diode_antipar (nid_s1di, nid_q3c, 4.352e-9f, 1.906f, 4.352e-9f, 1.906f); // D3, D4, 2x 1N4148 (or 1N914)
		dkm.add_bjt_npn (nid_q3e, nid_q3b, nid_q3c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q3, 2N5089

		// Fake load for the first stage(s)
		if (stg_cnt < _nbr_dist_stages - 1)
		{
			dkm.add_capacitor (nid_q3c, nid_tone3, 100e-9f); // C99
			dkm.add_resistor (nid_tone3, nid_gnd, 27e3f); // R99
			dkm.add_resistor (nid_tone3, nid_tone1, 18e3f); // R98
			dkm.add_capacitor (nid_tone1, nid_gnd, 3e-9f); // C98

			stage._idx_o = dkm.add_output (nid_q3c, nid_gnd);
		}

		// Tone stack for the last one
		else
		{
			const float    r25 = pot_val;
			dkm.add_resistor (nid_tone3, nid_gnd, 22e3f); // R5
			dkm.add_resistor (nid_q3c, nid_tone1, 20e3f); // R8
			dkm.add_pot (nid_tone1, nid_tone2, 1.f, r25, pot_tone); // R25CCW
			dkm.add_pot (nid_tone2, nid_tone3, r25, 1.f, pot_tone); // R25CW
			dkm.add_capacitor (nid_tone1, nid_gnd, 10e-9f); // C8
			dkm.add_capacitor (nid_q3c, nid_tone3, 3.9e-9f); // C9

			// Fake load + makes the matrix invertible
			dkm.add_capacitor (nid_tone2, nid_q1b, 100e-9f);
			dkm.add_resistor (nid_q1b, nid_gnd, 82e3f);

			stage._idx_o = dkm.add_output (nid_tone2, nid_gnd);
		}
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Output buffer

	_buf_out.set_max_nbr_it (_max_it);
	_buf_out.add_src_v (nid_vcc, nid_gnd, v_psu);
	_idx_buf_out_i = _buf_out.add_src_v (nid_tone2, nid_gnd, 0.f);
	_idx_buf_out_o = _buf_out.add_output (nid_vol2, nid_gnd);

	const float    r26 = pot_val;
	_buf_out.add_resistor (nid_q1b, nid_gnd, 100e3f); // R3
	_buf_out.add_resistor (nid_q1e, nid_gnd, 2.7e3f); // R4
	_buf_out.add_resistor (nid_vcc, nid_q1c, 10e3f); // R6
	_buf_out.add_resistor (nid_vcc, nid_q1b, 470e3f); // R7
	_buf_out.add_pot (nid_vol2, nid_gnd, 1.f, r26, pot_vol); // R26CCW
	_buf_out.add_pot (nid_vol2, nid_vol3, r26, 1.f, pot_vol); // R26CW
	_buf_out.add_resistor (nid_vol2, nid_gnd, 1e9f); // Makes the matrix invertible
	_buf_out.add_capacitor (nid_q1c, nid_vol3, 100e-9f); // C2
	_buf_out.add_capacitor (nid_tone2, nid_q1b, 100e-9f); // C3
	_buf_out.add_bjt_npn (nid_q1e, nid_q1b, nid_q1c, 5.911e-15f, 1.f, 1122.f, 1.271f); // Q1, 2N5089

	_buf_out.add_resistor (nid_tone2, nid_tone1, 50e3f);
	_buf_out.add_capacitor (nid_tone1, nid_gnd, 10e-9f);

	// Done
	_constructed_flag = true;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
