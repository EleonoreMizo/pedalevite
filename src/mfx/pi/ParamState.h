/*****************************************************************************

        ParamState.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ParamState_HEADER_INCLUDED)
#define mfx_pi_ParamState_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlagCascadeMulti.h"
#include "mfx/dsp/ctrl/InertiaLin.h"



namespace mfx
{

namespace piapi
{
	class ParamDescInterface;
}

namespace pi
{



class ParamState
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ParamState ();
	               ParamState (const ParamState &other) = default;
	               ParamState (ParamState &&other)      = default;
	virtual        ~ParamState ()                       = default;

	ParamState &   operator = (const ParamState &other) = default;
	ParamState &   operator = (ParamState &&other)      = default;

	void           set_sample_freq (double fs);
	void           set_ramp_time (double t);
	void           set_desc (const piapi::ParamDescInterface &desc);
	const piapi::ParamDescInterface &
	               use_desc () const;
	fstb::util::ObservableInterface &
	               use_notifier ();
	fstb::util::NotificationFlagInterface &
	               use_notif_flag ();
	void           clear_buffers ();

	void           set_val (double val_nrm);

	void           tick (int nbr_spl);
	bool           is_ramping () const;
	double         get_val_beg () const;
	double         get_val_end () const;
	double         get_val_tgt () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _val_beg_nrm;        // Current value, normalized, beginning of the block
	double         _val_end_nrm;        // Current value, normalized, end of the block

	mfx::dsp::ctrl::InertiaLin          // Final target value, normalized (0-1)
	               _smooth;

	fstb::util::NotificationFlagCascadeMulti
	               _notifier;

	const piapi::ParamDescInterface *
	               _desc_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ParamState &other) const = delete;
	bool           operator != (const ParamState &other) const = delete;

}; // class ParamState



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ParamState.hpp"



#endif   // mfx_pi_ParamState_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
