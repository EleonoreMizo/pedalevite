
// g++ --std=c++11 -I. -Wall -mfpu=neon `pkg-config --cflags --libs jack` -l wiringPi -l pthread main.cpp GpioPwm.cpp mailbox.c fstb/fnc.cpp mfx/Overdrive.cpp mfx/tuner/*.cpp
// sudo jackd -P70 -p16 -t2000 -dalsa -p64 -n3 -r44100 -s &
// sudo ./a.out
//
// -march=armv8-a doesn't work with std::thread on this GCC version,
// see last comment of bug #42734 on gcc.gnu.org 

#define MAIN_API_JACK 1
#define MAIN_API_ALSA 2
#define MAIN_API MAIN_API_JACK


#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#endif

#include "fstb/AllocAlign.h"
#include "fstb/fnc.h"
#include "mfx/tuner/FreqAnalyser.h"
#include "mfx/Overdrive.h"
#include "GpioPwm.h"

#if (MAIN_API == MAIN_API_JACK)
	#include <jack/jack.h>
#elif (MAIN_API == MAIN_API_ALSA)
	#include <alsa/asoundlib.h>
#else
	#error
#endif
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>

#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
	#include <Windows.h> // For Sleep()
#else
	#include <unistd.h>  // For sleep()
#endif
#include <signal.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <memory>
#include <thread>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>



static std::unique_ptr <GpioPwm> MAIN_pwm_uptr;
static const int  MAIN_nbr_led = 3;
static const int  MAIN_led_pin_arr [MAIN_nbr_led] = { 11, 13, 15};
static const int  MAIN_led_cycle = 10 * 1000; // in microseconds

static const int  MAIN_pin_reset     = 18;
static const int	MAIN_pin_interrupt = 8;
static const int  MAIN_i2c_dev_23017 = 0x20 + 0;   // Slave address, p. 8
static const int64_t MAIN_antibounce_time = 30 * 1000 * 1000; // Nanoseconds

// IOCON.BANK = 0
enum Cmd : uint8_t
{
	Cmd_IODIRA = 0x00,
	Cmd_IODIRB,
	Cmd_IPOLA,
	Cmd_IPOLB,
	Cmd_GPINTENA,
	Cmd_GPINTENB,
	Cmd_DEFVALA,
	Cmd_DEFVALB,
	Cmd_INTCONA,
	Cmd_INTCONB,
	Cmd_IOCONA,
	Cmd_IOCONB,
	Cmd_GPPUA,
	Cmd_GPPUB,
	Cmd_INTFA,
	Cmd_INTFB,
	Cmd_INTCAPA,
	Cmd_INTCAPB,
	Cmd_GPIOA,
	Cmd_GPIOB,
	Cmd_OLATA,
	Cmd_OLATB
};

enum IOCon : uint8_t
{
	IOCon_BANK   = 0x80,
	IOCon_MIRROR = 0x40,
	IOCon_SEQOP  = 0x20,
	IOCon_DISSLW = 0x10,
	IOCon_HAEN   = 0x08,
	IOCon_ODR    = 0x04,
	IOCon_INTPOL = 0x02
};

static const int  MAIN_adc_port = 0;



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



class ButtonState
{
public:
	bool           _flag      = false;
	int64_t        _time_last = 0;
};

class Context
{
public:
#if (MAIN_API == MAIN_API_JACK)
	::jack_client_t *
	               _client_ptr;
	::jack_port_t *               // [in/out] [chn]
	               _mfx_port_arr [2] [2];
#elif (MAIN_API == MAIN_API_ALSA)
	snd_pcm_t *    _handle_in;
	snd_pcm_t *    _handle_out;
	/*** To do ***/
#else
	#error
#endif

	int64_t        _time_beg = MAIN_get_time ();
	int64_t        _time_end = _time_beg;
	std::atomic <float>            // Negative: the main thread read the value.
	               _usage_max;
	std::atomic <float>            // Negative: the main thread read the value.
	               _usage_min;
	mfx::tuner::FreqAnalyser
	               _freq_analyser;
	std::atomic <float>
	               _detected_freq;
	const int      _tuner_subspl = 4;
	std::atomic <bool>
	               _tuner_flag;
	std::atomic <bool>
	               _disto_flag;
	std::atomic <float>
	               _disto_gain;
	mfx::Overdrive _disto;
	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	// Not for the audio thread
	int            _handle_port_exp = -1;
	std::array <ButtonState, 16>
	               _but_state_arr;
	std::atomic <bool>
	               _quit_flag;
};

static Context * volatile MAIN_context_ptr = 0;



#if (MAIN_API == MAIN_API_JACK)

static ::jack_client_t * volatile MAIN_client_ptr = 0;



