/*****************************************************************************

        Downsampler2xF64Sse2.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (hiir_Downsampler2xF64Sse2_HEADER_INCLUDED)
#define hiir_Downsampler2xF64Sse2_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"
#include "hiir/StageDataF64Sse2.h"

#include <emmintrin.h>

#include <array>



namespace hiir
{



template <int NC>
class Downsampler2xF64Sse2
{

	static_assert ((NC > 0), "Number of coefficient must be positive.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef double DataType;
	static const int  _nbr_chn = 1;

	enum {         NBR_COEFS = NC };

	               Downsampler2xF64Sse2 ();
	               Downsampler2xF64Sse2 (const Downsampler2xF64Sse2 <NC> &other) = default;
	               Downsampler2xF64Sse2 (Downsampler2xF64Sse2 <NC> &&other)      = default;

	Downsampler2xF64Sse2 <NC> &
	               operator = (const Downsampler2xF64Sse2 <NC> &other) = default;
	Downsampler2xF64Sse2 <NC> &
	               operator = (Downsampler2xF64Sse2 <NC> &&other)      = default;

	void           set_coefs (const double coef_arr []);

	hiir_FORCEINLINE double
	               process_sample (const double in_ptr [2]);
	void           process_block (double out_ptr [], const double in_ptr [], long nbr_spl);

	hiir_FORCEINLINE void
	               process_sample_split (double &low, double &high, const double in_ptr [2]);
	void           process_block_split (double out_l_ptr [], double out_h_ptr [], const double in_ptr [], long nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _stage_width = 2;
	static const int  _nbr_stages  = (NBR_COEFS + _stage_width - 1) / _stage_width;

	// Stage 0 contains only input memory
	typedef std::array <StageDataF64Sse2, _nbr_stages + 1> Filter;

	Filter         _filter;		// Should be the first member (thus easier to align)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Downsampler2xF64Sse2 <NC> &other) const = delete;
	bool           operator != (const Downsampler2xF64Sse2 <NC> &other) const = delete;

}; // class Downsampler2xF64Sse2



}  // namespace hiir



#include "hiir/Downsampler2xF64Sse2.hpp"



#endif   // hiir_Downsampler2xF64Sse2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
