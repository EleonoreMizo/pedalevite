/*****************************************************************************

        PinkShade.cpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/nz/PinkShade.h"
#include "mfx/dsp/mix/Generic.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace nz
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PinkShade::PinkShade ()
:	_buf ()
,	_bpos (0)
,	_lfsr (0x5EED41F5)
,	_inc (0x4CCCC)    // Balance initial states to avoid DC 
,	_dec (0x4CCCC)
,	_accu ()
,	_ncnt (0)         // Counter from zero
{
	mix::Generic::setup ();
}



void	PinkShade::set_seed (int seed)
{
	_lfsr = 0x5EED41F5 + seed;
}



void	PinkShade::process_block (float dst_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);

	int            pos = 0;
	do
	{
		const int      rem_dst  = nbr_spl - pos;
		if (_bpos == 0 && rem_dst >= _buf_len)
		{
			gen_16 (dst_ptr + pos);
			pos += _buf_len;
		}
		else
		{
			if (_bpos == 0)
			{
				gen_16 (&_buf [0]);
			}
			const int      rem_buf  = _buf_len - _bpos;
			const int      work_len = std::min (rem_buf, rem_dst);
			mix::Generic::copy_1_1 (dst_ptr + pos, &_buf [_bpos], work_len);
			_bpos = (_bpos + work_len) & (_buf_len - 1);
			pos  += work_len;
		}
	}
	while (pos < nbr_spl);
}



void	PinkShade::process_block_add (float dst_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);

	int            pos = 0;
	do
	{
		if (_bpos == 0)
		{
			gen_16 (&_buf [0]);
		}
		const int      rem_dst  = nbr_spl - pos;
		const int      rem_buf  = _buf_len - _bpos;
		const int      work_len = std::min (rem_buf, rem_dst);
		mix::Generic::mix_1_1 (dst_ptr + pos, &_buf [_bpos], work_len);
		_bpos = (_bpos + work_len) & (_buf_len - 1);
		pos  += work_len;
	}
	while (pos > 0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Generates 16 samples
void	PinkShade::gen_16 (float *dst_ptr)
{
	assert (dst_ptr != 0);

	int            inc  = _inc;   // Load old fashioned register variables-
	int            dec  = _dec;   // as ugly as this might seem, it improves 
	Combo          accu = _accu;  // the generated code for most compilers
	int            lfsr = _lfsr;
	int            bit;

	int            mask = _nmask [_ncnt];
	++ _ncnt;                     // _ncnt is 8 bit for relaxed modulo

#define PINK( bitmask) do {      /* Macro for processing:            */ \
	bit    = lfsr >> 31;          /* Spill random to all bits         */ \
	dec   &= ~bitmask;            /* Blank old decrement bit          */ \
	lfsr <<= 1;                   /* Shift lfsr                       */ \
	dec   |= inc & bitmask;       /* Copy increment to decrement bit  */ \
	inc   ^= bit & bitmask;       /* New random bit                   */ \
	const float    res = accu._f; /* Save biased value as float       */ \
	accu._i += inc - dec;         /* Integrate                        */ \
	lfsr    ^= bit & 0x46000001;  /* Update lfsr                      */ \
	const int      idx_a =  lfsr       & 0x3F; /* Add 1st half FIR & subtract bias */ \
	const int      idx_b = (lfsr >> 6) & 0x3F; /* Add 2nd half precalculated FIR.  */ \
	*dst_ptr = res + _fir_a [idx_a] + _fir_b [idx_b]; \
	++ dst_ptr; \
	} while (false)

	PINK (mask    ); PINK (0x040000); PINK (0x020000); PINK (0x040000);
	PINK (0x010000); PINK (0x040000); PINK (0x020000); PINK (0x040000);
	PINK (0x008000); PINK (0x040000); PINK (0x020000); PINK (0x040000);
	PINK (0x010000); PINK (0x040000); PINK (0x020000); PINK (0x040000);

#undef PINK

	_inc  = inc;   // Write back variables
	_dec  = dec;
	_accu = accu;
	_lfsr = lfsr;
}



#if    defined (F)    || defined (FA)  || defined (FB) \
    || defined (PM16) || defined (FA8) || defined (FB8)
	#error
#endif

// Let preprocessor and compiler calculate two lookup tables for 12-tap FIR filter with these coefficients:
// 1.190566,0.162580,0.002208,0.025475,-0.001522,0.007322,0.001774,0.004529,-0.001561,0.000776,-0.000486,0.002017

#define F( cf, m, shift) (float (0.0625f * cf * (2 * ((m) >> shift & 1) - 1)))

#define FA( n)  F(1.190566,n,0)+F(0.162580,n,1)+F(0.002208,n,2)+ F(0.025475,n,3)+F(-0.001522,n,4)+F(0.007322,n,5)-float (_pink_bias)
#define FB( n)  F(0.001774,n,0)+F(0.004529,n,1)+F(-0.001561,n,2)+F(0.000776,n,3)+F(-0.000486,n,4)+F(0.002017,n,5)

#define FA8( n) FA(n),FA(n+1),FA(n+2),FA(n+3),FA(n+4),FA(n+5),FA(n+6),FA(n+7)
#define FB8( n) FB(n),FB(n+1),FB(n+2),FB(n+3),FB(n+4),FB(n+5),FB(n+6),FB(n+7)

const float	 PinkShade::_fir_a [64] =  // 1st FIR lookup table
{
	FA8(0),FA8(8),FA8(16),FA8(24),FA8(32),FA8(40),FA8(48),FA8(56)
};
const float	 PinkShade::_fir_b [64] =  // 2nd FIR lookup table
{
	FB8(0),FB8(8),FB8(16),FB8(24),FB8(32),FB8(40),FB8(48),FB8(56)
};

#undef F
#undef FA
#undef FB
#undef FA8
#undef FB8

// Bitreversed lookup table
#define PM16(n)   n<<7,0x4000,0x2000,0x4000,0x1000,0x4000,0x2000,0x4000, \
                0x0800,0x4000,0x2000,0x4000,0x1000,0x4000,0x2000,0x4000

const unsigned int	PinkShade::_nmask [256] = 
{
    PM16(0),PM16(0x08),PM16(0x04),PM16(0x08),PM16(2),PM16(0x08),PM16(0x04),PM16(0x08),
    PM16(1),PM16(0x08),PM16(0x04),PM16(0x08),PM16(2),PM16(0x08),PM16(0x04),PM16(0x08)
};

#undef PM16



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
