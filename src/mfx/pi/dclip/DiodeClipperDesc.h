/*****************************************************************************

        DiodeClipperDesc.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dclip_DiodeClipperDesc_HEADER_INCLUDED)
#define mfx_pi_dclip_DiodeClipperDesc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamDescSet.h"
#include "mfx/piapi/PluginDescInterface.h"



namespace mfx
{
namespace pi
{
namespace dclip
{



class DiodeClipperDesc final
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       DiodeClipperDesc ();
	               ~DiodeClipperDesc () = default;

	ParamDescSet & use_desc_set ();

	static const double  _gain_min;
	static const double  _gain_max;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginDescInterface
	piapi::PluginInfo
	               do_get_info () const final;
	void           do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const final;
	bool           do_prefer_stereo () const final;
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

	               DiodeClipperDesc (const DiodeClipperDesc &other)          = delete;
	               DiodeClipperDesc (DiodeClipperDesc &&other)               = delete;
	DiodeClipperDesc &     operator = (const DiodeClipperDesc &other)        = delete;
	DiodeClipperDesc &     operator = (DiodeClipperDesc &&other)             = delete;
	bool           operator == (const DiodeClipperDesc &other) const = delete;
	bool           operator != (const DiodeClipperDesc &other) const = delete;

}; // class DiodeClipperDesc



}  // namespace dclip
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dclip/DiodeClipperDesc.hpp"



#endif   // mfx_pi_dclip_DiodeClipperDesc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
