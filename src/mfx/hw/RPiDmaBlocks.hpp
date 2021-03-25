/*****************************************************************************

        RPiDmaBlocks.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_hw_RPiDmaBlocks_CODEHEADER_INCLUDED)
#define mfx_hw_RPiDmaBlocks_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	RPiDmaBlocks::get_nbr_blocks () const
{
	return _nbr_cbs;
}



bcm2837dma::CtrlBlock &	RPiDmaBlocks::use_cb (int idx)
{
	assert (idx >= 0);
	assert (idx < _nbr_cbs);

	return _cb_ptr [idx];
}



const bcm2837dma::CtrlBlock &	RPiDmaBlocks::use_cb (int idx) const
{
	assert (idx >= 0);
	assert (idx < _nbr_cbs);

	return _cb_ptr [idx];
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



uint32_t	RPiDmaBlocks::virt_to_phys (const void *virt_ptr)
{
	const ptrdiff_t   offset =
		reinterpret_cast <const uint8_t *> (virt_ptr) - _mbox.get_virt_ptr ();
	assert (offset >= 0);
	assert (offset < _tot_len);

	return _mbox.get_phys_adr () + uint32_t (offset);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_RPiDmaBlocks_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
