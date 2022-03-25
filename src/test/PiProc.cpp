/*****************************************************************************

        PiProc.cpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/PluginInterface.h"
#include "test/PiProc.h"

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PiProc::set_desc (DescSPtr desc_sptr)
{
	assert (desc_sptr.get () != nullptr);

	_desc_sptr = desc_sptr;
}



int	PiProc::setup (mfx::piapi::PluginInterface &pi, int nbr_chn_i, int nbr_chn_o, double sample_freq, int max_block_size, int &latency)
{
	assert (_desc_sptr.get () != nullptr);
	const int      mbs_alig = (max_block_size + 3) & -4;
	int            nbr_i = 1;
	int            nbr_o = 1;
	int            nbr_s = 0;
	_desc_sptr->get_nbr_io (nbr_i, nbr_o, nbr_s);
	assert (nbr_chn_i >= nbr_i);
	assert (nbr_chn_o >= nbr_o);
	_buf_list.resize (nbr_chn_i * nbr_i + nbr_chn_o * nbr_o + nbr_s);
	_buf_src_ptr_list.resize (nbr_chn_i * nbr_i);
	_buf_dst_ptr_list.resize (nbr_chn_o * nbr_o);
	_buf_sig_ptr_list.resize (nbr_s);

	int            buf_idx = 0;

	for (int chn = 0; chn < nbr_chn_i * nbr_i; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_src_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._src_arr = nullptr;
	if (nbr_chn_i > 0)
	{
		_proc_info._src_arr = &_buf_src_ptr_list [0];
	}
	_proc_info._dir_arr [mfx::piapi::Dir_IN ]._nbr_chn = nbr_chn_i;

	for (int chn = 0; chn < nbr_chn_o * nbr_o; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_dst_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._dst_arr = nullptr;
	if (nbr_chn_o > 0)
	{
		_proc_info._dst_arr = &_buf_dst_ptr_list [0];
	}
	_proc_info._dir_arr [mfx::piapi::Dir_OUT]._nbr_chn = nbr_chn_o;

	for (int chn = 0; chn < nbr_s; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_sig_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._sig_arr = (nbr_s <= 0) ? nullptr : &_buf_sig_ptr_list [0];

	latency = 0;

	return pi.reset (sample_freq, max_block_size, latency);
}



void	PiProc::set_param_nat (int index, double val_nat)
{
	assert (_desc_sptr.get () != nullptr);
	assert (_evt_list.size () == _evt_ptr_list.size ());
	assert (index >= 0);
	assert (index < _desc_sptr->get_nbr_param (mfx::piapi::ParamCateg_GLOBAL));
	const mfx::piapi::ParamDescInterface & desc_param =
		_desc_sptr->get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
	const double   val_nrm = desc_param.conv_nat_to_nrm (val_nat);

	mfx::piapi::EventTs  evt;
	evt._timestamp = 0;
	evt._type      = mfx::piapi::EventType_PARAM;
	evt._evt._param._categ = mfx::piapi::ParamCateg_GLOBAL;
	evt._evt._param._note_id = 0;
	evt._evt._param._index = index;
	evt._evt._param._val   = float (val_nrm);
	_evt_list.push_back (evt);
	_evt_ptr_list.clear ();
	for (const auto &evt_p : _evt_list)
	{
		_evt_ptr_list.push_back (&evt_p);
	}
	_proc_info._evt_arr = &_evt_ptr_list [0];
	_proc_info._nbr_evt = int (_evt_ptr_list.size ());
}



void	PiProc::reset_param ()
{
	_evt_list.clear ();
	_evt_ptr_list.clear ();
	_proc_info._evt_arr = nullptr;
	_proc_info._nbr_evt = 0;
}



mfx::piapi::ProcInfo &	PiProc::use_proc_info ()
{
	return _proc_info;
}



float * const *	PiProc::use_buf_list_src () const
{
	return &_buf_src_ptr_list [0];
}



float * const *	PiProc::use_buf_list_dst () const
{
	return &_buf_dst_ptr_list [0];
}



float * const *	PiProc::use_buf_list_sig () const
{
	return (_buf_sig_ptr_list.empty ()) ? nullptr : &_buf_sig_ptr_list [0];
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
