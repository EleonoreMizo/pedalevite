/*****************************************************************************

        MmapPtr.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_hw_MmapPtr_CODEHEADER_INCLUDED)
#define mfx_hw_MmapPtr_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



uint32_t *	MmapPtr::get () const
{
	return _ptr;
}



uint32_t *	MmapPtr::operator -> () const
{
	return _ptr;
}



uint32_t &	MmapPtr::operator * () const
{
	return *_ptr;
}



const uint32_t &	MmapPtr::at (uint32_t ofs_byte) const
{
	assert ((ofs_byte & 3) == 0);
	assert (ofs_byte < _len);

	return _ptr [ofs_byte >> 2];
}



uint32_t &	MmapPtr::at (uint32_t ofs_byte)
{
	assert ((ofs_byte & 3) == 0);
	assert (ofs_byte < _len);

	return _ptr [ofs_byte >> 2];
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_MmapPtr_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
