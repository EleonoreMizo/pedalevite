/*****************************************************************************

        Upsampler.h
        Author: Laurent de Soras, 2022

Upsamples a signal before true peak detection, as specified in BS.1770

Reference: ITU-R BS.1770-4

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_Upsampler_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_Upsampler_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class Upsampler
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           process_sample_4x (float dst_ptr [4], float x) noexcept;
	void           process_block_4x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           process_sample_2x (float dst_ptr [2], float x) noexcept;
	void           process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _fir_len       = 12;
	static constexpr int _nbr_phases_l2 = 2;
	static constexpr int _nbr_phases    = 1 << _nbr_phases_l2;

	typedef std::array <float, _fir_len> Phase;
	typedef std::array <Phase, _nbr_phases> PhaseArray;

	template <int OL2>
	inline void    process_sample_nx (float dst_ptr [1 << OL2], float x) noexcept;
	template <int OL2>
	inline void    process_block_nx (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	template <int OL2>
	static inline void
	               process_block_nx_internal (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr, int nbr_spl) noexcept;
	template <int OL2>
	static inline void
	               process_sample_nx_internal (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr) noexcept;

	Phase          _buf {};       // Source buffer. Contains _fir_len - 1 samples

	alignas (fstb_SIMD128_ALIGN) static const PhaseArray
	               _phase_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Upsampler &other) const = delete;
	bool           operator != (const Upsampler &other) const = delete;

}; // class Upsampler



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/Upsampler.hpp"



#endif   // mfx_dsp_ana_r128_Upsampler_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
