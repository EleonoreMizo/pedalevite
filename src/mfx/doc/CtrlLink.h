/*****************************************************************************

        CtrlLink.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_CtrlLink_HEADER_INCLUDED)
#define mfx_doc_CtrlLink_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ControlCurve.h"
#include "mfx/ControlSource.h"
#include "mfx/Cst.h"

#include <set>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class CtrlLink
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               CtrlLink ()                        = default;
	               CtrlLink (const CtrlLink &other)   = default;
	virtual        ~CtrlLink ()                       = default;

	CtrlLink &     operator = (const CtrlLink &other) = default;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	bool           is_similar (const CtrlLink &other) const;

	ControlSource  _source;
	float          _step     = float (Cst::_step_param);  // For relative modes (incremental encoders). > 0

	ControlCurve   _curve    = ControlCurve_LINEAR;
	float          _base     = 0;       // Normalized value, for absolute mode
	float          _amp      = 1;       // Normalized scale, for all modes. Can be negative.
	bool           _u2b_flag = false;   // Unipolar to bipolar conversion (0...1 -> -1...1)

	std::set <float>                    // Normalized values. Applies on the final value
	               _notch_list;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CtrlLink &other) const = delete;
	bool           operator != (const CtrlLink &other) const = delete;

}; // class CtrlLink



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/CtrlLink.hpp"



#endif   // mfx_doc_CtrlLink_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
