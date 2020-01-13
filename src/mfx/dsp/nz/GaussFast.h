/*****************************************************************************

        GaussFast.h
        Author: Laurent de Soras, 2018

Template parameters:

- NIT: number of iterations, >= 3

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_nz_GaussFast_HEADER_INCLUDED)
#define mfx_dsp_nz_GaussFast_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <random>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace nz
{



template <int NIT>
class GaussFast
{
	static_assert (NIT >= 3, "The number of iterations must be >= 3");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_it = NIT;

	void           set_rough_level (float lvl);
	inline float   process_sample ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _resol = 16;

	std::minstd_rand
	               _gen;
	const int32_t  _sub  = (1 << _resol) * _nbr_it / 2;
	float          _mult = 1.0f / (1 << _resol);




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const GaussFast &other) const = delete;
	bool           operator != (const GaussFast &other) const = delete;

}; // class GaussFast



}  // namespace nz
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/nz/GaussFast.hpp"



#endif   // mfx_dsp_nz_GaussFast_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
