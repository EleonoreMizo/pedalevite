
// g++ --std=c++11 -I. -Wall -mfpu=neon `pkg-config --cflags --libs jack` -l wiringPi -l pthread main.cpp mailbox.c fstb/fnc.cpp mfx/*.cpp mfx/tuner/*.cpp mfx/ui/*.cpp
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
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/DistoSimple.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/tuner/FreqAnalyser.h"
#include "mfx/ui/DisplayPi3Pcd8544.h"
#include "mfx/ui/Font.h"
#include "mfx/ui/FontDataDefault.h"
#include "mfx/ui/LedPi3.h"
#include "mfx/ui/UserInputPi3.h"

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
	#include <unistd.h>
#endif
#include <signal.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>



static const int  MAIN_pin_reset = 18;



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



class Context
{
public:
	std::mutex     _mutex_spi;

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
	volatile float _detected_freq = 0;
	const int      _tuner_subspl  = 4;
	volatile bool  _tuner_flag    = false;
	volatile bool  _disto_flag    = false;
	std::atomic <float>
	               _disto_gain;
	volatile float _disto_gain_nat = 1;
	mfx::pi::DistoSimple
	               _disto;
	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	// Not for the audio thread
	volatile bool	_quit_flag       = false;
	volatile bool  _input_quit_flag = false;
	mfx::ui::DisplayPi3Pcd8544
	               _display;
	mfx::ui::UserInputInterface::MsgQueue
	               _user_input_queue;
	mfx::ui::UserInputPi3
	               _user_input;
	mfx::ui::LedPi3
	               _leds;
	mfx::ui::Font  _fnt_8x12;
	               
	Context () : _display (_mutex_spi), _user_input (_mutex_spi) { }
	~Context ()
	{
		mfx::ui::UserInputInterface::MsgCell * cell_ptr = 0;
		do
		{
			cell_ptr = _user_input_queue.dequeue ();
			if (cell_ptr != 0)
			{
				_user_input.return_cell (*cell_ptr);
			}
		}
		while (cell_ptr != 0);
	}
};

static std::unique_ptr <Context>	MAIN_context_ptr;



#if (MAIN_API == MAIN_API_JACK)

static ::jack_client_t * volatile MAIN_client_ptr = 0;



static void MAIN_signal_handler (int sig)
{
	fprintf (stderr, "\nSignal %d received, exiting...\n", sig);
	MAIN_context_ptr->_quit_flag = true;
}



static void MAIN_jack_shutdown (void *arg)
{
	fprintf (stderr, "\nJack exited, exiting too...\n");
	MAIN_context_ptr->_quit_flag = true;
}

#endif



static void MAIN_physical_input_thread (Context &ctx)
{
	while (! ctx._input_quit_flag)
	{
		mfx::ui::UserInputInterface::MsgCell * cell_ptr = 0;
		do
		{
			cell_ptr = ctx._user_input_queue.dequeue ();
			if (cell_ptr != 0)
			{
				const mfx::ui::UserInputType type = cell_ptr->_val.get_type ();
				const int      index = cell_ptr->_val.get_index ();
				const float    val   = cell_ptr->_val.get_val ();
				if (type == mfx::ui::UserInputType_SW)
				{
					switch (index)
					{
					case  0: // Tuner toggle
						if (val >= 0.5f)
						{
							ctx._tuner_flag = (! ctx._tuner_flag);
						}
						break;
					case  1: // Distortion toggle
						if (val >= 0.5f)
						{
							ctx._disto_flag= (! ctx._disto_flag);
						}
						break;
					}
				}
				else if (type == mfx::ui::UserInputType_POT)
				{
					if (index == 0)
					{
						ctx._disto_gain.exchange (val);
					}
				}

				ctx._user_input.return_cell (*cell_ptr);
			}
		}
		while (cell_ptr != 0 && ! ctx._input_quit_flag);

		// 10 ms between updates
		::delay (10);
	}
}



