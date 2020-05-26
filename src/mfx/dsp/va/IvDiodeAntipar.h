/*****************************************************************************

        IvDiodeAntipar.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_IvDiodeAntipar_HEADER_INCLUDED)
#define mfx_dsp_va_IvDiodeAntipar_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/va/IvInterface.h"



namespace mfx
{
namespace dsp
{
namespace va
{



class IvDiodeAntipar
:	public IvInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    set_d1_is (float is);
	inline void    set_d2_is (float is);
	inline void    set_d1_n (float n);
	inline void    set_d2_n (float n);

	// mfx::dsp::va::IvInterface
	fstb_FORCEINLINE void
	               eval (float &y, float &dy, float x) const final;
	fstb_FORCEINLINE float
	               get_max_step (float x) const final;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline float   compute_max_step (float n) const;
	inline float   compute_mv (float n) const;

	float          _vt   = 0.026f;   // Diode thermal voltage, volt. Sets the diode clipping threshold, around 0.65 V for 0.026
	float          _is1  = 0.1e-15f; // Diode 1 saturation current, ampere
	float          _is2  = 0.1e-6f;  // Diode 2 saturation current, ampere
	float          _n1   = 1;        // Diode 1 ideality factor (or scale, or serial multiplier)
	float          _n2   = 4;        // Diode 2 ideality factor ( " )

	float          _max_step_1 = compute_max_step (_n1); // Maximum voltager deviation between two iterations
	float          _max_step_2 = compute_max_step (_n2);
	float          _mv1        =  compute_mv (_n1);
	float          _mv2        = -compute_mv (_n2);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const IvDiodeAntipar &other) const = delete;
	bool           operator != (const IvDiodeAntipar &other) const = delete;

}; // class IvDiodeAntipar



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/IvDiodeAntipar.hpp"



#endif   // mfx_dsp_va_IvDiodeAntipar_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
