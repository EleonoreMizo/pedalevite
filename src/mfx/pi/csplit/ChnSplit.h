/*****************************************************************************

        ChnSplit.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_csplit_ChnSplit_HEADER_INCLUDED)
#define mfx_pi_csplit_ChnSplit_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/csplit/ChnSplitDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace csplit
{



class ChnSplit final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ChnSplit (piapi::HostInterface &host);
	               ~ChnSplit () = default;



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

	piapi::HostInterface &
	               _host;
	State          _state;

	ChnSplitDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ChnSplit ()                               = delete;
	               ChnSplit (const ChnSplit &other)          = delete;
	               ChnSplit (ChnSplit &&other)               = delete;
	ChnSplit &     operator = (const ChnSplit &other)        = delete;
	ChnSplit &     operator = (ChnSplit &&other)             = delete;
	bool           operator == (const ChnSplit &other) const = delete;
	bool           operator != (const ChnSplit &other) const = delete;

}; // class ChnSplit



}  // namespace csplit
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/csplit/ChnSplit.hpp"



#endif   // mfx_pi_csplit_ChnSplit_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
