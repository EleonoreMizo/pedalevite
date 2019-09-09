/*****************************************************************************

        DisplayLinuxFrameBuf.cpp
        Author: Laurent de Soras, 2019

Ref:
https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/fb.h
https://www.kernel.org/doc/html/latest/fb/api.html

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/ui/DisplayLinuxFrameBuf.h"

#if fstb_IS (ARCHI, X86)
	#include <emmintrin.h>
#elif fstb_IS (ARCHI, ARM)
	#include <arm_neon.h>
#else
	#error architecture not defined
#endif

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <stdexcept>
#include <system_error>

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DisplayLinuxFrameBuf::DisplayLinuxFrameBuf (std::string dev_name)
:	_dev_name (dev_name)
,	_fb_fd (open (dev_name.c_str (), O_RDWR))
,	_info_fix ()
,	_info_var ()
,	_mm_fb_ptr (nullptr)
,	_map_len (0)
,	_pix_fb_ptr (0)
,	_stride_s (0)
,	_disp_w (0)
,	_disp_h (0)
,	_fb_int ()
,	_stride_i (0)
{
	assert (! dev_name.empty ());

	try
	{
		if (_fb_fd == -1)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot open the framebuffer."
			);
		}

		if (ioctl (_fb_fd, FBIOGET_FSCREENINFO, &_info_fix) < 0)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot get screen info (fixed)."
			);
		}

		if (ioctl (_fb_fd, FBIOGET_VSCREENINFO, &_info_var) < 0)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot get screen info (variable)."
			);
		}

		_map_len   = _info_fix.smem_len;
		_mm_fb_ptr = reinterpret_cast <uint8_t *> (mmap (
			0, _map_len, PROT_READ | PROT_WRITE, MAP_SHARED, _fb_fd, 0
		));
		if (_mm_fb_ptr == MAP_FAILED)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot mmap the frame buffer."
			);
		}

		// We don't need the file descriptor anymore.
		close (_fb_fd);
		_fb_fd = -1;

		// Checks the pixel format
		if (_info_fix.type != FB_TYPE_PACKED_PIXELS)
		{
			throw std::runtime_error ("Only packed pixels are supported.");
		}

		if (   _info_fix.visual != FB_VISUAL_TRUECOLOR
		    && _info_fix.visual != FB_VISUAL_DIRECTCOLOR)
		{
			throw std::runtime_error ("Unsupported pixel encoding.");
		}

		if (_info_var.nonstd != 0)
		{
			throw std::runtime_error ("Non standard pixel format");
		}

		if (_info_var.bits_per_pixel != 32)
		{
			throw std::runtime_error ("Unsupported pixel bitdepth");
		}
		const int      bytes_per_pix = 4;

		if (    _info_var.red.length        != 8
		    || (_info_var.red.offset   & 3) != 0
		    ||  _info_var.red.msb_right     != 0
		    ||  _info_var.green.length      != 8
		    || (_info_var.green.offset & 3) != 0
		    ||  _info_var.green.msb_right   != 0
		    ||  _info_var.blue.length       != 8
		    || (_info_var.blue.offset  & 3) != 0
		    ||  _info_var.blue.msb_right    != 0)
		{
			throw std::runtime_error ("Unsupported pixel arrangement");
		}

		// Gets the resolution information
		_disp_w = _info_var.xres;
		_disp_h = _info_var.yres;
		_stride_s = _info_fix.line_length;
		_pix_fb_ptr =
			  _mm_fb_ptr
			+ _info_var.yoffset * _stride_s
			+ _info_var.xoffset * bytes_per_pix;

		// Allocates the internal framebuffer
		_stride_i = (_disp_w + 15) & ~15;
		_fb_int.resize (_stride_i * _disp_h, 0);
	}

	catch (...)
	{
		clean_up ();
		throw;
	}
}



DisplayLinuxFrameBuf::~DisplayLinuxFrameBuf ()
{
	clean_up ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DisplayLinuxFrameBuf::do_get_width () const
{
	return _disp_w;
}



int	DisplayLinuxFrameBuf::do_get_height () const
{
	return _disp_h;
}



int	DisplayLinuxFrameBuf::do_get_stride () const
{
	return _stride_i;
}



uint8_t *	DisplayLinuxFrameBuf::do_use_screen_buf ()
{
	return _fb_int.data ();
}



const uint8_t *	DisplayLinuxFrameBuf::do_use_screen_buf () const
{
	return _fb_int.data ();
}



void	DisplayLinuxFrameBuf::do_refresh (int x, int y, int w, int h)
{
	constexpr int  bypp_l2 = 2; // log2 (bytes_per_pixels)

	x = std::max (x, 0);
	y = std::max (y, 0);
	w = std::min (w, _disp_w - x);
	h = std::min (h, _disp_h - y);

	const uint8_t *   src_ptr = _fb_int.data () + y * _stride_i + x;
	uint8_t *      dst_ptr = _pix_fb_ptr + y * _stride_s + (x << bypp_l2);
	const int      w32 = w & ~31;

	for (int py = 0; py < h; ++py)
	{
		uint32_t *     dpix_ptr = reinterpret_cast <uint32_t *> (dst_ptr);

		// Starts with chunks of 32 pixels
		for (int px = 0; px < w32; px += 32)
		{
#if fstb_IS (ARCHI, X86)

			const auto  m0015 = _mm_loadu_si128 (
				reinterpret_cast <const __m128i *> (src_ptr + px     )
			);
			const auto  m1631 = _mm_loadu_si128 (
				reinterpret_cast <const __m128i *> (src_ptr + px + 16)
			);

			const auto  d0007 = _mm_unpacklo_epi8 (m0015, m0015);
			const auto  d0815 = _mm_unpackhi_epi8 (m0015, m0015);
			const auto  d1623 = _mm_unpacklo_epi8 (m1631, m1631);
			const auto  d2431 = _mm_unpackhi_epi8 (m1631, m1631);

			const auto  d0003 = _mm_unpacklo_epi16 (d0007, d0007);
			const auto  d0407 = _mm_unpackhi_epi16 (d0007, d0007);
			const auto  d0811 = _mm_unpacklo_epi16 (d0815, d0815);
			const auto  d1215 = _mm_unpackhi_epi16 (d0815, d0815);
			const auto  d1619 = _mm_unpacklo_epi16 (d1623, d1623);
			const auto  d2023 = _mm_unpackhi_epi16 (d1623, d1623);
			const auto  d2427 = _mm_unpacklo_epi16 (d2431, d2431);
			const auto  d2831 = _mm_unpackhi_epi16 (d2431, d2431);

			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px +  0), d0003);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px +  4), d0407);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px +  8), d0811);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px + 12), d1215);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px + 16), d1619);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px + 20), d2023);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px + 24), d2427);
			_mm_storeu_si128 (reinterpret_cast <__m128i *> (dpix_ptr + px + 28), d2831);

#else

			const auto  m0015 = vld1q_u8 (src_ptr + px     );
			const auto  m1631 = vld1q_u8 (src_ptr + px + 16);

			const auto  d0015 = vzipq_u8 (m0015, m0015);
			const auto  d1631 = vzipq_u8 (m1631, m1631);

			const auto  d0007 = vzipq_u8 (d0015.val [0], d0015.val [0]);
			const auto  d0815 = vzipq_u8 (d0015.val [1], d0015.val [1]);
			const auto  d1623 = vzipq_u8 (d1631.val [0], d1631.val [0]);
			const auto  d2431 = vzipq_u8 (d1631.val [1], d1631.val [1]);

			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px +  0), d0007.val [0]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px +  4), d0007.val [1]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px +  8), d0815.val [0]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px + 12), d0815.val [1]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px + 16), d1623.val [0]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px + 20), d1623.val [1]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px + 24), d2431.val [0]);
			vst1q_u8 (reinterpret_cast <uint8_t *> (dpix_ptr + px + 28), d2431.val [1]);

#endif
		}

		// The pixel per pixel
		for (int px = w32; px < w; ++px)
		{
			const int      v    = src_ptr [px];
			uint32_t       rgba = v | (v << 8);
			rgba = rgba | (rgba << 16);
			dpix_ptr [px] = rgba;
		}

		src_ptr += _stride_i;
		dst_ptr += _stride_s;
	}
}



void	DisplayLinuxFrameBuf::do_force_reset ()
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DisplayLinuxFrameBuf::clean_up ()
{
	if (_mm_fb_ptr != MAP_FAILED && _mm_fb_ptr != nullptr)
	{
		munmap (_mm_fb_ptr, _map_len);
		_mm_fb_ptr = nullptr;
	}

	if (_fb_fd != -1)
	{
		close (_fb_fd);
		_fb_fd = -1;
	}
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
