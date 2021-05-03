/*****************************************************************************

        AllPass2p.h
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
#if ! defined (mfx_dsp_iir_AllPass2p_HEADER_INCLUDED)
#define mfx_dsp_iir_AllPass2p_HEADER_INCLUDED

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



class AllPass2p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               AllPass2p () noexcept;
	               AllPass2p (const AllPass2p &other)  = default;
	               ~AllPass2p ()                       = default;
	AllPass2p &    operator = (const AllPass2p &other) = default;

	inline void    set_z_eq (float b0, float b1) noexcept;
	inline void    get_z_eq (float &b0, float &b1) const noexcept;
	inline void    copy_z_eq (const AllPass2p &other) noexcept;

	inline float   process_sample (float x) noexcept;
	inline float   process_sample (float x, const float inc_b [2]) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float inc_b [2]) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    step_z_eq (const float inc_b [2]) noexcept;

	std::array <float, 2>      // Direct coefficients, order z^(-n)
						_eq_z_b;

	std::array <float, 2>      // Ring buffer for input memory
	               _mem_x;
	std::array <float, 2>      // Ring buffer for output memory
	               _mem_y;
	int            _mem_pos;   // Write position in ring buffers. 0 or 1.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const AllPass2p &other) const = delete;
	bool           operator != (const AllPass2p &other) const = delete;

}; // class AllPass2p



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/AllPass2p.hpp"



#endif   // mfx_dsp_iir_AllPass2p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
