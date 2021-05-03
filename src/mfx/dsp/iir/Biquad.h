/*****************************************************************************

        Biquad.h
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
#if ! defined (mfx_dsp_iir_Biquad_HEADER_INCLUDED)
#define mfx_dsp_iir_Biquad_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <functional>
#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



class Biquad
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Biquad () noexcept;
	               Biquad (const Biquad &other)     = default;
	               ~Biquad ()                       = default;
	Biquad &       operator = (const Biquad &other) = default;

	inline void    neutralise () noexcept;
	inline void    set_z_eq (const float b [3], const float a [3]) noexcept;
	inline void    get_z_eq (float b [3], float a [3]) const noexcept;
	inline void    copy_z_eq (const Biquad &other) noexcept;
	inline float   get_state_y () const noexcept;
	inline void    set_state_y (float y) noexcept;
	inline void    get_state (float mem_x [2], float mem_y [2]) const noexcept;
	inline void    set_state (const float mem_x [2], const float mem_y [2]) noexcept;

	inline float   process_sample (float x) noexcept;
	inline float   process_sample (float x, const float inc_b [3], const float inc_a [3]) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float inc_b [3], const float inc_a [3]) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, std::function <float (float)> shaper) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    step_z_eq (const float inc_b [3], const float inc_a [3]) noexcept;

	std::array <float, 3>      // Direct coefficients, order z^(-n)
						_eq_z_b;
	std::array <float, 3>      // Recursive coefficients, order z^(-n). Index 0 is ignored.
						_eq_z_a;

	std::array <float, 2>      // Ring buffer for input memory
	               _mem_x;
	std::array <float, 2>      // Ring buffer for output memory
	               _mem_y;
	int            _mem_pos;   // Write position in ring buffers. 0 or 1.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Biquad &other) const = delete;
	bool           operator != (const Biquad &other) const = delete;

}; // class Biquad



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Biquad.hpp"



#endif   // mfx_dsp_iir_Biquad_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
