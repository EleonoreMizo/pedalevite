/*****************************************************************************

        VelvetConv.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_grn_VelvetConv_CODEHEADER_INCLUDED)
#define mfx_dsp_grn_VelvetConv_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/Hash.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace grn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: reserve
Description:
	Reserve enough memory to welcome grains of a given size without further
	allocation. This helps using set_granule() in a RT thread.
Input parameters:
	- grain_len: maximum length of the grain in samples, > 0.
Throws: standard container related exepctions
==============================================================================
*/

template <typename T>
void	VelvetConv <T>::reserve (int grain_len)
{
	assert (grain_len > 0);

	_buf.reserve (compute_raw_ring_buf_len (grain_len));
	_grain.reserve (grain_len);
}



/*
==============================================================================
Name: set_granule
Description:
	Set the current grain.
	The grain should be properly windowed.
	Call to this function is mandatory before using any other processing
	function.
	The function performs memory allocations if the grain size is greater
	than a previous call to reserve().
Input parameters:
	- grain_ptr: pointer on the grain data.
	- grain_len: grain size in samples, > 0.
	- keep_buf_flag: indicates that we current buffer content has to be
		preserved. This is important when changing the grain size in the
		middle of a processing.
Throws: standard container related exceptions if memory allocations occur
==============================================================================
*/

template <typename T>
void	VelvetConv <T>::set_granule (const T grain_ptr [], int grain_len, bool keep_buf_flag)
{
	assert (grain_ptr != nullptr);
	assert (grain_len > 0);

	// Changes the ring buffer length if required
	if (grain_len != int (_grain.size ()))
	{
		if (keep_buf_flag)
		{
			adapt_ring_buf_size (grain_len);
		}
		else
		{
			set_ring_buf_size (grain_len);
		}
	}

	// Stores the new grain
	_grain.assign (grain_ptr, grain_ptr + grain_len);

	update_density ();
}



/*
==============================================================================
Name: set_density
Description:
	Defines the grain density.
Input parameters:
	- density: average number of overlapping grains. > 0. 30 is usually a good
		number.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	VelvetConv <T>::set_density (float density) noexcept
{
	assert (density > 0);

	_density = density;

	if (! _grain.empty ())
	{
		update_density ();
	}
}



/*
==============================================================================
Name: process_sample
Description:
	Generates a single sample output
Returns:
	The output sample
Throws: Nothing
==============================================================================
*/

template <typename T>
T	VelvetConv <T>::process_sample () noexcept
{
	assert (! _grain.empty ());

	const T        read_val = _buf [_buf_pos];
	_buf [_buf_pos] = T (0);
	_buf_pos        = (_buf_pos + 1) & _buf_msk;

	generate_grain_conditional (_buf_pos);

	return read_val;
}



/*
==============================================================================
Name: process_block
Description:
	Generates the output for a block of samples
Input parameters:
	- dst_ptr: pointer on the destination samples
	- nbr_spl: number of samples to generate, > 0
Throws: Nothing
==============================================================================
*/

template <typename T>
void	VelvetConv <T>::process_block (T dst_ptr [], int nbr_spl) noexcept
{
	assert (! _grain.empty ());
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);

	dsp::dly::RingBufVectorizer rbv (_buf_len);
	for (rbv.start (nbr_spl, _buf_pos); rbv.end (); rbv.next ())
	{
		int            seg_len = rbv.get_seg_len ();
		_buf_pos = rbv.get_curs_pos (0);

		do
		{
			const int      work_len = std::min (seg_len, int (_max_block_size));
			T *            src_ptr  = &_buf [_buf_pos];
			std::copy (src_ptr, src_ptr + work_len, dst_ptr);
			std::fill (src_ptr, src_ptr + work_len, T (0));

			for (int k = 0; k < work_len; ++k)
			{
				generate_grain_conditional (_buf_pos + k);
			}

			_buf_pos += work_len;
			dst_ptr  += work_len;
			seg_len  -= work_len;
		}
		while (seg_len > 0);
	}

	_buf_pos = rbv.get_curs_pos (0);
}



/*
==============================================================================
Name: clear_buffers
Description:
Throws: Nothing
==============================================================================
*/

