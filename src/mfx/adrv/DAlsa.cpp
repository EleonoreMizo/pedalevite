/*****************************************************************************

        DAlsa.cpp
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

#include "fstb/fnc.h"
#include "mfx/adrv/DAlsa.h"

#include <stdexcept>

#include <cassert>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DAlsa::DAlsa ()
:	_cb_ptr (0)
,	_block_size (0)
,	_sample_freq (0)
,	_chn_base_arr ({{ 0, 0 }})
,	_handle_arr ()
,	_msg_err ()
,	_buf_alig ()
{
	assert (_instance_ptr == 0);
	if (_instance_ptr != 0)
	{
		throw std::runtime_error ("mfx::adrv::DAlsa already instantiated.");
	}
	_instance_ptr = this;
}



DAlsa::~DAlsa ()
{
	stop ();
	_instance_ptr = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Ref:
// http://www.alsa-project.org/alsa-doc/alsa-lib/group___p_c_m.html
// http://www.saunalahti.fi/~s7l/blog/2005/08/21/Full%20Duplex%20ALSA
// http://jzu.blog.free.fr/public/SLAB/slab.c
int	DAlsa::do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out)
{
	int            ret_val = 0;

	sample_freq    = 0;
	max_block_size = 0;
	_cb_ptr        = &callback;
	_chn_base_arr [Dir_IN ] = chn_idx_in;
	_chn_base_arr [Dir_OUT] = chn_idx_out;

	if (driver_0 == 0)
	{
		driver_0 = "plughw:0";
	}

	static const std::array <::snd_pcm_stream_t, Dir_NBR_ELT> stream_type_arr =
	{{
		::SND_PCM_STREAM_CAPTURE, ::SND_PCM_STREAM_PLAYBACK
	}};

	for (int dir = 0; dir < Dir_NBR_ELT && ret_val == 0; ++ dir)
	{
		ret_val = ::snd_pcm_open (
			&_handle_arr [dir],
			driver_0,
			stream_type_arr [dir],
			0
		);
		if (ret_val != 0)
		{
			char           txt_0 [1023+1];
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"Cannot open capture device, returned %d",
				ret_val
			);
			_msg_err = txt_0;
		}
	}


	/*** To do ***/


	return ret_val;
}



int	DAlsa::do_start ()
{
	int            ret_val = 0;

	/*** To do ***/

	return ret_val;
}



int	DAlsa::do_stop ()
{
	int            ret_val = 0;

	/*** To do ***/

	return ret_val;
}



std::string	DAlsa::do_get_last_error () const
{
	return _msg_err;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DAlsa::process_block ()
{


	/*** To do ***/

}



DAlsa *	DAlsa::_instance_ptr = 0;



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
