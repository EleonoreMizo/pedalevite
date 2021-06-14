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

#include "fstb/fnc.h"
#include "mfx/doc/PluginSettings.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <algorithm>
#include <tuple>

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	PluginSettings::operator == (const PluginSettings &other) const
{
	return (
		   _force_mono_flag  == other._force_mono_flag
		&& _force_reset_flag == other._force_reset_flag
		&& _param_list       == other._param_list
		&& _map_param_ctrl   == other._map_param_ctrl
		&& _map_param_pres   == other._map_param_pres
	);
}



bool	PluginSettings::operator != (const PluginSettings &other) const
{
	return ! (*this == other);
}



bool	PluginSettings::is_similar (const PluginSettings &other) const
{
	const float    tol = 1e-5f;

	bool           same_flag = true;

	same_flag &= (_force_mono_flag  == other._force_mono_flag );
	same_flag &= (_force_reset_flag == other._force_reset_flag);

	// Parameters
	const size_t   nbr_p = _param_list.size ();
	same_flag &= (nbr_p == other._param_list.size ());
	for (size_t index = 0; same_flag && index < nbr_p; ++index)
	{
		same_flag = fstb::is_eq (
			_param_list [index],
			other._param_list [index],
			tol
		);
	}

	// Controllers
	same_flag &= (_map_param_ctrl.size () == other._map_param_ctrl.size ());
	if (same_flag)
	{
		auto           it_1 = _map_param_ctrl.begin ();
		auto           it_2 = other._map_param_ctrl.begin ();
		while (same_flag && it_1 != _map_param_ctrl.end ())
		{
			same_flag = (
				   it_1->first == it_2->first
				&& it_1->second.is_similar (it_2->second)
			);

			++ it_1;
			++ it_2;
		}
	}

	// Presentations
	same_flag &= (_map_param_pres.size () == other._map_param_pres.size ());
	if (same_flag)
	{
		auto           it_1 = _map_param_pres.begin ();
		auto           it_2 = other._map_param_pres.begin ();
		while (same_flag && it_1 != _map_param_pres.end ())
		{
			same_flag = (
				   it_1->first == it_2->first
				&& it_1->second.is_similar (it_2->second)
			);

			++ it_1;
			++ it_2;
		}
	}

	return same_flag;
}



CtrlLinkSet &	PluginSettings::use_ctrl_link_set (int index)
{
	assert (index >= 0);

	auto           it = _map_param_ctrl.find (index);
	assert (it != _map_param_ctrl.end ());

	return it->second;
}



const CtrlLinkSet &	PluginSettings::use_ctrl_link_set (int index) const
{
	assert (index >= 0);

	auto           it = _map_param_ctrl.find (index);
	assert (it != _map_param_ctrl.end ());

	return it->second;
}



bool	PluginSettings::has_ctrl (int index) const
{
	assert (index >= 0);

	auto           it = _map_param_ctrl.find (index);

	return (it != _map_param_ctrl.end () && ! it->second.is_empty ());
}



bool	PluginSettings::has_any_ctrl () const
{
	for (auto & node : _map_param_ctrl)
	{
		if (! node.second.is_empty ())
		{
			return true;
		}
	}

	return false;
}



// Returns 0 if not found
const std::set <float> *	PluginSettings::find_notch_list (int index) const
{
	assert (index >= 0);
	assert (index < int (_param_list.size ()));

	const std::set <float> *   notch_list_ptr = nullptr;

	auto           it_cls = _map_param_ctrl.find (index);
	if (it_cls != _map_param_ctrl.end ())
	{
		const doc::CtrlLinkSet &   cls = it_cls->second;
		for (auto it_cl = cls._mod_arr.rbegin ()
		;	it_cl != cls._mod_arr.rend () && notch_list_ptr == nullptr
		;	++it_cl)
		{
			if (! (*it_cl)->_notch_list.empty ())
			{
				notch_list_ptr = &(*it_cl)->_notch_list;
			}
		}
		if (   notch_list_ptr == nullptr
		    && cls._bind_sptr.get () != nullptr
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

	const ParamPresentation * pres_ptr = nullptr;

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

	ParamPresentation * pres_ptr = nullptr;

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



void	PluginSettings::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_force_mono_flag);
	ser.write (_force_reset_flag);

	ser.begin_list ();
	for (auto &v : _param_list)
	{
		ser.write (v);
	}
	ser.end_list ();

	ser.begin_list ();
	for (const auto &sp : _map_param_ctrl)
	{
		if (! sp.second.is_empty ())
		{
			ser.begin_list ();

			ser.write (sp.first);
			sp.second.ser_write (ser);

			ser.end_list ();
		}
	}
	ser.end_list ();

	ser.begin_list ();
	for (const auto &sp : _map_param_pres)
	{
		ser.begin_list ();

		ser.write (sp.first);
		sp.second.ser_write (ser);

		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();
}



void	PluginSettings::ser_read (SerRInterface &ser, std::string model_id)
{
	ser.begin_list ();

	ser.read (_force_mono_flag);
	ser.read (_force_reset_flag);

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_param_list.resize (nbr_elt);
	for (auto &v : _param_list)
	{
		ser.read (v);

		// Rounds to 0 or 1 very small values, probably resulting from the
		// accumulation of rounding errors.
		if (fabsf (v) <= 1e-7f)
		{
			v = 0;
		}
		else if (fabsf (1 - v) <= 1e-7f)
		{
			v = 1;
		}
	}
	ser.end_list ();

	ser.begin_list (nbr_elt);
	_map_param_ctrl.clear ();
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		ser.begin_list ();

		int            index;
		ser.read (index);
		_map_param_ctrl [index].ser_read (ser);

		ser.end_list ();
	}
	ser.end_list ();

	ser.begin_list (nbr_elt);
	_map_param_pres.clear ();
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		ser.begin_list ();

		int            index;
		ser.read (index);
		_map_param_pres [index].ser_read (ser);

		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();

	// Version adaptation
	const int      version = ser.get_doc_version ();
	if (model_id == "ramp")
	{
		if (version < 7)
		{
			assert (_param_list.size () == 7);
			_param_list.push_back (0); // Initial delay
			_param_list.push_back (0); // State
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	operator < (const PluginSettings &lhs, const PluginSettings &rhs)
{
	return (std::tie (
		lhs._force_mono_flag,
		lhs._force_reset_flag,
		lhs._param_list,
		lhs._map_param_ctrl,
		lhs._map_param_pres
	) < std::tie (
		rhs._force_mono_flag,
		rhs._force_reset_flag,
		rhs._param_list,
		rhs._map_param_ctrl,
		rhs._map_param_pres
	));
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
