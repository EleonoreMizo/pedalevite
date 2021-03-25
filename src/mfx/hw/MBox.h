/*****************************************************************************

        MBox.h
        Author: Laurent de Soras, 2021

Minimal encapsulation for the Broadcom's mailbox.c/.h

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_MBox_HEADER_INCLUDED)
#define mfx_hw_MBox_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <system_error>

#include <cstdint>



namespace mfx
{
namespace hw
{



class MBox
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
	static constexpr int MEM_FLAG_DISCARDABLE    = 1 << 0; /* can be resized to 0 at any time. Use for cached data */
	static constexpr int MEM_FLAG_NORMAL         = 0 << 2; /* normal allocating alias. Don't use from ARM */
	static constexpr int MEM_FLAG_DIRECT         = 1 << 2; /* 0xC alias uncached */
	static constexpr int MEM_FLAG_COHERENT       = 2 << 2; /* 0x8 alias. Non-allocating in L2 but coherent */
	static constexpr int MEM_FLAG_L1_NONALLOCATING = (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT); /* Allocating in L2 */
	static constexpr int MEM_FLAG_ZERO           = 1 << 4;  /* initialise buffer to all zeros */
	static constexpr int MEM_FLAG_NO_INIT        = 1 << 5; /* don't initialise (default is initialise to all ones */
	static constexpr int MEM_FLAG_HINT_PERMALOCK = 1 << 6; /* Likely to be locked for long periods of time. */

	class Error
	:	public std::system_error
	{
		using system_error::system_error;
	};

	explicit       MBox (int size, int mem_flag, int align);
	               ~MBox ();

	inline uint32_t
	               get_phys_adr () const;
	template <typename T>
	inline T *     get_virt_ptr () const;
	inline uint8_t *
	               get_virt_ptr () const;
	inline uint32_t
	               get_size () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           cleanup ();

	int            _handle   = 0;       // From mbox_open ()
	uint32_t       _size     = 0;       // Required size
	uint32_t       _mem_ref  = 0;       // From mem_alloc ()
	uint32_t       _bus_adr  = 0;       // From mem_lock ()
	uint8_t *      _virt_ptr = nullptr; // From mapmem ()



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MBox ()                               = delete;
	               MBox (const MBox &other)              = delete;
	               MBox (MBox &&other)                   = delete;
	MBox &         operator = (const MBox &other)        = delete;
	MBox &         operator = (MBox &&other)             = delete;
	bool           operator == (const MBox &other) const = delete;
	bool           operator != (const MBox &other) const = delete;

}; // class MBox



}  // namespace hw
}  // namespace mfx



#include "mfx/hw/MBox.hpp"



#endif   // mfx_hw_MBox_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
