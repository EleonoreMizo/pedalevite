/*****************************************************************************

        TplMid.cpp
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
#include "mfx/pi/param/Tools.h"
#include "mfx/pi/param/TplMid.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// name_0 = printf format string, with group_index as optional argument (can
// be used up to 8 times)
TplMid::TplMid (double val_min, double val_max, double val_mid, const char *name_0, const char *unit_0, int group_index, const char *format_0)
:	_group_index (group_index)
,	_unit (unit_0)
,	_k ((val_mid - val_min) / (val_max - val_mid))
,	_vmin (val_min)
,	_vtopf (val_max * _k - val_min)
,	_vbotf (_k - 1)
{
	assert (val_min < val_max);
	assert (val_min < val_mid);
	assert (val_mid < val_max);
	assert (name_0 != nullptr);
	assert (unit_0 != nullptr);
	assert (format_0 != nullptr);

	_phdn.set_range (val_min, val_max);
	_phdn.set_print_format (format_0);
	_phdn.set_preset (HelperDispNum::Preset_FLOAT_STD);

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), name_0,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index
	);
	_name = txt_0;
}



void	TplMid::set_categ (Categ categ)
{
	assert (categ >= 0);
	assert (categ < Categ_NBR_ELT);

	_categ = categ;
}



HelperDispNum &	TplMid::use_disp_num ()
{
	return _phdn;
}



const HelperDispNum &	TplMid::use_disp_num () const
{
	return _phdn;
}



void	TplMid::set_flags (int32_t flags)
{
	_flags = flags;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	TplMid::do_get_name (int len) const
{
	if (len == 0)
	{
		return _name;
	}

	return Tools::print_name_bestfit (len, _name.c_str ());
}



std::string	TplMid::do_get_unit (int len) const
{
	if (len == 0)
	{
		return _unit;
	}

	return Tools::print_name_bestfit (len, _unit.c_str ());
}



piapi::ParamDescInterface::Range  TplMid::do_get_range () const
{
	return piapi::ParamDescInterface::Range_CONTINUOUS;
}



piapi::ParamDescInterface::Categ  TplMid::do_get_categ () const
{
	return _categ;
}



int32_t	TplMid::do_get_flags () const
{
	return _flags;
}



double	TplMid::do_get_nat_min () const
{
	return _phdn.get_val_min ();
}



double	TplMid::do_get_nat_max () const
{
	return _phdn.get_val_max ();
}



std::string	TplMid::do_conv_nat_to_str (double nat, int len) const
{
	const int      max_len = 1024;
	char           txt_0 [max_len+1];
	len = (len > 0) ? std::min (len, max_len) : max_len;
	_phdn.conv_to_str (nat, txt_0, len);

	return txt_0;
}



bool	TplMid::do_conv_str_to_nat (double &nat, const std::string &txt) const
{
	const int      ret_val = _phdn.conv_from_str (txt.c_str (), nat);

	return (ret_val == HelperDispNum::Err_OK);
}



double	TplMid::do_conv_nrm_to_nat (double nrm) const
{
	double         nat = (_vmin + nrm * _vtopf) / (1 + nrm * _vbotf);
	const double   a   = _phdn.get_val_min ();
	const double   b   = _phdn.get_val_max ();
	nat = fstb::limit (nat, a, b);

	return nat;
}



double	TplMid::do_conv_nat_to_nrm (double nat) const
{
	double         nrm = (nat - _vmin) / (_vtopf - nat * _vbotf);
	nrm = fstb::limit (nrm, 0.0, 1.0);

	return nrm;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
