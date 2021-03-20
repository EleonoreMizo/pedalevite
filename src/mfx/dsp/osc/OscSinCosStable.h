/*****************************************************************************

        OscSinCosStable.h
        Author: Laurent de Soras, 2019

Algorithm from:
Martin Vicanek, A New Recursive Quadrature Oscillator, 2015-10-21
https://vicanek.de/articles/QuadOsc.pdf

Previous implementation:
Jorg Arndt, "Matters Computational", chapter 21.3.2 (Fast Transforms / The
Fourier transform / Saving trigonometric computations / Recursive generation)
http://www.jjj.de/fxt/fxtpage.html#fxtbook

The Arndt oscillator was very stable at low frequencies but suffered from
instabilities at high frequencies > fs/4. Moreover, the Vicanek oscillator
looks faster on ARM (but slower on x86/x64).

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSinCosStable_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStable_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace osc
{



template <class T>
class OscSinCosStable
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;

	               OscSinCosStable ()                             = default;
	               OscSinCosStable (const OscSinCosStable &other) = default;
	virtual        ~OscSinCosStable ()                            = default;
	OscSinCosStable &
	               operator = (const OscSinCosStable &other)      = default;

	inline void    set_phase (DataType angle_rad);
	inline void    set_step (DataType angle_rad);

   fstb_FORCEINLINE void
	               step ();
	inline void    step (DataType angle_rad);

   fstb_FORCEINLINE DataType
	               get_cos () const;
   fstb_FORCEINLINE DataType
	               get_sin () const;

	void inline    clear_buffers ();
	void inline    correct ();
	void fstb_FORCEINLINE
	               correct_fast ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               step (DataType alpha, DataType beta);

	static inline void
	               compute_step (DataType &alpha, DataType &beta, DataType angle_rad);

   DataType       _pos_cos = DataType (1);
   DataType       _pos_sin = DataType (0);
   DataType       _alpha   = DataType (0);
   DataType       _beta    = DataType (0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSinCosStable &other) const = delete;
	bool           operator != (const OscSinCosStable &other) const = delete;

}; // class OscSinCosStable



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscSinCosStable.hpp"



#endif   // mfx_dsp_osc_OscSinCosStable_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
