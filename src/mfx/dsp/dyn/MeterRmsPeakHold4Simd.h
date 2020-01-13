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

#include "fstb/def.h"
#include "fstb/ToolsSimd.h"



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

	               MeterRmsPeakHold4Simd ();
	               MeterRmsPeakHold4Simd (const MeterRmsPeakHold4Simd &other) = default;
	               MeterRmsPeakHold4Simd (MeterRmsPeakHold4Simd &&other) = default;
	virtual        ~MeterRmsPeakHold4Simd ()                             = default;

	MeterRmsPeakHold4Simd &
	               operator = (const MeterRmsPeakHold4Simd &other)       = default;
	MeterRmsPeakHold4Simd &
	               operator = (MeterRmsPeakHold4Simd &&other)            = default;

	void           set_sample_freq (double freq);
	void           set_hold_time_s (double t);
	void           set_attack_time_s (double t);
	void           set_release_time_s (double t);

	void           clear_buffers ();
	void           process_block (const float * const data_ptr [4], int nbr_spl);
	void           process_sample (fstb::ToolsSimd::VectF32 x);
	fstb::ToolsSimd::VectF32
	               get_peak () const;
	fstb::ToolsSimd::VectF32
	               get_peak_hold () const;
	fstb::ToolsSimd::VectF32
	               get_rms () const;
	void           clear_peak ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_TYPEDEF_ALIGN (16, fstb::ToolsSimd::VectF32, VectF32);

	void           update_times ();
	inline void    process_sample_peak (fstb::ToolsSimd::VectF32 x_a, fstb::ToolsSimd::VectF32 &peak_max, fstb::ToolsSimd::VectF32 &peak_hold, fstb::ToolsSimd::VectF32 &hold_cnt, float coef_r_flt, int step_int) const;
	inline void    process_sample_rms (fstb::ToolsSimd::VectF32 x, fstb::ToolsSimd::VectF32 &rms_sq) const;

	double         _hold_time_s;     // Hold time, in s
	double         _attack_time_s;   // Attack time, in s
	double         _release_time_s;  // Release time, in s

	double         _sample_freq;     // Hz
	VectF32        _peak_max;        // Maximum peak value
	VectF32        _peak_hold;       // Hold value for peak
	VectF32        _hold_counter;    // Counter for holding time (samples). Set to the maximum at a peak, and decreased. When reaching 0, the peak is set to the instantaneous value.
   VectF32        _rms_sq;          // RMS value with an envelope, squared

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
