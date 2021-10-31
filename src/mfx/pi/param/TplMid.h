/*****************************************************************************

        TplMid.h
        Author: Laurent de Soras, 2021

Hyperbolic mapping taking the mid-course value as curve parameter.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplMid_HEADER_INCLUDED)
#define mfx_pi_param_TplMid_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/piapi/ParamDescInterface.h"



namespace mfx
{
namespace pi
{
namespace param
{



class TplMid
:	public piapi::ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       TplMid (double val_min, double val_max, double val_mid, const char *name_0, const char *unit_0, int group_index = 0, const char *format_0 = "%f");

	void           set_categ (Categ categ);

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
	int            _group_index = 0;
	std::string    _name;
	std::string    _unit;
	Categ          _categ   = piapi::ParamDescInterface::Categ_UNDEFINED;
	int32_t        _flags   = 0;
	double         _val_mid = 0.5; // In ]0 ; 1[



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplMid ()                               = delete;
	               TplMid (const TplMid &other)            = delete;
	               TplMid (TplMid &&other)                 = delete;
	TplMid &       operator = (const TplMid &other)        = delete;
	TplMid &       operator = (TplMid &&other)             = delete;
	bool           operator == (const TplMid &other) const = delete;
	bool           operator != (const TplMid &other) const = delete;

}; // class TplMid



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplMid.hpp"



#endif   // mfx_pi_param_TplMid_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

