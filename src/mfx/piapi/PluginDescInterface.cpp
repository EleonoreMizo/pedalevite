/*****************************************************************************

        PluginDescInterface.cpp
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

#include "mfx/piapi/PluginDescInterface.h"

#include <cassert>



namespace mfx
{
namespace piapi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Internal plug-ins (not shown to the user) are prefixed with a '\?'
std::string	PluginDescInterface::get_unique_id () const
{
	std::string    uid = do_get_unique_id ();
	assert (! uid.empty ());

	return uid;
}



std::string	PluginDescInterface::get_name () const
{
	std::string    name = do_get_name ();
	assert (! name.empty ());

	return name;
}



void	PluginDescInterface::get_nbr_io (int &nbr_i, int &nbr_o) const
{
	assert (nbr_i == 1);
	assert (nbr_o == 1);

	do_get_nbr_io (nbr_i, nbr_o);
	assert (nbr_i > 0);
	assert (nbr_o > 0);
}



bool	PluginDescInterface::prefer_stereo () const
{
	return do_prefer_stereo ();
}



int	PluginDescInterface::get_nbr_param (ParamCateg categ) const
{
	assert (categ >= 0);
	assert (categ < ParamCateg_NBR_ELT);

	const int      nbr_param = do_get_nbr_param (categ);
	assert (nbr_param >= 0);

	return (nbr_param);
}




const ParamDescInterface &	PluginDescInterface::get_param_info (ParamCateg categ, int index) const
{
	assert (categ >= 0);
	assert (categ < ParamCateg_NBR_ELT);
	assert (index >= 0);
	assert (index < get_nbr_param (categ));

	const auto &   desc = do_get_param_info (categ, index);

	return (desc);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace piapi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
