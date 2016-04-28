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
class ParamDescInterface;

class PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum State
	{
		State_CONSTRUCTED = 0,
		State_INITIALISED,
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
		Err_OK = 0
	};

	static const int  _max_nbr_chn = 2;

	class ProcInfo
	{
	public:
		float * const *
		               _dst_arr  = 0;
		float * const *
		               _byp_arr  = 0;
		const float * const *
		               _src_arr  = 0;
		int            _nbr_chn_arr [Dir_NBR_ELT];
		bool           _byp_flag = false;   // On input and output
		int            _nbr_spl  = 0;
		const EventTs * const *
		               _evt_arr  = 0;
		int            _nbr_evt  = 0;
	};

	virtual        ~PluginInterface () = default;

	State          get_state () const;

	int            init ();
	int            restore ();

	bool           has_sidechain () const;
	bool           prefer_stereo () const;

	int            get_nbr_param (ParamCateg categ) const;
	const ParamDescInterface &
	               get_param_info (ParamCateg categ, int index) const;
	double         get_param_val (ParamCateg categ, int index, int note_id) const;

	int            reset (double sample_freq, int max_block_size, int &latency);

	void           process_block (ProcInfo &proc);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual State  do_get_state () const = 0;
	virtual int    do_init () = 0;
	virtual int    do_restore () = 0;
	virtual bool   do_has_sidechain () const = 0;
	virtual bool   do_prefer_stereo () const = 0;
	virtual int    do_get_nbr_param (ParamCateg categ) const = 0;
	virtual const ParamDescInterface &
	               do_get_param_info (ParamCateg categ, int index) const = 0;
	virtual double do_get_param_val (ParamCateg categ, int index, int note_id) const = 0;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency) = 0;
	virtual void   do_process_block (ProcInfo &proc) = 0;



}; // class PluginInterface



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/PluginInterface.hpp"



#endif   // mfx_piapi_PluginInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
