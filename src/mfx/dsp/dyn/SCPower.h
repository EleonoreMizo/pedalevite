/*****************************************************************************

        SCPower.h
        Author: Laurent de Soras, 2016

Template parameters:

- AP: Additional processing object. Must have:
	AP::AP();
	AP::~AP();
	float AP::process_scalar (float in);
	fstb::ToolsSimd::VectF32 AP::process_vect (const fstb::ToolsSimd::VectF32 &in);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_SCPower_HEADER_INCLUDED)
#define mfx_dsp_dyn_SCPower_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



class SPower_Bypass
{
public:
	static inline constexpr float
	               process_scalar (float in) { return in; }
	static inline constexpr fstb::ToolsSimd::VectF32
	               process_vect (fstb::ToolsSimd::VectF32 in) { return in; }
};



template <class AP = SPower_Bypass>
class SCPower
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef AP AddProc;

	AddProc &      use_add_proc ();

	void           prepare_env_input (float out_ptr [], const float * const chn_ptr_arr [], int nbr_chn, int pos_beg, int pos_end);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           prepare_env_input_1chn (float out_ptr [], const float * const chn_ptr_arr [], int pos_beg, int pos_end);
	void           prepare_env_input_2chn (float out_ptr [], const float * const chn_ptr_arr [], int pos_beg, int pos_end);
	void           prepare_env_input_nchn (float out_ptr [], const float * const chn_ptr_arr [], int nbr_chn, int pos_beg, int pos_end);

	AddProc        _add_proc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SCPower <AP> &other) const = delete;
	bool           operator != (const SCPower <AP> &other) const = delete;

}; // class SCPower



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dyn/SCPower.hpp"



#endif   // mfx_dsp_dyn_SCPower_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
