/*****************************************************************************

        LatticeStereo.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_ltc_LatticeStereo_HEADER_INCLUDED)
#define mfx_dsp_spat_ltc_LatticeStereo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/spat/ltc/Lattice.h"

#include <array>
#include <utility>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace ltc
{



template <class D, int N>
class LatticeStereo
{
	static_assert (N > 0, "N must be greater than 0");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_proc = N;
	static constexpr int _nbr_chn  = 2;

	typedef Lattice <D, N> Ltc;
	typedef D Delay;
	typedef typename Delay::DataType DataType;

	fstb_FORCEINLINE D &
	               use_delay (int chn, int idx) noexcept;
	fstb_FORCEINLINE void
	               set_coef (int chn, int idx, DataType c) noexcept;
	fstb_FORCEINLINE void
	               set_cross_feedback (DataType cf) noexcept;

	fstb_FORCEINLINE std::pair <DataType, DataType>
	               process_sample (DataType xl, DataType xr) noexcept;
	void           clear_buffers () noexcept;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <Ltc, _nbr_chn> LtcArray;
	typedef std::array <DataType, _nbr_chn> ScalarArray;

	LtcArray       _ltc_arr;
	ScalarArray    _fdbk_arr {};
	DataType       _cross_fdbk {};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const LatticeStereo &other) const = delete;
	bool           operator != (const LatticeStereo &other) const = delete;

}; // class LatticeStereo



}  // namespace ltc
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/ltc/LatticeStereo.hpp"



#endif   // mfx_dsp_spat_ltc_LatticeStereo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
