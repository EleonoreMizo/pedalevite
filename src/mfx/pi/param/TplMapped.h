/*****************************************************************************

        TplMapped.h
        Author: Laurent de Soras, 2016

Template parameters:

- T requires:
	void    T::config (double val_min, double val_max);
	double  T::conv_norm_to_nat (double norm) const;
	double  T::conv_nat_to_norm (double nat) const;

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplMapped_HEADER_INCLUDED)
#define mfx_pi_param_TplMapped_HEADER_INCLUDED

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



template <class T>
class TplMapped
:	public piapi::ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	T	Mapper;

	explicit       TplMapped (double val_min, double val_max, const char *name_0, const char *unit_0, HelperDispNum::Preset preset = HelperDispNum::Preset_FLOAT_STD, int group_index = 0, const char *format_0 = "%f");
	virtual        ~TplMapped () = default;

	void           set_categ (Categ categ);

	Mapper &       use_mapper ();
	const Mapper & use_mapper () const;

	HelperDispNum& use_disp_num ();
	const HelperDispNum &
	               use_disp_num () const;

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

	Mapper         _mapper;
	HelperDispNum  _phdn;
	int            _group_index;
	std::string    _name;
	std::string    _unit;
	Categ          _categ;
	int32_t        _flags;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplMapped ()                               = delete;
	               TplMapped (const TplMapped &other)         = delete;
	TplMapped &    operator = (const TplMapped &other)        = delete;
	bool           operator == (const TplMapped &other) const = delete;
	bool           operator != (const TplMapped &other) const = delete;

}; // class TplMapped



}  // namespace param
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/param/TplMapped.hpp"



#endif   // mfx_pi_param_TplMapped_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
