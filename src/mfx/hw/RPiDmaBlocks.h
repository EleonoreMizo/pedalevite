/*****************************************************************************

        RPiDmaBlocks.h
        Author: Laurent de Soras, 2021

Class allocating DMA control blocks along with an extra memory buffer

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_RPiDmaBlocks_HEADER_INCLUDED)
#define mfx_hw_RPiDmaBlocks_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/bcm2837dma.h"
#include "mfx/hw/MBox.h"

#include <cstdint>



namespace mfx
{
namespace hw
{



class RPiDmaBlocks
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Memory page size, bytes
	static constexpr int _page_size_l2 = 12;
	static constexpr int _page_size    = 1 << _page_size_l2;
	static constexpr int _page_mask    = _page_size - 1;

	static constexpr int _align_min    = 32; // Bytes, power of 2
	static_assert (_align_min == sizeof (bcm2837dma::CtrlBlock), "");

	explicit       RPiDmaBlocks (int nbr_blocks, int extra_bytes);
	virtual        ~RPiDmaBlocks () = default;

	inline int     get_nbr_blocks () const;
	inline bcm2837dma::CtrlBlock &
	               use_cb (int idx);
	inline const bcm2837dma::CtrlBlock &
	               use_cb (int idx) const;

	inline int     get_buf_len () const;
	inline uint8_t *
	               use_buf ();
	inline const uint8_t *
	               use_buf () const;
	template <typename T>
	inline T *     use_buf ();
	template <typename T>
	inline  const T *
	               use_buf () const;

	inline  uint32_t
	               virt_to_phys (void *virt_ptr);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static int     compute_total_len (int nbr_blocks, int extra_bytes);
	static int     compute_nbr_pages (int total_len);

	int            _nbr_cbs   = 0;      // Number of control blocks
	int            _buf_len   = 0;      // Extra buffer length, bytes
	int            _tot_len   = 0;      // Reserved memory length, bytes
	int            _nbr_pages = 0;      // Number of memory pages
	MBox           _mbox;
	bcm2837dma::CtrlBlock *             // 32-bytes aligned
	               _cb_ptr    = nullptr;
	uint8_t *      _buf_ptr   = nullptr; // 32-bytes aligned



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               RPiDmaBlocks ()                               = delete;
	               RPiDmaBlocks (const RPiDmaBlocks &other)      = delete;
	               RPiDmaBlocks (RPiDmaBlocks &&other)           = delete;
	RPiDmaBlocks & operator = (const RPiDmaBlocks &other)        = delete;
	RPiDmaBlocks & operator = (RPiDmaBlocks &&other)             = delete;
	bool           operator == (const RPiDmaBlocks &other) const = delete;
	bool           operator != (const RPiDmaBlocks &other) const = delete;

}; // class RPiDmaBlocks



}  // namespace hw
}  // namespace mfx



#include "mfx/hw/RPiDmaBlocks.hpp"



#endif   // mfx_hw_RPiDmaBlocks_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
