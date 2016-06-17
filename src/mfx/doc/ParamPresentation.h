/*****************************************************************************

        ParamPresentation.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_ParamPresentation_HEADER_INCLUDED)
#define mfx_doc_ParamPresentation_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace doc
{



class ParamPresentation
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Time/freq display modes
	enum DispMode
	{
		DispMode_DEFAULT = 0,
		DispMode_MS,
		DispMode_HZ,
		DispMode_NOTE
	};

	               ParamPresentation ()                               = default;
	               ParamPresentation (const ParamPresentation &other) = default;
	virtual        ~ParamPresentation ()                              = default;
	ParamPresentation &
	               operator = (const ParamPresentation &other)        = default;

	DispMode       _disp_mode = DispMode_DEFAULT;
	float          _ref_beats = 0; // Parameter time value, in beats. 0 = tempo sync not activated.



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ParamPresentation &other) const = delete;
	bool           operator != (const ParamPresentation &other) const = delete;

}; // class ParamPresentation



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/ParamPresentation.hpp"



#endif   // mfx_doc_ParamPresentation_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
