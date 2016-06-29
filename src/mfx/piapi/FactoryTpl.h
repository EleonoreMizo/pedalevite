/*****************************************************************************

        FactoryTpl.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_FactoryTpl_HEADER_INCLUDED)
#define mfx_piapi_FactoryTpl_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/FactoryInterface.h"



namespace mfx
{
namespace piapi
{



template <class DSC, class PI>
class FactoryTpl
:	public FactoryInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FactoryTpl ()  = default;
	virtual        ~FactoryTpl () = default;

	static std::shared_ptr <FactoryInterface>
	               create ();


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::FactoryInterface
	virtual const PluginDescInterface &
	               do_describe ();
	virtual PluginInterface *
	               do_create ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	DSC            _desc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FactoryTpl (const FactoryTpl &other)        = delete;
	FactoryTpl &   operator = (const FactoryTpl &other)        = delete;
	bool           operator == (const FactoryTpl &other) const = delete;
	bool           operator != (const FactoryTpl &other) const = delete;

}; // class FactoryTpl



}  // namespace piapi
}  // namespace mfx



#include "mfx/piapi/FactoryTpl.hpp"



#endif   // mfx_piapi_FactoryTpl_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
