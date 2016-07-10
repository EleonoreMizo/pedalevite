/*****************************************************************************

        OscInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_OscInterface_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscInterface_HEADER_INCLUDED

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
namespace lfo
{



class OscInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Variation
	{
		Variation_TIME = 0,
		Variation_SHAPE,

		Variation_NBR_ELT
	};

	virtual        ~OscInterface () = default;

	inline void    set_sample_freq (double sample_freq);
	inline void    set_period (double per);
	inline void    set_phase (double phase);
	inline void    set_chaos (double chaos);
	inline void    set_phase_dist (double dist);
	inline void    set_sign (bool inv_flag);
	inline void    set_polarity (bool unipolar_flag);
	inline void    set_variation (int param, double val);
	inline bool    is_using_variation (int param) const;
	inline void    tick (long nbr_spl);
	inline double  get_val () const;
	inline double  get_phase () const;
	inline void    clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_sample_freq (double sample_freq) = 0;
	virtual void   do_set_period (double per) = 0;
	virtual void   do_set_phase (double phase) = 0;
	virtual void   do_set_chaos (double chaos) = 0;
	virtual void   do_set_phase_dist (double dist) = 0;
	virtual void   do_set_sign (bool inv_flag) = 0;
	virtual void   do_set_polarity (bool unipolar_flag) = 0;
	virtual void   do_set_variation (int param, double val) = 0;
	virtual bool   do_is_using_variation (int param) const = 0;
	virtual void   do_tick (long nbr_spl) = 0;
	virtual double do_get_val () const = 0;
	virtual double do_get_phase () const = 0;
	virtual void   do_clear_buffers () = 0;



}; // class OscInterface



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/lfo/OscInterface.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
