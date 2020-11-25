/*****************************************************************************

        Lattice.h
        Author: Laurent de Soras, 2020

Lattice stucture with internal processors (usually allpass delays).
The delays should be configured directly by the caller.

Template parameters:

- D: processor object for each cell. Requires:
	D::D ();
	D::~D ();
	typename D::DataType;
	D::DataType D::process_sample (D::DataType);
	void D::clear_buffers ();

- N: number of cells, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_ltc_Lattice_HEADER_INCLUDED)
#define mfx_dsp_spat_ltc_Lattice_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace ltc
{



template <class D, int N>
class Lattice
{
	static_assert (N > 0, "N must be greater than 0");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_proc = N;
	typedef D Delay;
	typedef typename Delay::DataType DataType;

	fstb_FORCEINLINE D &
	               use_delay (int idx);
	fstb_FORCEINLINE void
	               set_coef (int idx, DataType c);
	DataType       process_sample (DataType x);
	DataType &     use_delay_output (int idx);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <DataType, _nbr_proc> ScalarArray;
	typedef std::array <Delay, _nbr_proc> DelayArray;

	DelayArray     _delay_arr;
	ScalarArray    _coef_arr {};
	ScalarArray    _dly_out_arr {};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Lattice &other) const = delete;
	bool           operator != (const Lattice &other) const = delete;

}; // class Lattice



}  // namespace ltc
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/ltc/Lattice.hpp"



#endif   // mfx_dsp_spat_ltc_Lattice_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
