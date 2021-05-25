/*****************************************************************************

        VelvetConv.h
        Author: Laurent de Soras, 2021

Convolution between a single granule and velvet noise.

Template parameters:

- T: sample data type. Requires floating point semantics

Reference:

Stefano D'Angelo, Leonardo Gabrielli,
Efficient Signal Extrapolation by Granulation and Convolution with Velvet
Noise,
Proceedings of the 21st International Conference on Digital Audio Effects
(DAFx-18), 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_grn_VelvetConv_HEADER_INCLUDED)
#define mfx_dsp_grn_VelvetConv_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace grn
{



template <typename T>
class VelvetConv
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;

	void           reserve (int grain_len);

	void           set_granule (const T grain_ptr [], int grain_len, bool keep_buf_flag);

	void           set_density (float density) noexcept;
	inline T       process_sample () noexcept;
	void           process_block (T dst_ptr [], int nbr_spl) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Buffer headroom, samples, > 0
	static constexpr int _max_block_size = 64;

	class DiceRes
	{
	public:
		bool        _grain_flag = false;
		bool        _sign_flag  = false;
	};

	void           set_ring_buf_size (int grain_len);
	void           adapt_ring_buf_size (int grain_len);
	void           update_density () noexcept;
	inline void    generate_grain_conditional (int buf_pos) noexcept;
	inline DiceRes roll_dices () noexcept;

	static int     compute_raw_ring_buf_len (int grain_len) noexcept;
	template <typename OP>
	static inline void
	               overlap (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr, int nbr_spl, OP op) noexcept;

	typedef std::vector <T> Buffer;

	// Output ring buffer. When a sample is read, the corresponding buffer
	// cell is cleared so new frames can be mixed later without having to
	// check R/W positions etc.
	Buffer         _buf;

	// Length of the buffer in samples. Must be a power of 2.
	int            _buf_len    = 0;

	// Bit mask to wrap indexes in the buffer. Usually _buf_len - 1.
	int            _buf_msk    = 0;

	// Reading position within the buffer [0 ; _buf_len[
	int            _buf_pos    = 0;

	// Grain data
	Buffer         _grain;

	// Grain density, as an average number of overlapping grains. > 0
	float          _density    = 30;

	// Value to be hashed to form a pseudo-random value
	uint32_t       _hash_cnt   = 0;

	// Threshold below which we generate a grain
	uint32_t       _dens_thr   = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const VelvetConv &other) const = delete;
	bool           operator != (const VelvetConv &other) const = delete;

}; // class VelvetConv



}  // namespace grn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/grn/VelvetConv.hpp"



#endif   // mfx_dsp_grn_VelvetConv_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
