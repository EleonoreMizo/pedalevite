/*****************************************************************************

        ModelMsgCmdAsync.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ModelMsgCmdAsync_HEADER_INCLUDED)
#define mfx_ModelMsgCmdAsync_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ModelMsgCmdInterface.h"

#include <memory>



namespace mfx
{



class ModelMsgCmdAsync
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ModelMsgCmdAsync ()                              = default;
	               ModelMsgCmdAsync (const ModelMsgCmdAsync &other) = default;
	virtual        ~ModelMsgCmdAsync ()                             = default;
	ModelMsgCmdAsync &
	               operator = (const ModelMsgCmdAsync &other)       = default;

	// For fstb::MsgRet
	void           clear ();

	std::shared_ptr <ModelMsgCmdInterface>
	               _msg_sptr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ModelMsgCmdAsync &other) const = delete;
	bool           operator != (const ModelMsgCmdAsync &other) const = delete;

}; // class ModelMsgCmdAsync



}  // namespace mfx



//#include "mfx/ModelMsgCmdAsync.hpp"



#endif   // mfx_ModelMsgCmdAsync_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
