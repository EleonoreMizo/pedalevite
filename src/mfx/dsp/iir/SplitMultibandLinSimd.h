/*****************************************************************************

        SplitMultibandLinSimd.h
        Author: Laurent de Soras, 2021

This object must be aligned on 16-byte boundaries!

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitMultibandLinSimd_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandLinSimd_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/iir/Biquad4Simd.h"
#include "mfx/dsp/iir/OnePole4Simd.h"

#include <array>
#include <vector>

#include <cstddef>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int O>
class SplitMultibandLinSimd
{
	static_assert (O >= 2, "Low-pass filter order must be >= 2.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef SplitMultibandLinSimd <O> ThisType;

	// Number of 2- and 1-pole sections
	static constexpr int _nbr_2p = O / 2;
	static constexpr int _nbr_1p = O & 1;

	               SplitMultibandLinSimd ();
	               SplitMultibandLinSimd (SplitMultibandLinSimd &&other) = default;
	SplitMultibandLinSimd &
	               operator = (SplitMultibandLinSimd &&other)      = default;

	void           set_sample_freq (double sample_freq);
	void           reserve (int nbr_bands);
	void           set_nbr_bands (int nbr_bands, float * const band_ptr_arr []);
	inline int     get_nbr_bands () const noexcept;

	void           set_splitter_coef (int split_idx, float freq, const float coef_arr [O], float dly_ofs) noexcept;
	inline float   get_actual_xover_freq (int split_idx) const noexcept;
	int            get_global_delay () const noexcept;
	void           set_band_ptr_one (int band_idx, float *out_ptr) noexcept;
	void           set_band_ptr (float * const band_ptr_arr []) noexcept;
	inline void    offset_band_ptr (ptrdiff_t offset) noexcept;

	void           clear_buffers () noexcept;
	inline void    process_sample (float x) noexcept;
	void           process_block (const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	using TS = fstb::ToolsSimd;

	// Byte alignment for SIMD data
	static constexpr int _simd_alig_l2 = 4;
	static constexpr int _simd_alig    = 1 << _simd_alig_l2;

	// Number of channels per SIMD vector
	static constexpr int _simd_w_l2 = 2;
	static constexpr int _simd_w    = 1 << _simd_w_l2;

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

	// Filters and their normalized s-plane spec
	typedef Biquad4Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> Filter2pPack;
	typedef OnePole4Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> Filter1pPack;

	typedef std::array <Filter2pPack, _nbr_2p> F2pPackArray;
	typedef std::array <Filter1pPack, _nbr_1p> F1pPackArray;

	typedef std::array <Eq2p, _nbr_2p> Eq2pArray;
	typedef std::array <Eq1p, _nbr_1p> Eq1pArray;

	// Internal buffer size, samples, > 0, multiple of 4 for 16-byte alignment
	static constexpr int _max_buf_size = 64;
	static_assert (
		((_max_buf_size & (_simd_alig / _simd_w - 1)) == 0),
		"_max_buf_size must be a multiple of 4"
	);

	typedef std::array <float, _max_buf_size> Buffer;
	typedef std::array <TS::VectF32, _max_buf_size> BufferSimd;

	class Band
	{
	public:
		// Linear temporary buffer, for the delay output and input of the final
		// subtraction.
		alignas (_simd_alig) Buffer
		               _buf_tmp;

		// Output buffer, provided by class user. 0 = not set
		float *        _out_ptr = 0;
	};
	typedef std::vector <Band, fstb::AllocAlign <Band, _simd_alig> > BandArray;

	class Splitter
	{
	public:
		// Crossover target frequency, Hz. > 0
		float          _freq_tgt = 0;

		// Filter s-plane equations (normalised cutoff frequencies)
		Eq2pArray      _eq_2p;
		Eq1pArray      _eq_1p;

		// Delay offset, relative to the main delay value. 0 = neutral
		float          _dly_ofs  = 0;

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

	class SplitterPack
	{
	public:
		F2pPackArray   _f2p_arr;
		F1pPackArray   _f1p_arr;
		std::array <const float *, _simd_w>
		               _buf_s_ptr_arr {};
		std::array <float *, _simd_w>
		               _buf_d_ptr_arr {};
	};
	typedef std::vector <
		SplitterPack, fstb::AllocAlign <SplitterPack, _simd_alig>
	> SplitterPackArray;

	class SplitCoord
	{
	public:
		int            _pack_idx = 0;
		int            _elt_idx  = 0;
	};

	// Maximum delay time, s
	static const double  _max_dly_time;

	void           update_all ();
	void           update_single_splitter (int split_idx);
	void           update_post ();

	void           bilinear_2p (Eq2p &eq_z, const Eq2p &eq_s, float f0_pi_fs) noexcept;
	void           bilinear_1p (Eq1p &eq_z, const Eq1p &eq_s, float f0_pi_fs) noexcept;

	static inline SplitCoord
	               conv_split_to_pack (int split_idx) noexcept;

	// Sampling frequency, Hz, > 0. 0 = not set
	float          _sample_freq = 0;

	// Band data. Empty = not initialised yet.
	BandArray      _band_arr;

	// Crossover filter specs. Empty = not initialized yet.
	SplitterArray  _split_arr;

	// Crossover Filters, packed by 4
	SplitterPackArray
	               _flt_pack_arr;

	// Multi-tap compensation delay. Used for all bands excepted the highest-
	// frequency one.
	dly::DelaySimple <float>
	               _delay;

	// Maximum delay. Computed after all crossover splitters have been
	// set up.
	int            _max_delay = 0;

	// Temporary buffer of inverleaved SIMD data. For filter in/out
	alignas (_simd_alig) BufferSimd
	               _buf_pack;

	// Trash buffer, to collect the output of inexisting bands.
	// Could be static
	alignas (_simd_alig) Buffer
		            _buf_trash;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SplitMultibandLinSimd (const SplitMultibandLinSimd &other) = delete;
	SplitMultibandLinSimd &
	               operator = (const SplitMultibandLinSimd &other)        = delete;
	bool           operator == (const SplitMultibandLinSimd &other) const = delete;
	bool           operator != (const SplitMultibandLinSimd &other) const = delete;

}; // class SplitMultibandLinSimd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitMultibandLinSimd.hpp"



#endif   // mfx_dsp_iir_SplitMultibandLinSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
