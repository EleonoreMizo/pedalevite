/*****************************************************************************

        OnePole.h
        Author: Laurent de Soras, 2016

Can be inherited but is not polymorph.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_OnePole_HEADER_INCLUDED)
#define mfx_dsp_iir_OnePole_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



class OnePole
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               OnePole () noexcept;
	               OnePole (const OnePole &other)     = default;
	               ~OnePole ()                        = default;
	OnePole &       operator = (const OnePole &other) = default;

	inline void    neutralise () noexcept;
	inline void    set_z_eq (const float b [2], const float a [2]) noexcept;
	inline void    get_z_eq (float b [2], float a [2]) const noexcept;
	inline void    copy_z_eq (const OnePole &other) noexcept;

	inline float   process_sample (float x) noexcept;
	inline float   process_sample (float x, const float inc_b [2], const float inc_a [2]) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float inc_b [2], const float inc_a [2]) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    step_z_eq (const float inc_b [2], const float inc_a [2]) noexcept;

	std::array <float, 2>      // Direct coefficients, order z^(-n)
						_eq_z_b;
	std::array <float, 2>      // Recursive coefficients, order z^(-n). Index 0 is ignored.
						_eq_z_a;

	float          _mem_x;
	float          _mem_y;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OnePole &other) const = delete;
	bool           operator != (const OnePole &other) const = delete;

}; // class OnePole



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/OnePole.hpp"



#endif   // mfx_dsp_iir_OnePole_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
