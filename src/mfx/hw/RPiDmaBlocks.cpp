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
,	_tot_len (compute_total_len (nbr_blocks, extra_bytes))
,	_nbr_pages (compute_nbr_pages (_tot_len))
,	_mbox (_nbr_pages << _page_size_l2, MBox::MEM_FLAG_DIRECT, _page_size) // MEM_FLAG_L1_NONALLOCATING for Pi 1.
,	_cb_ptr (_mbox.get_virt_ptr <bcm2837dma::CtrlBlock> ())
,	_buf_ptr (reinterpret_cast <uint8_t *> (_cb_ptr + nbr_blocks))
{
	// Nothing
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
