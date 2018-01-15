/*****************************************************************************

        LimiterRms.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_LimiterRms_HEADER_INCLUDED)
#define mfx_dsp_dyn_LimiterRms_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "fstb/def.h"
#include "mfx/dsp/dyn/EnvFollowerRmsSimple.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace dyn
{



class LimiterRms
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               LimiterRms ();
	virtual        ~LimiterRms () = default;

	void           set_sample_freq (double fs);
	void           set_time (float t);
	void           set_level (float l);

	fstb_FORCEINLINE float
	               process_sample (float x);

	void           process_block (float dst_ptr [], const float src_ptr [], long nbr_spl);
	void           clear_buffers ();
	inline void    apply_volume (float gain);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	static const int  _tmp_buf_len = 256;

	dsp::dyn::EnvFollowerRmsSimple
	               _env;
	BufAlign       _buf;
	float          _lvl;
	float          _lvl_sq;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               LimiterRms (const LimiterRms &other)        = delete;
	LimiterRms &   operator = (const LimiterRms &other)        = delete;
	bool           operator == (const LimiterRms &other) const = delete;
	bool           operator != (const LimiterRms &other) const = delete;

}; // class LimiterRms



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dyn/LimiterRms.hpp"



#endif   // mfx_dsp_dyn_LimiterRms_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
