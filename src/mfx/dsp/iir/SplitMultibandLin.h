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

Band filters can be based on almost any kind of standard low-pass filter
designed in the s plane.

Template parameters:

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

#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/SplitMultibandLinBase.h"

#include <cstddef>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int O>
class SplitMultibandLin
:	public SplitMultibandLinBase <O>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef SplitMultibandLin <O> ThisType;
	typedef SplitMultibandLinBase <O> Inherited;

	static constexpr int _order  = O;

	// Number of 2- and 1-pole sections
	static constexpr int _nbr_2p = Inherited::_nbr_2p;
	static constexpr int _nbr_1p = Inherited::_nbr_1p;

	               SplitMultibandLin ();
	               SplitMultibandLin (const SplitMultibandLin &other) = default;
	               SplitMultibandLin (SplitMultibandLin &&other)      = default;
	SplitMultibandLin &
	               operator = (const SplitMultibandLin &other)        = default;
	SplitMultibandLin &
	               operator = (SplitMultibandLin &&other)             = default;

	void           set_sample_freq (double sample_freq);
	void           reserve (int nbr_bands);
	void           set_nbr_bands (int nbr_bands, float * const band_ptr_arr []);
	inline int     get_nbr_bands () const noexcept;

	void           set_splitter_coef (int split_idx, float freq, const float coef_arr [O], float dly_ofs) noexcept;
	void           set_band_ptr_one (int band_idx, float *out_ptr) noexcept;
	void           set_band_ptr (float * const band_ptr_arr []) noexcept;
	inline void    offset_band_ptr (ptrdiff_t offset) noexcept;

	void           clear_buffers () noexcept;
	inline void    process_sample (float x) noexcept;
	void           process_block (const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	using typename Inherited::Eq2p;
	using typename Inherited::Eq1p;
	using typename Inherited::Eq2pArray;
	using typename Inherited::Eq1pArray;
	using typename Inherited::Splitter;
	using typename Inherited::SplitterArray;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Crossover filter realisations
	typedef std::array <Biquad , _nbr_2p> F2pArray;
	typedef std::array <OnePole, _nbr_1p> F1pArray;

	// Internal buffer size, samples, > 0
	static constexpr int _max_buf_size = 64;

	typedef std::array <float, _max_buf_size> Buffer;

	class Band
	{
	public:
		// Output buffer, provided by class user. 0 = not set
		float *        _out_ptr = nullptr;

		Buffer         _buf_tmp;
	};
	typedef std::vector <Band> BandArray;

	class Filter
	{
	public:
		F2pArray       _f2p_arr;
		F1pArray       _f1p_arr;
	};
	typedef std::vector <Filter> FilterArray;

	void           update_all () noexcept;
	void           update_xover_coefs (int split_idx) noexcept;

	// Band data. Empty = not initialised yet.
	BandArray      _band_arr;

	// Crossover Filters
	FilterArray    _filter_arr;



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
