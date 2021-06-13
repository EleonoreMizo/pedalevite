/*****************************************************************************

        MmapPtr.cpp
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

#include "mfx/hw/MmapPtr.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MmapPtr::MmapPtr (uint32_t base, uint32_t len, const char dev_0 [], int flags)
:	_ptr (nullptr)
,	_len (0)
{
	assert (len > 0);
	assert (dev_0 != nullptr);
	assert (dev_0 [0] != '\0');

	int            fd = open (dev_0, flags);
	if (fd < 0)
	{
		throw Error (errno, std::system_category (), "Cannot open memory.\n");
	}

	void *         v_ptr =
		mmap (nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
	close (fd);

	if (v_ptr == MAP_FAILED)
	{
		throw Error (errno, std::system_category (), "Cannot map peripheral.");
	}

	_len = len;
	_ptr = reinterpret_cast <uint32_t *> (v_ptr);
}



MmapPtr::~MmapPtr ()
{
	if (_len > 0)
	{
		munmap (const_cast <uint32_t *> (_ptr), _len);
	}

	_ptr = nullptr;
	_len = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
