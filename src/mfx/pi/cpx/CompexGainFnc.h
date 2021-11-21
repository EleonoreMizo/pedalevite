/*****************************************************************************

        CompexGainFnc.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cpx_CompexGainFnc_HEADER_INCLUDED)
#define mfx_pi_cpx_CompexGainFnc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace cpx
{



class CompexGainFnc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               CompexGainFnc () = default;

	static constexpr float  _gain_min_l2   = -16;
	static constexpr float  _gain_max_l2   =   8;

	// Gain is always 0 dB for detected volum below this value.
	static constexpr float  _active_thr_l2 = -14;

	// -60 -> -84 dB
	static constexpr float  _active_mul    = 1.0f / (-10 - _active_thr_l2);

	// Reference level for autofix, does not move when the ratio are changed.
	static constexpr float  _lvl_ref_l2    = -4;    // -24 dB

	void           update_curve (float rl, float rh, float thr_l2, float gain_l2, float knee_l2, bool autofix_flag);

	template <bool store_flag>
	fstb_FORCEINLINE float
	               compute_gain (float env_2l2);

	template <bool store_flag>
	fstb_FORCEINLINE float
	               compute_gain_l2 (float env_2l2);

	template <bool store_flag>
	fstb_FORCEINLINE fstb::Vf32
	               compute_gain (const fstb::Vf32 env_2l2);

	inline void    clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _vol_offset_pre  = -2; // log2 (threshold)
	float          _vol_offset_post = -2; // log2 (threshold) + log2 (gain)
	float          _ratio_hi = 1;         // 1 / ratio_hi(dB)
	float          _ratio_lo = 1;         // 1 / ratio_lo(dB)
	std::array <float, 3>                 // Knee formula with log2(vol) as input and log2(vol) as output. Index = coef order
	               _knee_coef_arr {{ 0, 1, 0 }};
	float          _knee_th_abs = -0.5f;  // knee_lvl(dB) * 0.5 / 6.0206. * 0.5 because it's a radius.

	float          _cur_gain = 0;          // Stored as log2.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               CompexGainFnc (const CompexGainFnc &other)     = delete;
	               CompexGainFnc (CompexGainFnc &&other)          = delete;
	CompexGainFnc &
	               operator = (const CompexGainFnc &other)        = delete;
	CompexGainFnc &
	               operator = (CompexGainFnc &&other)             = delete;
	bool           operator == (const CompexGainFnc &other) const = delete;
	bool           operator != (const CompexGainFnc &other) const = delete;

}; // class CompexGainFnc



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/cpx/CompexGainFnc.hpp"



#endif   // mfx_pi_cpx_CompexGainFnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
