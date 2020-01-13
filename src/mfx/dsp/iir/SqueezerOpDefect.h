/*****************************************************************************

        SqueezerOpDefect.h
        Author: Laurent de Soras, 2016

Template parameters:

- HA: Harndness factor. 0 = smooth, 2 = mild, 5 = hard

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SqueezerOpDefect_HEADER_INCLUDED)
#define mfx_dsp_iir_SqueezerOpDefect_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int HA>
class SqueezerOpDefect
{
	static_assert (HA >= 0, "");
	static_assert (HA <= 5, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    config (float reso, float p1);
	inline float   process_sample (float x);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _param   = 1.25f;
	float          _param_i = 1.0f / _param;
	float          _param_2 = 2 * _param;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SqueezerOpDefect <HA> &other) const = delete;
	bool           operator != (const SqueezerOpDefect <HA> &other) const = delete;

}; // class SqueezerOpDefect



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SqueezerOpDefect.hpp"



#endif   // mfx_dsp_iir_SqueezerOpDefect_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
