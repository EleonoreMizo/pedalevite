/*****************************************************************************

        PluginDescInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_PluginDescInterface_HEADER_INCLUDED)
#define mfx_piapi_PluginDescInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/ParamCateg.h"

#include <string>



namespace mfx
{
namespace piapi
{



class ParamDescInterface;

class PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PluginDescInterface ()                        = default;
	               PluginDescInterface (const PluginDescInterface &other) = default;
	               PluginDescInterface (PluginDescInterface &&other)      = default;

	virtual        ~PluginDescInterface ()                       = default;

	virtual PluginDescInterface &
	               operator = (const PluginDescInterface &other) = default;
	virtual PluginDescInterface &
	               operator = (PluginDescInterface &&other)      = default;

	std::string    get_unique_id () const;
	std::string    get_name () const;

	void           get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const;
	bool           prefer_stereo () const;

	int            get_nbr_param (ParamCateg categ) const;
	const ParamDescInterface &
	               get_param_info (ParamCateg categ, int index) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual std::string
	               do_get_unique_id () const = 0;
	virtual std::string
	               do_get_name () const = 0;

	virtual void   do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const = 0;
	virtual bool   do_prefer_stereo () const = 0;

	virtual int    do_get_nbr_param (ParamCateg categ) const = 0;
	virtual const ParamDescInterface &
	               do_get_param_info (ParamCateg categ, int index) const = 0;



}; // class PluginDescInterface



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/PluginDescInterface.hpp"



#endif   // mfx_piapi_PluginDescInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
