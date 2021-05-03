/*****************************************************************************

        EnvFollowerPeak.h
        Author: Laurent de Soras, 2018

This class can be inherited but is not polymorph.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_EnvFollowerPeak_HEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerPeak_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



class EnvFollowerPeak
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               EnvFollowerPeak () noexcept;
	               ~EnvFollowerPeak () = default;

	void           set_sample_freq (double fs) noexcept;
	void           set_times (float at, float rt) noexcept;
	void           set_time_atk (float t) noexcept;
	void           set_time_rel (float t) noexcept;

	fstb_FORCEINLINE float
	               process_sample (float x) noexcept;

	void           process_block (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;
	float          analyse_block (const float data_ptr [], int nbr_spl) noexcept;
	float          analyse_block_cst (float x, int nbr_spl) noexcept;
	inline float   get_state () const noexcept;

	void           clear_buffers () noexcept;

	inline void    apply_volume (float gain) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
						process_sample_internal (float &state, float x) const noexcept;

	void           update_parameters () noexcept;

	float          _sample_freq;	// Hz, > 0
	float          _time_a;			// s, >= 0
	float          _time_r;			// s, >= 0

	float          _state;
	float          _coef_a;
	float          _coef_r;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EnvFollowerPeak (const EnvFollowerPeak &other)   = delete;
	EnvFollowerPeak &
	               operator = (const EnvFollowerPeak &other)        = delete;
	bool           operator == (const EnvFollowerPeak &other) const = delete;
	bool           operator != (const EnvFollowerPeak &other) const = delete;

}; // class EnvFollowerPeak



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dyn/EnvFollowerPeak.hpp"



#endif   // mfx_dsp_dyn_EnvFollowerPeak_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
