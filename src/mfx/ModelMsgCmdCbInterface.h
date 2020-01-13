/*****************************************************************************

        ModelMsgCmdCbInterface.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ModelMsgCmdCbInterface_HEADER_INCLUDED)
#define mfx_ModelMsgCmdCbInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



class ModelMsgCmdCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ModelMsgCmdCbInterface ()                        = default;
	               ModelMsgCmdCbInterface (const ModelMsgCmdCbInterface &other) = default;
	               ModelMsgCmdCbInterface (ModelMsgCmdCbInterface &&other)      = default;

	virtual        ~ModelMsgCmdCbInterface ()                       = default;

	virtual ModelMsgCmdCbInterface &
	               operator = (const ModelMsgCmdCbInterface &other) = default;
	virtual ModelMsgCmdCbInterface &
	               operator = (ModelMsgCmdCbInterface &&other)      = default;

	void           notify_model_error_code (int ret_val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_notify_model_error_code (int ret_val) = 0;



}; // class ModelMsgCmdCbInterface



}  // namespace mfx



//#include "mfx/ModelMsgCmdCbInterface.hpp"



#endif   // mfx_ModelMsgCmdCbInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
