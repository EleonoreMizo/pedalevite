/*****************************************************************************

        TplEnumEvol.h
        Author: Laurent de Soras, 2017

This class is intended to code integer values into a normalized floating point
range [0 ; 1]. The coding method is designed to allows the integer range to be
extended without losing backward compatibility with previous range.

Coding is done by subdividing the interval into smaller ones. For example:

0.0............0.25.............0.5............0.75............1.0
00                                                              01
                                02
                03                              04
        05              06              07              08
    09      10      11      12      13      14      15      16
  17  18  19  20  21  22  23  24  25  26 ...  _nbr_val-1

To be converted back, a floating point value is rounded to the nearest
possible integer value given the current _nbr_val.

nbr_splits allows you to choose the initial subdivison size. Set it to 2 to
get the minimal subdivision, perfect for binary IEEE floating point encoding.
Example with nbr_splits = 4:


0.0............0.33............0.67............1.0
00              01              02              03
        04              05              06
    07      08      09      10      11      12
  13  14  15  16  17  18  19  20  21  22  23  24
 252627 ...  _name_list.size()-1

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplEnumEvol_HEADER_INCLUDED)
#define mfx_pi_param_TplEnumEvol_HEADER_INCLUDED

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



class TplEnumEvol
:	public piapi::ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       TplEnumEvol (int nbr_splits, const char *val_list_0, const char *name_0, const char *unit_0, int group_index = 0, const char *format_0 = "%s");
	               ~TplEnumEvol () = default;

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

	typedef	std::vector <std::string>	NameList;

	void           build_table (int nbr_splits, int nbr_elt);
	double         conv_int_to_flt (int val, int nbr_splits) const;
	int            conv_flt_to_int (double val, int nbr_splits, int nbr_val, int higher_layer, int total_split_size, int nbr_subintervals) const;

	int            _group_index;
	NameList       _name_list;
	std::string    _name;
	std::string    _unit;
	std::string    _print_format;
	int32_t        _flags;
	int            _mult;
	std::vector <int>
	               _flt_int_list;
	std::vector <double>
	               _int_flt_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplEnumEvol ()                               = delete;
	               TplEnumEvol (const TplEnumEvol &other)       = delete;
	               TplEnumEvol (TplEnumEvol &&other)            = delete;
	TplEnumEvol &  operator = (const TplEnumEvol &other)        = delete;
	TplEnumEvol &  operator = (TplEnumEvol &&other)             = delete;
	bool           operator == (const TplEnumEvol &other) const = delete;
	bool           operator != (const TplEnumEvol &other) const = delete;

}; // class TplEnumEvol



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplEnumEvol.hpp"



#endif   // mfx_pi_param_TplEnumEvol_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
