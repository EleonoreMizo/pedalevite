/*****************************************************************************

        FilterCascadeIdEven.h
        Author: Laurent de Soras, 2016

Template parameters:

- FLT: filter type. Requires:
	FLT::FLT()
	FLT::~FLT()
	void FLT::clear_buffers ();
	float FLT::process_sample (float x);
	void FLT::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	process_block() should work in-place if NBR > 1.

- NBR: number of cascaded filters, > 0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_FilterCascadeIdEven_HEADER_INCLUDED)
#define mfx_dsp_FilterCascadeIdEven_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>

namespace mfx
{
namespace dsp
{



template <typename FLT, int NBR>
class FilterCascadeIdEven
{

	static_assert (NBR > 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef FLT FilterType;
	static const int  _nbr_units = NBR;


	inline FLT &   use_filter (int unit) noexcept;
	inline const FLT &
	               use_filter (int unit) const noexcept;

	fstb_FORCEINLINE float
	               process_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <FilterType, _nbr_units> FilterArray;

	FilterArray    _filter_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FilterCascadeIdEven &other) const = delete;
	bool           operator != (const FilterCascadeIdEven &other) const = delete;

}; // class FilterCascadeIdEven



}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/FilterCascadeIdEven.hpp"



#endif   // mfx_dsp_FilterCascadeIdEven_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
