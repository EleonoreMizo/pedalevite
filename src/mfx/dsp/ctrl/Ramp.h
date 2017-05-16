/*****************************************************************************

        Ramp.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_Ramp_HEADER_INCLUDED)
#define mfx_dsp_ctrl_Ramp_HEADER_INCLUDED

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



class Ramp
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Ramp ()                        = default;
	inline explicit
	               Ramp (float x);
	               Ramp (const Ramp &other)       = default;
	virtual        ~Ramp ()                       = default;
	Ramp &         operator = (const Ramp &other) = default;

	inline void    set_time (int nbr_spl, float time_step);
	inline void    set_val (float x);
	inline void    clear_buffers ();
	inline void    tick (int nbr_spl);
	inline float   get_beg () const;
	inline float   get_end () const;
	inline float   get_tgt () const;
	inline bool    is_ramping () const;
	inline int     get_rem_len () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            _ramp_time = 64;       // Samples
	int            _ramp_pos  = 0;        // 0 = no active ramp
	float          _time_step = 1.0f / _ramp_time;
	float          _val_beg   = 0;
	float          _val_end   = 0;
	float          _val_tgt   = 0;
	float          _step      = 0;        // Per sample



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Ramp &other) const = delete;
	bool           operator != (const Ramp &other) const = delete;

}; // class Ramp



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/Ramp.hpp"



#endif   // mfx_dsp_ctrl_Ramp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
