/*****************************************************************************

        TunerDesc.cpp
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

#include "fstb/def.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/tuner/TunerDesc.h"
#include "mfx/Cst.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace tuner
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TunerDesc::TunerDesc ()
:	_desc_set (0, 0)
,	_info ()
{
	_info._unique_id = Cst::_plugin_tuner;
	_info._name      = "Tuner";
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	TunerDesc::do_get_info () const
{
	return _info;
}



void	TunerDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	TunerDesc::do_prefer_stereo () const
{
	return false;
}



int	TunerDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	fstb::unused (categ);

	return 0;
}



const piapi::ParamDescInterface &	TunerDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	fstb::unused (categ, index);

	static const param::TplLin dummy (0, 1, "", "");

	return dummy;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace tuner
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
