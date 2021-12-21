/*****************************************************************************

        HalfBandNeon.h
        Author: Laurent de Soras, 2016

Half-band filter, low-pass or high-pass, using NEON instruction set.

This object must be aligned on a 16-byte boundary!

If the number of coefficients is 2 or 3 modulo 4, the output is delayed from
1 sample, compared to the theoretical formula (or FPU implementation).

Template parameters:
	- NC: number of coefficients, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (hiir_HalfBandNeon_HEADER_INCLUDED)
#define hiir_HalfBandNeon_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataNeonV4.h"

#include <arm_neon.h>

#include <array>



namespace hiir
{



template <int NC>
class HalfBandNeon
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef float DataType;
	static constexpr int _nbr_chn  = 1;
	static constexpr int NBR_COEFS = NC;

	               HalfBandNeon () noexcept;
	               HalfBandNeon (const HalfBandNeon <NC> &other) = default;
	               HalfBandNeon (HalfBandNeon <NC> &&other)      = default;
	               ~HalfBandNeon ()                                 = default;

	HalfBandNeon <NC> &
	               operator = (const HalfBandNeon <NC> &other)      = default;
	HalfBandNeon <NC> &
	               operator = (HalfBandNeon <NC> &&other)           = default;

	void           set_coefs (const double coef_arr []) noexcept;

	hiir_FORCEINLINE float
	               process_sample (float input) noexcept;
	void           process_block (float out_ptr [], const float in_ptr [], long nbr_spl) noexcept;

	hiir_FORCEINLINE float
	               process_sample_hpf (float input) noexcept;
	void           process_block_hpf (float out_ptr [], const float in_ptr [], long nbr_spl) noexcept;

	hiir_FORCEINLINE void
	               process_sample_split (float &low, float &hi, float input) noexcept;
	void           process_block_split (float out_l_ptr [], float out_h_ptr [], const float in_ptr [], long nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int STAGE_WIDTH = 4;
	static constexpr int NBR_STAGES  = (NBR_COEFS + STAGE_WIDTH-1) / STAGE_WIDTH;
	static constexpr int NBR_PHASES  = 2;

	typedef std::array <StageDataNeonV4, NBR_STAGES + 1> Filter;  // Stage 0 contains only input memory
   typedef std::array <Filter, NBR_PHASES> FilterBiPhase;

	hiir_FORCEINLINE float32x4_t
	               process_2_paths (float input) noexcept;

	FilterBiPhase  _filter;    // Should be the first member (thus easier to align)
	float          _prev;
	int            _phase;     // 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const HalfBandNeon <NC> &other) const = delete;
	bool           operator != (const HalfBandNeon <NC> &other) const = delete;

}; // class HalfBandNeon



}  // namespace hiir



#include "hiir/HalfBandNeon.hpp"



#endif   // hiir_HalfBandNeon_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
