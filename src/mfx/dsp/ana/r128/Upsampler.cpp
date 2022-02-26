/*****************************************************************************

        Upsampler.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/r128/Upsampler.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Upsampler::process_sample_4x (float dst_ptr [4], float x) noexcept
{
	process_sample_nx <2> (dst_ptr, x);
}



// nbr_spl refers to source samples
void	Upsampler::process_block_4x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	process_block_nx <2> (dst_ptr, src_ptr, nbr_spl);
}



void	Upsampler::process_sample_2x (float dst_ptr [2], float x) noexcept
{
	process_sample_nx <1> (dst_ptr, x);
}



// nbr_spl refers to source samples
void	Upsampler::process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	process_block_nx <1> (dst_ptr, src_ptr, nbr_spl);
}



void	Upsampler::clear_buffers () noexcept
{
	_buf.fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr int	Upsampler::_fir_len;
constexpr int	Upsampler::_nbr_phases;



template <int OL2>
void	Upsampler::process_sample_nx (float dst_ptr [1 << OL2], float x) noexcept
{
	static_assert (OL2 >= 1 && OL2 <= _nbr_phases_l2, "");
	assert (dst_ptr != nullptr);

	// Inserts the new value at the end of the input buffer
	_buf [_fir_len - 1] = x;

	// Oversamples each phase
	process_sample_nx_internal <OL2> (dst_ptr, _buf.data ());

	// Shifts the input buffer
	std::copy (_buf.begin () + 1, _buf.end (), _buf.begin ());
}



template <int OL2>
void	Upsampler::process_block_nx (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	static_assert (OL2 >= 1 && OL2 <= _nbr_phases_l2, "");
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	/*** To do: exact break point should be measured for max performance.
		Maybe we don't even need to handle this case. ***/
	if (nbr_spl <= 2)
	{
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			process_sample_nx <OL2> (dst_ptr + (pos << OL2), src_ptr [pos]);
		}
	}

	else
	{
		// First pass: put the content of the past sample buffer followed by the
		// beginning of the new data in a temporary buffer.
		constexpr int  buf_len = _fir_len * 2 - 1;
		std::array <float, buf_len> buf_i;
		std::copy (_buf.begin (), _buf.end () - 1, buf_i.begin ());
		const int      p1_len = std::min (nbr_spl, _fir_len - 1);
		std::copy (src_ptr, src_ptr + p1_len + 1, buf_i.begin () + _fir_len - 1);
		process_block_nx_internal <OL2> (dst_ptr, buf_i.data (), p1_len);

		// Do we need a second pass?
		const int      rem_len = nbr_spl - p1_len;
		if (rem_len > 0)
		{
			process_block_nx_internal <OL2> (
				dst_ptr + (p1_len << OL2), src_ptr, rem_len
			);
			const auto     e_ptr = src_ptr + nbr_spl;

			// Copies the source to the buffer
			std::copy (e_ptr - (_fir_len - 1), e_ptr, _buf.begin ());
		}
		else
		{
			// Shifts previous buffer input
			std::copy (_buf.begin () + p1_len, _buf.end () - 1, _buf.begin ());

			// Appends the end of the source to the buffer
			std::copy (src_ptr, src_ptr + p1_len, _buf.end () - 1 - p1_len);
		}
	}
}



template <int OL2>
void	Upsampler::process_block_nx_internal (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr, int nbr_spl) noexcept
{
	static_assert (OL2 >= 1 && OL2 <= _nbr_phases_l2, "");
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample_nx_internal <OL2> (dst_ptr + (pos << OL2), src_ptr + pos);
	}
}



template <int OL2>
void	Upsampler::process_sample_nx_internal (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr) noexcept
{
	static_assert (OL2 >= 1 && OL2 <= _nbr_phases_l2, "");
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);

	constexpr int  p_shift  = 2 - OL2;
	constexpr int  n_phases = 1 << OL2;
	for (int ph_idx = 0; ph_idx < n_phases; ++ph_idx)
	{
		const auto &   phase = _phase_arr [ph_idx << p_shift];
		float          val   = 0;
		for (int pos = 0; pos < _fir_len; ++pos)
		{
			const auto     s = src_ptr [pos];
			const auto     h = phase [pos];
			val += s * h;
		}
		
		dst_ptr [ph_idx] = val;
	}
}



// Coefficients from p. 17
// Phase contents are time-reversed so the convolution can be computed by using
// positive indexes only.
alignas (fstb_SIMD128_ALIGN) const Upsampler::PhaseArray	Upsampler::_phase_arr =
{{
	{{ // Phase 0
		-0.0083007812500f,  0.0148925781250f, -0.0266113281250f,  0.0476074218750f,
		-0.1022949218750f,  0.9721679687500f,  0.1373291015625f, -0.0594482421875f,
		 0.0332031250000f, -0.0196533203125f,  0.0109863281250f,  0.0017089843750f
	}},
	{{ // Phase 1
		-0.0189208984375f,  0.0330810546875f, -0.0582275390625f,  0.1015625000000f,
		-0.2003173828125f,  0.7797851562500f,  0.4650878906250f, -0.1665039062500f,
		 0.0891113281250f, -0.0517578125000f,  0.0292968750000f, -0.0291748046875f
	}},
	{{ // Phase 2
		-0.0291748046875f,  0.0292968750000f, -0.0517578125000f,  0.0891113281250f,
		-0.1665039062500f,  0.4650878906250f,  0.7797851562500f, -0.2003173828125f,
		 0.1015625000000f, -0.0582275390625f,  0.0330810546875f, -0.0189208984375f
	}},
	{{ // Phase 3
		 0.0017089843750f,  0.0109863281250f, -0.0196533203125f,  0.0332031250000f,
		-0.0594482421875f,  0.1373291015625f,  0.9721679687500f, -0.1022949218750f,
		 0.0476074218750f, -0.0266113281250f,  0.0148925781250f, -0.0083007812500f
	}}
}};



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
