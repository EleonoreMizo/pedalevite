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
#include "mfx/adrv/CbInterface.h"
#include "mfx/adrv/DAlsa.h"

#include <pthread.h>
#include <sched.h>

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
,	_block_size_alig (0)
,	_thread_audio ()
,	_stop_flag (false)
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
	_instance_ptr = 0;

	stop ();

	for (int dir = 0; dir < Dir_NBR_ELT && ret_val == 0; ++ dir)
	{
		if (_handle_arr [dir] != 0)
		{
			::snd_pcm_close (_handle_arr [dir]);
			_handle_arr [dir] = 0;
		}
	}
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
				"Cannot open %s device, returned %d",
				_dir_name_0_arr [dir],
				ret_val
			);
			_msg_err = txt_0;
		}
	}

	for (int dir = 0; dir < Dir_NBR_ELT && ret_val == 0; ++ dir)
	{
		ret_val = configure_alsa_audio (dir);
	}

	if (ret_val == 0)
	{
		_block_size_alig = (_block_size + 3) >> 2;
		_buf_alig.resize (_block_size_alig * _nbr_chn * Dir_NBR_ELT);

		sample_freq    = _sample_freq;
		max_block_size = _block_size;
	}

	return ret_val;
}



int	DAlsa::do_start ()
{
	int            ret_val = 0;

	for (int dir = 0; dir < Dir_NBR_ELT && ret_val == 0; ++ dir)
	{
		ret_val = ::snd_pcm_prepare (_handle_arr [dir]);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: cannot start %s stream (%s).\n",
				_dir_name_0_arr [dir],
				::snd_strerror (ret_val)
			);
		}
	}

	const int      policy   = SCHED_FIFO;
	int            max_prio = 0;
	if (ret_val == 0)
	{
		_stop_flag    = false;
		_thread_audio = std::thread (&DAlsa::process_audio, this);

		max_prio = ::sched_get_priority_max (policy);
		if (max_prio < 0)
		{
			ret_val = max_prio;
			fprintf (
				stderr,
				"Error: cannot retrieve the maximum priority value.\n"
			);
		}
	}
	if (ret_val == 0)
	{
		::sched_param  tparam;
		memset (&tparam, 0, sizeof (tparam));
		tparam.sched_priority = max_prio - 1;

		ret_val = ::pthread_setschedparam (
			_thread_audio.native_handle (),
			policy,
			&tparam
		);
		if (ret_val != 0)
		{
			fprintf (stderr, "Error: cannot set thread priority.\n");
		}
	}

	return ret_val;
}



int	DAlsa::do_stop ()
{
	int            ret_val = 0;

	if (_thread_audio.joinable ())
	{
		_quit_flag = true;
		_thread_audio.join ();
	}

	for (int dir = 0; dir < Dir_NBR_ELT; ++ dir)
	{
		ret_val = ::snd_pcm_drop (_handle_arr [dir]);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: cannot stop %s stream (%s).\n",
				_dir_name_0_arr [dir],
				::snd_strerror (ret_val)
			);
		}
	}

	return ret_val;
}



