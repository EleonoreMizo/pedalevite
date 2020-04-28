/*****************************************************************************

        PinkShade.h
        Author: Laurent de Soras, 2018

Generates pink noise with a -3 dB curve on 12 octaves up to Nyquist (above
5 Hz @ 44.1 kHz sampling rate).

Algorithm "A New Shade of Pink" by Stefan Stenzel, 2014
http://stenzel.waldorfmusic.de/post/a-new-shade-of-pink/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_nz_PinkShade_HEADER_INCLUDED)
#define mfx_dsp_nz_PinkShade_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace nz
{



class PinkShade
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PinkShade ();
	               PinkShade (const PinkShade &other)  = default;
	               PinkShade (PinkShade &&other)       = default;

	               ~PinkShade ()                       = default;

	PinkShade &    operator = (const PinkShade &other) = default;
	PinkShade &    operator = (PinkShade &&other)      = default;

	void           set_seed (int seed);
	inline float   process_sample ();
	void           process_block (float dst_ptr [], int nbr_spl);
	void           process_block_add (float dst_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _pink_bias =  3;  // Some temporary bias required for hack on floats
	static const int  _buf_len   = 16;  // Buffer size. Must be a power of 2.
	static_assert ((_buf_len & -_buf_len) == _buf_len, "must be a power of 2");

	union Combo
	{
		float          _f = float (_pink_bias);
		int32_t        _i;
	};

	void           gen_16 (float *dst_ptr);

	std::array <float, _buf_len>  // Pre-generated data
	               _buf;
	int            _bpos;         // Current reading position within the buffer, [0 ; _buf_len-1]. 0 = buffer needs to be generated beforehand
	int            _lfsr;         // Linear feedback shift register
	int            _inc;          // Increment for all noise sources (bits)
	int            _dec;          // Decrement for all noise sources
	Combo          _accu;         // Accu, also interpreted as float
	uint8_t        _ncnt;         // Overflowing counter as index to _nmask

	static const unsigned int
	               _nmask [256];
	static const float
	               _fir_a [64];
	static const float
	               _fir_b [64];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PinkShade &other) const = delete;
	bool           operator != (const PinkShade &other) const = delete;

}; // class PinkShade



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/nz/PinkShade.hpp"



#endif   // mfx_dsp_nz_PinkShade_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
