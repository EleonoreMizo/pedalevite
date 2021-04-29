/*****************************************************************************

        TplEnumEvol.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/pi/param/Tools.h"
#include "mfx/pi/param/TplEnumEvol.h"
#include	"fstb/txt/utf8/ConvNum.h"
#include	"fstb/txt/utf8/fnc.h"
#include "fstb/fnc.h"
#include	"fstb/Err.h"

#include <algorithm>

#include	<cassert>
#include	<cstdio>
#include	<cstring>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// name_0 = printf format string, with group_index as optional argument (can
// be used up to 8 times)
TplEnumEvol::TplEnumEvol (int nbr_splits, const char *val_list_0, const char *name_0, const char *unit_0, int group_index, const char *format_0)
:	_group_index (group_index)
,	_name_list ()
,	_name ()
,	_unit (unit_0)
,	_print_format (format_0)
,	_flags (0)
,	_mult (1)
,	_flt_int_list ()
,	_int_flt_list ()
{
	assert (nbr_splits >= 2);
	assert (val_list_0 != nullptr);
	assert (name_0 != nullptr);
	assert (unit_0 != nullptr);
	assert (format_0 != nullptr);

	const char *	eol_0 = val_list_0;
	do
	{
		const char *	beg_0 = eol_0;
		eol_0 = strchr (beg_0, '\n');
		size_t			len   = 0;
		if (eol_0 == nullptr)
		{
			len = strlen (beg_0);
		}
		else
		{
			len = eol_0 - beg_0;
			++ eol_0;
		}

		const std::string	val (beg_0, len);
		_name_list.push_back (val);
	}
	while (eol_0 != nullptr);

	build_table (nbr_splits, int (_name_list.size ()));

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), name_0,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index
	);
	_name = txt_0;
}



void	TplEnumEvol::set_flags (int32_t flags)
{
	_flags = flags;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	TplEnumEvol::do_get_name (int len) const
{
	if (len == 0)
	{
		return _name;
	}

	return (Tools::print_name_bestfit (len, _name.c_str ()));
}



std::string	TplEnumEvol::do_get_unit (int len) const
{
	if (len == 0)
	{
		return _unit;
	}

	return (Tools::print_name_bestfit (len, _unit.c_str ()));
}



piapi::ParamDescInterface::Range	TplEnumEvol::do_get_range () const
{
	return (piapi::ParamDescInterface::Range_DISCRETE);
}



piapi::ParamDescInterface::Categ	TplEnumEvol::do_get_categ () const
{
	return (piapi::ParamDescInterface::Categ_UNDEFINED);
}



int32_t	TplEnumEvol::do_get_flags () const
{
	return _flags;
}



double	TplEnumEvol::do_get_nat_min () const
{
	return 0;
}



double	TplEnumEvol::do_get_nat_max () const
{
	return (double (_name_list.size () - 1));
}



std::string	TplEnumEvol::do_conv_nat_to_str (double nat, int len) const
{
	const int      max_len = 1024;
	char           txt_0 [max_len+1];
	const int      index = fstb::round_int (nat);
	len = (len > 0) ? std::min (len, max_len) : max_len;

	fstb::snprintf4all (
		txt_0,
		len + 1,
		_print_format.c_str (),
		_name_list [index].c_str ()
	);

	return (txt_0);
}



bool	TplEnumEvol::do_conv_str_to_nat (double &nat, const std::string &txt) const
{
	bool           ok_flag = true;

	const long     nbr_str    = long (_name_list.size ());
	bool           found_flag = false;
	for (long index = 0
	;	index < nbr_str && ! found_flag && ok_flag
	;	++index)
	{
		const std::string &  str = _name_list [index];
		long           pos;
		int            loc_ret_val =
			fstb::txt::utf8::strstr_ci (txt.c_str (), str.c_str (), pos);
		ok_flag = (loc_ret_val == fstb::Err_OK);
		if (ok_flag && pos >= 0)
		{
			nat = index;
			assert (nat >= do_get_nat_min ());
			assert (nat <= do_get_nat_max ());
			found_flag = true;
		}
	}

	if (ok_flag && ! found_flag)
	{
		int64_t        index;
		if (fstb::txt::utf8::ConvNum::conv_str_to_int64 (
			index, txt.c_str (), 10, nullptr
		))
		{
			nat = double (index);
			if (   nat >= do_get_nat_min ()
			    && nat <= do_get_nat_max ())
			{
				found_flag = true;
			}
		}
	}

	if (ok_flag && ! found_flag)
	{
		ok_flag = false;
	}

	return (ok_flag);
}



double	TplEnumEvol::do_conv_nrm_to_nat (double nrm) const
{
	const int      pos   = fstb::round_int (nrm * _mult);
	assert (pos >= 0);
	assert (pos <= _mult);

	return _flt_int_list [pos];
}



double	TplEnumEvol::do_conv_nat_to_nrm (double nat) const
{
	const int      index = fstb::round_int (nat);
	assert (index >= 0);
	assert (index < int (_name_list.size ()));

	return _int_flt_list [index];
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TplEnumEvol::build_table (int nbr_splits, int nbr_elt)
{
	assert (nbr_elt > 0);
	assert (nbr_splits >= 2);

	const int		nbr_intervals    = nbr_splits - 1;
	const int		max_layer_val    = (nbr_elt - 2) / nbr_intervals;
	int            higher_layer     = 0;
	int            total_split_size = nbr_intervals;
	int            nbr_subintervals = 1;
	if (max_layer_val > 0)
	{
		higher_layer     = fstb::get_prev_pow_2 (max_layer_val);
		total_split_size = nbr_intervals << (higher_layer + 1);
		nbr_subintervals = 2 << higher_layer;
	}

	_flt_int_list.resize (total_split_size + 1);
	_int_flt_list.resize (nbr_elt);
	_mult = total_split_size;

	const double   mult_inv = 1.0 / double (total_split_size);
	for (int pos = 0; pos < total_split_size + 1; ++pos)
	{
		const int      index = conv_flt_to_int (
			double (pos) * mult_inv,
			nbr_splits,
			nbr_elt,
			higher_layer,
			total_split_size,
			nbr_subintervals
		);
		_flt_int_list [pos] = index;
	}

	for (int index = 0; index < nbr_elt; ++index)
	{
		const double   val = conv_int_to_flt (index, nbr_splits);
		_int_flt_list [index] = val;
	}
}



double	TplEnumEvol::conv_int_to_flt (int val, int nbr_splits) const
{
	assert (val >= 0);
	assert (val < int (_name_list.size ()));
	assert (nbr_splits >= 2);

	double         result = 0;
	const int      nbr_intervals = nbr_splits - 1;

	// Base split
	if (val < nbr_splits)
	{
		result = double (val) / nbr_intervals;
	}

	// Other splits
	else
	{
		const int      layer      =
			fstb::get_prev_pow_2 ((val - 1) / nbr_intervals);
		const int      split_size = nbr_intervals << layer;
		const int      offset     = split_size;
		result = (double (val - offset) - 0.5) / split_size;
	}

	assert (result >= 0);
	assert (result <= 1);

	return (result);
}



int	TplEnumEvol::conv_flt_to_int (double val, int nbr_splits, int nbr_val, int higher_layer, int total_split_size, int nbr_subintervals) const
{
	assert (val >= 0);
	assert (val <= 1);
	assert (nbr_splits >= 2);

	const int		pos = fstb::round_int (val * total_split_size);
	const int		subdiv = pos / nbr_subintervals;
	const int		subpos = pos % nbr_subintervals;
	int				result = 0;

	// Base split
	if (subpos == 0)
	{
		result = subdiv;
	}

	// Other splits
	else
	{
		assert (higher_layer >= 0);

		// Finds layer. It is given by the smallest non-zero bit
		int				layer    = higher_layer;
		int				bit_test = 1;
		while ((subpos & bit_test) == 0)
		{
			bit_test <<= 1;
			-- layer;
		}
		assert (layer >= 0);

		// Value computation
		const int		nbr_intervals = nbr_splits - 1;
		int		      offset        = (nbr_intervals + subdiv) << layer;
		int		      pos_in_layer  = subpos >> (higher_layer + 1 - layer);
		result = offset + pos_in_layer + 1;

		// Checks if we are on a partially filled layer
		if (result >= nbr_val)
		{
			if (layer == 0)
			{
				if (result >= nbr_intervals)
				{
					result -= nbr_intervals;
				}
			}
			else
			{
				-- layer;
				offset        = (nbr_intervals + subdiv) << layer;
				pos_in_layer  = subpos >> (higher_layer + 1 - layer);
				result = offset + pos_in_layer + 1;
			}
		}
	}

	// Range check, final
	result = std::min (result, nbr_val - 1);

	assert (result >= 0);
	assert (result < nbr_val);

	return (result);
}



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
