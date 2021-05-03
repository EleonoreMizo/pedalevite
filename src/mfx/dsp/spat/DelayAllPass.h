/*****************************************************************************

        DelayAllPass.h
        Author: Laurent de Soras, 2020

All-pass delay
https://ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html

Template parameters: same as DelayFrac.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_DelayAllPass_HEADER_INCLUDED)
#define mfx_dsp_spat_DelayAllPass_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/spat/DelayFrac.h"

#include <utility>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace spat
{



template <typename T, int NPL2>
class DelayAllPass
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;
	typedef DelayFrac <T, NPL2> Delay;

	static constexpr int _nbr_phases_l2 = Delay::_nbr_phases_l2;
	static constexpr int _nbr_phases    = Delay::_nbr_phases;
	// + 1 because we read the line before writing it.
	static constexpr int _delay_min     = Delay::_delay_min + 1; // Samples

	void           set_max_len (int len);

	fstb_FORCEINLINE void
	               set_delay_flt (float len_spl) noexcept;
	fstb_FORCEINLINE void
	               set_delay_fix (int len_fixp) noexcept;
	fstb_FORCEINLINE void
	               set_coef (T coef) noexcept;

	fstb_FORCEINLINE std::pair <T, T>
	               read (T x) const noexcept;
	fstb_FORCEINLINE T
	               read_at (int delay) const noexcept;
	fstb_FORCEINLINE void
	               write (T v) noexcept;
	fstb_FORCEINLINE void
	               step () noexcept;
	fstb_FORCEINLINE T
	               process_sample (T x) noexcept;

	fstb_FORCEINLINE int
	               get_max_block_len () const noexcept;
	fstb_FORCEINLINE void
	               read_block_at (T dst_ptr [], int delay, int len) const noexcept;
	void           process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept;
	void           process_block_var_dly (T dst_ptr [], const T src_ptr [], const int32_t dly_frc_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;





/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Delay         _delay;
	T             _coef { 0.f };



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayAllPass &other) const = delete;
	bool           operator != (const DelayAllPass &other) const = delete;

}; // class DelayAllPass



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/DelayAllPass.hpp"



#endif   // mfx_dsp_spat_DelayAllPass_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
