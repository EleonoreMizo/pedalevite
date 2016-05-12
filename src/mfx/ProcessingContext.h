/*****************************************************************************

        ProcessingContext.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ProcessingContext_HEADER_INCLUDED)
#define mfx_ProcessingContext_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ControlledParam.h"
#include "mfx/ControlSource.h"
#include "mfx/ProcessingContextNode.h"

#include <map>
#include <memory>
#include <vector>



namespace mfx
{



class ProcessingContext
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class PluginContext
	{
	public:
		ProcessingContextNode
		               _main;
		ProcessingContextNode
		               _mixer;
		bool           _mixer_flag = false;
	};

	typedef std::vector <PluginContext> PluginCtxArray;

	typedef std::map <ParamCoord, std::shared_ptr <ControlledParam> > MapParamCtrl;
	typedef std::multimap <ControlSource, std::shared_ptr <ControlledParam> > MapSourceParam;
	typedef std::multimap <ControlSource, std::shared_ptr <CtrlUnit> > MapSourceUnit;

	               ProcessingContext ()  = default;
	virtual        ~ProcessingContext () = default;

	ProcessingContextNode        // Initial and final aligned buffers
	               _interface_ctx;
	PluginCtxArray _context_arr;

	MapParamCtrl   _map_param_ctrl;
	MapSourceParam _map_src_param;
	MapSourceUnit  _map_src_unit;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProcessingContext (const ProcessingContext &other) = delete;
	ProcessingContext &
	               operator = (const ProcessingContext &other)        = delete;
	bool           operator == (const ProcessingContext &other) const = delete;
	bool           operator != (const ProcessingContext &other) const = delete;

}; // class ProcessingContext



}  // namespace mfx



//#include "mfx/ProcessingContext.hpp"



#endif   // mfx_ProcessingContext_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
