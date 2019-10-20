/*****************************************************************************

        TestDisplayFrameBufSimple.cpp
        Author: Laurent de Soras, 2019

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

#include "test/TestDisplayFrameBufSimple.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include <chrono>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestDisplayFrameBufSimple::perform_test ()
{
	int fb_fd = open ("/dev/fb0", O_RDWR);

	fb_fix_screeninfo finfo; // this has smem_len: screen bytes
	ioctl (fb_fd, FBIOGET_FSCREENINFO, &finfo);
	fb_var_screeninfo vinfo; // fetched with an ioctl
	ioctl (fb_fd, FBIOGET_VSCREENINFO, &vinfo);

	printf ("Width         : %d\n", vinfo.xres);
	printf ("Height        : %d\n", vinfo.yres);
	printf ("Buffer length : %d\n", vinfo.yres_virtual * finfo.line_length);
	printf ("Bits per pixel: %d\n", vinfo.bits_per_pixel);
	printf ("Stride (bytes): %d\n", finfo.line_length);

	uint32_t * fb_ptr = reinterpret_cast <uint32_t *> (
		mmap (0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0)
	);

	const int nbr_frames = vinfo.xres;

	typedef std::chrono::steady_clock Stopwatch;
	const auto t_beg = Stopwatch::now ();

	for (int a = 0; a < nbr_frames; ++a)
	{
		for (int y = 0; y < int (vinfo.yres); ++y)
		{
			for (int x = 0; x < int (vinfo.xres); ++x)
			{
				uint32_t c = 0x00000000;
				if (((x + y + a) & 255) == 0)
				{
					c = 0x0000FF00; // (A)RGB
				}
				fb_ptr [y * (finfo.line_length >> 2) + x] = c;
			}
		}
	}

	const auto t_end = Stopwatch::now ();
	const auto dur   = t_end - t_beg;
	const double per =
		  double (Stopwatch::duration::period::num)
		/ double (Stopwatch::duration::period::den);
	const double dur_s = double (dur.count ()) * per;
	const double fps = double (nbr_frames) / dur_s;
	printf ("Rate (FPS)    : %.1f\n", fps);

	munmap (fb_ptr, finfo.smem_len);

	close (fb_fd);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
