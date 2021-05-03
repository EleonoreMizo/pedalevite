/*****************************************************************************

        WhiteAccurate.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_nz_WhiteAccurate_HEADER_INCLUDED)
#define mfx_dsp_nz_WhiteAccurate_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace nz
{



class WhiteAccurate
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_seed (uint32_t seed) noexcept;
	void           set_rough_level (float lvl) noexcept;
	inline float   process_sample () noexcept;
	void           process_block (float dst_ptr [], int nbr_spl) noexcept;
	void           process_block_add (float dst_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr float  _scale_unity = 2 * fstb::TWOPM32;

	inline float   gen_new_val () noexcept;

	uint32_t       _idx   = 1;
	float          _scale = _scale_unity;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const WhiteAccurate &other) const = delete;
	bool           operator != (const WhiteAccurate &other) const = delete;

}; // class WhiteAccurate



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/nz/WhiteAccurate.hpp"



#endif   // mfx_dsp_nz_WhiteAccurate_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
