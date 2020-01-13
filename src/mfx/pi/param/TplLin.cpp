/*****************************************************************************

        TplLin.cpp
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
#include "mfx/pi/param/Tools.h"
#include "mfx/pi/param/TplLin.h"

#include <algorithm>

#include	<cassert>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// name_0 = printf format string, with group_index as optional argument (can
// be used up to 8 times)
TplLin::TplLin (double val_min, double val_max, const char *name_0, const char *unit_0, int group_index, const char *format_0)
:	_phdn ()
,	_group_index (group_index)
,	_name ()
,	_unit (unit_0)
,	_categ (piapi::ParamDescInterface::Categ_UNDEFINED)
,	_flags (0)
{
	assert (val_min < val_max);
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



void	TplLin::set_categ (Categ categ)
{
	assert (categ >= 0);
	assert (categ < Categ_NBR_ELT);

	_categ = categ;
}



HelperDispNum &	TplLin::use_disp_num ()
{
	return (_phdn);
}



const HelperDispNum &	TplLin::use_disp_num () const
{
	return (_phdn);
}



void	TplLin::set_flags (int32_t flags)
{
	_flags = flags;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	TplLin::do_get_name (int len) const
{
	if (len == 0)
	{
		return _name;
	}

	return (Tools::print_name_bestfit (len, _name.c_str ()));
}



std::string	TplLin::do_get_unit (int len) const
{
	if (len == 0)
	{
		return _unit;
	}

	return (Tools::print_name_bestfit (len, _unit.c_str ()));
}



piapi::ParamDescInterface::Range  TplLin::do_get_range () const
{
	return (piapi::ParamDescInterface::Range_CONTINUOUS);
}



piapi::ParamDescInterface::Categ  TplLin::do_get_categ () const
{
	return _categ;
}



int32_t	TplLin::do_get_flags () const
{
	return _flags;
}



double	TplLin::do_get_nat_min () const
{
	return (_phdn.get_val_min ());
}



double	TplLin::do_get_nat_max () const
{
	return (_phdn.get_val_max ());
}



std::string	TplLin::do_conv_nat_to_str (double nat, int len) const
{
	const int      max_len = 1024;
	char           txt_0 [max_len+1];
	len = (len > 0) ? std::min (len, max_len) : max_len;
	_phdn.conv_to_str (nat, txt_0, len);

	return (txt_0);
}



bool	TplLin::do_conv_str_to_nat (double &nat, const std::string &txt) const
{
	const int      ret_val = _phdn.conv_from_str (txt.c_str (), nat);

	return (ret_val == HelperDispNum::Err_OK);
}



double	TplLin::do_conv_nrm_to_nat (double nrm) const
{
	const double   val_min = _phdn.get_val_min ();
	const double   val_max = _phdn.get_val_max ();

	const double   nat = val_min + nrm * (val_max - val_min);

	return (nat);
}



double	TplLin::do_conv_nat_to_nrm (double nat) const
{
	const double   val_min = _phdn.get_val_min ();
	const double   val_max = _phdn.get_val_max ();

	const double   nrm = (nat - val_min) / (val_max - val_min);

	return (nrm);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
