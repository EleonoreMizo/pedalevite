/*****************************************************************************

        ParamCoord.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_ParamCoord_CODEHEADER_INCLUDED)
#define mfx_ParamCoord_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ParamCoord::ParamCoord (int plugin_id, int param_index)
:	_plugin_id   (plugin_id  )
,	_param_index (param_index)
{
	// Nothing
}



inline bool	ParamCoord::is_valid () const
{
	return (_plugin_id >= 0 && _param_index >= 0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



inline bool	operator < (const ParamCoord &lhs, const ParamCoord &rhs)
{
	return (
		    lhs._plugin_id <  rhs._plugin_id
		|| (lhs._plugin_id == rhs._plugin_id && lhs._param_index < rhs._param_index)
	);
}



}  // namespace mfx



#endif   // mfx_ParamCoord_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
