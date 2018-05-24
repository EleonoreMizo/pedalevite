/*****************************************************************************

        SegmentRc.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_env_SegmentRc_HEADER_INCLUDED)
#define mfx_dsp_ctrl_env_SegmentRc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace env
{



class SegmentRc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SegmentRc ()                        = default;
	               SegmentRc (const SegmentRc &other)  = default;
	virtual        ~SegmentRc ()                       = default;
	SegmentRc &    operator = (const SegmentRc &other) = default;

	void           setup (float final_val, float mult, float end_thr);
	void           setup (float final_val, float mult, float end_thr, int duration);
	void           setup_and_set_val (float final_val, float mult, float end_thr, float val);
	void           set_val (float val);
	inline float   get_val () const;
	inline float   get_final_val () const;
	inline int     get_nbr_rem_spl () const;
	inline float   process_sample ();
	void           process_block (float data_ptr [], int nbr_spl);
	void           skip_block (int nbr_spl);
	inline bool    is_finished () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    setup_partial (float final_val, float mult, float end_thr);
   inline void    set_val_direct (float val);
	inline void    compute_nbr_rem_spl ();

	float          _raw_val     = 1;          // May be 0 if _final_val is reached
	float          _end_thr     = 1 / 256.f;  // > 0
	float          _final_val   = 0;
	float          _mult        = 255 / 256.f;// ]-1 ; 1[. May be changed internally

	float          _offset      = 0;
	int            _nbr_rem_spl = 1417;       // >= 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SegmentRc &other) const = delete;
	bool           operator != (const SegmentRc &other) const = delete;

}; // class SegmentRc



}  // namespace env
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/env/SegmentRc.hpp"



#endif   // mfx_dsp_ctrl_env_SegmentRc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