std::string	DAlsa::do_get_last_error () const
{
	return _msg_err;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DAlsa::configure_alsa_audio (int dir)
{
	int            ret_val = 0;

	fprintf (stderr, "Configuring %s stream...\n", _dir_name_0_arr [dir]);

	// Allocate memory for hardware parameter structure
	::snd_pcm_hw_params_t * hw_params_ptr = 0;
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_hw_params_malloc (&hw_params_ptr);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: cannot allocate snd_pcm_hw_params_t structure (%s).\n",
				::snd_strerror (ret_val)
			);
		}
	}

	// Fill structure from current audio parameters
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_hw_params_any (_handle_arr [dir], hw_params_ptr);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: cannot initialize snd_pcm_hw_params_t structure (%s)\n",
				::snd_strerror (ret_val)
			);
		}
	}

	// Access type
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_hw_params_set_access (
			_handle_arr [dir], hw_params_ptr, SND_PCM_ACCESS_RW_NONINTERLEAVED
		);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: access type not available (%s)\n",
				::snd_strerror (ret_val)
			);
		}
	}

	// Sample format
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_hw_params_set_format (
			_handle_arr [dir], hw_params_ptr, SND_PCM_FORMAT_FLOAT
		);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: sample format not available (%s)\n"
				::snd_strerror (ret_val)
			);
		}
	}

	// Sampling rate
	if (ret_val == 0)
	{
		unsigned int   rate = 44100;
		ret_val = ::snd_pcm_hw_params_set_rate_near (
			_handle_arr [dir], hw_params_ptr, &rate, 0
		);
		if (ret_val == 0)
		{
			_sample_freq = rate;
			fprintf (stderr, "Samling rate: %u Hz\n", rate);
		}
		else
		{
			fprintf (
				stderr,
				"Error: sampling rate not available (%s)\n"
				::snd_strerror (ret_val)
			);
		}
	}

	// Number of channels
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_hw_params_set_channels (
			_handle_arr [dir], hw_params_ptr, _nbr_chn
		);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: channels not available (%s)\n"
				::snd_strerror (ret_val)
			);
		}
	}

	// Periods
	if (ret_val == 0)
	{
		unsigned int   nper = _tgt_nbr_periods;
		ret_val = ::snd_pcm_hw_params_set_periods_near (
			_handle_arr [dir], hw_params_ptr, &nper, 0
		);
		if (ret_val == 0)
		{
			fprintf (stderr, "Number of periods: %d\n", int (nper));
		}
		else
		{
			fprintf (
				stderr,
				"Error: cannot set the number of periods (%s)\n"
				::snd_strerror (ret_val)
			);
		}
	}

	// Period size
	if (ret_val == 0)
	{
		::snd_pcm_uframes_t  per_size = _tgt_period_size;
		ret_val = ::snd_pcm_hw_params_set_period_size_near (
			_handle_arr [dir], hw_params_ptr, &per_size, 0
		);
		if (ret_val == 0)
		{
			_block_size = int (per_size);
			fprintf (stderr, "Period size: %d samples\n", int (per_size));
		}
		else
		{
			fprintf (
				stderr,
				"Error: cannot set the period size (%s)\n"
				::snd_strerror (ret_val)
			);
		}
	}

	// Apply settings
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_hw_params (_handle_arr [dir], hw_params_ptr);
		if (ret_val != 0)
		{
			fprintf (
				stderr,
				"Error: cannot set hardware parameters (%s)\n"
				::snd_strerror (ret_val)
			);
		}
	}

	if (hw_params_ptr != 0)
	{
		::snd_pcm_hw_params_free (hw_params_ptr);
	}

	return ret_val
}



void	DAlsa::process_audio ()
{
	while (! _stop_flag)
	{
		process_block ();
	}

	_stop_flag = false;
}



void	DAlsa::process_block ()
{
	std::array <std::array <float *, _nbr_chn>, Dir_NBR_ELT> buf_ptr_arr;
	const int      ofs_r    = 0;
	const int      ofs_w    = _nbr_chn * _block_size_alig;

	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		const int      ofs_c = chn * _block_size_alig;
		buf_ptr_arr [Dir_IN ] [chn] = &_buf_alig [ofs_r + ofs_c];
		buf_ptr_arr [Dir_OUT] [chn] = &_buf_alig [ofs_w + ofs_c];
	}

	bool           ok_flag = false;
	while (! ok_flag && ! _stop_flag)
	{
		const int      ret_val = ::snd_pcm_readn (
			_handle_arr [Dir_IN ], &buf_ptr_arr [Dir_IN ] [0], _block_size
		);
		if (ret_val == -EAGAIN)
		{
			// Try again
		}
		else if (ret_val == -EBADFD)
		{
			// Stop?
		}
		else if (ret_val == -EPIPE)
		{
			_cb_ptr->notify_dropout ();
			::snd_pcm_prepare (_handle_arr [Dir_IN ]);
		}
		else if (ret_val == -ESTRPIPE)
		{
			// Wait?
		}
		else if (ret_val >= 0)
		{
			if (ret_val != _block_size)
			{
				// Short read...
			}
			else
			{
				ok_flag = true;
			}
		}
	}

	if (! _stop_flag)
	{
		_cb_ptr->process_block (
			const_cast <      float * const *> (&buf_ptr_arr [Dir_OUT] [0]),
			const_cast <const float * const *> (&buf_ptr_arr [Dir_IN ] [0]),
			_block_size
		);
	}

	ok_flag = false;
	while (! ok_flag && ! _stop_flag)
	{
		const int      ret_val = ::snd_pcm_writen (
			_handle_arr [Dir_OUT], &buf_ptr_arr [Dir_OUT] [0], _block_size
		);
		if (ret_val == -EAGAIN)
		{
			// Try again
		}
		else if (ret_val == -EBADFD)
		{
			// Stop?
		}
		else if (ret_val == -EPIPE)
		{
			_cb_ptr->notify_dropout ();
			::snd_pcm_prepare (_handle_arr [Dir_OUT]);
		}
		else if (ret_val == -ESTRPIPE)
		{
			// Wait?
		}
		else if (ret_val >= 0)
		{
			if (ret_val != _block_size)
			{
				// Short read...
			}
			else
			{
				ok_flag = true;
			}
		}
	}
}



DAlsa *	DAlsa::_instance_ptr = 0;



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