static void MAIN_signal_handler (int sig)
{
	MAIN_context_ptr = 0;
	::jack_client_close (MAIN_client_ptr);
	if (MAIN_pwm_uptr.get () != 0)
	{
		MAIN_pwm_uptr.reset ();
	}
	fprintf (stderr, "\nSignal %d received, exiting ...\n", sig);
	exit (0);
}



static void MAIN_jack_shutdown (void *arg)
{
	exit (0);
}

#endif



// Returns -1 on error
// Valid results are in range 0-1023
static int MAIN_read_adc (int port, int chn)
{
	assert (port >= 0);
	assert (port < 2);
	assert (chn >= 0);
	assert (chn < 8);

	// MCP doc, p. 21
	// We could shift everything from 7 bits to improve the latency.
	const int      msg_len = 3;
	uint8_t        buffer [msg_len] =
	{
		0x01,
		uint8_t (chn << 4),
		0
	};

	int            ret_val = ::wiringPiSPIDataRW (port, &buffer [0], msg_len);
	if (ret_val == -1)
	{
		fprintf (stderr, "Error reading the ADC. errno = %d\n", errno);
	}
	else
	{
		ret_val = ((buffer [1] & 3) << 8) + buffer [2];
	}

	return (ret_val);
}



static void MAIN_physical_input_thread (Context &ctx)
{
	while (! ctx._quit_flag.load ())
	{
		// Input buttons for the MCP23017
		uint16_t       state_all =
			::wiringPiI2CReadReg16 (ctx._handle_port_exp, Cmd_GPIOA);
		const int64_t  cur_time  = MAIN_get_time ();

		// Update with anti-bounce system
		for (int i = 0; i < 16; ++i)
		{
			ButtonState &  state_prev = ctx._but_state_arr [i];
			const bool     cur_flag   = (((state_all >> i) & 1) != 0);
			if (cur_flag != state_prev._flag)
			{
				const int64_t  dist = cur_time - state_prev._time_last;
				if (dist >= MAIN_antibounce_time)
				{
					state_prev._flag      = cur_flag;
					state_prev._time_last = cur_time;

					switch (i)
					{
					case  0: // Tuner toggle
						if (! cur_flag)
						{
							ctx._tuner_flag.store (! ctx._tuner_flag.load ());
						}
						break;
					case  1: // Distortion toggle
						if (! cur_flag)
						{
							ctx._disto_flag.store (! ctx._disto_flag.load ());
						}
						break;
					}
				}
			}
		}

		// Reads the ADC
		const int      val = MAIN_read_adc (MAIN_adc_port, 0);
		if (val < 0)
		{
			fprintf (stderr, "Error while reading the ADC.\n");
		}
		else
		{
			const float    pos  = float (val) / 0x3FF;
			const float    gain = std::max (pos * pos * 1000, 0.01f);
			ctx._disto_gain.store (gain);
		}

		// 10 ms between updates
		::delay (10);
	}
}



int MAIN_main_loop (Context &ctx)
{
	int            ret_val = 0;

	while (ret_val == 0)
	{
		const float    usage_max = ctx._usage_max.exchange (-1);
		const float    usage_min = ctx._usage_min.exchange (-1);
		const float    freq  = ctx._detected_freq.load ();
		char           cpu_0 [127+1] = "Time usage: ------ % / ------ %";
		if (usage_max >= 0 && usage_min >= 0)
		{
			sprintf (cpu_0, "Time usage: %6.2f %% / %6.2f %%", usage_min * 100, usage_max * 100);
		}

		char           freq_0 [127+1] = "Note: ---- ---- ------- Hz";
		float          lum_arr [MAIN_nbr_led] = { 0, 0, 0 };
		if (freq > 0)
		{
			const float    midi_pitch = log2 (freq / 220) * 12 - 3 + 60;
			const int      midi_note  = fstb::round_int (midi_pitch);
			const float    cents_dbl  = (midi_pitch - midi_note) * 100;
			const int      cents      = fstb::round_int (cents_dbl);
			const int      octave     = midi_note / 12;
			const int      note       = midi_note - octave * 12;
			static const char * const note_0_arr [12] =
			{
				"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
			};

			sprintf (freq_0, "Note: %2s%-2d %+4d %7.3lf Hz", note_0_arr [note], octave, cents, freq);

			const int      mid_index = (MAIN_nbr_led - 1) / 2;
			const float    cents_abs = fabs (cents_dbl);
			lum_arr [mid_index    ] = std::max (5 - cents_abs, 0.0f) * (1.0f / 5);

			const float    lum      = fstb::limit (cents_abs * (1.0f / 25), 0.0f, 1.0f);
			lum_arr [mid_index - 1] = (cents_dbl < 0) ? lum : 0;
			lum_arr [mid_index + 1] = (cents_dbl > 0) ? lum : 0;
		}

		for (int led_index = 0; led_index < MAIN_nbr_led; ++led_index)
		{
			const int       w = fstb::round_int (MAIN_led_cycle * (lum_arr [led_index] * 1.0f + 0.0f));
			MAIN_pwm_uptr->set_pulse (0, MAIN_led_pin_arr [led_index], 0, w);
		}

		char param_0 [255+1];
		const bool      tuner_flag = ctx._tuner_flag.load ();
		const bool      disto_flag = ctx._disto_flag.load ();
		const float     disto_gain = ctx._disto_gain.load ();
		sprintf (
			param_0,
			"[%s] [%s] [%6.1f]",
			tuner_flag ? "T" : " ",
			disto_flag ? "D" : " ",
			disto_gain
		);

		fprintf (stderr, "%s %s %s\r", cpu_0, freq_0, param_0);
		fflush (stderr);

#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
		::Sleep (100);
#else
		::delay (100);
#endif
	}

	return ret_val;
}



