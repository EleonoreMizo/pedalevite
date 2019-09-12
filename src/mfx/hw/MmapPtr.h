/*****************************************************************************

        MmapPtr.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_MmapPtr_HEADER_INCLUDED)
#define mfx_hw_MmapPtr_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace hw
{



class MmapPtr
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       MmapPtr (uint32_t base, uint32_t len, const char dev_0 [], int flags);
	               ~MmapPtr ();

	inline uint32_t *
	               get () const;
	inline uint32_t *
	               operator -> () const;
	inline uint32_t &
	               operator * () const;
	inline const uint32_t &
	               at (uint32_t ofs_byte) const;
	inline uint32_t &
	               at (uint32_t ofs_byte);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	uint32_t *     _ptr;
	uint32_t       _len;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MmapPtr ()                               = delete;
	               MmapPtr (const MmapPtr &other)           = delete;
	MmapPtr &      operator = (const MmapPtr &other)        = delete;
	bool           operator == (const MmapPtr &other) const = delete;
	bool           operator != (const MmapPtr &other) const = delete;

}; // class MmapPtr



}  // namespace hw
}  // namespace mfx



#include "mfx/hw/MmapPtr.hpp"



#endif   // mfx_hw_MmapPtr_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
