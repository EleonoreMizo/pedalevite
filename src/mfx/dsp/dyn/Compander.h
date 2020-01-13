/*****************************************************************************

        Compander.h
        Author: Laurent de Soras, 2017

Matched compresser/expander

Reference:

Antti Huovilainen, Enhanced Digital Models for Analog Modulation Effects,
Proc. Digital Audio Effects (DAFx-05), 2005, pp. 155–160.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_Compander_HEADER_INCLUDED)
#define mfx_dsp_dyn_Compander_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dyn
{



class Compander
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_EXP = 0,
		Type_COMP,

		Type_NBR_ELT
	};

	void           set_sample_freq (double sample_freq);
	void           set_type (Type type);
	void           set_time (float t);
	void           set_threshold_lvl (float thr);

	void           clear_buffers ();
	float          process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_coef ();
	inline void    set_new_state (float x);

	double         _sample_freq = 0;          // Hz. 0 = not initialised
	Type           _type        = Type_EXP;
	float          _avg_time    = 0.010f;     // s, > 0
	float          _lvl_thr     = 1e-6f;      // > 0

	float          _coef        = 0.5f;
	float          _state       = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Compander &other) const = delete;
	bool           operator != (const Compander &other) const = delete;

}; // class Compander



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dyn/Compander.hpp"



#endif   // mfx_dsp_dyn_Compander_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
