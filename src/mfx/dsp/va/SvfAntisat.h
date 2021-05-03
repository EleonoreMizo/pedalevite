/*****************************************************************************

        SvfAntisat.h
        Author: Laurent de Soras, 2020

State Variable Filter with antisaturation shaping curve.
3 outputs: low pass, band pass and high pass.
Able to self-oscillate.

Implements the structure described in:
Vadim Zavalishin,
The Art of VA Filter Design, v2.1.2
Fig. 6.52 - An SVF with antisaturator, chap. 6.11, p. 213

Some thoughts collected from:
https://www.kvraudio.com/forum/viewtopic.php?f=33&t=538190

The SVF is stable up to Nyquist and with resonance in the [0 ; 1] range with
all the proposed antisaturator curves. For higher resonance settings, it's
safer to oversample 2x at least.

Template parameters:

- AS: Antisaturator shape. Requires:
	static void  AS::eval (float &y, float &dy, float x);
	static float AS::eval_inv (float x);

AntisatSq    is the best compromise between rich harmonics and speed
AntisatAtanh has the hardest (more harmonics) behaviour
AntisatRcp   is the softest one
AntisatSinh  is the slowest one and most prone to generate artefacts

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_SvfAntisat_HEADER_INCLUDED)
#define mfx_dsp_va_SvfAntisat_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/va/SolverNR.h"



namespace mfx
{
namespace dsp
{
namespace va
{



template <class AS>
class SvfAntisat
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq) noexcept;
	void           set_freq (float f) noexcept;
	void           set_reso (float r) noexcept;

	void           process_sample (float &y_lp, float &y_bp, float &y_hp, float x) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class EqBp
	{
	public:
		typedef float DataType;
		fstb_FORCEINLINE void
		               set_a (float a) noexcept;
		fstb_FORCEINLINE void
		               set_b (float b) noexcept;
		fstb_FORCEINLINE void
		               set_estimation (float y) noexcept;
		fstb_FORCEINLINE float
		               estimate () noexcept;
		fstb_FORCEINLINE void
		               eval (float &y, float &dy, float x) noexcept;
	private:
		float          _a = 0;
		float          _b = 1;
		float          _one_over_b     = 1;
		float          _one_over_1_m_b = 0;
		float          _y = 0;
	};

	inline void    update_b () noexcept;

	float          _sample_freq = 0;
	float          _inv_fs      = 0;

	float          _g     = 0;
	float          _g_inv = 0;
	float          _b     = 0;
	float          _k     = 0;  // R - 1

	SolverNR <EqBp, 50, true>
	               _solver { EqBp (), 1e-6f, 0.2f };

	// States
	float          _s1 = 0;
	float          _s2 = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SvfAntisat <AS> &other) const = delete;
	bool           operator != (const SvfAntisat <AS> &other) const = delete;

}; // class SvfAntisat



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/SvfAntisat.hpp"



#endif   // mfx_dsp_va_SvfAntisat_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
