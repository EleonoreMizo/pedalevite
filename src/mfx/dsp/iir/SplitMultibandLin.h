/*****************************************************************************

        SplitMultibandLin.h
        Author: Laurent de Soras, 2021

Filterbank with perfect reconstruction (unit-magnitude and linear-phase).
Uses LPF as crossover filters, the high part being a time-delayed subtraction
between the input and the low part.

For the high-pass part of the crossover filter, don't expect slopes steeper
than 18 dB/octave. Creating a notch by offsetting the delay can help to get
something closer to 24 dB/octave below the cutoff frequency, but the
asymptotic slope will drop to 6 dB/octave.

Band filters can be based on any kind of standard low-pass filter designed in
the s plane.

Template parameters:

- T: data type, floating point

- O: order of the low-pass filter, >= 2

Basic usage:

1. Setup
	- Possibly call reserve() first to preallocate memory
	- Call set_sample_freq() to set the sampling rate
	- Call set_nbr_bands() to set the number of bands
	- Call set_splitter_coef() to configure the crossover filters
2. Processing
	- Call process_sample() or process_block() to split the input signal

Once split, the bands can be summed directly for perfect, delayed
reconstruction.

Reference:
Stanley P. Lipshitz, John Vanderkooy,
A Family of Linear-Phase Crossover Networks of High Slope Derived by Time Delay,
Journal of the Audio Engineering Society, Vol. 31, No. 1/2, 1983-01

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitMultibandLin_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandLin_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/OnePole.h"

#include <array>
#include <vector>

#include <cstddef>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <typename T, int O>
class SplitMultibandLin
{
	static_assert (O >= 2, "Low-pass filter order must be >= 2.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;
	typedef SplitMultibandLin <T, O> ThisType;

	// Number of 2- and 1-pole sections
	static constexpr int _nbr_2p = O / 2;
	static constexpr int _nbr_1p = O & 1;

	               SplitMultibandLin ();
	               SplitMultibandLin (const SplitMultibandLin &other) = default;
	               SplitMultibandLin (SplitMultibandLin &&other)      = default;
	SplitMultibandLin &
	               operator = (const SplitMultibandLin &other)        = default;
	SplitMultibandLin &
	               operator = (SplitMultibandLin &&other)             = default;

	void           set_sample_freq (double sample_freq);
	void           reserve (int nbr_bands);
	void           set_nbr_bands (int nbr_bands, T * const band_ptr_arr []);
	inline int     get_nbr_bands () const noexcept;

	void           set_splitter_coef (int split_idx, T freq, const T coef_arr [O], T dly_ofs) noexcept;
	inline T       get_actual_xover_freq (int split_idx) const noexcept;
	int            get_global_delay () const noexcept;
	void           set_band_ptr_one (int band_idx, T *out_ptr) noexcept;
	void           set_band_ptr (T * const band_ptr_arr []) noexcept;
	inline void    offset_band_ptr (ptrdiff_t offset) noexcept;

	void           clear_buffers () noexcept;
	inline void    process_sample (T x) noexcept;
	void           process_block (const T src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <int N>
	class FilterEq
	{
	public:
		std::array <T, N> _b { 1 }; // Numerator
		std::array <T, N> _a { 1 }; // Denominator
		static constexpr int _nbr_coef = N * 2;
		int            fill_with (const T coef_ptr [_nbr_coef]) noexcept;
	};
	typedef FilterEq <3> Eq2p;
	typedef FilterEq <2> Eq1p;

	// Filters and their normalized s-plane spec
	class Filter2p
	{
	public:
		Eq2p           _eq_s;
		Biquad         _flt;
	};
	class Filter1p
	{
	public:
		Eq1p           _eq_s;
		OnePole        _flt;
	};
	typedef std::array <Filter2p, _nbr_2p> F2pArray;
	typedef std::array <Filter1p, _nbr_1p> F1pArray;

	// Internal buffer size, samples, > 0
	static constexpr int _max_buf_size = 64;

	typedef std::array <T, _max_buf_size> Buffer;

	class Band
	{
	public:
		// Output buffer, provided by class user. 0 = not set
		T *            _out_ptr = nullptr;

		Buffer         _buf_tmp;
	};
	typedef std::vector <Band> BandArray;

	class Splitter
	{
	public:
		// Crossover target frequency, Hz. > 0
		T              _freq_tgt = 0;

		// Delay offset, relative to the main delay value. 0 = neutral
		T              _dly_ofs  = 0;

		// Filters
		F2pArray       _f2p_arr;
		F1pArray       _f1p_arr;

		// Warped version of the target frequency, for the bilinear transform
		// This value is relative to Fs/pi
		T              _freq_warp = 0;

		// Ideal compensation delay for this single crossover filter, samples
		T              _dly_comp = 0;

		// 1st order coefficient for the developed denominator of the whole
		// filter. Called b1 in eq. 21
		T              _b1       = 0;

		// Rounded value for _dly_comp
		int            _dly_int  = 0;

		// Final band delay, samples, >= 0
		int            _dly_b    = 0;

		// Actual crossover frequency (bilinear-warped), relative to Fs/pi. > 0
		T              _freq_act = 0;
	};
	typedef std::vector <Splitter> SplitterArray;

	// Maximum delay time, s
	static const double  _max_dly_time;

	void           update_all ();
	void           update_single_splitter (int split_idx);
	void           update_post ();

	void           bilinear_2p (Eq2p &eq_z, const Eq2p &eq_s, T f0_pi_fs) noexcept;
	void           bilinear_1p (Eq1p &eq_z, const Eq1p &eq_s, T f0_pi_fs) noexcept;

	// Sampling frequency, Hz, > 0. 0 = not set
	T              _sample_freq = 0;

	// Band data. Empty = not initialised yet.
	BandArray      _band_arr;

	// Crossover filters. Empty = not initialized yet.
	SplitterArray  _split_arr;

	// Multi-tap compensation delay. Used for all bands excepted the highest-
	// frequency one.
	dly::DelaySimple <T>
	               _delay;

	// Maximum delay. Computed after all crossover splitters have been
	// set up.
	int            _max_delay = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SplitMultibandLin &other) const = delete;
	bool           operator != (const SplitMultibandLin &other) const = delete;

}; // class SplitMultibandLin



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitMultibandLin.hpp"



#endif   // mfx_dsp_iir_SplitMultibandLin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
