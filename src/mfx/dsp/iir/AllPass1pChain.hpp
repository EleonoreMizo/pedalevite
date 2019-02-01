/*****************************************************************************

        AllPass1pChain.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_AllPass1pChain_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_AllPass1pChain_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	AllPass1pChain::get_length () const
{
	return int (_stage_arr.size ());
}



/*
==============================================================================
Name: set_coef
Description:
	Set the coefficient for the first order all pass transfert function:
	                    -1
	       b0_i +      z
	H(z) = ---------------
	                    -1
	       1    + b0_i.z
	The equivalent time equation is:
	y[t] = b0_i.x[t-0] + x[t-1] - b0_i.y[t-1]
Input parameters:
	- idx: Index of the stage, [0 ; N-1]
	- b0: Coefficient b0_i, [-1 ; 1].
Throws: Nothing
==============================================================================
*/

void	AllPass1pChain::set_coef (int idx, float b0)
{
	assert (idx >= 0);
	assert (idx < get_length ());

	_stage_arr [idx]._eq_z_b0 = b0;
}



float	AllPass1pChain::get_coef (int idx) const
{
	assert (idx >= 0);
	assert (idx < get_length ());

	return _stage_arr [idx]._eq_z_b0;
}



float	AllPass1pChain::get_state (int idx) const
{
	assert (idx >= 0);
	assert (idx <= get_length ());

	return (idx == 0) ? _mem_x : _stage_arr [idx - 1]._mem_y;
}



void	AllPass1pChain::set_state (int idx, float mem)
{
	assert (idx >= 0);
	assert (idx <= get_length ());

	if (idx == 0)
	{
		_mem_x = mem;
	}
	else
	{
		_stage_arr [idx - 1]._mem_y = mem;
	}
}



float	AllPass1pChain::process_sample (float x)
{
	if (_stage_arr.size () > 0)
	{
		float          mx = _mem_x;
		_mem_x = x;

		for (auto &stage : _stage_arr)
		{
			x  = stage._eq_z_b0 * (x - stage._mem_y) + mx;
			mx = stage._mem_y;
			stage._mem_y = x;
		}
	}

	return x;
}



// Uses the provided coefficient for all the stages
float	AllPass1pChain::process_sample_coef (float x, float b0)
{
	if (_stage_arr.size() > 0)
	{
		float          mx = _mem_x;
		_mem_x = x;

		for (auto &stage : _stage_arr)
		{
			x = b0 * (x - stage._mem_y) + mx;
			mx = stage._mem_y;
			stage._mem_y = x;
		}
	}

	return x;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_AllPass1pChain_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
