/*****************************************************************************

        ApfLine.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_ApfLine_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_ApfLine_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <climits>

#include <algorithm>



namespace mfx
{
namespace dsp
{
namespace spat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_delay_max (int len)
{
	assert (DF);
	assert (len > 0);

	_delay.setup (len, _max_blk_size);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_apd_max_delay (int idx, int len)
{
	assert (idx >= 0);
	assert (idx < _nbr_apd);

	_apd_arr [idx].set_max_len (len + _max_blk_size);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_delay (int len) noexcept
{
	assert (DF);
	assert (len >= 0);

	_delay.set_delay (len);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_apd_delay_flt (int idx, float len_spl) noexcept
{
	assert (idx >= 0);
	assert (idx < _nbr_apd);

	_apd_arr [idx].set_delay_flt (len_spl);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_apd_coef (int idx, float coef) noexcept
{
	assert (idx >= 0);
	assert (idx < _nbr_apd);
	assert (coef >= -1);
	assert (coef <= +1);

	_apd_arr [idx].set_coef (coef);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_filter_coefs (float g0, float g1, float g2) noexcept
{
	assert (FF);

	_filter.set_coefs (g0, g1, g2);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::set_filter_mix (float v0m, float v1m, float v2m) noexcept
{
	assert (FF);

	_filter.set_mix (v0m, v1m, v2m);
}



template <int NAPD, bool DF, bool FF>
float	ApfLine <NAPD, DF, FF>::process_sample (float x) noexcept
{
	for (auto &apd : _apd_arr)
	{
		x = apd.process_sample (x);
	}
	if (DF)
	{
		x = _delay.process_sample (x);
	}
	if (FF)
	{
		x = _filter.process_sample (x);
	}

	return x;
}



// Reading the line is meant to occur after process_sample().
template <int NAPD, bool DF, bool FF>
float	ApfLine <NAPD, DF, FF>::read_apd (int idx, int delay) const noexcept
{
	assert (idx >= 0);
	assert (idx < _nbr_apd);

	return _apd_arr [idx].read_at (delay + 1);
}



template <int NAPD, bool DF, bool FF>
float	ApfLine <NAPD, DF, FF>::read_delay (int delay) const noexcept
{
	assert (DF);

	return _delay.read_at (delay + 1);
}



template <int NAPD, bool DF, bool FF>
int	ApfLine <NAPD, DF, FF>::compute_max_block_len () const noexcept
{
	int            max_len = (DF) ? _max_blk_size : INT_MAX;
	for (const auto &apd : _apd_arr)
	{
		const int      max_len_apd = apd.get_max_block_len ();
		max_len = std::min (max_len, max_len_apd);
	}

	return max_len;
}



// nbr_spl should be bounded by the all-pass delay requirements
// -> compute_max_block_len ()
template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= compute_max_block_len ());

	for (auto &apd : _apd_arr)
	{
		apd.process_block (dst_ptr, src_ptr, nbr_spl);
		src_ptr = dst_ptr;
	}
	if (DF)
	{
		_delay.process_block (dst_ptr, dst_ptr, nbr_spl);
	}
	if (FF)
	{
		_filter.process_block (dst_ptr, dst_ptr, nbr_spl);
	}
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::process_block_var_dly (float dst_ptr [], const float src_ptr [], const int32_t * const * dly_frc_arr_ptr, int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (dly_frc_arr_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= compute_max_block_len ());

	for (int apd_idx = 0; apd_idx < int (_apd_arr.size ()); ++apd_idx)
	{
		auto &         apd         = _apd_arr [apd_idx];
		const int32_t* dly_frc_ptr = dly_frc_arr_ptr [apd_idx];
		assert (dly_frc_ptr != nullptr);
		apd.process_block_var_dly (dst_ptr, src_ptr, dly_frc_ptr, nbr_spl);
		src_ptr = dst_ptr;
	}
	if (DF)
	{
		_delay.process_block (dst_ptr, dst_ptr, nbr_spl);
	}
	if (FF)
	{
		_filter.process_block (dst_ptr, dst_ptr, nbr_spl);
	}
}



// Reading the line is meant to occur after process_block()
// For consistent results, nbr_spl should be the same in both calls
template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::read_apd_block (float dst_ptr [], int idx, int delay, int nbr_spl) const noexcept
{
	assert (dst_ptr != nullptr);
	assert (idx >= 0);
	assert (idx < _nbr_apd);
	assert (delay > 0);
	assert (nbr_spl > 0);

	_apd_arr [idx].read_block_at (dst_ptr, delay + nbr_spl, nbr_spl);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::read_delay_block (float dst_ptr [], int delay, int nbr_spl) const noexcept
{
	assert (DF);
	assert (dst_ptr != nullptr);
	assert (delay > 0);
	assert (nbr_spl > 0);

	_delay.read_block_at (dst_ptr, delay + nbr_spl, nbr_spl);
}



template <int NAPD, bool DF, bool FF>
void	ApfLine <NAPD, DF, FF>::clear_buffers () noexcept
{
	for (auto &apd : _apd_arr)
	{
		apd.clear_buffers ();
	}
	if (DF)
	{
		_delay.clear_buffers ();
	}
	if (FF)
	{
		_filter.clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_ApfLine_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
