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

	void           set_sample_freq (double sample_freq) noexcept;
	void           set_base_freq (float freq) noexcept;
	void           set_sensitivity (float s) noexcept;

	void           force_val (float x) noexcept;
	float          process_sample (float x) noexcept;
	float          skip_block (int nbr_spl) noexcept;
	float          process_block (float x, int nbr_spl) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	float          get_target_val () const noexcept;
	float          get_smooth_val () const noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_g0_cond () noexcept;
	float          compute_g0 (float f0) noexcept;
	float          process_sample (float x, float g0, float sense) noexcept;

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
