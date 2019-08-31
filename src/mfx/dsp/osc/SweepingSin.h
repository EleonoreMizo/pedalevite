/*****************************************************************************

        SweepingSin.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_SweepingSin_HEADER_INCLUDED)
#define mfx_dsp_osc_SweepingSin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace osc
{



class SweepingSin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       SweepingSin (double fs, double f1, double f2);
	virtual        ~SweepingSin () = default;

	void           generate (float data_ptr [], int nbr_spl);
	void           generate (float data_ptr [], int nbr_spl, int block_start, int block_len);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _fs = 44100.0;   // Hz
	double         _f1 =    20.0;   // Hz, ]0 ; _f2[
	double         _f2 = 20000.0;   // Hz, ]f1 ; fs * 0.5[



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SweepingSin ()                               = delete;
	               SweepingSin (const SweepingSin &other)       = delete;
	SweepingSin &  operator = (const SweepingSin &other)        = delete;
	bool           operator == (const SweepingSin &other) const = delete;
	bool           operator != (const SweepingSin &other) const = delete;

}; // class SweepingSin



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/osc/SweepingSin.hpp"



#endif   // mfx_dsp_osc_SweepingSin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
