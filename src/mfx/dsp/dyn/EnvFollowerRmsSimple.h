/*****************************************************************************

        EnvFollowerRmsSimple.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_EnvFollowerRmsSimple_HEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerRmsSimple_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/dyn/EnvHelper.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



class EnvFollowerRmsSimple
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double fs);
	void           set_time (float t);

	fstb_FORCEINLINE float
	               process_sample (float x);
	fstb_FORCEINLINE float
	               process_sample_no_sqrt (float x);

	void           process_block (float out_ptr [], const float in_ptr [], int nbr_spl);
	void           process_block_no_sqrt (float out_ptr [], const float in_ptr [], int nbr_spl);
	void           process_block_raw (float out_ptr [], const float in_ptr [], int nbr_spl);
	float          analyse_block (const float data_ptr [], int nbr_spl);
	float          analyse_block_no_sqrt (const float data_ptr [], int nbr_spl);
	float          analyse_block_raw (const float data_ptr [], int nbr_spl);
	float          analyse_block_raw_cst (float x2, int nbr_spl);
	inline float   get_state_no_sqrt () const;

	void           clear_buffers ();

	inline void    apply_volume (float gain);
	inline void    set_state_raw (float x2);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
						process_sample_internal (float &state, float x) const;
	fstb_FORCEINLINE void
						process_sample_internal_no_sq (float &state, float x2) const;

	void           update_parameters ();

	float          _sample_freq = 44100.f;    // Hz, > 0
	float          _time        =     0.001f; // s, >= 0

	float          _state       = 0;          // Sample ^ 2
	float          _coef        =
		float (EnvHelper::compute_env_coef_simple (_time, _sample_freq));



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const EnvFollowerRmsSimple &other) const = delete;
	bool           operator != (const EnvFollowerRmsSimple &other) const = delete;

}; // class EnvFollowerRmsSimple



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dyn/EnvFollowerRmsSimple.hpp"



#endif   // mfx_dsp_dyn_EnvFollowerRmsSimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
