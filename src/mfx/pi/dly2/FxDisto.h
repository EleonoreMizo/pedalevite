/*****************************************************************************

        FxDisto.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_FxDisto_HEADER_INCLUDED)
#define mfx_pi_dly2_FxDisto_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dyn/EnvFollowerRmsSimple.h"
#include "mfx/dsp/InertiaLin.h"



namespace mfx
{
namespace pi
{
namespace dly2
{



class FxDisto
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FxDisto ()  = default;
	virtual        ~FxDisto () = default;

	void           set_sample_freq (double sample_freq);
	void           set_amount (float amt);
	void           set_foldback (float foldback);
	void           clear_buffers ();
	void           process_block (float data_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_gains ();
	void           process_softclip (float data_ptr [], int nbr_spl);
	void           process_foldback (float data_ptr [], int nbr_spl);

	dsp::dyn::EnvFollowerRmsSimple
	               _env_pre;
	dsp::dyn::EnvFollowerRmsSimple
	               _env_post;
	float          _limiter_level = 0.75f;
	float          _amount_cur    = 0;
	float          _amount_old    = 0;
	float          _fold_cur      = 1;
	float          _fold_old      = 1;

	float          _gain_pre      = 0.25f;
	float          _gain_post     = 4;
	float          _clip_val      = 1.5f;

	static const float                  // Maximum gain for volume compensation (not taking _gain_post into account)
	               _gain_max_comp;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FxDisto (const FxDisto &other)           = delete;
	FxDisto &      operator = (const FxDisto &other)        = delete;
	bool           operator == (const FxDisto &other) const = delete;
	bool           operator != (const FxDisto &other) const = delete;

}; // class FxDisto



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/FxDisto.hpp"



#endif   // mfx_pi_dly2_FxDisto_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
