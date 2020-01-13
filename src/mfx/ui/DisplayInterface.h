/*****************************************************************************

        DisplayInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_DisplayInterface_HEADER_INCLUDED)
#define mfx_ui_DisplayInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace ui
{



class DisplayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum BlendMode
	{
		BlendMode_OPAQUE = 0,
		BlendMode_XOR,
		BlendMode_MIN,
		BlendMode_MAX,

		BlendMode_NBR_ELT
	};

	               DisplayInterface ()                              = default;
	               DisplayInterface (const DisplayInterface &other) = default;
	               DisplayInterface (DisplayInterface &&other)      = default;
	virtual        ~DisplayInterface ()                             = default;

	virtual DisplayInterface &
	               operator = (const DisplayInterface &other)       = default;
	virtual DisplayInterface &
	               operator = (DisplayInterface &&other)            = default;

	int            get_width () const;
	int            get_height () const;
	int            get_stride () const;
	uint8_t *      use_screen_buf ();
	const uint8_t *use_screen_buf () const;

	void           refresh (int x, int y, int w, int h);
	void           force_reset ();

	// Utility functions
	void           bitblt (int xd, int yd, const uint8_t *src_ptr, int xs, int ys, int ws, int hs, int ss, BlendMode mode);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_get_width () const = 0;
	virtual int    do_get_height () const = 0;
	virtual int    do_get_stride () const = 0;
	virtual uint8_t *
	               do_use_screen_buf () = 0;
	virtual const uint8_t *
	               do_use_screen_buf () const = 0;

	virtual void   do_refresh (int x, int y, int w, int h) = 0;
	virtual void   do_force_reset () = 0;



}; // class DisplayInterface



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/DisplayInterface.hpp"



#endif   // mfx_ui_DisplayInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
