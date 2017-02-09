/*****************************************************************************

        EnvFollowerRms.h
        Author: Laurent de Soras, 2016

This class can be inherited but is not polymorph.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_EnvFollowerRms_HEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerRms_HEADER_INCLUDED

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



class EnvFollowerRms
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               EnvFollowerRms ();
	               ~EnvFollowerRms () = default;

	void           set_sample_freq (double fs);
	void           set_times (float at, float rt);
	void           set_time_atk (float t);
	void           set_time_rel (float t);

	fstb_FORCEINLINE float
	               process_sample (float x);
	fstb_FORCEINLINE float
	               process_sample_no_sqrt (float x);

	void           process_block (float out_ptr [], const float in_ptr [], long nbr_spl);
	void           process_block_no_sqrt (float out_ptr [], const float in_ptr [], long nbr_spl);
	void           process_block_raw (float out_ptr [], const float in_ptr [], long nbr_spl);
	float          analyse_block (const float data_ptr [], long nbr_spl);
	float          analyse_block_raw (const float data_ptr [], long nbr_spl);
	float          analyse_block_raw_cst (float x2, long nbr_spl);
	inline float   get_state_no_sqrt () const;

	void           clear_buffers ();

	inline void    apply_volume (float gain);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
						process_sample_internal (float &state, float x) const;
	fstb_FORCEINLINE void
						process_sample_internal_no_sq (float &state, float x2) const;

	void           update_parameters ();

	float          _sample_freq;	// Hz, > 0
	float          _time_a;			// s, >= 0
	float          _time_r;			// s, >= 0

	float          _state;			// Sample ^ 2
	float          _coef_a;
	float          _coef_r;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EnvFollowerRms (const EnvFollowerRms &other)    = delete;
	EnvFollowerRms &
	               operator = (const EnvFollowerRms &other)        = delete;
	bool           operator == (const EnvFollowerRms &other) const = delete;
	bool           operator != (const EnvFollowerRms &other) const = delete;

}; // class EnvFollowerRms



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dyn/EnvFollowerRms.hpp"



#endif   // mfx_dsp_dyn_EnvFollowerRms_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
