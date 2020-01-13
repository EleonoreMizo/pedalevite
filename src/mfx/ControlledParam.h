/*****************************************************************************

        ControlledParam.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ControlledParam_HEADER_INCLUDED)
#define mfx_ControlledParam_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/CtrlUnit.h"
#include "mfx/ParamCoord.h"

#include <memory>
#include <vector>



namespace mfx
{



class PluginPool;

class ControlledParam
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::vector <std::shared_ptr <CtrlUnit> > CtrlUnitList;

	explicit       ControlledParam (const ParamCoord &param);
	virtual        ~ControlledParam () = default;

	const ParamCoord &
	               use_coord () const;
	CtrlUnitList & use_unit_list ();

	void           update_internal_val (float val_nrm);
	float          compute_final_val (PluginPool &pi_pool) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	const ParamCoord
	               _param;
	CtrlUnitList   _ctrl_list;    // If there is an absolute controller, it can be only in first position.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ControlledParam ()                               = delete;
	               ControlledParam (const ControlledParam &other)   = delete;
	               ControlledParam (const ControlledParam &&other)  = delete;
	ControlledParam &
	               operator = (const ControlledParam &other)        = delete;
	ControlledParam &
	               operator = (const ControlledParam &&other)       = delete;
	bool           operator == (const ControlledParam &other) const = delete;
	bool           operator != (const ControlledParam &other) const = delete;

}; // class ControlledParam



}  // namespace mfx



//#include "mfx/ControlledParam.hpp"



#endif   // mfx_ControlledParam_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
