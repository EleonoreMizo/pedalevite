/*****************************************************************************

        InertiaLin.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_InertiaLin_HEADER_INCLUDED)
#define mfx_dsp_InertiaLin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{



class InertiaLin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               InertiaLin ()                        = default;
	               InertiaLin (const InertiaLin &other) = default;
	virtual        ~InertiaLin ()                       = default;

	InertiaLin &   operator = (const InertiaLin &other) = default;

	inline void		set_inertia_time (double inertia_time);
	inline double	get_inertia_time () const;

	inline void		set_sample_freq (double fs);
	inline void		set_val (double val);
	inline void		force_val (double val);	// No inertia
	inline double	get_val () const;
	inline double	get_target_val () const;
	inline void		tick (long nbr_spl);
	inline bool		is_ramping () const;
	inline double	get_step () const;
   inline void    stop ();
	inline void		clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _inertia_time = 0.100;  // s
	double         _fs           = 44100;  // Hz
	double         _old_val      = 0;
	double         _new_val      = 0;
	double         _cur_val      = 0;
	double         _step         = 0;      // per sample



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const InertiaLin &other) const = delete;
	bool           operator != (const InertiaLin &other) const = delete;

}; // class InertiaLin



}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/InertiaLin.hpp"



#endif   // mfx_dsp_InertiaLin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
