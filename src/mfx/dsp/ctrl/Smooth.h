/*****************************************************************************

        Smooth.h
        Author: Laurent de Soras, 2020

Algorithm from:

Andrew Simper, Dynamic Smoothing Using Self Modulating Filter, 2016-12
https://cytomic.com/index.php?q=technical-papers

This class implements the "efficient" version, with an approximation to
compute g0 (the same used in the processing loop of the "accurate" version).

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_Smooth_HEADER_INCLUDED)
#define mfx_dsp_ctrl_Smooth_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace ctrl
{



class Smooth
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Smooth ()                        = default;
	               Smooth (const Smooth &other)     = default;
	               Smooth (Smooth &&other)          = default;
	               ~Smooth ()                       = default;

	Smooth &       operator = (const Smooth &other) = default;
	Smooth &       operator = (Smooth &&other)      = default;

	void           set_sample_freq (double sample_freq);
	void           set_base_freq (float freq);
	void           set_sensitivity (float s);

	void           force_val (float x);
	float          process_sample (float x);
	float          skip_block (int nbr_spl);
	float          process_block (float x, int nbr_spl);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);

	float          get_target_val () const;
	float          get_smooth_val () const;

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_g0_cond ();
	float          compute_g0 (float f0);
	float          process_sample (float x, float g0, float sense);

	// Parameters
	float          _sample_freq = 0; // Sampling rate, Hz, > 0. 0 = not set
	float          _inv_fs      = 0; // 1 / _sample_freq. 0 = not set
	float          _f0          = 2; // Base cutoff frequency, Hz. > 0.
	float          _sense       = 2; // Sensitivity to the signal changes. >= 0.

	// Internal values
	float          _g0          = 0.5f;

	// State
	float          _val         = 0; // Input value (target)
	float          _low1        = 0;
	float          _low2        = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Smooth &other) const = delete;
	bool           operator != (const Smooth &other) const = delete;

}; // class Smooth



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/Smooth.hpp"



#endif   // mfx_dsp_ctrl_Smooth_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
