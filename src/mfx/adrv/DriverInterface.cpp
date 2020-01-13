/*****************************************************************************

        DriverInterface.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/adrv/DriverInterface.h"

#include <cassert>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DriverInterface::init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out)
{
	assert (driver_0 == nullptr || driver_0 [0] != '\0');
	assert (chn_idx_in  >= 0);
	assert (chn_idx_out >= 0);

	const int      ret_val = do_init (
		sample_freq,
		max_block_size,
		callback,
		driver_0,
		chn_idx_in,
		chn_idx_out
	);
	assert (ret_val != 0 || sample_freq >= 41000);
	assert (ret_val != 0 || max_block_size > 0);

	return ret_val;
}



int	DriverInterface::start ()
{
	return do_start ();
}



int	DriverInterface::stop ()
{
	return do_stop ();
}



void	DriverInterface::restart ()
{
	do_restart ();
}



std::string	DriverInterface::get_last_error () const
{
	return do_get_last_error ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const std::array <const char *, Dir_NBR_ELT>	DriverInterface::_dir_name_0_arr =
{{
	"capture", "playback"
}};



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
