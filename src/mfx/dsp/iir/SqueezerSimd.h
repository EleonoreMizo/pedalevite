/*****************************************************************************

        SqueezerSimd.h
        Author: Laurent de Soras, 2016

Template parameters:

- BR: enables the bit reduction in the feedback path

- LFOP: Lo-Fi operator. Typically one of the SqueezerOp*. Requires:
   LFOP::LFOP ();
	void LFOP::config (float reso, float p1);
	float LFOP::process_sample (float x);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SqueezerSimd_HEADER_INCLUDED)
#define mfx_dsp_iir_SqueezerSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/ToolsSimd.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <bool BR, class LFOP>
class SqueezerSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_stages = 4;

	static const bool _br_flag    = BR;
	typedef LFOP LoFiOperator;

	               SqueezerSimd ();
	               SqueezerSimd (const SqueezerSimd <BR, LFOP> &other) = default;
	               SqueezerSimd (SqueezerSimd <BR, LFOP> &&other)      = default;

	               ~SqueezerSimd ()                                    = default;

	SqueezerSimd <BR, LFOP> &
	               operator = (const SqueezerSimd <BR, LFOP> &other)   = default;
	SqueezerSimd <BR, LFOP> &
	               operator = (SqueezerSimd <BR, LFOP> &&other)        = default;

	inline void    set_sample_freq (float fs);
	inline void    set_freq (float f0);
	fstb_FORCEINLINE void
	               set_freq_and_update_eq_fast (float f0);
	inline float   get_freq () const;
	inline void    set_reso (float reso);
	inline float   get_reso () const;
	inline void    set_p1 (float p1);
	inline float   get_p1 () const;
	fstb_FORCEINLINE void
	               update_eq ();

	fstb_FORCEINLINE float
	               process_sample (float x);

	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           process_block (float spl_ptr [], int nbr_spl);
	void           process_block_fm (float dst_ptr [], const float src_ptr [], const float freq_ptr [], int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    update_internal_variables (float &r, float &g, float &p, float &out_gain, float fs, float freq, float reso);
	static void    update_internal_variables_fast (float &r, float &g, float &p, float &out_gain, float fs, float freq, float reso);
	static fstb::ToolsSimd::VectF32
	               shape_feedback (fstb::ToolsSimd::VectF32 x);

	// Internal variables, SSE code. Do not change the order!
	std::array <float, _nbr_stages>
	               _y;               // y_{n} = x_{n+1}
	float          _x;               // x_{0}
	float          _r;               // Feedback coefficient. 4 is max, but it may be bigger.
	float          _p;               // Pole, < -1
	float          _g;               // Gain for the forward path of the 1-pole filters
	LoFiOperator   _lofi_op;

   // Other internal variables
   float          _gain_out;        // Compensation gain for output (because of reso tweaking)
	float          _gain_out_cur;    // Current value (low-pass filtered) for _out_gain
   float          _gain_out_lerp_step; // Step for the low-pass filter on gain, ]0 ; 1]

	float          _br_scale;        // Scale for the bit reduction (step level). Could be assimilated with _r to save a multiplication
	float          _br_scale_inv;    // Inverse scale for the bit reduction
	float          _br_amt;          // Amount of bit-reduced feedback [0 ; 1]

	// Classic variables
	float          _fs;              // Hz, > 0
	float          _freq;            // Hz, ]0..._fs/2[
	float          _reso;            // %, [0...1]
	float          _p1;              // %, [0...1]



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SqueezerSimd <BR, LFOP> &other) const = delete;
	bool           operator != (const SqueezerSimd <BR, LFOP> &other) const = delete;

}; // class SqueezerSimd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SqueezerSimd.hpp"



#endif   // mfx_dsp_iir_SqueezerSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
