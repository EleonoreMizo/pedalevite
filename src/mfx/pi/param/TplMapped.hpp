/*****************************************************************************

        TplMapped.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_TplMapped_CODEHEADER_INCLUDED)
#define mfx_pi_param_TplMapped_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/pi/param/Tools.h"

#include <algorithm>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// name_0 = printf format string, with group_index as optional argument (can
// be used up to 4 times)
template <class T>
TplMapped <T>::TplMapped (double val_min, double val_max, const char *name_0, const char *unit_0, HelperDispNum::Preset preset, int group_index, const char *format_0)
:	_mapper ()
,	_phdn ()
,	_group_index (group_index)
,	_name ()
,	_unit (unit_0)
,	_categ (piapi::ParamDescInterface::Categ_UNDEFINED)
,	_flags (0)
{
	assert (val_min < val_max);
	assert (name_0 != nullptr);
	assert (unit_0 != nullptr);
	assert (preset >= 0);
	assert (preset < HelperDispNum::Preset_NBR_ELT);
	assert (format_0 != nullptr);

	_phdn.set_range (val_min, val_max);
	_phdn.set_print_format (format_0);
	_phdn.set_preset (preset);

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), name_0,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index
	);
	_name = txt_0;

	_mapper.config (val_min, val_max);
}



template <class T>
void	TplMapped <T>::set_categ (Categ categ)
{
	assert (categ >= 0);
	assert (categ < Categ_NBR_ELT);

	_categ = categ;
}



template <class T>
typename TplMapped <T>::Mapper &	TplMapped <T>::use_mapper ()
{
	return (_mapper);
}



template <class T>
const typename TplMapped <T>::Mapper &	TplMapped <T>::use_mapper () const
{
	return (_mapper);
}



template <class T>
HelperDispNum &	TplMapped <T>::use_disp_num ()
{
	return (_phdn);
}



template <class T>
const HelperDispNum &	TplMapped <T>::use_disp_num () const
{
	return (_phdn);
}



template <class T>
void	TplMapped <T>::set_flags (int32_t flags)
{
	_flags = flags;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
std::string	TplMapped <T>::do_get_name (int len) const
{
	if (len == 0)
	{
		return _name;
	}

	return (Tools::print_name_bestfit (len, _name.c_str ()));
}



template <class T>
std::string	TplMapped <T>::do_get_unit (int len) const
{
	if (len == 0)
	{
		return _unit;
	}

	return (Tools::print_name_bestfit (len, _unit.c_str ()));
}



template <class T>
piapi::ParamDescInterface::Range  TplMapped <T>::do_get_range () const
{
	return (piapi::ParamDescInterface::Range_CONTINUOUS);
}



template <class T>
piapi::ParamDescInterface::Categ  TplMapped <T>::do_get_categ () const
{
	return _categ;
}



template <class T>
int32_t	TplMapped <T>::do_get_flags () const
{
	return _flags;
}



template <class T>
double	TplMapped <T>::do_get_nat_min () const
{
	return _phdn.get_val_min ();
}



template <class T>
double	TplMapped <T>::do_get_nat_max () const
{
	return _phdn.get_val_max ();
}



template <class T>
std::string	TplMapped <T>::do_conv_nat_to_str (double nat, int len) const
{
	const int      max_len = 1024;
	char           txt_0 [max_len+1];
	len = (len > 0) ? std::min (len, max_len) : max_len;
	_phdn.conv_to_str (nat, txt_0, len);

	return txt_0;
}



template <class T>
bool	TplMapped <T>::do_conv_str_to_nat (double &nat, const std::string &txt) const
{
	const int      ret_val = _phdn.conv_from_str (txt.c_str (), nat);

	return (ret_val == HelperDispNum::Err_OK);
}



template <class T>
double	TplMapped <T>::do_conv_nrm_to_nat (double nrm) const
{
	double         nat     = _mapper.conv_norm_to_nat (nrm);
	const double   val_min = _phdn.get_val_min ();
	const double   val_max = _phdn.get_val_max ();
#if ! defined (NDEBUG)
	const double   margin  = 1e-3;      // For rounding errors
	assert (nat >= val_min - margin);
	assert (nat <= val_max + margin);
#endif
	nat = fstb::limit (nat, val_min, val_max);

	return nat;
}



template <class T>
double	TplMapped <T>::do_conv_nat_to_nrm (double nat) const
{
	double         nrm    = _mapper.conv_nat_to_norm (nat);
#if ! defined (NDEBUG)
	const double   margin = 1e-6;       // For rounding errors
	assert (nrm >= 0.0 - margin);
	assert (nrm <= 1.0 + margin);
#endif
	nat = fstb::limit (nrm, 0.0, 1.0);

	return nrm;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_TplMapped_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
