/*****************************************************************************

        PluginInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_PluginInterface_HEADER_INCLUDED)
#define mfx_piapi_PluginInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/ParamCateg.h"

#include <cstdint>



namespace mfx
{
namespace piapi
{



class EventTs;

class PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum State
	{
		State_CREATED = 0,
		State_ACTIVE,

		State_NBR_ELT
	};

	enum Dir
	{
		Dir_IN = 0,
		Dir_OUT,

		Dir_NBR_ELT
	};

	enum Err
	{
		Err_OK = 0,

		Err_EXCEPTION = -999,
		Err_MEMORY_ALLOCATION,
		Err_VALUE_NOT_SUPPORTED
	};

	enum BypassState
	{
		BypassState_IGNORE = 0,
		BypassState_ASK,
		BypassState_PRODUCED,

		BypassState_NBR_ELT
	};

	static const int  _max_nbr_chn = 2;

	class ProcInfo
	{
	public:
		float * const *
		               _dst_arr   = 0;
		float * const *
		               _byp_arr   = 0;
		const float * const *
		               _src_arr   = 0;
		int            _nbr_chn_arr [Dir_NBR_ELT];
		BypassState    _byp_state = BypassState_IGNORE; // On input and output
		int            _nbr_spl   = 0;
		const EventTs * const *
		               _evt_arr   = 0;
		int            _nbr_evt   = 0;
	};

	virtual        ~PluginInterface () = default;

	State          get_state () const;
	double         get_param_val (ParamCateg categ, int index, int note_id) const;
	int            reset (double sample_freq, int max_block_size, int &latency);
	void           process_block (ProcInfo &proc);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual State  do_get_state () const = 0;
	virtual double do_get_param_val (ParamCateg categ, int index, int note_id) const = 0;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency) = 0;
	virtual void   do_process_block (ProcInfo &proc) = 0;



}; // class PluginInterface



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/PluginInterface.hpp"



#endif   // mfx_piapi_PluginInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
