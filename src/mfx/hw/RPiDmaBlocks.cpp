/*****************************************************************************

        RPiDmaBlocks.cpp
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

#include "mfx/hw/RPiDmaBlocks.h"

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



RPiDmaBlocks::RPiDmaBlocks (int nbr_blocks, int extra_bytes)
:	_nbr_cbs (nbr_blocks)
,	_buf_len (extra_bytes)
,	_tot_len (nbr_blocks, extra_bytes)
,	_nbr_pages (compute_nbr_pages (_tot_len)
,	_mbox (_nbr_pages << _page_size_l2, MBox::MEM_FLAG_DIRECT, _page_size) // MEM_FLAG_L1_NONALLOCATING for Pi 1.
,	_cb_ptr (_mbox.get_virt_ptr <bcm2837dma::CtrlBlock> ())
,	_buf_ptr (reinterpret_cast <uint8_t *> (_cb_ptr + nbr_blocks));
{
	// Nothing
}



int	RPiDmaBlocks::get_nbr_blocks () const
{
	return _nbr_cbs;
}



bcm2837dma::CtrlBlock &	RPiDmaBlocks::use_cb (int idx)
{
	assert (idx >= 0);
	assert (idx < _nbr_cbs);

	return _cb_ptr + idx;
}



const bcm2837dma::CtrlBlock &	RPiDmaBlocks::use_cb (int idx) const
{
	assert (idx >= 0);
	assert (idx < _nbr_cbs);

	return _cb_ptr + idx;
}



int	RPiDmaBlocks::get_buf_len () const
{
	return _buf_len;
}



// Buffer is 32-byte aligned (_align_min)
uint8_t *	RPiDmaBlocks::use_buf ()
{
	return _buf_ptr;
}



const uint8_t*	RPiDmaBlocks::use_buf () const
{
	return _buf_ptr;
}



template <typename T>
T *	RPiDmaBlocks::use_buf ()
{
	return reinterpret_cast <T *> (_buf_ptr);
}



template <typename T>
const T *	RPiDmaBlocks::use_buf () const
{
	return reinterpret_cast <const T *> (_buf_ptr);
}



uint32_t	RPiDmaBlocks::virt_to_phys (void *virt_ptr)
{
	const ptrdiff_t   offset =
		reinterpret_cast <uint8_t *> (virt_ptr) - _mbox.get_virt_ptr ();
	assert (offset >= 0);
	assert (offset < _tot_len);

	return _mbox.get_phys_adr () + uint32_t (offset);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	RPiDmaBlocks::compute_total_len (int nbr_blocks, int extra_bytes)
{
	assert (nbr_blocks > 0);
	assert (extra_bytes >= 0);

	return nbr_blocks * sizeof (bcm2837dma::CtrlBlock) + extra_bytes;
}



int	RPiDmaBlocks::compute_nbr_pages (int total_len)
{
	return ((total_len + (_page_size - 1)) >> _page_size_l2);
}



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
