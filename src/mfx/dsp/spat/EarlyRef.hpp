/*****************************************************************************

        EarlyRef.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_EarlyRef_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_EarlyRef_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/fnc.h"
#include "fstb/Hash.h"
#include "mfx/dsp/mix/Simd.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace spat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	EarlyRef <T>::reset (double sample_freq, double max_predelay_time, double max_duration)
{
	assert (sample_freq > 0);
	assert (max_predelay_time >= 0);
	assert (max_duration > 0);

	_sample_freq       = sample_freq;
	_max_predelay_time = max_predelay_time;
	_max_duration      = max_duration;

	const int      len_spl =
		  fstb::ceil_int (_max_predelay_time * _sample_freq)
		+ fstb::ceil_int (_max_duration      * _sample_freq);
	_delay.setup (len_spl, _max_blk_size);
}



// duration in seconds
// lvl_end is the relative level of the last tap (first tap is 1)
template <typename T>
void	EarlyRef <T>::generate_taps (uint32_t seed, int nbr_taps, float duration, float lvl_end, float gain)
{
	assert (_sample_freq > 0);
	assert (nbr_taps >= 0);
	assert (nbr_taps <= _max_nbr_taps);
	assert (duration > 0);
	assert (lvl_end > 0);
	assert (lvl_end <= 1);

	_nbr_taps = nbr_taps;

	// Random generator
	constexpr int  rnd_inc = 4321;
	uint32_t       rnd_idx = seed;
	auto           rnd     = [&rnd_idx, rnd_inc] ()
	{
		const uint32_t val = fstb::Hash::hash (rnd_idx);
		rnd_idx += rnd_inc;
		return double (val) * fstb::TWOPM32;
	};

	// Generates randomly spaced taps, on an arbitrary time scale
	std::array <double, _max_nbr_taps> time_arr;
	double         t_tot = 0;
	for (int k = 0; k < nbr_taps; ++k)
	{
		const double   t = rnd () + 0.1;
		time_arr [k] = t;
		t_tot += t;
	}

	// Rescale the times
	const double   time_scale = duration * _sample_freq / t_tot;
	double         t_cur      = 0;
	for (int k = 0; k < nbr_taps; ++k)
	{
		const double   t_scaled = time_scale * time_arr [k];
		t_cur += t_scaled;
		_tap_arr [k]._pos = fstb::round_int (t_cur);
	}

	// Now resets the random generator to make sure the gain series is
	// independant of the number of taps. So each tap keeps the same gain.
	rnd_idx = seed + _max_nbr_taps * rnd_inc;

	// Sets the gains
	constexpr double  epsilon = 1e-9;
	const double   mul        = log (lvl_end) / (t_cur + epsilon);
	const double   g_scale    =
		gain / (1 + sqrt (double (nbr_taps) / double (_max_nbr_taps)));
	for (int k = 0; k < nbr_taps; ++k)
	{
		const double   decay = exp (double (_tap_arr [k]._pos) * mul);
		const double   g     = rnd () * 2 - 1;
		_tap_arr [k]._gain = T (g * g_scale * decay);
	}
}



template <typename T>
void	EarlyRef <T>::set_predelay (float delay)
{
	assert (_sample_freq > 0);
	assert (delay >= 0);
	assert (delay <= _max_predelay_time);

	_predelay_spl = fstb::round_int (delay * _sample_freq);
	_delay.set_delay (_predelay_spl);
}



// .first: pre-delayed source
// .second: early reflections
template <typename T>
std::pair <T, T>	EarlyRef <T>::process_sample (T x)
{
	assert (_sample_freq > 0);

	const T        d = _delay.process_sample (x);
	T              e = T (0);

	const int      offset = _predelay_spl + 1;
	for (const auto &tap : _tap_arr)
	{
		e += _delay.read_at (tap._pos + offset) * tap._gain;
	}

	return std::make_pair (d, e);
}



template <typename T>
void	EarlyRef <T>::process_block (T dly_ptr [], T erf_ptr [], const T src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (dly_ptr != nullptr);
	assert (erf_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	alignas (16) std::array <T, _max_blk_size>   buf;

	using Mix = dsp::mix::Simd <
		fstb::DataAlign <false>,
		fstb::DataAlign <true>
	>;

	int            pos = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, _max_blk_size);

		_delay.process_block (dly_ptr, src_ptr, work_len);

		const int      offset     = _predelay_spl + work_len;
		bool           empty_flag = true;
		for (const auto &tap : _tap_arr)
		{
			const float    g = tap._gain;
			_delay.read_block_at (buf.data (), tap._pos + offset, work_len);
			if (empty_flag)
			{
				Mix::copy_1_1_v (erf_ptr, buf.data (), work_len, g);
				empty_flag = false;
			}
			else
			{
				Mix::mix_1_1_v (erf_ptr, buf.data (), work_len, g);
			}
		}
		if (empty_flag)
		{
			Mix::clear (erf_ptr, work_len);
		}

		pos     += work_len;
		dly_ptr += work_len;
		erf_ptr += work_len;
		src_ptr += work_len;
	}
	while (pos < nbr_spl);
}



template <typename T>
void	EarlyRef <T>::clear_buffers ()
{
	_delay.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_EarlyRef_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