#if (MAIN_API == MAIN_API_JACK)



static int MAIN_process (::jack_nframes_t nbr_spl, void *arg)
{
	const int64_t  time_beg    = MAIN_get_time ();
	Context &      ctx         = *reinterpret_cast <Context *> (arg);
	const int64_t  dur_tot     =      time_beg - ctx._time_beg;
	const int64_t  dur_act     = ctx._time_end - ctx._time_beg;
	const float    usage_frame = float (dur_act) / float (dur_tot);
	float          usage_max   = ctx._usage_max.load ();
	float          usage_min   = ctx._usage_min.load ();
	usage_max = std::max (usage_max, usage_frame);
	usage_min = (usage_min < 0) ? usage_frame : std::min (usage_min, usage_frame);
	ctx._usage_max.store (usage_max);
	ctx._usage_min.store (usage_min);
	ctx._time_beg = time_beg;
	
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

	if (ctx._disto_flag.load ())
	{
		float             gain = ctx._disto_gain.load ();
		ctx._disto.set_gain (gain);
		for (int chn = 0; chn < 2; ++chn)
		{
			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				ctx._buf_alig [pos] = src_arr [chn] [pos];
			}
			ctx._disto.process_block (&ctx._buf_alig [0], nbr_spl);
			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				dst_arr [chn] [pos] = ctx._buf_alig [pos];
			}
		}
	}
	else
	{
		for (int chn = 0; chn < 2; ++chn)
		{
			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				float          val = src_arr [chn] [pos];
				dst_arr [chn] [pos] = val;
			}
		}
	}

	float           freq = 0;
	if (ctx._tuner_flag.load ())
	{
		if (ctx._tuner_subspl == 1)
		{
			freq = ctx._freq_analyser.process_block (&src_arr [0] [0], nbr_spl);
		}
		else
		{
			const int       buf_size = 1024;
			std::array <float, buf_size>   buf;
			int             work_pos = 0;
			while (work_pos < int (nbr_spl))
			{
				const int       work_len =
					std::min (int (nbr_spl) - work_pos, buf_size * ctx._tuner_subspl);
				const int       len_ss = work_len / ctx._tuner_subspl;
				for (int i = 0; i < len_ss; ++i)
				{
					float            sum  = 0;
					const int        base = work_pos + i * ctx._tuner_subspl;
					for (int j = 0; j < ctx._tuner_subspl; ++j)
					{
						sum += src_arr [0] [base + j];
					}
					buf [i] = sum;
				}
				freq = ctx._freq_analyser.process_block (&buf [0], len_ss);
			
				work_pos += work_len;
			}
		}
		ctx._detected_freq.store (freq);
		for (int chn = 0; chn < 2; ++chn)
		{
			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				dst_arr [chn] [pos] = 0;
			}
		}
	}

	ctx._time_end = MAIN_get_time ();

	return 0;
}



