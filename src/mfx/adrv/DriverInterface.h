/*****************************************************************************

        DriverInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_DriverInterface_HEADER_INCLUDED)
#define mfx_adrv_DriverInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>
#include <string>



namespace mfx
{
namespace adrv
{



class CbInterface;

class DriverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_chn = 2;

	enum Dir
	{
		Dir_IN = 0,
		Dir_OUT,

		Dir_NBR_ELT
	};

	virtual        ~DriverInterface () = default;

	int            init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out);
	int            start ();
	int            stop ();
	std::string    get_last_error () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out) = 0;
	virtual int    do_start () = 0;
	virtual int    do_stop () = 0;
	virtual std::string
	               do_get_last_error () const = 0;

	static const std::array <const char *, Dir_NBR_ELT>
	               _dir_name_0_arr;



}; // class DriverInterface



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/DriverInterface.hpp"



#endif   // mfx_adrv_DriverInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
