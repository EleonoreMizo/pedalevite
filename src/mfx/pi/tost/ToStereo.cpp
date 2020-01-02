/*****************************************************************************

        ToStereo.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/tost/ToStereo.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace tost
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ToStereo::ToStereo ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	ToStereo::do_get_state () const
{
	return _state;
}



double	ToStereo::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	ToStereo::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (sample_freq, max_buf_len);

	_param_proc.req_steady ();

	latency = 0;

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	ToStereo::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_in <= nbr_chn_out);
	
	// Events
	_param_proc.handle_msg (proc);

	for (int chn_dst = 0; chn_dst < nbr_chn_out; ++chn_dst)
	{
		const int      chn_src = (chn_dst < nbr_chn_in) ? chn_dst : 0;
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_dst],
			proc._src_arr [chn_src],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace tost
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
