/*****************************************************************************

        PiProc.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (PiProc_HEADER_INCLUDED)
#define PiProc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/ProcInfo.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace piapi
{
	class PluginDescInterface;
	class PluginInterface;
}
}

class PiProc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <mfx::piapi::PluginDescInterface> DescSPtr;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	               PiProc () = default;

	void           set_desc (DescSPtr desc_sptr);
	int            setup (mfx::piapi::PluginInterface &pi, int nbr_chn_i, int nbr_chn_o, double sample_freq, int max_block_size, int &latency);
	void           set_param_nat (int index, double val_nat);
	void           reset_param ();
	mfx::piapi::ProcInfo &
	               use_proc_info ();
	float *const * use_buf_list_src () const;
	float *const * use_buf_list_dst () const;
	float *const * use_buf_list_sig () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	DescSPtr       _desc_sptr;
	std::vector <mfx::piapi::EventTs>
	               _evt_list;
	std::vector <const mfx::piapi::EventTs *>
	               _evt_ptr_list;
	mfx::piapi::ProcInfo
	               _proc_info;
	std::vector <BufAlign>
	               _buf_list;
	std::vector <float *>
	               _buf_src_ptr_list {};
	std::vector <float *>
	               _buf_dst_ptr_list {};
	std::vector <float *>
	               _buf_sig_ptr_list {};


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PiProc (const PiProc &other)            = delete;
	               PiProc (PiProc &&other)                 = delete;
	PiProc &       operator = (const PiProc &other)        = delete;
	PiProc &       operator = (PiProc &&other)             = delete;
	bool           operator == (const PiProc &other) const = delete;
	bool           operator != (const PiProc &other) const = delete;

}; // class PiProc



//#include "test/PiProc.hpp"



#endif   // PiProc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
