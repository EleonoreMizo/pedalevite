/*****************************************************************************

        WhiteFast.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_nz_WhiteFast_HEADER_INCLUDED)
#define mfx_dsp_nz_WhiteFast_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace nz
{



class WhiteFast
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               WhiteFast ()                        = default;
	virtual        ~WhiteFast ()                       = default;
	               WhiteFast (const WhiteFast &other)  = default;
	WhiteFast &    operator = (const WhiteFast &other) = default;

	void           set_seed (uint32_t seed);
	void           set_rough_level (float lvl);
	inline float   process_sample ();
	void           process_block (float dst_ptr [], int nbr_spl);
	void           process_block_add (float dst_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const uint32_t   _a        = 1103515245;
	static const uint32_t   _c        = 12345;
	static const uint32_t   _m        = 1U << 31;
	static const uint32_t   _mm       = _m - 1;      // Logic AND mask
	static const uint32_t   _mant_and = 0x807FFFFF;

	union Mixed
	{
		uint32_t       _i;
		float          _f;
	};

	inline float   gen_new_val (uint32_t &state);

	uint32_t       _state  = 1;
	uint32_t       _exp_or = 0x3F000000;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const WhiteFast &other) const = delete;
	bool           operator != (const WhiteFast &other) const = delete;

}; // class WhiteFast



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/nz/WhiteFast.hpp"



#endif   // mfx_dsp_nz_WhiteFast_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
