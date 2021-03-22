/*****************************************************************************

        MBox.cpp
        Author: Laurent de Soras, 2021

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

#include "mfx/hw/MBox.h"
#include "mailbox.h"

#include <cassert>
#include <cstdlib>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Uses the mailbox interface to request memory from the VideoCore
// We specifiy (-1) for the handle rather than calling mbox_open ()
// so multiple users can share the resource.
MBox::MBox (int size, int mem_flag)
:	_handle (-1)
,	_size (size)
,	_mem_ref (mem_alloc (_handle, _size, 4096, mem_flag))
,	_bus_adr (0)
,	_virt_ptr (nullptr)
{
	try
	{
		if (_mem_ref == static_cast <unsigned int> (-1))
		{
			throw Error (
				errno, std::system_category (),
				"MBox: failed to allocate memory from VideoCore.\n"
			);
		}
		_bus_adr = mem_lock (_handle, _mem_ref);
		if (_bus_adr == static_cast <unsigned int> (~0))
		{
			throw Error (
				errno, std::system_category (),
				"MBox: failed to lock memory from VideoCore.\n"
			);
		}
		_virt_ptr = reinterpret_cast <uint8_t *> (
			mapmem (_bus_adr & 0x3FFFFFFF, _size, DEV_MEM)
		);
		if (_virt_ptr == nullptr)
		{
			throw Error (
				errno, std::system_category (),
				"MBox: cannot use the mailbox interface.\n"
			);
		}
	}
	catch (...)
	{
		cleanup ();
		throw;
	}
}



MBox::~MBox ()
{
	cleanup ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MBox::cleanup ()
{
	if (_mem_ref != 0 && _mem_ref != static_cast <uint32_t> (-1))
	{
		if (_virt_ptr != nullptr)
		{
			unmapmem (_virt_ptr, _size);
			_virt_ptr = nullptr;
		}
		mem_unlock (_handle, _mem_ref);
		mem_free (_handle, _mem_ref);
		_mem_ref = 0;
	}

	if (_handle >= 0)
	{
		mbox_close (_handle);
		_handle = 0;
	}
}



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
