/*****************************************************************************

        SvfMixerHigh.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SvfMixerHigh_HEADER_INCLUDED)
#define mfx_dsp_iir_SvfMixerHigh_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



class SvfMixerHigh
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	static fstb_FORCEINLINE T
	               mix (T v0, T v1, T v2, T v0m, T v1m, T v2m);

	template <typename T>
	static fstb_FORCEINLINE void
	               inc (T &v0m, T &v1m, T &v2m, T v0mi, T v1mi, T v2mi);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SvfMixerHigh ()                               = delete;
	               SvfMixerHigh (const SvfMixerHigh &other)      = delete;
	               ~SvfMixerHigh ()                              = delete;
	SvfMixerHigh &
	               operator = (const SvfMixerHigh &other)        = delete;
	bool           operator == (const SvfMixerHigh &other) const = delete;
	bool           operator != (const SvfMixerHigh &other) const = delete;

}; // class SvfMixerHigh



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SvfMixerHigh.hpp"



#endif   // mfx_dsp_iir_SvfMixerHigh_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
