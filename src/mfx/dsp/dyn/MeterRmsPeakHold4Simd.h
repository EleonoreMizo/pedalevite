/*****************************************************************************

        MeterRmsPeakHold4Simd.h
        Author: Laurent de Soras, 2016

This object must be aligned on 16-byte boundaries

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_MeterRmsPeakHold4Simd_HEADER_INCLUDED)
#define mfx_dsp_dyn_MeterRmsPeakHold4Simd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Vf32.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



class MeterRmsPeakHold4Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MeterRmsPeakHold4Simd () noexcept;
	               MeterRmsPeakHold4Simd (const MeterRmsPeakHold4Simd &other) = default;
	               MeterRmsPeakHold4Simd (MeterRmsPeakHold4Simd &&other) = default;
	virtual        ~MeterRmsPeakHold4Simd ()                             = default;

	MeterRmsPeakHold4Simd &
	               operator = (const MeterRmsPeakHold4Simd &other)       = default;
	MeterRmsPeakHold4Simd &
	               operator = (MeterRmsPeakHold4Simd &&other)            = default;

	void           set_sample_freq (double freq) noexcept;
	void           set_hold_time_s (double t) noexcept;
	void           set_attack_time_s (double t) noexcept;
	void           set_release_time_s (double t) noexcept;

	void           clear_buffers () noexcept;
	void           process_block (const float * const data_ptr [4], int nbr_spl) noexcept;
	void           process_sample (fstb::Vf32 x) noexcept;
	fstb::Vf32     get_peak () const noexcept;
	fstb::Vf32     get_peak_hold () const noexcept;
	fstb::Vf32     get_rms () const noexcept;
	void           clear_peak () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_times () noexcept;
	inline void    process_sample_peak (fstb::Vf32 x_a, fstb::Vf32 &peak_max, fstb::Vf32 &peak_hold, fstb::Vf32 &hold_cnt, float coef_r_flt, int step_int) const noexcept;
	inline void    process_sample_rms (fstb::Vf32 x, fstb::Vf32 &rms_sq) const noexcept;

	double         _hold_time_s;     // Hold time, in s
	double         _attack_time_s;   // Attack time, in s
	double         _release_time_s;  // Release time, in s

	double         _sample_freq;     // Hz
	alignas (16) fstb::Vf32
	               _peak_max;        // Maximum peak value
	alignas (16) fstb::Vf32
	               _peak_hold;       // Hold value for peak
	alignas (16) fstb::Vf32
	               _hold_counter;    // Counter for holding time (samples). Set to the maximum at a peak, and decreased. When reaching 0, the peak is set to the instantaneous value.
	alignas (16) fstb::Vf32
                  _rms_sq;          // RMS value with an envelope, squared

	int            _hold_time;       // Samples
	float          _coef_r;
	float          _coef_r4x;
	float          _coef_r2;
	float          _coef_a2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MeterRmsPeakHold4Simd &other) const = delete;
	bool           operator != (const MeterRmsPeakHold4Simd &other) const = delete;

}; // class MeterRmsPeakHold4Simd



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dyn/MeterRmsPeakHold4Simd.hpp"



#endif   // mfx_dsp_dyn_MeterRmsPeakHold4Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
