/*****************************************************************************

        ModelMsgCmdConfLdSv.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/Model.h"
#include "mfx/ModelMsgCmdCbInterface.h"
#include "mfx/ModelMsgCmdConfLdSv.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ModelMsgCmdConfLdSv::ModelMsgCmdConfLdSv (Type type, std::string pathname, ModelMsgCmdCbInterface *cb_ptr)
:	_type (type)
,	_pathname (pathname)
,	_cb_ptr (cb_ptr)
,	_ret_val (0)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);
	assert (! pathname.empty ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ModelMsgCmdConfLdSv::do_process (Model &model)
{
	switch (_type)
	{
	case Type_LOAD:
		_ret_val = model.load_from_disk (_pathname);
		break;
	case Type_SAVE:
		_ret_val = model.save_to_disk (_pathname);
		break;
	default:
		assert (false);
		break;
	}
}



void	ModelMsgCmdConfLdSv::do_clear ()
{
	if (_cb_ptr != nullptr)
	{
		_cb_ptr->notify_model_error_code (_ret_val);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
