/*****************************************************************************

        DistRandWalk.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_DistRandWalk_HEADER_INCLUDED)
#define mfx_dsp_shape_DistRandWalk_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/nz/GaussFast.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



class DistRandWalk
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq) noexcept;
	void           clear_buffers () noexcept;
	inline float   process_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _freq       = 2300;     // Hz

	float          _sample_freq   = 44100;
	nz::GaussFast <4>
						_rnd;
	float          _sum           = 0;
	float          _val           = 0;
	float          _inc           = 0;
	float          _avg_per       = _sample_freq / float (_freq);
	int            _pos           = 0;
	int            _nbr_spl       = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DistRandWalk &other) const = delete;
	bool           operator != (const DistRandWalk &other) const = delete;

}; // class DistRandWalk



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/DistRandWalk.hpp"



#endif   // mfx_dsp_shape_DistRandWalk_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
