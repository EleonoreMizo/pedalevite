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



class HostInterface;
class ProcInfo;

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

	static const int  _max_nbr_chn  =  2;
	static const int  _max_nbr_pins =  4;
	static const int  _max_nbr_sig  = 16;

	               PluginInterface ()                             = default;
	               PluginInterface (const PluginInterface &other) = default;
	               PluginInterface (PluginInterface &&other)      = default;
	virtual        ~PluginInterface ()                            = default;

	virtual PluginInterface &
	               operator = (const PluginInterface &other)      = default;
	virtual PluginInterface &
	               operator = (PluginInterface &&other)           = default;

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
