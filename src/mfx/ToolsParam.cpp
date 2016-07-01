/*****************************************************************************

        ToolsParam.cpp
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
#include "mfx/doc/CtrlLinkSet.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/ToolsParam.h"

#include <algorithm>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Automatically clips to legal values
double	ToolsParam::conv_nrm_to_beats (double val_nrm, const piapi::ParamDescInterface &desc, double tempo)
{
	assert (val_nrm >= 0);
	assert (val_nrm <= 1);
	assert (desc.get_nat_min () >= 0);
	assert (tempo > 0);

	double         val_beats = 1;

	const piapi::ParamDescInterface::Categ categ = desc.get_categ ();
	const double   val_nat = desc.conv_nrm_to_nat (val_nrm);

	switch (categ)
	{
	case piapi::ParamDescInterface::Categ_TIME_S:
		val_beats = val_nat * tempo / 60;
		break;
	case piapi::ParamDescInterface::Categ_TIME_HZ:
	case piapi::ParamDescInterface::Categ_FREQ_HZ:
		val_beats = tempo / (std::max (val_nat, 1e-6) * 60);
		break;
	default:
		assert (false);
	}

	return val_beats;
}



// Automatically clips to legal values
double	ToolsParam::conv_beats_to_nrm (double val_beats, const piapi::ParamDescInterface &desc, double tempo)
{
	assert (val_beats >= 0);
	assert (desc.get_nat_min () >= 0);
	assert (tempo > 0);

	double         val_nat = 1;

	const piapi::ParamDescInterface::Categ categ = desc.get_categ ();
	switch (categ)
	{
	case piapi::ParamDescInterface::Categ_TIME_S:
		val_nat = val_beats * 60 / tempo;
		break;
	case piapi::ParamDescInterface::Categ_TIME_HZ:
	case piapi::ParamDescInterface::Categ_FREQ_HZ:
		val_nat = tempo / (std::max (val_beats, 1e-6) * 60);
		break;
	default:
		assert (false);
	}

	const double   nat_min = desc.get_nat_min ();
	const double   nat_max = desc.get_nat_max ();
	val_nat = fstb::limit (val_nat, nat_min, nat_max);

	const double   val_nrm = desc.conv_nat_to_nrm (val_nat);

	return val_nrm;
}



std::set <float>::const_iterator	ToolsParam::find_closest_notch (float val, const std::set <float> &notch_list)
{
	assert (! notch_list.empty ());

	std::set <float>::const_iterator it_found = notch_list.end ();

	auto           it_l = notch_list.lower_bound (val);
	if (it_l == notch_list.end ())
	{
		-- it_l;
		it_found = it_l;
	}
	else if (it_l == notch_list.begin ())
	{
		it_found = it_l;
	}
	else
	{
		assert (notch_list.size () >= 2);
		it_found = it_l;
		const float    v1 = *it_l;
		-- it_l;
		const float    v0 = *it_l;
		const float    d1 = fabs (val - v1);
		const float    d0 = fabs (val - v0);
		if (d0 < d1)
		{
			it_found = it_l;
		}
	}

	return it_found;
}



std::set <float>::const_iterator	ToolsParam::advance_to_notch (float val, const std::set <float> &notch_list, int dir)
{
	assert (! notch_list.empty ());

	auto           it_notch =
		find_closest_notch (val, notch_list);
	const float    val_n    = *it_notch;
	const bool     eq_flag  = fstb::is_eq_rel (val, val_n, 1e-3f);
	if (     dir > 0 && (val > val_n || eq_flag) && it_notch != --notch_list.end ())
	{
		++ it_notch;
	}
	else if (dir < 0 && (val < val_n || eq_flag) && it_notch != notch_list.begin ())
	{
		-- it_notch;
	}

	return it_notch;
}



// Parameter must be tempo-controlled.
// Does not replace an existing notch list
void	ToolsParam::add_beat_notch_list_if_linked (doc::CtrlLinkSet &cls, const piapi::ParamDescInterface &desc, double tempo)
{
	if (cls._bind_sptr.get () != 0 && cls._bind_sptr->_notch_list.empty ())
	{
		const double   nat_min = desc.get_nat_min ();
		const double   nat_max = desc.get_nat_max ();
		const double   nrm_min = desc.conv_nat_to_nrm (nat_min);
		const double   nrm_max = desc.conv_nat_to_nrm (nat_max);
		double         bt_min  = conv_nrm_to_beats (nrm_min, desc, tempo);
		double         bt_max  = conv_nrm_to_beats (nrm_max, desc, tempo);

		// Elements may be reversed for multiple reasons,
		// ensures we get a working order.
		if (bt_max < bt_min)
		{
			std::swap (bt_max, bt_min);
		}

		auto           it_beg = _beat_notch_list.lower_bound (float (bt_min));
		auto           it_end = _beat_notch_list.upper_bound (float (bt_max));
		for (auto it = it_beg; it != it_end; ++it)
		{
			const double   val_nrm = conv_beats_to_nrm (*it, desc, tempo);
			cls._bind_sptr->_notch_list.insert (float (val_nrm));
		}
	}
}



// Returns a set of notches for time pots, in beats
std::set <float>	ToolsParam::create_beat_notches ()
{
	std::set <float>  notches;

	for (float base = 1.0f / 256; base < 256; base *= 2)
	{
		notches.insert (base);
		notches.insert (base * (4.0f / 3));
		notches.insert (base * (3.0f / 2));
		notches.insert (base * (8.0f / 5));
	}

	return notches;
}



const std::set <float>	ToolsParam::_beat_notch_list =
	ToolsParam::create_beat_notches ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
