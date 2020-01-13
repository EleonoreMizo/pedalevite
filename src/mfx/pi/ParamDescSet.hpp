/*****************************************************************************

        ParamDescSet.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_ParamDescSet_CODEHEADER_INCLUDED)
#define mfx_pi_ParamDescSet_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// sptr is reset after adding the parameter, so it cannot be added twice
// by mistake.
template <typename T>
void	ParamDescSet::add (piapi::ParamCateg categ, int index, std::shared_ptr <T> &sptr)
{
	assert (categ >= 0);
	assert (categ < piapi::ParamCateg_NBR_ELT);
	assert (index >= 0);
	assert (index < get_nbr_param (categ));
	assert (sptr.get () != nullptr);
	assert (_categ_arr [categ] [index].get () == nullptr);

	_categ_arr [categ] [index] =
		std::static_pointer_cast <piapi::ParamDescInterface> (sptr);
	-- _nbr_rem_param;
	sptr.reset ();
}



template <typename T>
void	ParamDescSet::add_glob (int index, std::shared_ptr <T> &sptr)
{
	add (piapi::ParamCateg_GLOBAL, index, sptr);
}



template <typename T>
void	ParamDescSet::add_note (int index, std::shared_ptr <T> &sptr)
{
	add (piapi::ParamCateg_NOTE, index, sptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_ParamDescSet_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
