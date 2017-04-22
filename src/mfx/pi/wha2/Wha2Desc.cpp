/*****************************************************************************

        Wha2Desc.cpp
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

#include "mfx/pi/wha2/Wha2Desc.h"
#include "mfx/pi/wha2/Param.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace wha2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Wha2Desc::Wha2Desc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Position
	param::Simple *   sim_ptr = new param::Simple ("Position\nPos");
	sim_ptr->set_flags (piapi::ParamDescInterface::Flags_AUTOLINK);
	_desc_set.add_glob (Param_POS, sim_ptr);

	// Type
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"GCB-95\nV-847\nOriginal\nMcCoy\nVocal\nExtreme",
		"Type",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_TYPE, enu_ptr);
}



ParamDescSet &	Wha2Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	Wha2Desc::do_get_unique_id () const
{
	return "wah2";
}



std::string	Wha2Desc::do_get_name () const
{
	return "Wah-wah (CryBaby)";
}



void	Wha2Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	Wha2Desc::do_prefer_stereo () const
{
	return false;
}



int	Wha2Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Wha2Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wha2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
