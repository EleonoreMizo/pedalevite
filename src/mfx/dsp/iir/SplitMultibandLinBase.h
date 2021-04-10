/*****************************************************************************

        SplitMultibandLinBase.h
        Author: Laurent de Soras, 2021

Base class for the SplitMulitband* implementations.
Public member functions are available in the derived classes.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitMultibandLinBase_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandLinBase_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/DelaySimple.h"

#include <array>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int O>
class SplitMultibandLinBase
{
	static_assert (O >= 2, "Low-pass filter order must be >= 2.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Number of 2- and 1-pole sections
	static constexpr int _nbr_2p = O / 2;
	static constexpr int _nbr_1p = O & 1;

	inline float   get_actual_xover_freq (int split_idx) const noexcept;
	inline int     get_global_delay () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	template <int N>
	class FilterEq
	{
	public:
		std::array <float, N> _b { 1 }; // Numerator
		std::array <float, N> _a { 1 }; // Denominator
		static constexpr int _nbr_coef = N * 2;
		int            fill_with (const float coef_ptr [_nbr_coef]) noexcept;
	};
	typedef FilterEq <3> Eq2p;
	typedef FilterEq <2> Eq1p;

	// Normalized s-plane spec
	typedef std::array <Eq2p, _nbr_2p> Eq2pArray;
	typedef std::array <Eq1p, _nbr_1p> Eq1pArray;

	// Maximum delay time, s
	static const double  _max_dly_time;

	class Splitter
	{
	public:
		// Crossover target frequency, Hz. > 0
		float          _freq_tgt = 0;

		// Delay offset, relative to the main delay value. 0 = neutral
		float          _dly_ofs  = 0;

		// Filter s-plane equations (normalised cutoff frequencies)
		Eq2pArray      _eq_2p;
		Eq1pArray      _eq_1p;

		// Warped version of the target frequency, for the bilinear transform
		// This value is relative to Fs/pi
		float          _freq_warp = 0;

		// Ideal compensation delay for this single crossover filter, samples
		float          _dly_comp = 0;

		// 1st order coefficient for the developed denominator of the whole
		// filter. Called b1 in eq. 21
		float          _b1       = 0;

		// Rounded value for _dly_comp
		int            _dly_int  = 0;

		// Final band delay, samples, >= 0
		int            _dly_b    = 0;

		// Actual crossover frequency (bilinear-warped), relative to Fs/pi. > 0
		float          _freq_act = 0;
	};
	typedef std::vector <Splitter> SplitterArray;

	bool           set_sample_freq_internal (double sample_freq, int max_buf_size);
	void           set_splitter_coef_internal (int split_idx, float freq, const float coef_arr [O], float dly_ofs) noexcept;
	bool           update_single_splitter (int split_idx) noexcept;
	void           update_post () noexcept;

	void           bilinear_2p (Eq2p &eq_z, const Eq2p &eq_s, double f0_pi_fs) noexcept;
	void           bilinear_1p (Eq1p &eq_z, const Eq1p &eq_s, double f0_pi_fs) noexcept;

	// Sampling frequency, Hz, > 0. 0 = not set
	float          _sample_freq = 0;

	// Crossover filter specs. Empty = not initialized yet.
	SplitterArray  _split_arr;

	// Multi-tap compensation delay. Used for all bands excepted the highest-
	// frequency one.
	dly::DelaySimple <float>
	               _delay;

	// Maximum delay. Computed after all crossover splitters have been
	// set up.
	int            _max_delay = 0;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SplitMultibandLinBase &other) const = delete;
	bool           operator != (const SplitMultibandLinBase &other) const = delete;

}; // class SplitMultibandLinBase



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitMultibandLinBase.hpp"



#endif   // mfx_dsp_iir_SplitMultibandLinBase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
