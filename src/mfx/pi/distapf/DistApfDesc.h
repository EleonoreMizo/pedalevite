/*****************************************************************************

        DistApfDesc.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_distapf_DistApfDesc_HEADER_INCLUDED)
#define mfx_pi_distapf_DistApfDesc_HEADER_INCLUDED

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
namespace distapf
{



class DistApfDesc
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DistApfDesc ();
	virtual        ~DistApfDesc () = default;

	ParamDescSet & use_desc_set ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginDescInterface
	virtual std::string
	               do_get_unique_id () const;
	virtual std::string
	               do_get_name () const;
	virtual void   do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const;
	virtual bool   do_prefer_stereo () const;
	virtual int    do_get_nbr_param (piapi::ParamCateg categ) const;
	virtual const piapi::ParamDescInterface &
	               do_get_param_info (piapi::ParamCateg categ, int index) const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ParamDescSet   _desc_set;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistApfDesc (const DistApfDesc &other)       = delete;
	DistApfDesc &  operator = (const DistApfDesc &other)        = delete;
	bool           operator == (const DistApfDesc &other) const = delete;
	bool           operator != (const DistApfDesc &other) const = delete;

}; // class DistApfDesc



}  // namespace distapf
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distapf/DistApfDesc.hpp"



#endif   // mfx_pi_distapf_DistApfDesc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
