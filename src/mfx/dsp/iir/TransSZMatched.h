/*****************************************************************************

        TransSZMatched.h
        Author: Laurent de Soras, 2017

Formulas from:

Martin Vicanek, Matched Second Order Digital Filters, 2016-02-14,
http://www.vicanek.de/articles/BiquadFits.pdf

The low/band/high-pass parts don't sum very well when the center frequency is
high so this design is not suited for generic biquad transformation.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_TransSZMatched_HEADER_INCLUDED)
#define mfx_dsp_iir_TransSZMatched_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



class TransSZMatched
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:


	template <class TS, class TZ>
	static void    map_s_to_z (TZ z_eq_b [3], TZ z_eq_a [3], const TS s_eq_b [3], const TS s_eq_a [3], double f0, double fs);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <class TS>
	static double  compute_h2s (const TS s_eq_b [3], const TS s_eq_a [3], double w);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TransSZMatched ()                               = delete;
	               TransSZMatched (const TransSZMatched &other)    = delete;
	virtual        ~TransSZMatched ()                              = delete;
	TransSZMatched &
	               operator = (const TransSZMatched &other)        = delete;
	bool           operator == (const TransSZMatched &other) const = delete;
	bool           operator != (const TransSZMatched &other) const = delete;

}; // class TransSZMatched



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/TransSZMatched.hpp"



#endif   // mfx_dsp_iir_TransSZMatched_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
