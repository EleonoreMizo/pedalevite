/*****************************************************************************

        InterpFirPolyphase.h
        Author: Laurent de Soras, 2019

This class can be inherited but is not polymorph.

Template parameters:

- IP: interpolation phase. Should have the following members:
	IP::IP ();
	void IP::set_data (int pos, float imp, float dif);
	void IP::precompute_impulse (IP::Buffer &imp, float q) const;
	float	IP::convolve (const float data_ptr [], const IP::Buffer &imp) const;
	float IP::convolve (const float data_ptr [], float q) const;
	static const int IP::PHASE_LEN;
	typename IP::Buffer;
	IP::Buffer::Buffer ();
	float IP::Buffer::operator [] (size_t);
	const float IP::Buffer::operator [] (size_t) const;

	IP must have its alignment already fixed if it requires a specific
	one; same for IP::Buffer. Also, IP::Buffer may be built on the stack.

- NPL2: base-2 logarithm of the number of phases for the filter.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_rspl_InterpFirPolyphase_HEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFirPolyphase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace rspl
{



template <class IP, int NPL2>
class InterpFirPolyphase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef IP InterpPhase;
	static const int NBR_PHASES_L2 = NPL2;

	static const int NBR_PHASES  = 1 << NBR_PHASES_L2;
	static const int PHASE_LEN   = InterpPhase::PHASE_LEN;
	static const int IMPULSE_LEN = NBR_PHASES * PHASE_LEN;

	void				set_impulse (const double impulse_ptr [IMPULSE_LEN]) noexcept;
	fstb_FORCEINLINE float
	               interpolate (const float data_ptr [], uint32_t frac_pos) const noexcept;
	fstb_FORCEINLINE void
	               interpolate_multi_chn (float * const out_ptr_arr [], long out_offset, const float * const in_ptr_arr [], long in_offset, uint32_t frac_pos, int nbr_chn) const noexcept;
	fstb_FORCEINLINE void
	               interpolate_multi_chn (float out_ptr [], const float in_ptr [], uint32_t frac_pos, int nbr_chn, long chn_stride_in, long chn_stride_out) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static fstb_FORCEINLINE void
	               compute_phase_info (float &q, int &phase_index, uint32_t frac_pos) noexcept;

	std::array <InterpPhase, NBR_PHASES>
	               _phase_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const InterpFirPolyphase &other) const = delete;
	bool           operator != (const InterpFirPolyphase &other) const = delete;

}; // class InterpFirPolyphase



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/rspl/InterpFirPolyphase.hpp"



#endif   // mfx_dsp_rspl_InterpFirPolyphase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
