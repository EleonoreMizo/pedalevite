/*****************************************************************************

        MeterRmsPeakHold.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_MeterRmsPeakHold_HEADER_INCLUDED)
#define mfx_dsp_dyn_MeterRmsPeakHold_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dyn
{



class MeterRmsPeakHold
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MeterRmsPeakHold ();
	               MeterRmsPeakHold (const MeterRmsPeakHold &other)  = default;
	virtual        ~MeterRmsPeakHold ()                              = default;
	MeterRmsPeakHold &
	               operator = (const MeterRmsPeakHold &other)        = default;

	void           set_sample_freq (double freq);
	void           set_hold_time_s (double t);
	void           set_attack_time_s (double t);
	void           set_release_time_s (double t);

	void           clear_buffers ();
	void           process_block (const float data_ptr [], int nbr_spl);
	void           process_sample (float x);
	void           skip_block (int nbr_spl);
	double         get_peak () const;
	double         get_peak_hold () const;
	double         get_rms () const;
	void           clear_peak ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_times ();
	void           process_sample_internal (float x);
	void           fix_tiny_values ();

	double         _hold_time_s;     // Hold time, in s
	double         _attack_time_s;   // Attack time, in s
	double         _release_time_s;  // Release time, in s

	double         _sample_freq;     // Hz
	double         _peak_max;        // Maximum peak value
	double         _peak_hold;       // Hold value for peak
	int            _hold_counter;    // Counter for holding time (samples). Set to the maximum at a peak, and decreased. When reaching 0, the peak is set to the instantaneous value.
   double         _rms_sq;          // RMS value with an envelope, squared

	int            _hold_time;       // Samples
	double         _coef_r;
	double         _coef_r2;
	double         _coef_a2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MeterRmsPeakHold &other) const = delete;
	bool           operator != (const MeterRmsPeakHold &other) const = delete;

}; // class MeterRmsPeakHold



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dyn/MeterRmsPeakHold.hpp"



#endif   // mfx_dsp_dyn_MeterRmsPeakHold_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
