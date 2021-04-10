/*****************************************************************************

        SplitMultibandBustad.h
        Author: Laurent de Soras, 2021

Filterbank with perfect unit-magnitude reconstruction.
The object should be used for merging bands once they are processed, too.

The splitter works with pairs of low-pass and high-pass filters that can be
summed with these conditions (ex: Linkwitz-Riley). To keep everything in
phase, all bands are filtered at all boundary frequencies, by the low- or
high-pass filters depending on the band position, according to the flowgraph
described by Christofer Bustad on the Music-DSP mailing list:
http://music.columbia.edu/pipermail/music-dsp/2004-March/059520.html

In -+-->HP0----+--->HP1----+--> ... ---+--->HPn-3----+--->HPn-2--.
    |          |           |           |             |           |
    v          v           v           v             v           |
   LP0        LP1         LP2        LPn-3         LPn-2         |
    |          |           |           |             |           |
    v          v           v           v             v           v
    P0         P1          P2         Pn-3          Pn-2        Pn-1
    |          |           |           |             |           |
    |          v           v           v             v           v
    `-->LP1-->(+)-->LP2-->(+)-> ... ->(+)-->LPn-2-->(+)-------->(+)-> Out

HPi/LPi are the filters at the i-th frequency between band i and i + 1.
Pi is the processing for the i-th band (not handled by this class).

Importante note: because the bands are filtered multiple times, the actual
filter slopes become steeper accross the bands:
- During split, the low part of the spectrum is extra-filtered
- During merge, high part of the spectrum is extra-filtered too.
So harmonics generated above the band boundaries may be filtered out during
reconstruction.

Template parameters:

- O: order of the low-pass filter, >= 2

Basic usage:

1. Setup
	- Possibly call reserve() first to preallocate memory
	- Call set_sample_freq() to set the sampling rate
	- Call set_nbr_bands() to set the number of bands
	- Call set_splitter_coef() to configure the crossover filters
2. Processing
	- Call process_sample_split() or process_block_split() to split the input
		signal into multiple bands.
	- Call process_sample_merge() or process_block_merge() to merge together
		the band that were previously split.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitMultibandBustad_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandBustad_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/OnePole.h"

#include <array>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int O>
class SplitMultibandBustad
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Number of 2- and 1-pole sections
	static constexpr int _nbr_2p = O / 2;
	static constexpr int _nbr_1p = O & 1;

	static constexpr int _order  = O;

	void           set_sample_freq (double sample_freq);
	void           reserve (int nbr_bands);
	void           set_nbr_bands (int nbr_bands, float * const band_ptr_arr []);
	inline int     get_nbr_bands () const noexcept;

	void           set_splitter_coef (int split_idx, float freq, const float lpf_coef_arr [O], const float hpf_coef_arr [O]) noexcept;
	float          compute_group_delay (float freq) const noexcept;
	void           set_band_ptr_one (int band_idx, float *band_ptr) noexcept;
	void           set_band_ptr (float * const band_ptr_arr []) noexcept;
	inline void    offset_band_ptr (ptrdiff_t offset) noexcept;

	void           clear_buffers () noexcept;
	inline void    process_sample_split (float x) noexcept;
	inline float   process_sample_merge () noexcept;
	void           process_block_split (const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_merge (float dst_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Internal buffer size, samples, > 0
	static constexpr int _max_buf_size = 64;

	typedef std::array <float, _max_buf_size> Buffer;

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

	// Crossover filter realisations
	typedef std::array <Biquad , _nbr_2p> F2pArray;
	typedef std::array <OnePole, _nbr_1p> F1pArray;

	class Spec
	{
	public:
		void           fill_with (const float coef_arr []) noexcept;
		Eq2pArray      _eq_2p;
		Eq1pArray      _eq_1p;
	};

	class Split
	{
	public:
		float          _freq = 0;
		Spec           _lpf;
		Spec           _hpf;
	};
	typedef std::vector <Split> SplitArray;

	class Filter
	{
	public:
		void           clear_buffers () noexcept;
		fstb_FORCEINLINE float
		               process_sample (float x) noexcept;
		void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
		F2pArray       _f2p_arr;
		F1pArray       _f1p_arr;
	};

	class SplitFilter
	{
	public:
		Filter         _lpf;
		Filter         _hpf;
		Filter         _fix; // LPF
	};
	typedef std::vector <SplitFilter> FilterArray;

	class Band
	{
	public:
		float *        _spl_ptr = nullptr;
	};
	typedef std::vector <Band> BandArray;

	void           update_all () noexcept;
	void           update_xover_coefs (int split_idx) noexcept;

	template <typename EQS, typename FILT>
	static void    set_filter_coefs (const EQS &eq_arr, FILT &filt_arr, FILT *filt2_arr_ptr, void (*blt) (float*, float*, const float*, const float*, float), float k);

	// Sample frequency, Hz. > 0. 0 = not set
	float          _sample_freq = 0;

	// Sampling period, 1 / Fs. > 0. 0 = not set
	float          _inv_fs      = 0;

	SplitArray     _split_arr;
	FilterArray    _filter_arr;
	BandArray      _band_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if 0
	               SplitMultibandBustad ()                               = delete;
	               SplitMultibandBustad (const SplitMultibandBustad &other) = delete;
	               SplitMultibandBustad (SplitMultibandBustad &&other)      = delete;
	SplitMultibandBustad &
	               operator = (const SplitMultibandBustad &other)        = delete;
	SplitMultibandBustad &
	               operator = (SplitMultibandBustad &&other)             = delete;
#endif
	bool           operator == (const SplitMultibandBustad &other) const = delete;
	bool           operator != (const SplitMultibandBustad &other) const = delete;

}; // class SplitMultibandBustad



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitMultibandBustad.hpp"



#endif   // mfx_dsp_iir_SplitMultibandBustad_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
