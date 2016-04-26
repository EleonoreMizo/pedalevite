/*****************************************************************************

        ParamDescInterface.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_piapi_ParamDescInterface_CODEHEADER_INCLUDED)
#define	mfx_piapi_ParamDescInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace piapi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// len = 0: no length constraint
std::string	ParamDescInterface::get_name (int len) const
{
	assert (len >= 0);

	const auto      s = do_get_name (len);
	assert (len == 0 || s.length () <= size_t (len));

	return s;
}



// len = 0: no length constraint
std::string	ParamDescInterface::get_unit (int len) const
{
	assert (len >= 0);

	const auto      s = do_get_unit (len);
	assert (len == 0 || s.length () <= size_t (len));

	return s;
}



ParamDescInterface::Range	ParamDescInterface::get_range () const
{
	const auto     range = do_get_range ();
	assert (range >= 0);
	assert (range < Range_NBR_ELT);

	return range;
}



ParamDescInterface::Categ	ParamDescInterface::get_categ () const
{
	auto           categ = do_get_categ ();
	assert (categ >= 0);
	assert (categ < Categ_NBR_ELT);

	return categ;
}



// min <= max
double	ParamDescInterface::get_nat_min () const
{
	return do_get_nat_min ();
}



// min <= max
double	ParamDescInterface::get_nat_max () const
{
	return do_get_nat_max ();
}



std::string	ParamDescInterface::conv_nat_to_str (double nat, int len) const
{
	assert (nat >= do_get_nat_min ());
	assert (nat <= do_get_nat_max ());
	assert (len >= 0);

	const auto      s = do_conv_nat_to_str (nat, len);
	assert (len == 0 || s.length () <= size_t (len));

	return s;
}



// As output, nat always contains something valid, even if the conversion
// has failed.
bool	ParamDescInterface::conv_str_to_nat (double &nat, const std::string &txt) const
{
	bool           ok_flag = do_conv_str_to_nat (nat, txt);
	assert (nat >= do_get_nat_min ());
	assert (nat <= do_get_nat_max ());

	return ok_flag;
}



// All normalised values should produce a valid natural value
double	ParamDescInterface::conv_nrm_to_nat (double nrm) const
{
	assert (nrm >= 0);
	assert (nrm <= 1);

	const double   nat = do_conv_nrm_to_nat (nrm);
	assert (nat >= do_get_nat_min ());
	assert (nat <= do_get_nat_max ());

	return nat;
}



// All natural values within the min-max range should produce a valid
// normalised value
double	ParamDescInterface::conv_nat_to_nrm (double nat) const
{
	assert (nat >= do_get_nat_min ());
	assert (nat <= do_get_nat_max ());

	const double   nrm = do_conv_nat_to_nrm (nat);
	assert (nrm >= 0);
	assert (nrm <= 1);

	return nrm;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace piapi
}	// namespace mfx



#endif	// mfx_piapi_ParamDescInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
