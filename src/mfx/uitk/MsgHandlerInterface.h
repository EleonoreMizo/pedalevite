/*****************************************************************************

        MsgHandlerInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_MsgHandlerInterface_HEADER_INCLUDED)
#define mfx_uitk_MsgHandlerInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace uitk
{



class NodeEvt;

class MsgHandlerInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum EvtProp
	{
		EvtProp_PASS = 0,
		EvtProp_CATCH,

		EvtProp_NBR_ELT
	};

	virtual        ~MsgHandlerInterface () = default;

	EvtProp        handle_evt (const NodeEvt &evt);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt) = 0;



}; // class MsgHandlerInterface



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/MsgHandlerInterface.hpp"



#endif   // mfx_uitk_MsgHandlerInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
