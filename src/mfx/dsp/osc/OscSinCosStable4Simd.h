/*****************************************************************************

        OscSinCosStable4Simd.h
        Author: Laurent de Soras, 2021

Template parameters:

- VP: class writing and reading memory with SIMD vectors (internal data).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr) noexcept;
	static fstb::Vf32 VD::load_f32 (const void *ptr) noexcept;
	static void VD::store_f32 (void *ptr, fstb::Vf32 val) noexcept;

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSinCosStable4Simd_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStable4Simd_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"

#include <tuple>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <class VP>
class OscSinCosStable4Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    set_phase (fstb::Vf32 angle_rad) noexcept;
	inline void    set_step (fstb::Vf32 angle_rad) noexcept;

	fstb_FORCEINLINE std::tuple <fstb::Vf32, fstb::Vf32>
	               step () noexcept;
	inline std::tuple <fstb::Vf32, fstb::Vf32>
	               step (fstb::Vf32 angle_rad) noexcept;

   fstb_FORCEINLINE fstb::Vf32
	               get_cos () const noexcept;
   fstb_FORCEINLINE fstb::Vf32
	               get_sin () const noexcept;

	inline void    clear_buffers () noexcept;
	void fstb_FORCEINLINE
	               correct_fast () noexcept;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE std::tuple <fstb::Vf32, fstb::Vf32>
	               step (fstb::Vf32 alpha, fstb::Vf32 beta) noexcept;

	static inline std::tuple <fstb::Vf32, fstb::Vf32>
	               compute_step (fstb::Vf32 angle_rad) noexcept;

   fstb::Vf32     _pos_cos { 1 };
   fstb::Vf32     _pos_sin { 0 };
   fstb::Vf32     _alpha   { 0 };
   fstb::Vf32     _beta    { 0 };



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSinCosStable4Simd &other) const = delete;
	bool           operator != (const OscSinCosStable4Simd &other) const = delete;

}; // class OscSinCosStable4Simd



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscSinCosStable4Simd.hpp"



#endif   // mfx_dsp_osc_OscSinCosStable4Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
