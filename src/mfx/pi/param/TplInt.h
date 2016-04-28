/*****************************************************************************

        TplInt.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplInt_HEADER_INCLUDED)
#define mfx_pi_param_TplInt_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/piapi/ParamDescInterface.h"



namespace mfx
{
namespace pi
{
namespace param
{



class TplInt
:	public piapi::ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       TplInt (int val_min, int val_max, const char *name_0, const char *unit_0, int group_index = 0, const char *format_0 = "%.0f");
	virtual        ~TplInt () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::ParamDescInterface
	virtual std::string
	               do_get_name (int len) const;
	virtual std::string
	               do_get_unit (int len) const;
	virtual Range  do_get_range () const;
	virtual Categ  do_get_categ () const;
	virtual double do_get_nat_min () const;
	virtual double do_get_nat_max () const;
	virtual std::string
	               do_conv_nat_to_str (double nat, int len) const;
	virtual bool   do_conv_str_to_nat (double &nat, const std::string &txt) const;
	virtual double do_conv_nrm_to_nat (double nrm) const;
	virtual double do_conv_nat_to_nrm (double nat) const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	HelperDispNum  _phdn;
	int            _group_index;
	std::string    _name;
	std::string    _unit;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplInt ()                               = delete;
	               TplInt (const TplInt &other)            = delete;
	TplInt &       operator = (const TplInt &other)        = delete;
	bool           operator == (const TplInt &other) const = delete;
	bool           operator != (const TplInt &other) const = delete;

}; // class TplInt



}  // namespace param
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/param/TplInt.hpp"



#endif   // mfx_pi_param_TplInt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
