/*****************************************************************************

        ToStereoDesc.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_tost_ToStereoDesc_HEADER_INCLUDED)
#define mfx_pi_tost_ToStereoDesc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamDescSet.h"
#include "mfx/piapi/PluginDescInterface.h"



namespace mfx
{
namespace pi
{
namespace tost
{



class ToStereoDesc final
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ToStereoDesc ();

	ParamDescSet & use_desc_set ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginDescInterface
	std::string    do_get_unique_id () const final;
	std::string    do_get_name () const final;
	void           do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const final;
	bool           do_prefer_stereo () const final;
	int            do_get_nbr_param (piapi::ParamCateg categ) const final;
	const piapi::ParamDescInterface &
	               do_get_param_info (piapi::ParamCateg categ, int index) const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ParamDescSet   _desc_set;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ToStereoDesc (const ToStereoDesc &other)      = delete;
	ToStereoDesc & operator = (const ToStereoDesc &other)        = delete;
	bool           operator == (const ToStereoDesc &other) const = delete;
	bool           operator != (const ToStereoDesc &other) const = delete;

}; // class ToStereoDesc



}  // namespace tost
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/tost/ToStereoDesc.hpp"



#endif   // mfx_pi_tost_ToStereoDesc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
