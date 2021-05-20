/*****************************************************************************

        FactoryTpl.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_piapi_FactoryTpl_CODEHEADER_INCLUDED)
#define mfx_piapi_FactoryTpl_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace piapi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class DSC, class PI>
std::unique_ptr <FactoryInterface>	FactoryTpl <DSC, PI>::create ()
{
	return std::make_unique <FactoryTpl <DSC, PI> > ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class DSC, class PI>
const PluginDescInterface &	FactoryTpl <DSC, PI>::do_describe ()
{
	return _desc;
}



template <class DSC, class PI>
std::unique_ptr <PluginInterface>	FactoryTpl <DSC, PI>::do_create ()
{
	std::unique_ptr <PluginInterface>   pi_uptr;
	try
	{
		pi_uptr = std::make_unique <PI> ();
	}
	catch (...)
	{
		assert (false);
		pi_uptr.reset ();
	}

	return pi_uptr;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace piapi
}  // namespace mfx



#endif   // mfx_piapi_FactoryTpl_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
