/*****************************************************************************

        VolumeCloneDesc.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_vclone_VolumeCloneDesc_HEADER_INCLUDED)
#define mfx_pi_vclone_VolumeCloneDesc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamDescSet.h"
#include "mfx/piapi/PluginDescInterface.h"



namespace mfx
{
namespace pi
{
namespace vclone
{



class VolumeCloneDesc final
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       VolumeCloneDesc ();
	               ~VolumeCloneDesc () = default;

	ParamDescSet & use_desc_set ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginDescInterface
	piapi::PluginInfo
	               do_get_info () const final;
	void           do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const final;
	int            do_get_nbr_param (piapi::ParamCateg categ) const final;
	const piapi::ParamDescInterface &
	               do_get_param_info (piapi::ParamCateg categ, int index) const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ParamDescSet   _desc_set;
	piapi::PluginInfo
	               _info;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               VolumeCloneDesc (const VolumeCloneDesc &other)          = delete;
	               VolumeCloneDesc (VolumeCloneDesc &&other)               = delete;
	VolumeCloneDesc &     operator = (const VolumeCloneDesc &other)        = delete;
	VolumeCloneDesc &     operator = (VolumeCloneDesc &&other)             = delete;
	bool           operator == (const VolumeCloneDesc &other) const = delete;
	bool           operator != (const VolumeCloneDesc &other) const = delete;

}; // class VolumeCloneDesc



}  // namespace vclone
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/vclone/VolumeCloneDesc.hpp"



#endif   // mfx_pi_vclone_VolumeCloneDesc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
