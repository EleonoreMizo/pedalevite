/*****************************************************************************

        Synth0Desc.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/pi/syn0/Synth0Desc.h"
#include "mfx/pi/syn0/Param.h"
#include "mfx/pi/param/TplLin.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace syn0
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Synth0Desc::Synth0Desc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "syn0";
	_info._name      = "Synth Zero\nSynth 0\nSyn 0";

	// Pitch
	auto           lin_sptr = std::make_shared <param::TplLin> (
		-3, 9,
		"Pitch\nP",
		"Oct",
		0,
		"%+5.2f"
	);
	_desc_set.add_glob (Param_PITCH, lin_sptr);

	// Trigger On
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Trigger on\nTrig on\nOn",
		"%",
		0,
		"%+6.1f"
	);
	_desc_set.add_glob (Param_TRIG_ON, lin_sptr);

	// Trigger Off
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Trigger off\nTrig off\nOff",
		"%",
		0,
		"%+6.1f"
	);
	_desc_set.add_glob (Param_TRIG_OFF, lin_sptr);
}



ParamDescSet &	Synth0Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	Synth0Desc::do_get_info () const
{
	return _info;
}



void	Synth0Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	Synth0Desc::do_prefer_stereo () const
{
	return false;
}



int	Synth0Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Synth0Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace syn0
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
