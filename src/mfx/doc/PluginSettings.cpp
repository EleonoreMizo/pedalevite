/*****************************************************************************

        PluginSettings.cpp
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

#include "mfx/doc/PluginSettings.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CtrlLinkSet &	PluginSettings::use_ctrl_link_set (int index)
{
	auto           it = _map_param_ctrl.find (index);
	assert (it != _map_param_ctrl.end ());

	return it->second;
}



const CtrlLinkSet &	PluginSettings::use_ctrl_link_set (int index) const
{
	auto           it = _map_param_ctrl.find (index);
	assert (it != _map_param_ctrl.end ());

	return it->second;
}



// Returns 0 if not found
const std::set <float> *	PluginSettings::find_notch_list (int index) const
{
	assert (index >= 0);
	assert (index < int (_param_list.size ()));

	const std::set <float> *   notch_list_ptr = 0;

	auto           it_cls = _map_param_ctrl.find (index);
	if (it_cls != _map_param_ctrl.end ())
	{
		const doc::CtrlLinkSet &   cls = it_cls->second;
		for (auto it_cl = cls._mod_arr.rbegin ()
		;	it_cl != cls._mod_arr.rend () && notch_list_ptr == 0
		;	++it_cl)
		{
			if (! (*it_cl)->_notch_list.empty ())
			{
				notch_list_ptr = &(*it_cl)->_notch_list;
			}
		}
		if (   notch_list_ptr == 0
		    && cls._bind_sptr.get () != 0
		    && ! cls._bind_sptr->_notch_list.empty ())
		{
			notch_list_ptr = &cls._bind_sptr->_notch_list;
		}
	}

	return notch_list_ptr;
}



const ParamPresentation *	PluginSettings::use_pres_if_tempo_ctrl (int index) const
{
	assert (index >= 0);
	assert (index < int (_param_list.size ()));

	const ParamPresentation * pres_ptr = 0;

	const auto     it_pres = _map_param_pres.find (index);
	if (it_pres != _map_param_pres.end ())
	{
		if (it_pres->second._ref_beats >= 0)
		{
			pres_ptr = &it_pres->second;
		}
	}

	return pres_ptr;
}



ParamPresentation *	PluginSettings::use_pres_if_tempo_ctrl (int index)
{
	assert (index >= 0);
	assert (index < int (_param_list.size ()));

	ParamPresentation * pres_ptr = 0;

	const auto     it_pres = _map_param_pres.find (index);
	if (it_pres != _map_param_pres.end ())
	{
		if (it_pres->second._ref_beats >= 0)
		{
			pres_ptr = &it_pres->second;
		}
	}

	return pres_ptr;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
