/*****************************************************************************

        ToolsParam.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ToolsParam_HEADER_INCLUDED)
#define mfx_ToolsParam_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <set>



namespace mfx
{

namespace doc
{
	class CtrlLinkSet;
}

namespace piapi
{
	class ParamDescInterface;
}



class ToolsParam
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~ToolsParam () = default;

	static double  conv_nrm_to_beats (double val_nrm, const piapi::ParamDescInterface &desc, double tempo);
	static double  conv_beats_to_nrm (double val_beats, const piapi::ParamDescInterface &desc, double tempo);

	static std::set <float>::const_iterator
	               find_closest_notch (float val, const std::set <float> &notch_list);
	static std::set <float>::const_iterator
	               advance_to_notch (float val, const std::set <float> &notch_list, int dir);
	static void    add_beat_notch_list_if_linked (doc::CtrlLinkSet &cls, const piapi::ParamDescInterface &desc, double tempo);
	static std::set <float>
	               create_beat_notches ();

	static const std::set <float>
	               _beat_notch_list;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ToolsParam ()                               = delete;
	               ToolsParam (const ToolsParam &other)        = delete;
	ToolsParam &   operator = (const ToolsParam &other)        = delete;
	bool           operator == (const ToolsParam &other) const = delete;
	bool           operator != (const ToolsParam &other) const = delete;

}; // class ToolsParam



}  // namespace mfx



//#include "mfx/ToolsParam.hpp"



#endif   // mfx_ToolsParam_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
