/*****************************************************************************

        ToStereo.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_tost_ToStereo_HEADER_INCLUDED)
#define mfx_pi_tost_ToStereo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/tost/ToStereoDesc.h"
#include "mfx/piapi/PluginInterface.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"



namespace mfx
{
namespace pi
{
namespace tost
{



class ToStereo final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ToStereo (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	piapi::HostInterface &
	               _host;
	State          _state;

	ToStereoDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ToStereo ()                               = delete;
	               ToStereo (const ToStereo &other)          = delete;
	               ToStereo (ToStereo &&other)               = delete;
	ToStereo &     operator = (const ToStereo &other)        = delete;
	ToStereo &     operator = (ToStereo &&other)             = delete;
	bool           operator == (const ToStereo &other) const = delete;
	bool           operator != (const ToStereo &other) const = delete;

}; // class ToStereo



}  // namespace tost
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/tost/ToStereo.hpp"



#endif   // mfx_pi_tost_ToStereo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
