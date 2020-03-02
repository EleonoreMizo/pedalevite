/*****************************************************************************

        DAlsa.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DAlsa_HEADER_INCLUDED)
#define mfx_adrv_DAlsa_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/adrv/DriverInterface.h"

#include <alsa/asoundlib.h>

#include <array>
#include <atomic>
#include <thread>
#include <vector>



namespace mfx
{
namespace adrv
{



class DAlsa final
:	public DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _tgt_nbr_periods = 3;
	static const int  _tgt_period_size = 64;

	               DAlsa ();
	virtual        ~DAlsa ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::adrv::DriverInterface
	int            do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) final;
	int            do_start () final;
	int            do_stop () final;
	void           do_restart () final;
	std::string    do_get_last_error () const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            configure_alsa_audio (int dir);
	void           process_audio ();
	void           process_block (bool read_flag, bool write_flag);

	CbInterface *  _cb_ptr;
	int            _block_size;
	double         _sample_freq;
	std::array <int, Dir_NBR_ELT>
	               _chn_base_arr;
	std::array <::snd_pcm_t *, Dir_NBR_ELT>
	               _handle_arr;
	std::string    _msg_err;

	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;
	int            _block_size_alig;
	std::string    _driver;
	int            _nbr_periods_actual;

	std::thread    _thread_audio;
	volatile bool  _quit_flag;

	static DAlsa * _instance_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DAlsa (const DAlsa &other)             = delete;
	               DAlsa (DAlsa &&other)                  = delete;
	DAlsa &        operator = (const DAlsa &other)        = delete;
	DAlsa &        operator = (DAlsa &&other)             = delete;
	bool           operator == (const DAlsa &other) const = delete;
	bool           operator != (const DAlsa &other) const = delete;

}; // class DAlsa



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DAlsa.hpp"



#endif   // mfx_adrv_DAlsa_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