template <typename T>
void	VelvetConv <T>::clear_buffers () noexcept
{
	_hash_cnt = 0;
	_buf_pos  = 0;
	std::fill (_buf.begin (), _buf.end (), T (0));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	VelvetConv <T>::set_ring_buf_size (int grain_len)
{
	assert (grain_len > 0);

	_buf_len    = compute_raw_ring_buf_len (grain_len);
	_buf_msk    = _buf_len - 1;
	_buf_pos   &= _buf_msk;
	_buf.resize (_buf_len, T (0)); // Fills new samples with zeros
}



// We don't reduce the buffer size if the new grain is shorter than the
// previous one in order to keep the grains already stored in the buffer.
template <typename T>
void	VelvetConv <T>::adapt_ring_buf_size (int grain_len)
{
	assert (grain_len > 0);

	const int      buf_len_new = compute_raw_ring_buf_len (grain_len);
	if (buf_len_new > _buf_len)
	{
		const int      buf_len_old = _buf_len;
		_buf_len = buf_len_new;
		_buf_msk = _buf_len - 1;
		_buf.resize (_buf_len);

		const int      add_len = buf_len_new - buf_len_old;
		auto           it      = _buf.begin ();

		// First, copies the beginning of the buffer to the newly created space.
		const int      beg_len = std::min (add_len, _buf_pos);
		fstb::copy_no_overlap (&*it + buf_len_old, &*it, beg_len);

		// Moves the remaining data to the end
		const int      mov_rem_len = add_len - beg_len;
		if (mov_rem_len > 0)
		{
			std::copy (it + _buf_pos - mov_rem_len, it + _buf_pos, it);
		}

		// Now clears data ahead of the previously stored data
		const int      clr_pos = (_buf_pos + buf_len_old) & _buf_msk;
		const int      clr_len = std::min (add_len, buf_len_new - clr_pos);
		std::fill (it + clr_pos, it + clr_pos + clr_len, T (0));
		const int      clr_rem_len = add_len - clr_len;
		if (clr_rem_len > 0)
		{
			std::fill (it, it + clr_rem_len, T (0));
		}
	}
}



template <typename T>
void	VelvetConv <T>::update_density () noexcept
{
	assert (! _grain.empty ());

	// Per-sample probability to generate a grain
	const float    proba = _density / float (_grain.size ());

	// Scales to the PRNG value range. We use double as intermediate type
	// to keep the whole 32 bits, but actually this is not really necessary.
	_dens_thr  = uint32_t (std::min (
		proba * double (fstb::TWOP32) + 0.5,
		double (fstb::TWOP32 - 1)
	));

	// We use bit 0 for the sign, odd values would favour one sign value
	// over the opposite.
	_dens_thr &= ~1;
}



template <typename T>
void	VelvetConv <T>::generate_grain_conditional (int buf_pos) noexcept
{
	assert (buf_pos >= 0);
	assert (buf_pos < _buf_len);

	const auto     dice { roll_dices () };
	if (dice._grain_flag)
	{
		const int      grain_len = int (_grain.size ());
		int            grain_pos = 0;

		dsp::dly::RingBufVectorizer rbv (_buf_len);
		for (rbv.start (grain_len, buf_pos); rbv.end (); rbv.next ())
		{
			const int      seg_len = rbv.get_seg_len ();
			const int      seg_pos = rbv.get_curs_pos (0);

			const T * fstb_RESTRICT src_ptr = &_grain [grain_pos];
			T * fstb_RESTRICT       dst_ptr = &_buf [seg_pos];

			if (dice._sign_flag)
			{
				overlap (dst_ptr, src_ptr, seg_len, [] (T &a, T b) { a -= b; });
			}
			else
			{
				overlap (dst_ptr, src_ptr, seg_len, [] (T &a, T b) { a += b; });
			}

			grain_pos += seg_len;
		}
	}
}



// Result: { grain flag, sign }
template <typename T>
typename VelvetConv <T>::DiceRes	VelvetConv <T>::roll_dices () noexcept
{
	const uint32_t val = fstb::Hash::hash (_hash_cnt);
	++ _hash_cnt;

	const bool     grain_flag = (val <= _dens_thr);
	const bool     sign_flag  = ((val & 1) != 0);

	return { grain_flag, sign_flag };
}



template <typename T>
int	VelvetConv <T>::compute_raw_ring_buf_len (int grain_len) noexcept
{
	assert (grain_len > 0);

	return 1 << fstb::get_next_pow_2 (grain_len + (_max_block_size - 1));
}



template <typename T>
template <typename OP>
void	VelvetConv <T>::overlap (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr, int nbr_spl, OP op) noexcept
{
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		op (dst_ptr [pos], src_ptr [pos]);
	}
}



}  // namespace grn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_grn_VelvetConv_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
