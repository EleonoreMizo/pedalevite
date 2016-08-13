/*****************************************************************************

        DesignEq2p.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_DesignEq2p_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_DesignEq2p_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: make_dc_killer
Description:
	Builds a 2nd-order DC-killer. Its specifications are:
		-12 dB at Fc/2 Hz
		-2.53 dB at Fc Hz
		+/-0.05 dB in [Fc*2 ; Fc*10[
		+/-0.01 dB in [Fc*10 ; Fs/2[
	Good values for Fc are in the range 10 - 15 Hz.
Input parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_dc_killer (T bs [3], T as [3])
{
	assert (bs != 0);
	assert (as != 0);

	bs [0] = 0;
	bs [1] = 0;
	bs [2] = 1;

	as [0] = 1;
	as [1] = T (4) / T (3);
	as [2] = 1;
}



/*
==============================================================================
Name: make_3db_shelf_lo
Description:
	Builds a 2nd-order low shelf in the S-plane. When the shelf level is 0,
	the level at 1 rad/s is -3 dB.
Input parameters:
	- q: Filter selectivity, > 0. sqrt (0.5) gives the flattest slope.
	- lvl: Level of the shelf below the base frequency. >= 0.
Output parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_3db_shelf_lo (T bs [3], T as [3], T q, T lvl)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);
	assert (lvl >= 0);

	const T        lvl_r2 = sqrt (lvl);

	bs [0] = lvl;
	bs [1] = lvl_r2 / q;
	bs [2] = 1;

	as [0] = 1;
	as [1] = 1 / q;
	as [2] = 1;
}



/*
==============================================================================
Name: make_3db_shelf_hi
Description:
	Builds a 2nd-order high shelf in the S-plane. When the shelf level is 0,
	the level at 1 rad/s is -3 dB.
Input parameters:
	- q: Filter selectivity, > 0. sqrt (0.5) gives the flattest slope.
	- lvl: Level of the shelf above the base frequency. >= 0.
Output parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_3db_shelf_hi (T bs [3], T as [3], T q, T lvl)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);
	assert (lvl >= 0);

	const T        lvl_r2 = sqrt (lvl);

	bs [0] = 1;
	bs [1] = lvl_r2 / q;
	bs [2] = lvl;

	as [0] = 1;
	as [1] = 1 / q;
	as [2] = 1;
}



/*
==============================================================================
Name: make_3db_peak
Description:
	Builds a 2nd-order peaking filter in the S-plane. The peak (or hole) is
	located at 1 rad/s.
Input parameters:
	- q: Filter selectivity, modulates the bandwidth, > 0.
	- lvl: Peak level, >= 0.
Output parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_3db_peak (T bs [3], T as [3], T q, T lvl)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);
	assert (lvl >= 0);

	bs [0] = 1;
	bs [1] = lvl / q;
	bs [2] = 1;

	as [0] = 1;
	as [1] = 1 / q;
	as [2] = 1;
}



/*
==============================================================================
Name: make_mid_shelf_lo
Description:
	Builds a 2nd-order low shelf in the S-plane. The gain at 1 rad/s is half
	the shelf gain (in dB). The shelf level cannot be 0.
Input parameters:
	- q: Filter selectivity, > 0. sqrt (0.5) gives the flattest slope.
	- lvl: Level of the shelf below the base frequency. > 0.
Output parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_mid_shelf_lo (T bs [3], T as [3], T q, T lvl)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);
	assert (lvl > 0);

	const T        lvl_r2  = sqrt (lvl);
	const T        lvl_r4  = sqrt (lvl_r2);
	const T        lvl_r4q = lvl_r4 / q;

	bs [0] = lvl;
	bs [1] = lvl_r2 * lvl_r4q;
	bs [2] = lvl_r2;

	as [0] = 1;
	as [1] = lvl_r4q;
	as [2] = lvl_r2;
}



/*
==============================================================================
Name: make_mid_shelf_hi
Description:
	Builds a 2nd-order high shelf in the S-plane. The gain at 1 rad/s is half
	the shelf gain (in dB). The shelf level cannot be 0.
Input parameters:
	- q: Filter selectivity, > 0. sqrt (0.5) gives the flattest slope.
	- lvl: Level of the shelf above the base frequency. > 0.
Output parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_mid_shelf_hi (T bs [3], T as [3], T q, T lvl)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);
	assert (lvl > 0);

	const T        lvl_r2  = sqrt (lvl);
	const T        lvl_r4  = sqrt (lvl_r2);
	const T        lvl_r4q = lvl_r4 / q;
	const T        lvl_r2i = 1 / lvl_r2;

	bs [0] = 1;
	bs [1] = lvl_r4q;
	bs [2] = lvl_r2;

	as [0] = 1;
	as [1] = lvl_r4q * lvl_r2i;
	as [2] = lvl_r2i;
}



/*
==============================================================================
Name: make_mid_peak
Description:
	Builds a 2nd-order peaking filter in the S-plane. The peak (or hole) is
	located at 1 rad/s. The gain of the given bandwidth is half the peak gain
	(in dB). The peak level cannot be 0.
Input parameters:
	- q: Filter selectivity, modulates the bandwidth, > 0.
	- lvl: Peak level, > 0.
Output parameters:
	- bs: Num. of the equation in the S-plane, index is the power of s.
	- as: Denom. of the equation in the S-plane.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_mid_peak (T bs [3], T as [3], T q, T lvl)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);
	assert (lvl >= 0);

	const T        lvl_r2 = sqrt (lvl);

	bs [0] = 1;
	bs [1] = lvl_r2 / q;
	bs [2] = 1;

	as [0] = 1;
	as [1] = 1 / (lvl_r2 * q);
	as [2] = 1;
}



/*
==============================================================================
Name: make_low_pass
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_low_pass (T bs [3], T as [3], T q)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);

	bs [0] = 1;
	bs [1] = 0;
	bs [2] = 0;

	as [0] = 1;
	as [1] = 1 / q;
	as [2] = 1;
}



/*
==============================================================================
Name: make_hi_pass
==============================================================================
*/

template <typename T>
void	DesignEq2p::make_hi_pass (T bs [3], T as [3], T q)
{
	assert (bs != 0);
	assert (as != 0);
	assert (q > 0);

	bs [0] = 0;
	bs [1] = 0;
	bs [2] = 1;

	as [0] = 1;
	as [1] = 1 / q;
	as [2] = 1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_DesignEq2p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