int MAIN_main_loop (Context &ctx)
{
	int            ret_val = 0;

	uint8_t *      p_ptr  = ctx._display.use_screen_buf ();
	const int      scr_w  = ctx._display.get_width ();
	const int      scr_h  = ctx._display.get_height ();
	const int      scr_s  = ctx._display.get_stride ();
	bool           scr_clean_flag = false;
	bool           scr_rfrsh_flag  = true;

	while (ret_val == 0 && ! ctx._quit_flag)
	{
		const bool   tuner_flag = ctx._tuner_flag;
		const bool   disto_flag = ctx._disto_flag;
		const float  disto_gain = ctx._disto_gain_nat;
		const float  usage_max  = ctx._usage_max.exchange (-1);
		const float  usage_min  = ctx._usage_min.exchange (-1);
		const float  freq = (tuner_flag) ? ctx._detected_freq : 0;
		char         cpu_0 [127+1] = "Time usage: ------ % / ------ %";
		if (usage_max >= 0 && usage_min >= 0)
		{
			sprintf (cpu_0, "Time usage: %6.2f %% / %6.2f %%", usage_min * 100, usage_max * 100);
		}

		char           freq_0 [127+1] = "Note: ---- ---- ------- Hz";
		char           note3_0 [127+1] = "-";
		char           note4_0 [127+1] = "----";
		const int      nbr_led           = 3;
		float          lum_arr [nbr_led] = { 0, 0, 0 };
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

			if (octave >= 0 && octave <= 9)
			{
				sprintf (note3_0, "%s%1d", note_0_arr [note], octave);
			}
			sprintf (note4_0, "%2s%-2d", note_0_arr [note], octave);
			sprintf (freq_0, "Note: %4s %+4d %7.3lf Hz", note4_0, cents, freq);

			const int      mid_index = (nbr_led - 1) / 2;
			const float    cents_abs = fabs (cents_dbl);
			lum_arr [mid_index    ] = std::max (5 - cents_abs, 0.0f) * (1.0f / 5);

			const float    lum      = fstb::limit (cents_abs * (1.0f / 25), 0.0f, 1.0f);
			lum_arr [mid_index - 1] = (cents_dbl < 0) ? lum : 0;
			lum_arr [mid_index + 1] = (cents_dbl > 0) ? lum : 0;
		}

		for (int led_index = 0; led_index < nbr_led; ++led_index)
		{
			ctx._leds.set_led (led_index, lum_arr [led_index]);
		}

		char param_0 [255+1];
		sprintf (
			param_0,
			"[%s] [%s] [%6.1f]",
			tuner_flag ? "T" : " ",
			disto_flag ? "D" : " ",
			disto_gain
		);

		fprintf (stderr, "%s %s %s\r", cpu_0, freq_0, param_0);
		fflush (stderr);

		// Display test
		if (! scr_clean_flag)
		{
			memset (p_ptr, 0, scr_s * scr_h);
			scr_clean_flag = true;
			scr_rfrsh_flag = true;
		}
		if (tuner_flag)
		{
			const int      char_w  = ctx._fnt_8x12.get_char_w ();
			const int      char_h  = ctx._fnt_8x12.get_char_h ();
			const int      txt_len = int (strlen (note3_0));
			const int      mag_x   = scr_w / (txt_len * char_w);
			const int      mag_y   = scr_h / char_h;
			const int      pos_x   = (scr_w - txt_len * char_w * mag_x) >> 1;
			const int      pos_y   = (scr_h -           char_h * mag_y) >> 1;
			for (int i = 0; i < txt_len; ++i)
			{
				const int      c = static_cast <unsigned char> (note3_0 [i]);
				ctx._fnt_8x12.render_char (
					p_ptr + pos_x + pos_y * scr_s + i * char_w * mag_x,
					c,
					scr_s,
					mag_x, mag_y
				);
			}
			scr_clean_flag = false;
			scr_rfrsh_flag = true;
		}
		if (scr_rfrsh_flag)
		{
			ctx._display.refresh (0, 0, scr_w, scr_h);
			scr_rfrsh_flag = false;
		}

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

	float             gain_nrm = ctx._disto_gain.exchange (-1);
	if (gain_nrm >= 0)
	{
		const mfx::piapi::ParamDescInterface & desc =
			ctx._disto.get_param_info (mfx::piapi::ParamCateg_GLOBAL, 0);
		ctx._disto_gain_nat = desc.conv_nrm_to_nat (gain_nrm);
	}

	for (int chn = 0; chn < 2; ++chn)
	{
		if (chn == 0 && ctx._disto_flag)
		{
			mfx::piapi::PluginInterface::ProcInfo proc;
			mfx::piapi::EventTs evt;
			std::array <const mfx::piapi::EventTs *, 1>	evt_ptr_arr;
			evt_ptr_arr [0] = &evt;
			
			if (gain_nrm >= 0)
			{
				evt._timestamp           = 0;
				evt._type                = mfx::piapi::EventType_PARAM;
				evt._evt._param._categ   = mfx::piapi::ParamCateg_GLOBAL;
				evt._evt._param._index   = 0;
				evt._evt._param._val     = gain_nrm;
				evt._evt._param._note_id = -1;
				proc._nbr_evt = 1;
				proc._evt_arr = &evt_ptr_arr [0];
			}
			
			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				ctx._buf_alig [pos] = src_arr [chn] [pos];
			}
			std::array <      float *, 1> dst_ptr_arr;
			std::array <const float *, 1> src_ptr_arr;
			dst_ptr_arr [0] = &ctx._buf_alig [0];
			src_ptr_arr [0] = &ctx._buf_alig [0];

			proc._dst_arr  = &dst_ptr_arr [0];
			proc._byp_arr  = 0;
			proc._src_arr  = &src_ptr_arr [0];
			proc._nbr_chn_arr [mfx::piapi::PluginInterface::Dir_IN ] = 1;
			proc._nbr_chn_arr [mfx::piapi::PluginInterface::Dir_OUT] = 1;
			proc._nbr_spl  = nbr_spl;
			
			ctx._disto.process_block (proc);

			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				dst_arr [chn] [pos] = ctx._buf_alig [pos];
			}
		}
		else
		{
			for (int pos = 0; pos < int (nbr_spl); ++pos)
			{
				float          val = src_arr [chn] [pos];
				dst_arr [chn] [pos] = val;
			}
		}
	}

	float           freq = 0;
	if (ctx._tuner_flag)
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
		ctx._detected_freq = freq;
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

	::pinMode (MAIN_pin_reset, OUTPUT);

	::digitalWrite (MAIN_pin_reset, LOW);
	::delay (1);
	::digitalWrite (MAIN_pin_reset, HIGH);
	::delay (1);

	mfx::dsp::mix::Align::setup ();

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

	MAIN_context_ptr = std::unique_ptr <Context> (new Context);
	Context &      ctx = *MAIN_context_ptr;
	ctx._client_ptr = client_ptr;
	ctx._usage_min.store (-1);
	ctx._usage_max.store (-1);
	const double   sample_freq = double (jack_get_sample_rate (client_ptr));
	ctx._freq_analyser.set_sample_freq (sample_freq / ctx._tuner_subspl);
	ctx._buf_alig.resize (4096);
	mfx::ui::FontDataDefault::make_08x12 (ctx._fnt_8x12);
	for (int type = 0; type < mfx::ui::UserInputType_NBR_ELT; ++type)
	{
		const int      nbr_param = ctx._user_input.get_nbr_param (
			static_cast <mfx::ui::UserInputType> (type)
		);
		for (int index = 0; index < nbr_param; ++index)
		{
			ctx._user_input.set_msg_recipient (
				static_cast <mfx::ui::UserInputType> (type),
				index,
				&ctx._user_input_queue
			);
		}
	}
	ctx._disto.init ();
	int             latency;
	ctx._disto.reset (sample_freq, 4096, latency);
	ctx._disto_gain.store (0);
	
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
	}
	
	// User input thread
	std::thread     user_input_thread;
	if (ret_val == 0)
	{
		user_input_thread = std::thread (MAIN_physical_input_thread, std::ref (ctx));
	}

	if (ret_val == 0)
	{
		ret_val = MAIN_main_loop (ctx);
		
		ctx._input_quit_flag = true;
		user_input_thread.join ();
	}

	if (client_ptr != 0)
	{
		::jack_client_close (client_ptr);
		MAIN_client_ptr = 0;
	}

	MAIN_context_ptr.reset ();

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



