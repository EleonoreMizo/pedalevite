
// Soundcard test. Directly copy the input to the output.

// g++ --std=c++11 -I. -Wall `pkg-config --cflags --libs jack` test-jack-01.cpp
// jackd -P70 -p16 -t2000 -dalsa -p64 -n3 -r44100 -s &
// ./a.out



#include <jack/jack.h>

#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h> // For Sleep()
#else
	#include <unistd.h>  // For sleep()
#endif
#include <signal.h>

#include <cstdio>
#include <cstdlib>



static ::jack_client_t * volatile MAIN_client_ptr = 0;



static void MAIN_signal_handler (int sig)
{
	::jack_client_close (MAIN_client_ptr);
	fprintf (stderr, "Signal %d received, exiting ...\n", sig);
	exit (0);
}



class Context
{
public:
	::jack_client_t *
	               _client_ptr;
	::jack_port_t *               // [in/out] [chn]
	               _mfx_port_arr [2] [2];
};



static void MAIN_jack_shutdown (void *arg)
{
	exit (0);
}



static int MAIN_process (::jack_nframes_t nbr_spl, void *arg)
{
	Context &      ctx = *reinterpret_cast <Context *> (arg);

	const jack_default_audio_sample_t * src_arr [2];
	jack_default_audio_sample_t *       dst_arr [2];
	for (int chn = 0; chn < 2; ++chn)
	{
		src_arr [chn] = reinterpret_cast <const jack_default_audio_sample_t *> (
			::jack_port_get_buffer (ctx._mfx_port_arr [0] [chn], nbr_spl)
		);
		dst_arr [chn] = reinterpret_cast <jack_default_audio_sample_t *> (
			::jack_port_get_buffer (ctx._mfx_port_arr [1] [chn], nbr_spl)
		);
	}

	for (int chn = 0; chn < 2; ++chn)
	{
		for (int pos = 0; pos < int (nbr_spl); ++pos)
		{
			float          val = src_arr [chn] [pos];
			dst_arr [chn] [pos] = val;
		}
	}

	return 0;
}


int main (int argc, char *argv [])
{
	int            ret_val = 0;

	::jack_status_t   status = ::JackServerFailed;
	MAIN_client_ptr = jack_client_open (
		"MultiFX",
		::JackNullOption,
		&status,
		0
	);
	::jack_client_t * client_ptr = MAIN_client_ptr;
	if (client_ptr == 0)
	{
		fprintf (
			stderr,
			"jack_client_open() failed, status = 0x%2.0x\n",
			status
		);
		if ((status & ::JackServerFailed) != 0)
		{
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		ret_val = -1;
	}

	Context        ctx;
	ctx._client_ptr = client_ptr;

	static const ::JackPortFlags port_dir [2] =
	{
		::JackPortIsInput, ::JackPortIsOutput
	};
	static const char *  ext_port_dir_name_0_arr [2] =
	{
		"capture", "playback"
	};

	if (ret_val == 0)
	{
		if ((status & ::JackServerStarted) != 0)
		{
			fprintf (stderr, "JACK server started\n");
		}
		if ((status & ::JackNameNotUnique) != 0)
		{
			const char *   name_0 = ::jack_get_client_name (client_ptr);
			fprintf (stderr, "Unique name \"%s\" assigned\n", name_0);
		}

		::jack_set_process_callback (client_ptr, MAIN_process, &ctx);
		::jack_on_shutdown (client_ptr, MAIN_jack_shutdown, &ctx);

		static const char *  port_name_0_arr [2] [2] =
		{
			{ "Input L", "Input R" }, { "Output L", "Output R" }
		};
		for (int dir = 0; dir < 2 && ret_val == 0; ++dir)
		{
			for (int chn = 0; chn < 2 && ret_val == 0; ++chn)
			{
				ctx._mfx_port_arr [dir] [chn] = ::jack_port_register (
					client_ptr,
					port_name_0_arr [dir] [chn],
					JACK_DEFAULT_AUDIO_TYPE,
					port_dir [dir],
					0
				);
				if (ctx._mfx_port_arr [dir] [chn] == 0)
				{
					fprintf (stderr, "No more JACK ports available.\n");
					ret_val = -1;
				}
			}
		}
	}

	if (ret_val == 0)
	{
		ret_val = ::jack_activate (client_ptr);
		if (ret_val != 0)
		{
			fprintf (stderr, "cannot activate client, returned %d.\n", ret_val);
		}
	}

	for (int dir = 0; dir < 2 && ret_val == 0; ++dir)
	{
		const char **  port_0_arr = ::jack_get_ports (
			client_ptr,
			0,
			0,
			::JackPortIsPhysical | port_dir [1 - dir]
		);
		if (port_0_arr == 0)
		{
			fprintf (
				stderr,
				"No physical %s port available.\n",
				ext_port_dir_name_0_arr [dir]
			);
			ret_val = -1;
		}
		else
		{
			fprintf (stderr, "Available ports for %s:\n", ext_port_dir_name_0_arr [dir]);
			for (int index = 0; port_0_arr [index] != 0; ++index)
			{
				fprintf (stderr, "%d: %s\n", index, port_0_arr [index]);
			}

			for (int chn = 0; chn < 2 && ret_val == 0; ++chn)
			{
				if (port_0_arr [chn] == 0)
				{
					fprintf (
						stderr,
						"Not enough physical %s port available.\n",
						ext_port_dir_name_0_arr [dir]
					);
					ret_val = -1;
				}
				else
				{
					const char * inout_0 [2];
					inout_0 [1 - dir] = ::jack_port_name (ctx._mfx_port_arr [dir] [chn]);
					inout_0 [    dir] = port_0_arr [chn];
					ret_val = ::jack_connect (client_ptr, inout_0 [0], inout_0 [1]);
					if (ret_val != 0)
					{
						fprintf (
							stderr,
							"Cannot connect to %s port %d.\n",
							ext_port_dir_name_0_arr [dir],
							chn
						);
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
		signal (SIGINT,  MAIN_signal_handler);
		signal (SIGTERM, MAIN_signal_handler);
#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
		signal (SIGABRT, MAIN_signal_handler);
#else
		signal (SIGQUIT, MAIN_signal_handler);
		signal (SIGHUP,  MAIN_signal_handler);
#endif

		fprintf (stderr, "Now running...\n");
	}

	while (ret_val == 0)
	{
#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
			::Sleep (1000);
#else
			sleep (1);
#endif
	}

	if (client_ptr != 0)
	{
		::jack_client_close (client_ptr);
		MAIN_client_ptr = 0;
	}

	return ret_val;
}

