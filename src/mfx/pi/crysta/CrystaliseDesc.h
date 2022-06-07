/*****************************************************************************

        CrystaliseDesc.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_crysta_CrystaliseDesc_HEADER_INCLUDED)
#define mfx_pi_crysta_CrystaliseDesc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamDescSet.h"
#include "mfx/piapi/PluginDescInterface.h"



namespace mfx
{
namespace pi
{
namespace crysta
{



class CrystaliseDesc final
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       CrystaliseDesc ();
	               ~CrystaliseDesc () = default;

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

	               CrystaliseDesc (const CrystaliseDesc &other)    = delete;
	               CrystaliseDesc (CrystaliseDesc &&other)         = delete;
	CrystaliseDesc &
	               operator = (const CrystaliseDesc &other)        = delete;
	CrystaliseDesc &
	               operator = (CrystaliseDesc &&other)             = delete;
	bool           operator == (const CrystaliseDesc &other) const = delete;
	bool           operator != (const CrystaliseDesc &other) const = delete;

}; // class CrystaliseDesc



}  // namespace crysta
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/crysta/CrystaliseDesc.hpp"



#endif   // mfx_pi_crysta_CrystaliseDesc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
