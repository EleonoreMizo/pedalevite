/*****************************************************************************

        SplitMultibandLinSimd.h
        Author: Laurent de Soras, 2021

This object must be aligned on 16-byte boundaries!

See SplitMultibandLin for use.

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
#include "mfx/dsp/iir/Biquad4Simd.h"
#include "mfx/dsp/iir/OnePole4Simd.h"
#include "mfx/dsp/iir/SplitMultibandLinBase.h"

#include <cstddef>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int O>
class SplitMultibandLinSimd
:	public SplitMultibandLinBase <O>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef SplitMultibandLinSimd <O> ThisType;
	typedef SplitMultibandLinBase <O> Inherited;

	static constexpr int _order  = O;

	// Number of 2- and 1-pole sections
	static constexpr int _nbr_2p = Inherited::_nbr_2p;
	static constexpr int _nbr_1p = Inherited::_nbr_1p;

	               SplitMultibandLinSimd ();
	               SplitMultibandLinSimd (SplitMultibandLinSimd &&other) = default;
	SplitMultibandLinSimd &
	               operator = (SplitMultibandLinSimd &&other) = default;

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

	using TS = fstb::ToolsSimd;

	// Byte alignment for SIMD data
	static constexpr int _simd_alig_l2 = 4;
	static constexpr int _simd_alig    = 1 << _simd_alig_l2;

	// Number of channels per SIMD vector
	static constexpr int _simd_w_l2 = 2;
	static constexpr int _simd_w    = 1 << _simd_w_l2;

	// Crossover filter realisations
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

	// Internal buffer size, samples, > 0, multiple of 4 for 16-byte alignment
	static constexpr int _max_buf_size = 64;
	static_assert (
		((_max_buf_size & (_simd_alig / _simd_w - 1)) == 0),
		"_max_buf_size must be a multiple of 4"
	);

	typedef std::array <float, _max_buf_size> Buffer;
	typedef std::array <fstb::Vf32, _max_buf_size> BufferSimd;

	class Band
	{
	public:
		// Linear temporary buffer, for the delay output and input of the final
		// subtraction.
		alignas (_simd_alig) Buffer
		               _buf_tmp;

		// Output buffer, provided by class user. nullptr = not set
		float *        _out_ptr = nullptr;
	};
	typedef std::vector <Band, fstb::AllocAlign <Band, _simd_alig> > BandArray;

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

	void           update_all () noexcept;
	void           update_xover_coefs (int split_idx) noexcept;

	static inline SplitCoord
	               conv_split_to_pack (int split_idx) noexcept;

	// Band data. Empty = not initialised yet.
	BandArray      _band_arr;

	// Crossover Filters, packed by 4
	SplitterPackArray
	               _flt_pack_arr;

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
