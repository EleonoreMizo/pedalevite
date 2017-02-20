/*****************************************************************************

        DManual.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DManual_HEADER_INCLUDED)
#define mfx_adrv_DManual_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/adrv/DriverInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace adrv
{



class DManual
:	public DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DManual ()  = default;
	virtual        ~DManual () = default;

	void           get_buffers (float * in_ptr_arr [_nbr_chn], const float * out_ptr_arr [_nbr_chn]);
	void           process_block ();
	size_t         get_sample_index () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::adrv::DriverInterface
	virtual int    do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out);
	virtual int    do_start ();
	virtual int    do_stop ();
	virtual void   do_restart ();
	virtual std::string
	               do_get_last_error () const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_LOADED = 0,
		State_INIT,
		State_RUN,

		State_NBR_ELT
	};

	typedef std::vector <float, fstb::AllocAlign <float, 16 > > BufAlign;

	State          _state          = State_LOADED;
	double         _sample_freq    = 0;
	int            _max_block_size = 0;
	CbInterface *  _cb_ptr         = 0;
	std::array <std::array <BufAlign, _nbr_chn>, Dir_NBR_ELT>
	               _chn_buf_arr;
	size_t         _sample_index   = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DManual (const DManual &other)           = delete;
	DManual &      operator = (const DManual &other)        = delete;
	bool           operator == (const DManual &other) const = delete;
	bool           operator != (const DManual &other) const = delete;

}; // class DManual



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DManual.hpp"



#endif   // mfx_adrv_DManual_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
