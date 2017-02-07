/*****************************************************************************

        AllPass1p.h
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
#if ! defined (mfx_dsp_iir_AllPass1p_HEADER_INCLUDED)
#define mfx_dsp_iir_AllPass1p_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



class AllPass1p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               AllPass1p ()                        = default;
	               AllPass1p (const AllPass1p &other)  = default;
	               ~AllPass1p ()                       = default;
	AllPass1p &    operator = (const AllPass1p &other) = default;

	inline void    set_coef (float b0);
	inline float   get_coef () const;

	inline float   process_sample (float x);
	inline float   process_sample (float x, float inc_b0);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, float inc_b0);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    step_z_eq (float inc_b0);

	float          _eq_z_b0 = 0;  // Direct coefficients, order 0

	float          _mem_x   = 0;
	float          _mem_y   = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const AllPass1p &other) const = delete;
	bool           operator != (const AllPass1p &other) const = delete;

}; // class AllPass1p



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/AllPass1p.hpp"



#endif   // mfx_dsp_iir_AllPass1p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
