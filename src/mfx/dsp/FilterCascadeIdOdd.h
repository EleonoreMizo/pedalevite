/*****************************************************************************

        FilterCascadeIdOdd.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_FilterCascadeIdOdd_HEADER_INCLUDED)
#define mfx_dsp_FilterCascadeIdOdd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/FilterCascadeIdEven.h"



namespace mfx
{
namespace dsp
{



template <typename F1P, typename F2P, int NBR>
class FilterCascadeIdOdd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef F1P Filter1Type;
	typedef F2P Filter2Type;

	static const int  _nbr_units = NBR;

	inline F1P &   use_filter () noexcept;
	inline const F1P &
	               use_filter () const noexcept;

	inline F2P &   use_filter (int unit) noexcept;
	inline const F2P &
	               use_filter (int unit) const noexcept;

	fstb_FORCEINLINE float
	               process_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	FilterCascadeIdEven <Filter2Type, _nbr_units>
	               _even;
	Filter1Type    _odd;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FilterCascadeIdOdd &other) const = delete;
	bool           operator != (const FilterCascadeIdOdd &other) const = delete;

}; // class FilterCascadeIdOdd



}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/FilterCascadeIdOdd.hpp"



#endif   // mfx_dsp_FilterCascadeIdOdd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
