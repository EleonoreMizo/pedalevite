/*****************************************************************************

        ParamMapFdbk.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_ParamMapFdbk_HEADER_INCLUDED)
#define	mfx_pi_ParamMapFdbk_HEADER_INCLUDED
#pragma once

#if defined (_MSC_VER)
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/shape/MapSaturate.h"



namespace mfx
{
namespace pi
{



class ParamMapFdbk
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ParamMapFdbk ()                          = default;
	               ParamMapFdbk (const ParamMapFdbk &other) = default;
	virtual        ~ParamMapFdbk ()                         = default;

	ParamMapFdbk & operator = (const ParamMapFdbk &other)   = default;

	inline void    config (double val_min, double val_max);
	inline double  conv_norm_to_nat (double norm) const;
	inline double  conv_nat_to_norm (double nat) const;

	static inline double 
	               get_nat_min () const;
	static inline double 
	               get_nat_max () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	dsp::shape::MapSaturate <
		double,
		std::ratio <  1,    2>,
		std::ratio <  1,    1>,
		std::ratio <995, 1000>
	>	Mapper;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ParamMapFdbk &other) const = delete;
	bool           operator != (const ParamMapFdbk &other) const = delete;

};	// class ParamMapFdbk



}	// namespace pi
}	// namespace mfx



#include	"mfx/pi/ParamMapFdbk.hpp"



#endif	// mfx_pi_ParamMapFdbk_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
