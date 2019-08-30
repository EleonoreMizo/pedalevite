/*****************************************************************************

        RemezPoint.h
        Author: Laurent de Soras, 2019

This class determines the specification of a band for the Parks-McClellan
(Remez) algorithm.

lower_freq and upper_freq are the bound of the bands. Outside this range,
filter is not specified (transition bands).

amp is the desired band amplitude (linear scale)

weight is the inverse error tolerance, compared to other bands. The lower
weight, the bigger ripples on this band, relatively to others. Weight of the
stopband can be calculated with Remez::compute_ripple_ratio () function if
passband weight is set to 1.0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_RemezPoint_HEADER_INCLUDED)
#define mfx_dsp_fir_RemezPoint_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace fir
{



class RemezPoint
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               RemezPoint ()                               = default;
	               RemezPoint (const RemezPoint &other)        = default;
	explicit       RemezPoint (double lower_freq, double upper_freq, double amp, double weight = 1);
	virtual        ~RemezPoint ()                              = default;
	RemezPoint &   operator = (const RemezPoint &other)        = default;

	void           set_lower_freq (double freq);
	double         get_lower_freq () const;
	void           set_upper_freq (double freq);
	double         get_upper_freq () const;

	void           set_amp (double amp);
	double         get_amp () const;

	void           set_weight (double weight);
	double         get_weight () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _lower_freq = 0.2;
	double         _upper_freq = 0.3;
	double         _amp        = 1.0;
	double         _weight     = 1.0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:


}; // class RemezPoint



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/fir/RemezPoint.hpp"



#endif   // mfx_dsp_fir_RemezPoint_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
