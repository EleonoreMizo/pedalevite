/*****************************************************************************

        AllPass1pChain.h
        Author: Laurent de Soras, 2017

Can be inherited but is not polymorph.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_AllPass1pChain_HEADER_INCLUDED)
#define mfx_dsp_iir_AllPass1pChain_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace dsp
{
namespace iir
{



class AllPass1pChain
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               AllPass1pChain ()                            = default;
	               AllPass1pChain (const AllPass1pChain &other) = default;
	AllPass1pChain &
	               operator = (const AllPass1pChain &other)     = default;

	void           reserve (int nbr_ap);
	void           set_length (int nbr_ap);
	inline int     get_length () const;
	inline void    set_coef (int idx, float b0);
	inline float   get_coef (int idx) const;
	void           set_coef_all (float b0);
	inline float   get_state (int idx) const;
	inline void    set_state (int idx, float mem);

	inline float   process_sample (float x);
	inline float   process_sample_coef (float x, float b0);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class ApStage
	{
	public:
		float          _eq_z_b0 = 0;  // Direct coefficients, order 0
		float          _mem_y   = 0;  // Memory of the recursive part
	};
	typedef std::vector <ApStage> StageArray;

	StageArray     _stage_arr;
	float          _mem_x = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const AllPass1pChain &other) const = delete;
	bool           operator != (const AllPass1pChain &other) const = delete;

}; // class AllPass1pChain



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/AllPass1pChain.hpp"



#endif   // mfx_dsp_iir_AllPass1pChain_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
