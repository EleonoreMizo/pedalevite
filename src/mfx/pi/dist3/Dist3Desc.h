/*****************************************************************************

        Dist3Desc.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist3_Dist3Desc_HEADER_INCLUDED)
#define mfx_pi_dist3_Dist3Desc_HEADER_INCLUDED

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
namespace dist3
{



class Dist3Desc final
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Class
	{
		Class_A = 0,
		Class_B,

		Class_NBR_ELT
	};

	enum BiasSrc
	{
		BiasSrc_DIRECT = 0,
		BiasSrc_FDBK,

		BiasSrc_NBR_ELT
	};

	explicit       Dist3Desc ();
	               ~Dist3Desc () = default;

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

	               Dist3Desc (const Dist3Desc &other)         = delete;
	               Dist3Desc (Dist3Desc &&other)              = delete;
	Dist3Desc &    operator = (const Dist3Desc &other)        = delete;
	Dist3Desc &    operator = (Dist3Desc &&other)             = delete;
	bool           operator == (const Dist3Desc &other) const = delete;
	bool           operator != (const Dist3Desc &other) const = delete;

}; // class Dist3Desc



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist3/Dist3Desc.hpp"



#endif   // mfx_pi_dist3_Dist3Desc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
