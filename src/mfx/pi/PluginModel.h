/*****************************************************************************

        PluginModel.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_PluginModel_HEADER_INCLUDED)
#define mfx_pi_PluginModel_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



namespace mfx
{
namespace pi
{



enum PluginModel
{
	PluginModel_INVALID = -1,

	PluginModel_DRYWET  = 0,
	PluginModel_TUNER,

	PluginModel_DISTO_SIMPLE,
	PluginModel_TREMOLO,
	PluginModel_WHA,
	PluginModel_FREQ_SHIFT,

	PluginModel_NBR_ELT

}; // enum PluginModel



std::string PluginModel_get_name (PluginModel model);



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/PluginModel.hpp"



#endif   // mfx_pi_PluginModel_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
