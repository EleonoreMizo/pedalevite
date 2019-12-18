/*****************************************************************************

        DJack.cpp
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
#include "mfx/adrv/DJack.h"

#include <signal.h>

#include <stdexcept>

#include <cassert>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DJack::DJack ()
:	_cb_ptr (0)
,	_chn_base_arr ({{ 0, 0 }})
,	_client_ptr (0)
,	_mfx_port_arr ()
,	_status (::JackServerFailed)
,	_msg_err ()
{
	assert (_instance_ptr == 0);
	if (_instance_ptr != 0)
	{
		throw std::runtime_error ("mfx::adrv::DJack already instantiated.");
	}
	_instance_ptr = this;
}



DJack::~DJack ()
{
	stop ();
	_instance_ptr = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DJack::do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out)
{
	fstb::unused (driver_0);

	int            ret_val = 0;

	_msg_err.clear ();
	sample_freq    = 44100;
	max_block_size = 4096;
	_cb_ptr        = &callback;
	_chn_base_arr [Dir_IN ] = chn_idx_in;
	_chn_base_arr [Dir_OUT] = chn_idx_out;

	_client_ptr = ::jack_client_open (
		"Pedalevite", ::JackNullOption, &_status, 0
	);
	if (_client_ptr == 0)
	{
		char           txt_0 [1023+1];
		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"jack_client_open() failed, status = 0x%2.0x",
			_status
		);
		_msg_err = txt_0;
		if ((_status & ::JackServerFailed) != 0)
		{
			_msg_err += ". Unable to connect to JACK server";
		}
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		sample_freq    = ::jack_get_sample_rate (_client_ptr);
		max_block_size = 4096;
	}

	return ret_val;
}



int	DJack::do_start ()
{
	int            ret_val = 0;

	_msg_err.clear ();

	static const std::array <::JackPortFlags, Dir_NBR_ELT>   port_dir =
	{{
		::JackPortIsInput, ::JackPortIsOutput
	}};

	if (ret_val == 0)
	{
		if ((_status & ::JackServerStarted) != 0)
		{
			fprintf (stderr, "JACK server started\n");
		}
		if ((_status & ::JackNameNotUnique) != 0)
		{
			const char *   name_0 = ::jack_get_client_name (_client_ptr);
			fprintf (stderr, "Unique name \"%s\" assigned\n", name_0);
		}

		::jack_set_process_callback (_client_ptr, &process_jack, this);
		::jack_set_xrun_callback (_client_ptr, &notify_audio_dropout, this);
		::jack_on_shutdown (_client_ptr, &jack_shutdown, this);

		static const char *  port_name_0_arr [Dir_NBR_ELT] [_nbr_chn] =
		{
			{ "Input L", "Input R" }, { "Output L", "Output R" }
		};
		for (int dir = 0; dir < Dir_NBR_ELT && ret_val == 0; ++dir)
		{
			for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
			{
				_mfx_port_arr [dir] [chn] = ::jack_port_register (
					_client_ptr,
					port_name_0_arr [dir] [chn],
					JACK_DEFAULT_AUDIO_TYPE,
					port_dir [dir],
					0
				);
				if (_mfx_port_arr [dir] [chn] == 0)
				{
					_msg_err = "No more JACK ports available.";
					ret_val = -1;
				}
			}
		}
	}

	if (ret_val == 0)
	{
		ret_val = ::jack_activate (_client_ptr);
		if (ret_val != 0)
		{
			char           txt_0 [1023+1];
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"cannot activate client, returned %d.",
				ret_val
			);
			_msg_err = txt_0;
		}
	}

	for (int dir = 0; dir < Dir_NBR_ELT && ret_val == 0; ++dir)
	{
		const char **  port_0_arr = ::jack_get_ports (
			_client_ptr,
			0,
			0,
			::JackPortIsPhysical | port_dir [1 - dir]
		);
		if (port_0_arr == 0)
		{
			char           txt_0 [1023+1];
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"No physical %s port available.",
				_dir_name_0_arr [dir]
			);
			_msg_err = txt_0;
			ret_val = -1;
		}
		else
		{
			fprintf (stderr, "Available ports for %s:\n", _dir_name_0_arr [dir]);
			for (int index = 0; port_0_arr [index] != 0; ++index)
			{
				fprintf (stderr, "%d: %s\n", index, port_0_arr [index]);
			}

			for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
			{
				const int      chn_jack = chn + _chn_base_arr [dir];
				if (port_0_arr [chn_jack] == 0)
				{
					char           txt_0 [1023+1];
					fstb::snprintf4all (
						txt_0, sizeof (txt_0),
						"Not enough physical %s port available.",
						_dir_name_0_arr [dir]
					);
					_msg_err = txt_0;
					ret_val = -1;
				}
				else
				{
					const char * inout_0 [2];
					inout_0 [1 - dir] = ::jack_port_name (_mfx_port_arr [dir] [chn]);
					inout_0 [    dir] = port_0_arr [chn_jack];
					ret_val = ::jack_connect (_client_ptr, inout_0 [0], inout_0 [1]);
					if (ret_val != 0)
					{
						char           txt_0 [1023+1];
						fstb::snprintf4all (
							txt_0, sizeof (txt_0),
							"Cannot connect to %s port %d.",
							_dir_name_0_arr [dir],
							chn
						);
						_msg_err = txt_0;
					}
				}
			}
		}

		if (port_0_arr != 0)
		{
			::jack_free (port_0_arr);
		}
	}

	if (ret_val == 0)
	{
		fprintf (stderr, "Audio now running...\n");

		signal (SIGINT,  &signal_handler);
		signal (SIGTERM, &signal_handler);
#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
		signal (SIGABRT, &signal_handler);
#else
		signal (SIGQUIT, &signal_handler);
		signal (SIGHUP,  &signal_handler);
#endif
	}

	return ret_val;
}



void	DJack::do_restart ()
{
	/*** To do ***/
}



