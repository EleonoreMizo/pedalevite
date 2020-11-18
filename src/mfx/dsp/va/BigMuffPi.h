/*****************************************************************************

        BigMuffPi.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_BigMuffPi_HEADER_INCLUDED)
#define mfx_dsp_va_BigMuffPi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/dkm/Simulator.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{



class BigMuffPi
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Pot
	{
		Pot_SUS = 0, // Gain
		Pot_TONE,
		Pot_VOL,

		Pot_NBR_ELT
	};

	void           set_sample_freq (double sample_freq);
	void           clear_buffers ();
	void           set_pot (Pot pot, float val);
	float          process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	void           reset_stats ();
	void           get_stats (dkm::Simulator::Stats &st) const;
#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _max_it = 20;

	void           setup_circuit ();

	dkm::Simulator _dkm;
	bool           _constructed_flag = false;
	int            _idx_in  = -1;
	int            _idx_out = -1;

	// For each pot, we have two values:
	// - between the CCW pin and the cursor
	// - between the cursor and the CW pin
	// The second value is re-inverted in the construction, so both internal
	// pots should be set at the same value.
	std::array <std::array <int, 2>, Pot_NBR_ELT * 2>
	               _idx_pot_arr {{ {{ -1, -1 }}, {{ -1, -1 }}, {{ -1, -1 }} }};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if 0
	               BigMuffPi ()                        = default;
	               BigMuffPi (const BigMuffPi &other)  = default;
	               BigMuffPi (BigMuffPi &&other)       = default;
	               ~BigMuffPi ()                       = default;
	BigMuffPi &    operator = (const BigMuffPi &other) = default;
	BigMuffPi &    operator = (BigMuffPi &&other)      = default;
#endif
	bool           operator == (const BigMuffPi &other) const = delete;
	bool           operator != (const BigMuffPi &other) const = delete;

}; // class BigMuffPi



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/BigMuffPi.hpp"



#endif   // mfx_dsp_va_BigMuffPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
