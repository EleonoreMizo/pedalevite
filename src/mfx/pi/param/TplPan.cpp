/*****************************************************************************

        TplPan.cpp
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

#include "fstb/txt/neutral/fnc.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/pi/param/TplPan.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TplPan::TplPan (const char *name_0, int group_index)
:	_group_index (group_index)
,	_name ()
,	_flags (0)
{
	assert (name_0 != 0);

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), name_0,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index,
		_group_index, _group_index, _group_index, _group_index
	);
	_name = txt_0;
}



void	TplPan::set_flags (int32_t flags)
{
	_flags = flags;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	TplPan::do_get_name (int len) const
{
	if (len == 0)
	{
		return _name;
	}

	return (Tools::print_name_bestfit (len, _name.c_str ()));
}



std::string	TplPan::do_get_unit (int len) const
{
	fstb::unused (len);

	return "%";
}



piapi::ParamDescInterface::Range	TplPan::do_get_range () const
{
	return (piapi::ParamDescInterface::Range_CONTINUOUS);
}



piapi::ParamDescInterface::Categ	TplPan::do_get_categ () const
{
	return piapi::ParamDescInterface::Categ_UNDEFINED;
}



int32_t	TplPan::do_get_flags () const
{
	return _flags;
}



double	TplPan::do_get_nat_min () const
{
	return -1;
}



double	TplPan::do_get_nat_max () const
{
	return 1;
}



std::string	TplPan::do_conv_nat_to_str (double nat, int len) const
{
	char           txt_0 [1023+1];
	size_t         buf_len (sizeof (txt_0));
	if (len > 0)
	{
		buf_len = std::min (size_t (len + 1), buf_len);
	}

	const int      pos = fstb::round_int (fabs (nat) * 100);
	if (pos == 0)
	{
		fstb::snprintf4all (txt_0, buf_len, "Mid");
	}
	else
	{
		const char *   d_0 = (nat < 0) ? "L" : "R";
		fstb::snprintf4all (txt_0, buf_len, "%s%3d", d_0, pos);
	}

	return txt_0;
}



bool	TplPan::do_conv_str_to_nat (double &nat, const std::string &txt) const
{
	bool           ok_flag = false;
	std::string    u (fstb::txt::neutral::trim_spaces (txt.c_str ()));
	if (! txt.empty ())
	{
		fstb::txt::neutral::to_lcase_inplace (u);
		const char     d = txt [0];
		if (d == 'l' || d == 'r' || isdigit (d) || d == '-')
		{
			const int      sgn = (d == 'l') ? -1 : 1;

			const char *   beg_0 = txt.c_str ();
			if (d == 'l' || d == 'r')
			{
				++ beg_0;
			}
			char *         end_0;
			double         v = strtod (beg_0, &end_0);
			if (end_0 > beg_0 && std::isfinite (v))
			{
				nat     = sgn * v;
				ok_flag = true;
			}
		}
		else if (txt == "mid")
		{
			nat     = 0;
			ok_flag = true;
		}
	}

	if (ok_flag)
	{
		nat = fstb::limit (nat, get_nat_min (), get_nat_max ());
	}

	return ok_flag;
}



double	TplPan::do_conv_nrm_to_nat (double nrm) const
{
	double         nat;

	if (nrm < 0.375f)
	{
		if (nrm < 1.0 / 6)
		{
			nat = nrm * 0.5f - 1;
		}
		else
		{
			nat = nrm * 2 - 1.25f;
		}
	}
	else if (nrm > 0.625f)
	{
		if (nrm > 5.0 / 6)
		{
			nat = nrm * 0.5f + 0.5f;
		}
		else
		{
			nat = nrm * 2 - 0.75f;
		}
	}
	else
	{
		nat = nrm * 4 - 2;
	}

	return nat;
}



double	TplPan::do_conv_nat_to_nrm (double nat) const
{
	double         nrm;

	if (nat < -0.5f)
	{
		if (nat < -11.0 / 12)
		{
			nrm = nat * 2 + 2;
		}
		else
		{
			nrm = nat * 0.5f + 0.625f;
		}
	}
	else if (nat > 0.5f)
	{
		if (nat > 11.0 / 12)
		{
			nrm = nat * 2 - 1;
		}
		else
		{
			nrm = nat * 0.5f + 0.375f;
		}
	}
	else
	{
		nrm = nat * 0.25f + 0.5f;
	}

	return nrm;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
