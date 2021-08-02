/*****************************************************************************

        NgSetup.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_nzbl_NgSetup_HEADER_INCLUDED)
#define mfx_pi_nzbl_NgSetup_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace nzbl
{



class NgSetup
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class EnvTimes
	{
	public:
		float          _at = 0; // Attack time, s, >= 0
		float          _rt = 0; // Release time, s, >= 0
	};

	static inline EnvTimes
	               compute_env_times (int band_idx) noexcept;

	template <class B>
	static double  update_filters_compute_latency (B &splitter, double f_lat, double sample_freq, float ka, float kb) noexcept;

	static constexpr inline float
	               compute_split_freq (int split_idx) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               NgSetup ()                               = delete;
	               NgSetup (const NgSetup &other)           = delete;
	               NgSetup (NgSetup &&other)                = delete;
	NgSetup &      operator = (const NgSetup &other)        = delete;
	NgSetup &      operator = (NgSetup &&other)             = delete;
	bool           operator == (const NgSetup &other) const = delete;
	bool           operator != (const NgSetup &other) const = delete;

}; // class NgSetup



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/nzbl/NgSetup.hpp"



#endif   // mfx_pi_nzbl_NgSetup_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
