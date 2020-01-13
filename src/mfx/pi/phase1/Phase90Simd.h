/*****************************************************************************

        Phase90Simd.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase1_Phase90Simd_HEADER_INCLUDED)
#define mfx_pi_phase1_Phase90Simd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if fstb_IS (ARCHI, X86)
	#include "hiir/PhaseHalfPiSse.h"
#elif fstb_IS (ARCHI, ARM)
	#include "hiir/PhaseHalfPiNeon.h"
#else
	#include "hiir/PhaseHalfPiFpu.h"
#endif

#include "mfx/pi/phase1/Phase90Interface.h"



namespace mfx
{
namespace pi
{
namespace phase1
{



template <int NC>
class Phase90Simd
:	public Phase90Interface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::pi::phase1::Phase90Interface
	int            do_get_nbr_coefs () const override;
	void           do_set_coefs (const double coef_arr []) override;
	void           do_clear_buffers () override;
	void           do_process_block (float dst_0_ptr [], float dst_1_ptr [], const float src_ptr [], int nbr_spl) override;
	void           do_process_sample (float &dst_0, float &dst_1, float src) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if fstb_IS (ARCHI, X86)
	typedef hiir::PhaseHalfPiSse <NC>  Phaser;
#elif fstb_IS (ARCHI, ARM)
	typedef hiir::PhaseHalfPiNeon <NC> Phaser;
#else
	typedef hiir::PhaseHalfPiFpu <NC>  Phaser;
#endif

	Phaser         _phaser;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Phase90Simd &other) const = delete;
	bool           operator != (const Phase90Simd &other) const = delete;

}; // class Phase90Simd



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/phase1/Phase90Simd.hpp"



#endif   // mfx_pi_phase1_Phase90Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
