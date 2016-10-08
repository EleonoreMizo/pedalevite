/*****************************************************************************

        ControlledParam.cpp
        Author: Laurent de Soras, 2016

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

#include "fstb/fnc.h"
#include "mfx/ControlledParam.h"
#include "mfx/CtrlUnit.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ControlledParam::ControlledParam (const ParamCoord &param)
:	_param (param)
,	_ctrl_list ()
{
	// Nothing
}



ControlledParam::CtrlUnitList & ControlledParam::use_unit_list ()
{
	return _ctrl_list;
}



const ParamCoord &	ControlledParam::use_coord () const
{
	return _param;
}



// For parameters with relative sources
void	ControlledParam::update_internal_val(float val_nrm)
{
	assert (! _ctrl_list.empty () && _ctrl_list [0]->_abs_flag);

	CtrlUnit &     unit = *_ctrl_list [0];
	unit.update_internal_val (val_nrm);
}



// Controller absolute values should have been updated beforehand
// Parameter value in the plug-in pool is updated if there is an absolute controler.
float	ControlledParam::compute_final_val (PluginPool &pi_pool) const
{
	PluginPool::PluginDetails &   details =
		pi_pool.use_plugin (_param._plugin_id);

	float          val = 0;
	int            beg = 0;
	const int      index = _param._param_index;

	if (   ! _ctrl_list.empty ()
	    && _ctrl_list [0]->_abs_flag)
	{
		CtrlUnit &     unit = *_ctrl_list [0];

		// If the modification comes from the audio thread (controller),
		// we calculate the new value and update the reference.
		if (details._param_update_from_audio [index])
		{
			val = unit.evaluate (0);
			details._param_arr [index] = fstb::limit (val, 0.f, 1.f);
		}
		// Otherwise, we get the value from the reference.
		else
		{
			val = details._param_arr [index];
			if (unit._source.is_relative ())
			{
				// Forces internal value for relative sources
				unit.update_internal_val (val);
			}
		}
		beg = 1;
	}
	else
	{
		val = details._param_arr [index];
	}

	for (int m = beg; m < int (_ctrl_list.size ()); ++m)
	{
		const CtrlUnit &  unit = *_ctrl_list [m];
		val = unit.evaluate (val);
	}

	val = fstb::limit (val, 0.f, 1.f);

	return val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
