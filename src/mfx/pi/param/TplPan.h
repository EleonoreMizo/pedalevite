/*****************************************************************************

        TplPan.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplPan_HEADER_INCLUDED)
#define mfx_pi_param_TplPan_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/ParamDescInterface.h"



namespace mfx
{
namespace pi
{
namespace param
{



class TplPan
:	public piapi::ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit			TplPan (const char *name_0, int group_index = 0);
	virtual        ~TplPan () = default;

	void           set_flags (int32_t flags);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::ParamDescInterface
	virtual std::string
	               do_get_name (int len) const;
	virtual std::string
	               do_get_unit (int len) const;
	virtual Range  do_get_range () const;
	virtual Categ  do_get_categ () const;
	virtual int32_t
	               do_get_flags () const;
	virtual double do_get_nat_min () const;
	virtual double do_get_nat_max () const;
	virtual std::string
	               do_conv_nat_to_str (double nat, int len) const;
	virtual bool   do_conv_str_to_nat (double &nat, const std::string &txt) const;
	virtual double do_conv_nrm_to_nat (double nrm) const;
	virtual double do_conv_nat_to_nrm (double nat) const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            _group_index;
	std::string    _name;
	int32_t        _flags;


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplPan ()                               = delete;
	               TplPan (const TplPan &other)            = delete;
	TplPan &       operator = (const TplPan &other)        = delete;
	bool           operator == (const TplPan &other) const = delete;
	bool           operator != (const TplPan &other) const = delete;

}; // class TplPan



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplPan.hpp"



#endif   // mfx_pi_param_TplPan_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
