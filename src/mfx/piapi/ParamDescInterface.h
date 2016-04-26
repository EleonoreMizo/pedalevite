/*****************************************************************************

        ParamDescInterface.h
        Copyright (c) 2015 Ohm Force

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
		Categ_FREQ_PITCH,

		Categ_NBR_ELT
	};

	virtual        ~ParamDescInterface () {}

	inline std::string
	               get_name (int len) const;
	inline std::string
	               get_unit (int len) const;
	inline Range   get_range () const;
	inline Categ   get_categ () const;
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
