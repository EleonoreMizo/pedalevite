/*****************************************************************************

        DisplayLinuxFrameBuf.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_DisplayLinuxFrameBuf_HEADER_INCLUDED)
#define mfx_ui_DisplayLinuxFrameBuf_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/DisplayInterface.h"

#include <linux/fb.h>

#include <string>
#include <vector>



namespace mfx
{
namespace ui
{



class DisplayLinuxFrameBuf
:	public DisplayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       DisplayLinuxFrameBuf (std::string dev_name);
	virtual        ~DisplayLinuxFrameBuf ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// DisplayInterface
	virtual int    do_get_width () const;
	virtual int    do_get_height () const;
	virtual int    do_get_stride () const;
	virtual uint8_t *
	               do_use_screen_buf ();
	virtual const uint8_t *
	               do_use_screen_buf () const;

	virtual void   do_refresh (int x, int y, int w, int h);
	virtual void   do_force_reset ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <uint8_t> FrameBufInt;

	void           clean_up ();

	std::string    _dev_name;  // Linux output device. For example "/dev/fb0". -1 if not open or failed
	int            _fb_fd;     // File descriptor
	::fb_fix_screeninfo
	               _info_fix;
	::fb_var_screeninfo
	               _info_var;
	uint8_t *      _mm_fb_ptr; // Address allocated with mmap
	int            _map_len;
	uint8_t *      _pix_fb_ptr;// Pointer on the top-left pixel (system FB)
	int            _stride_s;  // Stride for the system framebuffer, in bytes.

	int            _disp_w;    // Width
	int            _disp_h;    // Height

	FrameBufInt    _fb_int;    // Internal frame buffer
	int            _stride_i;  // Stride for the internal framebuffer, in bytes



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DisplayLinuxFrameBuf ()                               = delete;
	               DisplayLinuxFrameBuf (const DisplayLinuxFrameBuf &other) = delete;
	DisplayLinuxFrameBuf &
	               operator = (const DisplayLinuxFrameBuf &other)        = delete;
	bool           operator == (const DisplayLinuxFrameBuf &other) const = delete;
	bool           operator != (const DisplayLinuxFrameBuf &other) const = delete;

}; // class DisplayLinuxFrameBuf



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/DisplayLinuxFrameBuf.hpp"



#endif   // mfx_ui_DisplayLinuxFrameBuf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
