/*****************************************************************************

        InterpFtor.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_rspl_InterpFtor_HEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFtor_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace rspl
{



class InterpFtor
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Hold
	{
	public:
		static const int  DATA_PRE  = 0;
		static const int  DATA_POST = 0;
		fstb_FORCEINLINE float
		               operator () (float frac_pos, const float data []) const;
	};

	class Linear
	{
	public:
		static const int  DATA_PRE  = 0;
		static const int  DATA_POST = 1;
		fstb_FORCEINLINE float
		               operator () (float frac_pos, const float data []) const;
	};

	class CubicHermite
	{
	public:
		static const int  DATA_PRE  = 1;
		static const int  DATA_POST = 2;
		fstb_FORCEINLINE float
		               operator () (float frac_pos, const float data []) const;
		fstb_FORCEINLINE float
		               operator () (uint32_t frac_pos, const float data []) const;
		fstb_FORCEINLINE int32_t
		               operator () (uint32_t frac_pos, const int16_t data []) const;
	};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               InterpFtor ()                               = delete;
	               InterpFtor (const InterpFtor &other)        = delete;
	virtual        ~InterpFtor ()                              = delete;
	InterpFtor &   operator = (const InterpFtor &other)        = delete;
	bool           operator == (const InterpFtor &other) const = delete;
	bool           operator != (const InterpFtor &other) const = delete;

}; // class InterpFtor



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/rspl/InterpFtor.hpp"



#endif   // mfx_dsp_rspl_InterpFtor_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
