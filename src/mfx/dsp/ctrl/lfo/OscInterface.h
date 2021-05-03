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

	               OscInterface ()                          = default;
	               OscInterface (const OscInterface &other) = default;
	               OscInterface (OscInterface &&other)      = default;
	virtual        ~OscInterface ()                         = default;

	virtual OscInterface &
	               operator = (const OscInterface &other)   = default;
	virtual OscInterface &
	               operator = (OscInterface &&other)        = default;

	inline void    set_sample_freq (double sample_freq) noexcept;
	inline void    set_period (double per) noexcept;
	inline void    set_phase (double phase) noexcept;
	inline void    set_chaos (double chaos) noexcept;
	inline void    set_phase_dist (double dist) noexcept;
	inline void    set_phase_dist_offset (double ofs) noexcept;
	inline void    set_sign (bool inv_flag) noexcept;
	inline void    set_polarity (bool unipolar_flag) noexcept;
	inline void    set_variation (int param, double val) noexcept;
	inline bool    is_using_variation (int param) const noexcept;
	inline void    tick (int nbr_spl) noexcept;
	inline double  get_val () const noexcept;
	inline double  get_phase () const noexcept;
	inline void    clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_sample_freq (double sample_freq) = 0;
	virtual void   do_set_period (double per) noexcept = 0;
	virtual void   do_set_phase (double phase) noexcept = 0;
	virtual void   do_set_chaos (double chaos) noexcept = 0;
	virtual void   do_set_phase_dist (double dist) noexcept = 0;
	virtual void   do_set_phase_dist_offset (double ofs) noexcept = 0;
	virtual void   do_set_sign (bool inv_flag) noexcept = 0;
	virtual void   do_set_polarity (bool unipolar_flag) noexcept = 0;
	virtual void   do_set_variation (int param, double val) noexcept = 0;
	virtual bool   do_is_using_variation (int param) const noexcept = 0;
	virtual void   do_tick (int nbr_spl) noexcept = 0;
	virtual double do_get_val () const noexcept = 0;
	virtual double do_get_phase () const noexcept = 0;
	virtual void   do_clear_buffers () noexcept = 0;



}; // class OscInterface



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/lfo/OscInterface.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
