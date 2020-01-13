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
	               ~TplInt () = default;

	HelperDispNum& use_disp_num ();
	const HelperDispNum &
						use_disp_num () const;

	void           set_flags (int32_t flags);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::ParamDescInterface
	std::string    do_get_name (int len) const override;
	std::string    do_get_unit (int len) const override;
	Range          do_get_range () const override;
	Categ          do_get_categ () const override;
	int32_t        do_get_flags () const override;
	double         do_get_nat_min () const override;
	double         do_get_nat_max () const override;
	std::string    do_conv_nat_to_str (double nat, int len) const override;
	bool           do_conv_str_to_nat (double &nat, const std::string &txt) const override;
	double         do_conv_nrm_to_nat (double nrm) const override;
	double         do_conv_nat_to_nrm (double nat) const override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	HelperDispNum  _phdn;
	int            _group_index;
	std::string    _name;
	std::string    _unit;
	int32_t        _flags;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplInt ()                               = delete;
	               TplInt (const TplInt &other)            = delete;
	               TplInt (TplInt &&other)                 = delete;
	TplInt &       operator = (const TplInt &other)        = delete;
	TplInt &       operator = (TplInt &&other)             = delete;
	bool           operator == (const TplInt &other) const = delete;
	bool           operator != (const TplInt &other) const = delete;

}; // class TplInt



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplInt.hpp"



#endif   // mfx_pi_param_TplInt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
