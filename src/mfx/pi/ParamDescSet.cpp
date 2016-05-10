/*****************************************************************************

        ParamDescSet.cpp
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

#include "mfx/pi/ParamDescSet.h"

#include <cassert>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ParamDescSet::ParamDescSet (int nbr_glob, int nbr_notes)
:	_categ_arr ()
,	_nbr_rem_param (nbr_glob + nbr_notes)
{
	assert (nbr_glob >= 0);
	assert (nbr_notes >= 0);

	_categ_arr [piapi::ParamCateg_GLOBAL].resize (nbr_glob);
	_categ_arr [piapi::ParamCateg_NOTE  ].resize (nbr_notes);
}



void	ParamDescSet::add (piapi::ParamCateg categ, int index, piapi::ParamDescInterface *ptr)
{
	assert (categ >= 0);
	assert (categ < piapi::ParamCateg_NBR_ELT);
	assert (index >= 0);
	assert (index < get_nbr_param (categ));
	assert (ptr != 0);
	assert (_categ_arr [categ] [index].get () == 0);

	_categ_arr [categ] [index] = ParamSPtr (ptr);
}



void	ParamDescSet::add_glob (int index, piapi::ParamDescInterface *ptr)
{
	add (piapi::ParamCateg_GLOBAL, index, ptr);
}



void	ParamDescSet::add_note (int index, piapi::ParamDescInterface *ptr)
{
	add (piapi::ParamCateg_NOTE, index, ptr);
}



bool	ParamDescSet::is_ready () const
{
	return (_nbr_rem_param == 0);
}



int	ParamDescSet::get_nbr_param (piapi::ParamCateg categ) const
{
	assert (is_ready ());
	assert (categ >= 0);
	assert (categ < piapi::ParamCateg_NBR_ELT);

	return int (_categ_arr [categ].size ());
}



const piapi::ParamDescInterface &	ParamDescSet::use_param (piapi::ParamCateg categ, int index) const
{
	assert (is_ready ());
	assert (categ >= 0);
	assert (categ < piapi::ParamCateg_NBR_ELT);
	assert (index >= 0);
	assert (index < get_nbr_param (categ));

	return *(_categ_arr [categ] [index]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
