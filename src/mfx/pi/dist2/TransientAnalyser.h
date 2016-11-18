/*****************************************************************************

        TransientAnalyser.h
        Author: Laurent de Soras, 2016

Objects of this class should be aligned on 16-bytes boudaries.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist2_TransientAnalyser_HEADER_INCLUDED)
#define mfx_pi_dist2_TransientAnalyser_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/AllocAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvFollowerAHR4SimdHelper.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace dist2
{



class TransientAnalyser
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               TransientAnalyser ()  = default;
	virtual        ~TransientAnalyser () = default;

	void           set_epsilon (float eps);
	void           reset (double sample_freq, int max_block_size);
	void           process_block (float atk_ptr [], float sus_ptr [], const float * const src_ptr_arr [], int nbr_chn, int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <
		fstb::ToolsSimd::VectF32,
		fstb::AllocAlign <fstb::ToolsSimd::VectF32, 16>
	> Buffer4;

	typedef dsp::dyn::EnvFollowerAHR4SimdHelper <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> EnvHelper;

	float          compute_coef (float t) const;
	static void    perpare_mono_input (fstb::ToolsSimd::VectF32 buf_ptr [], const float * const src_ptr_arr [], int nbr_chn, int nbr_spl);
	static inline void
	               spread_and_store (fstb::ToolsSimd::VectF32 dst_ptr [], fstb::ToolsSimd::VectF32 x);

	EnvHelper      _env_helper;         // 0 = fast env, 1 = slow env
	Buffer4        _buf;
	float          _sample_freq    = 0; // Hz, > 0. 0 = not initialized.
	int            _max_block_size = 0; // samples, > 0. 0 = not initialized.
	float          _eps_sq         = 1e-18f;  // Squared epsilon



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TransientAnalyser (const TransientAnalyser &other) = delete;
	TransientAnalyser &
	               operator = (const TransientAnalyser &other)        = delete;
	bool           operator == (const TransientAnalyser &other) const = delete;
	bool           operator != (const TransientAnalyser &other) const = delete;

}; // class TransientAnalyser



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist2/TransientAnalyser.hpp"



#endif   // mfx_pi_dist2_TransientAnalyser_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
