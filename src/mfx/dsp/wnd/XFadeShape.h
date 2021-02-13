/*****************************************************************************

        XFadeShape.h
        Author: Laurent de Soras, 2017

Template parameters:

- OP: operation class. Requires:
	static std::array <float, 2> OP::compute_gain (float x);
	static std::array <fstb::ToolsSimd::VectF32, 2> OP::compute_gain (fstb::ToolsSimd::VectF32 x);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_XFadeShape_HEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeShape_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <typename SHP>
class XFadeShape
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef SHP Shape;

	void           set_duration (double duration, float fade_ratio = 1);
	void           set_sample_freq (double sample_freq);

	inline bool    is_ready () const;
	inline int     get_len () const;
	inline const float *
	               use_shape () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           make_shape ();

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	double         _sample_freq = 0;       // Hz. 0 = not set
	double         _duration    = 0.010;   // s, > 0
	int            _len         = 0;       // 0 = not set
	float          _fade_ratio  = 1;       // Proportion of fade in the shape. ]0 ; 1]
	BufAlign       _shape;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const XFadeShape &other) const = delete;
	bool           operator != (const XFadeShape &other) const = delete;

}; // class XFadeShape



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/XFadeShape.hpp"



#endif   // mfx_dsp_wnd_XFadeShape_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
