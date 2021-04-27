/*****************************************************************************

        DesignElliptic.h
        Author: Laurent de Soras, 2021

Elliptic filter design in the s plane.

Reference: lost/forgotten...

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_DesignElliptic_HEADER_INCLUDED)
#define mfx_dsp_iir_DesignElliptic_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



class Zpk;

class DesignElliptic
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_spec (double passband_ripple, double stopband_ripple, double passband_freq, double stopband_freq) noexcept;
	int            compute_min_order () noexcept;
	int            compute_coefs (Zpk &zpk, int order = -1);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            compute_coefs_main (Zpk &zpk);
	double         compute_eq_6_13 (double vv);
	double         compute_eq_6_15 (double mu);

	double         _passband_ripple =  1.0; // dB, > 0
	double         _stopband_ripple = 20.0; // dB, > 0
	double         _passband_freq   =  0.3; // Relative to the main frequency
	double         _stopband_freq   =  3.0; // Relative to the main frequency
	int            _order           =  1;

	int  	         _min_order       =  0;
	double         _selec_factor    =  0;
	double         _modular_const   =  0;
	bool           _valid_precomputation_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DesignElliptic &other) const = delete;
	bool           operator != (const DesignElliptic &other) const = delete;

}; // class DesignElliptic



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/DesignElliptic.hpp"



#endif   // mfx_dsp_iir_DesignElliptic_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
