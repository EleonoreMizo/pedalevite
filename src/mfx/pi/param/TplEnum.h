/*****************************************************************************

        TplEnum.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplEnum_HEADER_INCLUDED)
#define mfx_pi_param_TplEnum_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/ParamDescInterface.h"

#include <string>
#include <vector>



namespace mfx
{
namespace pi
{
namespace param
{



class TplEnum
:	public piapi::ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       TplEnum (const char *val_list_0, const char *name_0, const char *unit_0, int group_index = 0, const char *format_0 = "%s");
	virtual        ~TplEnum () = default;

	void           set_flags (int32_t flags);
	int            get_nbr_elt () const;



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

	typedef	std::vector <std::string>	NameList;

	int            _group_index;
	NameList       _name_list;
	std::string    _name;
	std::string    _unit;
	std::string    _print_format;
	int32_t        _flags;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplEnum ()                               = delete;
	               TplEnum (const TplEnum &other)           = delete;
	TplEnum &      operator = (const TplEnum &other)        = delete;
	bool           operator == (const TplEnum &other) const = delete;
	bool           operator != (const TplEnum &other) const = delete;

}; // class TplEnum



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplEnum.hpp"



#endif   // mfx_pi_param_TplEnum_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
