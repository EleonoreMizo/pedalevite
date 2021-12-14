/*****************************************************************************

        SlewRateLimiter.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_SlewRateLimiter_HEADER_INCLUDED)
#define mfx_dsp_shape_SlewRateLimiter_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename T>
class SlewRateLimiter
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    set_max_rate_p (T rate_max) noexcept;
	inline void    set_max_rate_n (T rate_max) noexcept;
	inline void    set_state (T state) noexcept;

	inline T       process_sample (T x) noexcept;
	void           process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	T              _rate_max_p = T (1); // Positive limit, units/spl, > 0
	T              _rate_max_n = T (1); // Negative limit, units/spl, > 0
	T              _state      = T (0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SlewRateLimiter &other) const = delete;
	bool           operator != (const SlewRateLimiter &other) const = delete;

}; // class SlewRateLimiter



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/SlewRateLimiter.hpp"



#endif   // mfx_dsp_shape_SlewRateLimiter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
