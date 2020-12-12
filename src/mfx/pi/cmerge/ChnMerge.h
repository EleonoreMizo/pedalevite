/*****************************************************************************

        ChnMerge.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cmerge_ChnMerge_HEADER_INCLUDED)
#define mfx_pi_cmerge_ChnMerge_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/cmerge/ChnMergeDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace cmerge
{



class ChnMerge final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ChnMerge ();
	               ~ChnMerge () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           clear_buffers ();

	State          _state;

	ChnMergeDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ChnMerge (const ChnMerge &other)              = delete;
	               ChnMerge (ChnMerge &&other)                   = delete;
	ChnMerge &         operator = (const ChnMerge &other)        = delete;
	ChnMerge &         operator = (ChnMerge &&other)             = delete;
	bool           operator == (const ChnMerge &other) const = delete;
	bool           operator != (const ChnMerge &other) const = delete;

}; // class ChnMerge



}  // namespace cmerge
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cmerge/ChnMerge.hpp"



#endif   // mfx_pi_cmerge_ChnMerge_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
