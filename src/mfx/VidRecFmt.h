/*****************************************************************************

        VidRecFmt.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_VidRecFmt_HEADER_INCLUDED)
#define mfx_VidRecFmt_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <chrono>

#include <cstdint>



namespace mfx
{



class VidRecFmt
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _fmt_version = 1;

	typedef std::chrono::microseconds TimestampUnit;

	enum PixCode
	{
		PixCode_GREY8 = 0
	};

	class HeaderFile
	{
	public:
		uint16_t       _version  = _fmt_version;  // _fmt_version
		uint16_t       _width    = 0;
		uint16_t       _height   = 0;
		uint16_t       _pix_code = PixCode_GREY8; // One of the PixCode enum
	};

	class HeaderFrame
	{
	public:
		uint64_t       _timestamp = 0; // Microseconds. Should be 0 for the 1st frame
		uint16_t       _x = 0;
		uint16_t       _y = 0;
		uint16_t       _w = 0;
		uint16_t       _h = 0;
		// Following: compressed picture data
		// Length depends on _w, _h and _pix_code
		// Stride is minimum (generally exactly equal to _w)
		// Picture is compressed with CompressSimple
		// If _w == 0 or _h == 0: this is the end.
		// There is no compressed picture data, only _timestamp is significant.
	};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               VidRecFmt ()                               = delete;
	               VidRecFmt (const VidRecFmt &other)         = delete;
	               VidRecFmt (VidRecFmt &&other)              = delete;
	               ~VidRecFmt ()                              = delete;
	VidRecFmt &    operator = (const VidRecFmt &other)        = delete;
	VidRecFmt &    operator = (VidRecFmt &&other)             = delete;
	bool           operator == (const VidRecFmt &other) const = delete;
	bool           operator != (const VidRecFmt &other) const = delete;

}; // class VidRecFmt



}  // namespace mfx



//#include "mfx/VidRecFmt.hpp"



#endif   // mfx_VidRecFmt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
