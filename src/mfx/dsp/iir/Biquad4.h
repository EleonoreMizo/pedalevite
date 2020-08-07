/*****************************************************************************

        Biquad4.h
        Author: Laurent de Soras, 2020

Substitute for Biquad4Simd when no SIMD instruction set is available and
DATA amount is low.

Functions using ToolsSimd::VectF32 types are not implemented.

There is no latency for serial processing, so the _immediate and _latency map
to the same functions.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Biquad4_HEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/Biquad.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



class Biquad4
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_units      = 4;  // Number of processing units
	static constexpr int _latency_serial = 0;  // Latency for serial processing, samples
	static constexpr int _latency_2x2    = 0;  // Latency for 2-way processing, samples

	typedef float VectFloat4 [_nbr_units];

	               Biquad4 ();
	               Biquad4 (const Biquad4 &other)    = default;
	               Biquad4 (Biquad4 &&other)         = default;

	               ~Biquad4 ()                       = default;

	Biquad4 &      operator = (const Biquad4 &other) = default;
	Biquad4 &      operator = (Biquad4 &&other)      = default;

	void           neutralise ();
	void           neutralise_one (int biq);
	void           set_z_eq (const VectFloat4 b [3], const VectFloat4 a [3]);
	void           set_z_eq_same (const float b [3], const float a [3]);
	void           set_z_eq_one (int biq, const float b [3], const float a [3]);

	void           get_z_eq (VectFloat4 b [3], VectFloat4 a [3]) const;
	void           get_z_eq_one (int biq, float b [3], float a [3]) const;

	void           copy_z_eq (const Biquad4 &other);

	void           set_state_one (int biq, float const mem_x [2], const float mem_y [2]);
	void           get_state_one (int biq, float mem_x [2], float mem_y [2]) const;

	inline void    process_block_serial_latency (float dst_ptr [], const float src_ptr [], int nbr_spl);
	fstb_FORCEINLINE float
	               process_sample_serial_latency (float x_s);

	void           process_block_serial_immediate (float dst_ptr [], const float src_ptr [], int nbr_spl);
	fstb_FORCEINLINE float
	               process_sample_serial_immediate (float x_s);

	inline void    process_block_2x2_latency (float dst_ptr [], const float src_ptr [], int nbr_spl);

	void           process_block_2x2_immediate (float dst_ptr [], const float src_ptr [], int nbr_spl);

	void           clear_buffers ();
	void           clear_buffers_one (int biq);

	fstb_FORCEINLINE static bool
	               check_stability (float a1, float a2);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <Biquad, _nbr_units> BiqArray;

	BiqArray       _biq_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Biquad4 &other) const = delete;
	bool           operator != (const Biquad4 &other) const = delete;

}; // class Biquad4



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Biquad4.hpp"



#endif   // mfx_dsp_iir_Biquad4_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
