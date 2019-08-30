/*****************************************************************************

        DenormStop.cpp
        Author: Laurent de Soras, 2019

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



#define mfx_dsp_DenormStop_USE_SIMD



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#if defined (mfx_dsp_DenormStop_USE_SIMD)
	#include "fstb/ToolsSimd.h"
#endif
#include "mfx/dsp/DenormStop.h"

#include <cassert>



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DenormStop::process_sample (float x)
{
	return x + gen_new_rnd_val ();
}



void	DenormStop::process_block (float buf_ptr [], int nbr_spl)
{
	assert (buf_ptr != 0);
	assert (nbr_spl > 0);

	if (nbr_spl <= 2)
	{
		const float    antid = gen_new_rnd_val ();
		buf_ptr [0] += antid;
	}
	else
	{
		const int      mid_pos = nbr_spl >> 1;
		buf_ptr [      0] += +7 * fstb::ANTI_DENORMAL_F32;
		buf_ptr [mid_pos] += -3 * fstb::ANTI_DENORMAL_F32;
	}
}



// 2 interleaved channels
void	DenormStop::process_block_2chn (float buf_ptr [], int nbr_frames)
{
	assert (buf_ptr != 0);
	assert (nbr_frames > 0);

	if (nbr_frames <= 2)
	{
		const float    antid = gen_new_rnd_val ();
		buf_ptr [0] += antid;
		buf_ptr [1] += antid;
	}
	else
	{
		const int      mid_pos = nbr_frames & ~1;
		const float    a = +7 * fstb::ANTI_DENORMAL_F32;
		const float    b = -3 * fstb::ANTI_DENORMAL_F32;
		buf_ptr [          0] += a;
		buf_ptr [          1] += a;
		buf_ptr [mid_pos    ] += b;
		buf_ptr [mid_pos + 1] += b;
	}
}



// 4 interleaved channels
void	DenormStop::process_block_4chn (float buf_ptr [], int nbr_frames)
{
	assert (buf_ptr != 0);
	assert (nbr_frames > 0);

	if (nbr_frames <= 2)
	{
		const float    antid = gen_new_rnd_val ();
#if defined (mfx_dsp_DenormStop_USE_SIMD)
		auto           vb = fstb::ToolsSimd::loadu_f32 (buf_ptr);
		vb += fstb::ToolsSimd::set1_f32 (antid);
		fstb::ToolsSimd::storeu_f32 (buf_ptr, vb);
#else
		buf_ptr [0] += antid;
		buf_ptr [1] += antid;
		buf_ptr [2] += antid;
		buf_ptr [3] += antid;
#endif
	}
	else
	{
		const int      mid_pos = (nbr_frames & ~1) << 1;
		const float    a = +7 * fstb::ANTI_DENORMAL_F32;
		const float    b = -3 * fstb::ANTI_DENORMAL_F32;
#if defined (mfx_dsp_DenormStop_USE_SIMD)
		auto           vb = fstb::ToolsSimd::loadu_f32 (buf_ptr          );
		auto           vm = fstb::ToolsSimd::loadu_f32 (buf_ptr + mid_pos);
		vb += fstb::ToolsSimd::set1_f32 (a);
		vm += fstb::ToolsSimd::set1_f32 (b);
		fstb::ToolsSimd::storeu_f32 (buf_ptr          , vb);
		fstb::ToolsSimd::storeu_f32 (buf_ptr + mid_pos, vm);
#else
		buf_ptr [          0] += a;
		buf_ptr [          1] += a;
		buf_ptr [          2] += a;
		buf_ptr [          3] += a;
		buf_ptr [mid_pos    ] += b;
		buf_ptr [mid_pos + 1] += b;
		buf_ptr [mid_pos + 2] += b;
		buf_ptr [mid_pos + 3] += b;
#endif
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DenormStop::gen_new_rnd_val ()
{
	// Not thread-safe at all, but for this purpose we don't care.
	const uint32_t new_rnd_val = _rnd_val * 1234567 + 890123;
	_rnd_val = new_rnd_val;

	// Keeps only most significant bits
	const uint32_t mantissa = new_rnd_val & 0x807F0000U;

	union
	{
		float          _f;
		uint32_t       _i;
	}					multi;

	// Sets the exponent
	multi._i = mantissa | 0x1E000000;

	return multi._f;
}



uint32_t	DenormStop::_rnd_val = 0;



}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
