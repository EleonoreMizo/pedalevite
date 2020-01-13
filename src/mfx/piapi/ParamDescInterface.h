/*****************************************************************************

        ParamDescInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

All strings are UTF-8

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_ParamDescInterface_HEADER_INCLUDED)
#define	mfx_piapi_ParamDescInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>

#include <cstdint>



namespace mfx
{
namespace piapi
{



class ParamDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Range
	{
		Range_CONTINUOUS = 0,
		Range_DISCRETE,

		Range_NBR_ELT
	};

	enum Categ
	{
		Categ_UNDEFINED = 0,
		Categ_TIME_S,
		Categ_TIME_HZ,
		Categ_FREQ_HZ,

		Categ_NBR_ELT
	};

	enum Flags : int32_t
	{
		Flags_AUTOLINK = 1 << 0 // Automatic link to a controller (for example, the filter cutoff on a wha effects)
	};

	               ParamDescInterface ()                        = default;
	               ParamDescInterface (const ParamDescInterface &other) = default;
	               ParamDescInterface (ParamDescInterface &&other)      = default;

	virtual        ~ParamDescInterface ()                       = default;

	virtual ParamDescInterface &
	               operator = (const ParamDescInterface &other) = default;
	virtual ParamDescInterface &
	               operator = (ParamDescInterface &&other)      = default;

	inline std::string
	               get_name (int len) const;
	inline std::string
	               get_unit (int len) const;
	inline Range   get_range () const;
	inline Categ   get_categ () const;
	inline int32_t get_flags () const;
	inline double  get_nat_min () const;
	inline double  get_nat_max () const;
	inline std::string
	               conv_nat_to_str (double nat, int len) const;
	inline bool    conv_str_to_nat (double &nat, const std::string &txt) const;
	inline double  conv_nrm_to_nat (double nrm) const;
	inline double  conv_nat_to_nrm (double nat) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual std::string
	               do_get_name (int len) const = 0;
	virtual std::string
	               do_get_unit (int len) const = 0;
	virtual Range  do_get_range () const = 0;
	virtual Categ  do_get_categ () const = 0;
	virtual int32_t
	               do_get_flags () const = 0;
	virtual double do_get_nat_min () const = 0;
	virtual double do_get_nat_max () const = 0;
	virtual std::string
	               do_conv_nat_to_str (double nat, int len) const = 0;
	virtual bool   do_conv_str_to_nat (double &nat, const std::string &txt) const = 0;
	virtual double do_conv_nrm_to_nat (double nrm) const = 0;
	virtual double do_conv_nat_to_nrm (double nat) const = 0;



};	// class ParamDescInterface



}	// namespace piapi
}	// namespace mfx



#include "mfx/piapi/ParamDescInterface.hpp"



#endif	// mfx_piapi_ParamDescInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
