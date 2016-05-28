/*****************************************************************************

        PluginInterface.cpp
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

#include "fstb/fnc.h"
#include	"mfx/piapi/PluginInterface.h"

#include	<cassert>



namespace mfx
{
namespace piapi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PluginInterface::State	PluginInterface::get_state () const
{
	return (do_get_state ());
}



int	PluginInterface::init ()
{
	assert (get_state () == State_CONSTRUCTED);

	const int      ret_val = do_init ();
	assert (   (get_state () == State_INITIALISED && ret_val == Err_OK)
	        || (get_state () == State_CONSTRUCTED && ret_val != Err_OK));

	return (ret_val);
}



int	PluginInterface::restore ()
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);

	const int      ret_val = do_restore ();
	assert (get_state () == State_CONSTRUCTED);

	return (ret_val);
}



void	PluginInterface::get_nbr_io (int &nbr_i, int &nbr_o) const
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);

	do_get_nbr_io (nbr_i, nbr_o);
	assert (nbr_i > 0);
	assert (nbr_o > 0);
}



bool	PluginInterface::prefer_stereo () const
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);

	return do_prefer_stereo ();
}



int	PluginInterface::get_nbr_param (ParamCateg categ) const
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);
	assert (categ >= 0);
	assert (categ < ParamCateg_NBR_ELT);

	const int      nbr_param = do_get_nbr_param (categ);
	assert (nbr_param >= 0);

	return (nbr_param);
}




const ParamDescInterface &	PluginInterface::get_param_info (ParamCateg categ, int index) const
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);
	assert (categ >= 0);
	assert (categ < ParamCateg_NBR_ELT);
	assert (index >= 0);
	assert (index < get_nbr_param (categ));

	const auto &   desc = do_get_param_info (categ, index);

	return (desc);
}



double	PluginInterface::get_param_val (ParamCateg categ, int index, int note_id) const
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);
	assert (categ >= 0);
	assert (categ < ParamCateg_NBR_ELT);

	const double   val = do_get_param_val (categ, index, note_id);
	assert (val >= 0);
	assert (val <= 1);

	return (val);
}



int	PluginInterface::reset (double sample_freq, int max_block_size, int &latency)
{
	assert (   get_state () == State_INITIALISED
	        || get_state () == State_ACTIVE);
	assert (sample_freq > 0);
	assert (max_block_size > 0);
	assert (latency == 0);

	const int      ret_val = do_reset (sample_freq, max_block_size, latency);
	assert (ret_val == Err_OK || get_state () == State_INITIALISED);
	assert (ret_val != Err_OK || get_state () == State_ACTIVE     );
	assert (ret_val != Err_OK || latency >= 0);

	return (ret_val);
}



void	PluginInterface::process_block (ProcInfo &proc)
{
	assert (get_state () == State_ACTIVE);
	assert (proc._nbr_chn_arr [Dir_OUT] == 0 || proc._dst_arr  != 0);
	assert (proc._nbr_chn_arr [Dir_OUT] == 0 || fstb::is_ptr_align_nz (proc._dst_arr [0]));
	assert (proc._byp_state == BypassState_IGNORE || proc._nbr_chn_arr [Dir_OUT] == 0 || proc._byp_arr [0] != 0);
	assert (proc._byp_state == BypassState_IGNORE || proc._nbr_chn_arr [Dir_OUT] == 0 || fstb::is_ptr_align_nz (proc._byp_arr [0]));
	assert (proc._byp_state == BypassState_IGNORE || proc._byp_state == BypassState_ASK);
	assert (proc._nbr_chn_arr [Dir_IN ] == 0 || proc._src_arr [0] != 0);
	assert (proc._nbr_chn_arr [Dir_IN ] == 0 || fstb::is_ptr_align_nz (proc._src_arr [0]));
	assert (proc._nbr_chn_arr != 0);
	assert (proc._nbr_chn_arr [Dir_IN ] >= 0);
	assert (proc._nbr_chn_arr [Dir_IN ] <= _max_nbr_chn);
	assert (proc._nbr_chn_arr [Dir_OUT] >= 0);
	assert (proc._nbr_chn_arr [Dir_OUT] <= _max_nbr_chn);
	assert (proc._nbr_spl > 0);
	assert (proc._evt_arr != 0 || proc._nbr_evt == 0);
	assert (proc._nbr_evt >= 0);

	do_process_block (proc);

	assert (proc._byp_state >= 0);
	assert (proc._byp_state < BypassState_NBR_ELT);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace piapi
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
