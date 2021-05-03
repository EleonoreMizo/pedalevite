/*****************************************************************************

        DJack.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DJack_HEADER_INCLUDED)
#define mfx_adrv_DJack_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/adrv/DriverInterface.h"

#include <jack/jack.h>

#include <array>
#include <string>



namespace mfx
{
namespace adrv
{



class DJack final
:	public DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DJack ();
	virtual        ~DJack ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::adrv::DriverInterface
	int            do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) noexcept final;
	int            do_start () noexcept final;
	int            do_stop () noexcept final;
	void           do_restart () noexcept final;
	std::string    do_get_last_error () const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           process_block (int nbr_spl) noexcept;

	static void    jack_shutdown (void *arg) noexcept;
	static int     notify_audio_dropout (void *arg) noexcept;
	static int     process_jack (::jack_nframes_t nbr_spl, void *arg) noexcept;

	CbInterface *  _cb_ptr;
	std::array <int, Dir_NBR_ELT>
	               _chn_base_arr;
	::jack_client_t * volatile
	               _client_ptr;
	std::array <
		std::array <::jack_port_t * volatile, _nbr_chn>,
		Dir_NBR_ELT
	>              _mfx_port_arr;
	::jack_status_t
	               _status;

	std::string    _msg_err;

	static DJack * _instance_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DJack (const DJack &other)             = delete;
	               DJack (DJack &&other)                  = delete;
	DJack &        operator = (const DJack &other)        = delete;
	DJack &        operator = (DJack &&other)             = delete;
	bool           operator == (const DJack &other) const = delete;
	bool           operator != (const DJack &other) const = delete;

}; // class DJack



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DJack.hpp"



#endif   // mfx_adrv_DJack_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
