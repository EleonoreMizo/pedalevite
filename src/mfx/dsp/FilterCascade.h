/*****************************************************************************

        FilterCascade.h
        Author: Laurent de Soras, 2018

Template parameters:

F1 is processed first, and F4 is the last one. They must have the following
functions:

Fn::Fn ();
float	Fn::process_sample (float sample);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_FilterCascade_HEADER_INCLUDED)
#define mfx_dsp_FilterCascade_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/FilterBypass.h"
#include "fstb/def.h"



namespace mfx
{
namespace dsp
{



template <class F1 = FilterBypass, class F2 = FilterBypass, class F3 = FilterBypass, class F4 = FilterBypass>
class FilterCascade
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FilterCascade ()                           = default;
	               FilterCascade (const FilterCascade &other) = default;
	               ~FilterCascade ()                          = default;
	FilterCascade &
	               operator = (const FilterCascade &other)    = default;

	typedef F1 Filter1Type;
	typedef F2 Filter2Type;
	typedef F3 Filter3Type;
	typedef F4 Filter4Type;

	fstb_FORCEINLINE float
	               process_sample (float sample);

	inline Filter1Type &
	               use_filter_1 ();
	inline const Filter1Type &
	               use_filter_1 () const;

	inline Filter2Type &
	               use_filter_2 ();
	inline const Filter2Type &
	               use_filter_2 () const;

	inline Filter3Type &
	               use_filter_3 ();
	inline const Filter3Type &
	               use_filter_3 () const;

	inline Filter4Type &
	               use_filter_4 ();
	inline const Filter4Type &
	               use_filter_4 () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Filter1Type    _f1;
	Filter2Type    _f2;
	Filter3Type    _f3;
	Filter4Type    _f4;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class FilterCascade



}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/FilterCascade.hpp"



#endif   // mfx_dsp_FilterCascade_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
