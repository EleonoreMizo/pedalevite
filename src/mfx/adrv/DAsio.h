/*****************************************************************************

        DAsio.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DAsio_HEADER_INCLUDED)
#define mfx_adrv_DAsio_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "asiosdk2/common/asio.h"
#include "asiosdk2/host/asiodrivers.h"
#include "fstb/AllocAlign.h"
#include "mfx/adrv/DriverInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace adrv
{



class DAsio
:	public DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DAsio ();
	virtual        ~DAsio ();


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::adrv::DriverInterface
	virtual int    do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out);
	virtual int    do_start ();
	virtual int    do_stop ();
	virtual std::string
	               do_get_last_error () const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_UNLOADED = 0,
		State_LOADED,
		State_INITIALISED,
		State_PREPARED,
		State_RUNNING,

		State_NBR_ELT
	};

	void           process_block (long buf_index);

	static void    process_asio (long doubleBufferIndex, ::ASIOBool directProcess);
	static void    samplerate_did_change (::ASIOSampleRate sRate);
	static long    asio_message (long selector, long value, void* message, double* opt);
	static ::ASIOTime *
	               buffer_switch_time_info (::ASIOTime* params, long doubleBufferIndex, ::ASIOBool directProcess);

	State          _state;
	CbInterface *  _cb_ptr;
	::AsioDrivers* _drivers_ptr;
	int            _driver_index;
	int            _block_size;
	double         _sample_freq;
	::ASIODriverInfo
	               _asio_info;
	std::array <int, Dir_NBR_ELT>
	               _chn_base_arr;
	std::string    _msg_err;
	const ::ASIOCallbacks
	               _asio_callbacks;
	std::array <std::array <::ASIOBufferInfo, _nbr_chn>, Dir_NBR_ELT>
	               _buf_info_arr;
	std::array <std::array <::ASIOChannelInfo, _nbr_chn>, Dir_NBR_ELT>
	               _chn_info_arr;

	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	static DAsio * _instance_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DAsio (const DAsio &other)             = delete;
	DAsio &        operator = (const DAsio &other)        = delete;
	bool           operator == (const DAsio &other) const = delete;
	bool           operator != (const DAsio &other) const = delete;

}; // class DAsio



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DAsio.hpp"



#endif   // mfx_adrv_DAsio_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
