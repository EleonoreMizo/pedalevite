/*****************************************************************************

        BigMuff1Desc.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_bmp1_BigMuff1Desc_HEADER_INCLUDED)
#define mfx_pi_bmp1_BigMuff1Desc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamDescSet.h"
#include "mfx/piapi/PluginDescInterface.h"


namespace mfx
{
namespace pi
{
namespace bmp1
{



class BigMuff1Desc final
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       BigMuff1Desc ();
	               ~BigMuff1Desc () = default;

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

	               BigMuff1Desc (const BigMuff1Desc &other)      = delete;
	               BigMuff1Desc (BigMuff1Desc &&other)           = delete;
	BigMuff1Desc & operator = (const BigMuff1Desc &other)        = delete;
	BigMuff1Desc & operator = (BigMuff1Desc &&other)             = delete;
	bool           operator == (const BigMuff1Desc &other) const = delete;
	bool           operator != (const BigMuff1Desc &other) const = delete;

}; // class BigMuff1Desc



}  // namespace bmp1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/bmp1/BigMuff1Desc.hpp"



#endif   // mfx_pi_bmp1_BigMuff1Desc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
