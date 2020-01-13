/*****************************************************************************

        ProgSwitcher.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ProgSwitcher_HEADER_INCLUDED)
#define mfx_ProgSwitcher_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/ProgSwitchMode.h"
#include "mfx/WaMsgQueue.h"
#include "mfx/ProcessingContextNode.h"



namespace mfx
{



class BufPack;

class ProgSwitcher
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ProgSwitcher (const ProcessingContext * &ctx_ptr, WaMsgQueue &queue_to_cmd, BufPack &buf_pack);
	               ~ProgSwitcher ();

	void           reset (double sample_freq, int max_block_size);
	bool           frame_beg ();
	void           handle_msg_ctx (WaMsgQueue::CellType &cell);
	bool           is_ctx_delayed () const;
	void           process_buffers_i (const ProcessingContextNode::Side &side, int nbr_spl);
	void           process_buffers_o (const ProcessingContextNode::Side &side, int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           fade_inout_buf (const ProcessingContextNode::Side &side, int nbr_spl, bool fadeout_flag);

	const ProcessingContext * &
	               _ctx_ptr;
	WaMsgQueue &   _queue_to_cmd;
	BufPack &      _buf_pack;

	int            _fade_len;           // Samples

	doc::ProgSwitchMode
	               _prog_switch_mode;
	bool           _switch_flag;        // For fade out/in mode, indicates that we are in the fade out part
	WaMsgQueue::CellType *              // For fade out/in mode, only during the first frame.
	               _delayed_ctx_msg_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProgSwitcher ()                               = delete;
	               ProgSwitcher (const ProgSwitcher &other)      = delete;
	               ProgSwitcher (ProgSwitcher &&other)           = delete;
	ProgSwitcher & operator = (const ProgSwitcher &other)        = delete;
	ProgSwitcher & operator = (ProgSwitcher &&other)             = delete;
	bool           operator == (const ProgSwitcher &other) const = delete;
	bool           operator != (const ProgSwitcher &other) const = delete;

}; // class ProgSwitcher



}  // namespace mfx



//#include "mfx/ProgSwitcher.hpp"



#endif   // mfx_ProgSwitcher_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