int main (int argc, char *argv [])
{
	int            ret_val = 0;

	::wiringPiSetupPhys ();

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
	ctx._usage_min.store (-1);
	ctx._usage_max.store (-1);
	ctx._detected_freq.store (0);
	const double   sample_freq = double (jack_get_sample_rate (client_ptr));
	ctx._freq_analyser.set_sample_freq (sample_freq / ctx._tuner_subspl);
	ctx._tuner_flag.store (false);
	ctx._disto_flag.store (false);
	ctx._disto_gain.store (1);
	ctx._buf_alig.resize (4096);
	ctx._quit_flag.store (false);
	
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
		fprintf (stderr, "Audio now running...\n");

		signal (SIGINT,  MAIN_signal_handler);
		signal (SIGTERM, MAIN_signal_handler);
#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
		signal (SIGABRT, MAIN_signal_handler);
#else
		signal (SIGQUIT, MAIN_signal_handler);
		signal (SIGHUP,  MAIN_signal_handler);
#endif

		// LED stuff
		for (int i = 0; i < MAIN_nbr_led; ++i)
		{
			::pinMode  (MAIN_led_pin_arr [i], OUTPUT);
			::digitalWrite (MAIN_led_pin_arr [i], LOW);
		}
		MAIN_pwm_uptr = std::unique_ptr <GpioPwm> (new GpioPwm (100)); // Resolution: 0.1 ms
		ret_val = MAIN_pwm_uptr->init_chn (0, MAIN_led_cycle); // Cycle: 10 ms
		if (ret_val != 0)
		{
			fprintf (stderr, "Error while initializing the PWM\n");
		}
	}
	
	if (ret_val == 0)
	{
		// Port expander stuff
		::pinMode (MAIN_pin_interrupt, INPUT );
		::pinMode (MAIN_pin_reset    , OUTPUT);

		::digitalWrite (MAIN_pin_reset, LOW);
		::delay (100);
		::digitalWrite (MAIN_pin_reset, HIGH);
		::delay (100);

		ctx._handle_port_exp = ::wiringPiI2CSetup (MAIN_i2c_dev_23017);
		if (ctx._handle_port_exp == -1)
		{
			ret_val = errno;
			fprintf (stderr, "Error initializing I2C. errno = %d\n", ret_val);
		}
	}
	if (ret_val == 0)
	{
		::wiringPiI2CWriteReg8 (ctx._handle_port_exp, Cmd_IOCONA, IOCon_MIRROR);

		// All the pins are set in read mode.
		::wiringPiI2CWriteReg16 (ctx._handle_port_exp, Cmd_IODIRA, 0xFFFF);

		// All the pins will cause an interrupt on input change
		::wiringPiI2CWriteReg16 (ctx._handle_port_exp, Cmd_INTCONA , 0x0000);
		::wiringPiI2CWriteReg16 (ctx._handle_port_exp, Cmd_GPINTENA, 0xFFFF);

#if 0 // Doesn't work well. We use polling instead.
		ret_val = ::wiringPiISR (MAIN_pin_interrupt, INT_EDGE_BOTH, &MAIN_button_interrupt_cb);
		if (ret_val != 0)
		{
			fprintf (stderr, "Failed to initialize an interrupt handler. Returned %d\n", ret_val);
		}
#endif
	}

	// ADC
	int            adc_handle = -1;
	if (ret_val == 0)
	{
		adc_handle = ::wiringPiSPISetup  (MAIN_adc_port, 1000000);   // SPI clock: 1 MHz
		if (adc_handle == -1)
		{
			ret_val = errno;
			fprintf (stderr, "Error initializing SPI. errno = %d\n", ret_val);
		}
	}
	
	// User input thread
	std::thread     user_input_thread;
	if (ret_val == 0)
	{
		user_input_thread = std::thread (MAIN_physical_input_thread, std::ref (ctx));
	}

	if (ret_val == 0)
	{
		MAIN_context_ptr = &ctx;

		ret_val = MAIN_main_loop (ctx);
		
		ctx._quit_flag.store (true);
		user_input_thread.join ();
	}

	MAIN_context_ptr = 0;
	if (adc_handle != -1)
	{
		close (adc_handle);
		adc_handle = -1;
	}
	if (ctx._handle_port_exp != -1)
	{
		close (ctx._handle_port_exp);
		ctx._handle_port_exp = -1;
	}
	if (client_ptr != 0)
	{
		::jack_client_close (client_ptr);
		MAIN_client_ptr = 0;
	}

	return ret_val;
}



#elif (MAIN_API == MAIN_API_ALSA)



// Ref:
// http://www.saunalahti.fi/~s7l/blog/2005/08/21/Full%20Duplex%20ALSA
// http://jzu.blog.free.fr/public/SLAB/slab.c


int MAIN_config_alsa (::snd_pcm_t *handle)
{
	int            ret_val = 0;

	const int      nbr_chn     = 2;
	const int      sample_freq = 44100;

	return ret_val;
}



int main (int argc, char *argv [])
{
	int            ret_val = 0;

	Context        ctx;

	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_open (
			&ctx._handle_in,
			"plughw:0",
			SND_PCM_STREAM_CAPTURE,
			0
		);
		if (ret_val != 0)
		{
			fprintf (stderr, "Error: cannot open capture device, returned %d.\n", ret_val);
		}
	}
	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_open (
			&ctx._handle_out,
			"plughw:0",
			SND_PCM_STREAM_PLAYBACK,
			0
		);
		if (ret_val != 0)
		{
			fprintf (stderr, "Error: cannot open playback device, returned %d.\n", ret_val);
		}
	}


	/*** To do ***/

	return ret_val;
}



#else // MAIN_API
	#error
#endif