int	DJack::do_stop ()
{
	_msg_err.clear ();

	if (_client_ptr != 0)
	{
		::jack_client_close (_client_ptr);
		_client_ptr = 0;
	}

	return 0;
}



std::string	DJack::do_get_last_error () const
{
	return _msg_err;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DJack::process_block (int nbr_spl)
{
	std::array <const jack_default_audio_sample_t *, _nbr_chn>  src_arr;
	std::array <      jack_default_audio_sample_t *, _nbr_chn>  dst_arr;
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		src_arr [chn] = reinterpret_cast <const jack_default_audio_sample_t *> (
			::jack_port_get_buffer (_mfx_port_arr [0] [chn], nbr_spl)
		);
		dst_arr [chn] = reinterpret_cast <jack_default_audio_sample_t *> (
			::jack_port_get_buffer (_mfx_port_arr [1] [chn], nbr_spl)
		);
	}

	_cb_ptr->process_block (&dst_arr [0], &src_arr [0], nbr_spl);
}



void	DJack::signal_handler (int sig)
{
	fprintf (stderr, "\nSignal %d received, exiting...\n", sig);
	_instance_ptr->_cb_ptr->request_exit ();
}



void	DJack::jack_shutdown (void *arg)
{
	fprintf (stderr, "\nJack exited, exiting too...\n");
	DJack &        obj = *reinterpret_cast <DJack *> (arg);
	obj._cb_ptr->request_exit ();
}



int	DJack::notify_audio_dropout (void *arg)
{
	DJack &        obj = *reinterpret_cast <DJack *> (arg);
	obj._cb_ptr->notify_dropout ();

	return 0;
}



int	DJack::process_jack (::jack_nframes_t nbr_spl, void *arg)
{
	DJack &        obj = *reinterpret_cast <DJack *> (arg);
	obj.process_block (nbr_spl);

	return 0;
}



DJack *	DJack::_instance_ptr = 0;



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
