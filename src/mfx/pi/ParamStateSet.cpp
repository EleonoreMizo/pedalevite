/*****************************************************************************

        ParamStateSet.cpp
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

#include "fstb/BitFieldSparseIterator.h"
#include "mfx/pi/ParamDescSet.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/ParamDescInterface.h"

#include <cassert>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamStateSet::init (piapi::ParamCateg categ, const ParamDescSet &param_desc_arr)
{
	assert (categ >= 0);
	assert (categ < piapi::ParamCateg_NBR_ELT);
	assert (&param_desc_arr != 0);

	_categ = categ;
	const int      nbr_param = param_desc_arr.get_nbr_param (categ);
	_state_arr.resize (nbr_param);
	_change_flag_arr.set_nbr_elt (nbr_param);

	for (int param_index = 0; param_index < nbr_param; ++param_index)
	{
		ParamState &   state = _state_arr [param_index];
		const piapi::ParamDescInterface &  desc =
			param_desc_arr.use_param (categ, param_index);
		state.set_desc (desc);
	}
}



void	ParamStateSet::set_sample_freq (double fs)
{
	assert (_categ >= 0);
	assert (fs > 0);

	const int      nbr_param = int (_state_arr.size ());
	for (int param_index = 0; param_index < nbr_param; ++param_index)
	{
		ParamState &   state = _state_arr [param_index];
		state.set_sample_freq (fs);
		state.clear_buffers ();
	}
	_change_flag_arr.fill ();
}



void	ParamStateSet::clear_buffers ()
{
	assert (_categ >= 0);

	const int      nbr_param = int (_state_arr.size ());
	for (int param_index = 0; param_index < nbr_param; ++param_index)
	{
		ParamState &   state = _state_arr [param_index];
		state.clear_buffers ();
	}
	_change_flag_arr.fill ();
}



void	ParamStateSet::set_ramp_time (int index, double t)
{
	assert (_categ >= 0);
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));
	assert (t >= 0);

	ParamState &   state = _state_arr [index];
	state.set_ramp_time (t);
}



void	ParamStateSet::set_val (int index, double val)
{
	assert (_categ >= 0);
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));
	assert (val >= 0);
	assert (val <= 1);

	ParamState &   state = _state_arr [index];
	state.set_val (val);
	_change_flag_arr.fill_bit (index);
}



void	ParamStateSet::set_val_nat (const ParamDescSet &param_desc_arr, int index, double val)
{
	assert (_categ >= 0);
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));

	const piapi::ParamDescInterface &   desc =
		param_desc_arr.use_param (_categ, index);
	assert (val >= desc.get_nat_min ());
	assert (val <= desc.get_nat_max ());

	const double   val_nrm = desc.conv_nat_to_nrm (val);
	set_val (index, val_nrm);
}



const ParamState &	ParamStateSet::use_state (int index) const
{
	assert (_categ >= 0);
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));

	return _state_arr [index];
}



void	ParamStateSet::process_block (int nbr_spl)
{
	assert (_categ >= 0);
	assert (nbr_spl > 0);

	fstb::BitFieldSparseIterator it (_change_flag_arr);
	for (it.start (); it.is_rem_elt (); it.iterate ())
	{
		const long     index = it.get_bit_index ();
		ParamState &   state = _state_arr [index];
		state.tick (nbr_spl);
		if (! state.is_ramping ())
		{
			_change_flag_arr.set_bit (index, false);
		}
	}
}



void	ParamStateSet::add_observer (int index, fstb::util::ObserverInterface &observer)
{
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));
	assert (&observer != 0);

	fstb::util::ObservableInterface &   notifier =
		_state_arr [index].use_notifier ();
	notifier.add_observer (observer);
}



double	ParamStateSet::get_val_beg_nat (int index) const
{
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));

	const ParamState &               param = _state_arr [index];
	const piapi::ParamDescInterface& desc  = param.use_desc ();
	const double   val_nrm = param.get_val_beg ();
	const double   val_nat = desc.conv_nrm_to_nat (val_nrm);

	return val_nat;
}



double	ParamStateSet::get_val_end_nat (int index) const
{
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));

	const ParamState &               param = _state_arr [index];
	const piapi::ParamDescInterface& desc  = param.use_desc ();
	const double   val_nrm = param.get_val_end ();
	const double   val_nat = desc.conv_nrm_to_nat (val_nrm);

	return val_nat;
}



double	ParamStateSet::get_val_tgt_nat (int index) const
{
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));

	const ParamState &               param = _state_arr [index];
	const piapi::ParamDescInterface& desc  = param.use_desc ();
	const double   val_nrm = param.get_val_tgt ();
	const double   val_nat = desc.conv_nrm_to_nat (val_nrm);

	return val_nat;
}



bool	ParamStateSet::check_change (int index)
{
	assert (index >= 0);
	assert (index < int (_state_arr.size ()));

	fstb::util::NotificationFlagInterface &   notif_flag =
		_state_arr [index].use_notif_flag ();
	return notif_flag (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
