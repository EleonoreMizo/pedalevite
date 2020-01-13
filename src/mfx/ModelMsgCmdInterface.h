/*****************************************************************************

        ModelMsgCmdInterface.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ModelMsgCmdInterface_HEADER_INCLUDED)
#define mfx_ModelMsgCmdInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



class Model;

class ModelMsgCmdInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ModelMsgCmdInterface ()                        = default;
	               ModelMsgCmdInterface (const ModelMsgCmdInterface &other) = default;
	               ModelMsgCmdInterface (ModelMsgCmdInterface &&other)      = default;
	virtual        ~ModelMsgCmdInterface ()                       = default;

	virtual ModelMsgCmdInterface &
	               operator = (const ModelMsgCmdInterface &other) = default;
	virtual ModelMsgCmdInterface &
	               operator = (ModelMsgCmdInterface &&other)      = default;

	void           process (Model &model);
	void           clear ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_process (Model &model) = 0;
	virtual void   do_clear () = 0;



}; // class ModelMsgCmdInterface



}  // namespace mfx



//#include "mfx/ModelMsgCmdInterface.hpp"



#endif   // mfx_ModelMsgCmdInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
