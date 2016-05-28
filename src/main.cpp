
// g++ --std=c++11 -I. -Wall -mfpu=neon `pkg-config --cflags --libs jack` -l wiringPi -l pthread main.cpp mailbox.c fstb/fnc.cpp mfx/*.cpp mfx/tuner/*.cpp mfx/ui/*.cpp
// sudo jackd -P70 -p16 -t2000 -dalsa -p64 -n3 -r44100 -s &
// sudo ./a.out
//
// -march=armv8-a doesn't work with std::thread on this GCC version,
// see last comment of bug #42734 on gcc.gnu.org 


#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#endif

#define MAIN_USE_ST7920

#include "fstb/def.h"

#define MAIN_API_JACK 1
#define MAIN_API_ALSA 2
#define MAIN_API_WIN  3
#if fstb_IS (ARCHI, ARM)
	#define MAIN_API MAIN_API_JACK
#else
	#define MAIN_API MAIN_API_WIN
#endif

#include "fstb/AllocAlign.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/doc/FxId.h"
#include "mfx/pi/DistoSimple.h"
#include "mfx/pi/DryWet.h"
#include "mfx/pi/Tuner.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/ui/Font.h"
#include "mfx/ui/FontDataDefault.h"
#include "mfx/ui/TimeShareThread.h"
#include "mfx/Model.h"
#include "mfx/ModelObserverInterface.h"
#include "mfx/MsgQueue.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"
#include "mfx/WorldAudio.h"

#if fstb_IS (ARCHI, ARM)
 #if defined (MAIN_USE_ST7920)
	#include "mfx/ui/DisplayPi3St7920.h"
 #else
	#include "mfx/ui/DisplayPi3Pcd8544.h"
 #endif
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
	#include <unistd.h>
	#include <signal.h>

#elif fstb_IS (ARCHI, X86)
	#include "mfx/ui/DisplayVoid.h"
	#include "mfx/ui/LedVoid.h"
	#include "mfx/ui/UserInputVoid.h"

	#include <Windows.h>

#else
	#error Unsupported architecture

#endif

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
#if fstb_IS (ARCHI, ARM)
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;

#else
	::LARGE_INTEGER t;
	::QueryPerformanceCounter (&t);
	static double per = 0;
	if (per == 0)
	{
		::LARGE_INTEGER f;
		::QueryPerformanceFrequency (&f);
		per = 1e9 / double (f.QuadPart);
	}
	return int64_t (t.QuadPart * per);

#endif
}




class Context
:	public mfx::ModelObserverInterface
{
public:
#if fstb_IS (ARCHI, ARM)
	mfx::ui::TimeShareThread
	               _thread_spi;
#endif
	int64_t        _time_beg = MAIN_get_time ();
	int64_t        _time_end = _time_beg;
	std::atomic <float>            // Negative: the main thread read the value.
	               _usage_max;
	std::atomic <float>            // Negative: the main thread read the value.
	               _usage_min;
	volatile float _detected_freq = 0;
	const int      _tuner_subspl  = 4;
	volatile bool  _tuner_flag    = false;
	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	// New audio engine
	mfx::ProcessingContext
	               _proc_ctx;
	mfx::ProcessingContext
	               _tune_ctx;
	mfx::ui::UserInputInterface::MsgQueue
	               _queue_input_to_cmd;
	mfx::ui::UserInputInterface::MsgQueue
	               _queue_input_to_audio;
	mfx::ModelObserverInterface::SlotInfoList
	               _slot_info_list;
	mfx::Model     _model;
	volatile int   _pi_id_disto_main = -1;
	volatile int   _pi_id_disto_mix  = -1;
	volatile int   _pi_id_tuner_main = -1;

	// Not for the audio thread
	volatile bool	_quit_flag = false;
#if fstb_IS (ARCHI, ARM)
 #if defined (MAIN_USE_ST7920)
	mfx::ui::DisplayPi3St7920
 #else
	mfx::ui::DisplayPi3Pcd8544
 #endif
	               _display;
	mfx::ui::UserInputPi3
	               _user_input;
	mfx::ui::LedPi3
	               _leds;
#else
	mfx::ui::DisplayVoid
	               _display;
	mfx::ui::UserInputVoid
	               _user_input;
	mfx::ui::LedVoid
	               _leds;
#endif
	mfx::ui::Font  _fnt_8x12;
	mfx::ui::Font  _fnt_6x8;
	mfx::ui::Font  _fnt_6x6;

	Context (double sample_freq, int max_block_size);
	~Context ();
protected:
	// mfx::ModelObserverInterface
	virtual void   do_set_tuner (bool active_flag);
	virtual void   do_set_tuner_freq (float freq);
	virtual void	do_set_slot_info_for_current_preset (const mfx::ModelObserverInterface::SlotInfoList &info_list);
};

Context::Context (double sample_freq, int max_block_size)
:
#if fstb_IS (ARCHI, ARM)
	_thread_spi (10 * 1000)
,
#endif
	_buf_alig (4096)
,	_proc_ctx ()
,	_queue_input_to_cmd ()
,	_queue_input_to_audio ()
,	_slot_info_list ()
,	_model (_queue_input_to_cmd, _queue_input_to_audio, _user_input)
#if fstb_IS (ARCHI, ARM)
,	_display (_thread_spi)
,	_user_input (_thread_spi)
,	_leds ()
#else
,	_display ()
,	_user_input ()
,	_leds ()
#endif
,	_fnt_8x12 ()
,	_fnt_6x6 ()
{
	_usage_min.store (-1);
	_usage_max.store (-1);
	mfx::ui::FontDataDefault::make_08x12 (_fnt_8x12);
	mfx::ui::FontDataDefault::make_06x08 (_fnt_6x8);
	mfx::ui::FontDataDefault::make_06x06 (_fnt_6x6);

	// Default: everything to the main queue, except pot to the audio engine
	for (int type = 0; type < mfx::ui::UserInputType_NBR_ELT; ++type)
	{
		const int      nbr_param = _user_input.get_nbr_param (
			static_cast <mfx::ui::UserInputType> (type)
		);
		for (int index = 0; index < nbr_param; ++index)
		{
			_user_input.set_msg_recipient (
				static_cast <mfx::ui::UserInputType> (type),
				index,
				  (type == mfx::ui::UserInputType_POT)
				? &_queue_input_to_audio
				: &_queue_input_to_cmd
			);
		}
	}

	_model.set_observer (this);

	mfx::doc::Bank bank;
	mfx::doc::Preset& preset   = bank._preset_arr [0];
	preset._name = "Preset 1";
	mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
	preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
	slot_ptr->_label    = "Disto 1";
	slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
	slot_ptr->_settings_mixer._param_list =
		std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
	mfx::doc::PluginSettings & pi_settings =
		slot_ptr->_settings_all [slot_ptr->_pi_model];

	pi_settings._param_list = std::vector <float> (1, 0);

	mfx::doc::CtrlLinkSet cls_main;
	cls_main._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
	cls_main._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_POT);
	cls_main._bind_sptr->_source._index = 0;
	cls_main._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
	cls_main._bind_sptr->_u2b_flag      = false;
	cls_main._bind_sptr->_base          = 0;
	cls_main._bind_sptr->_amp           = 1;
	pi_settings._map_param_ctrl [mfx::pi::DistoSimple::Param_GAIN] = cls_main;

	{
		mfx::doc::PedalActionCycle &  cycle =
			preset._layout._pedal_arr [1]._action_arr [mfx::doc::ActionTrigger_PRESS];
		const mfx::doc::FxId    fx_id (slot_ptr->_label, mfx::PiType_MIX);
		mfx::doc::PedalActionCycle::ActionArray   action_arr (1);
		for (int i = 0; i < 2; ++i)
		{
			static const float val_arr [2] = { 1, 0 };
			const float        val = val_arr [i];
			action_arr [0] = mfx::doc::PedalActionCycle::ActionSPtr (
				new mfx::doc::ActionParam (fx_id, mfx::pi::DryWet::Param_BYPASS, val)
			);
			cycle._cycle.push_back (action_arr);
		}
	}
	{
		mfx::doc::PedalActionCycle &  cycle =
			preset._layout._pedal_arr [0]._action_arr [mfx::doc::ActionTrigger_PRESS];
		mfx::doc::PedalActionCycle::ActionArray   action_arr;
		action_arr.push_back (mfx::doc::PedalActionCycle::ActionSPtr (
			new mfx::doc::ActionToggleTuner
		));
		cycle._cycle.push_back (action_arr);
	}
	
	_model.load_bank (bank, 0);

	_model.set_process_info (sample_freq, max_block_size);
}

Context::~Context ()
{
	// Nothing
}

void	Context::do_set_tuner (bool active_flag)
{
	_tuner_flag = active_flag;
}

void	Context::do_set_tuner_freq (float freq)
{
	_detected_freq = freq;
}

void	Context::do_set_slot_info_for_current_preset (const mfx::ModelObserverInterface::SlotInfoList &info_list)
{
	_slot_info_list = info_list;
}

static std::unique_ptr <Context>	MAIN_context_ptr;



static int MAIN_audio_process (Context &ctx, float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	const int64_t  time_beg    = MAIN_get_time ();
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

	// Audio graph
	ctx._model.process_block (dst_arr, src_arr, nbr_spl);

	ctx._time_end = MAIN_get_time ();

	return 0;
}



#if (MAIN_API == MAIN_API_JACK)



static ::jack_client_t * volatile MAIN_client_ptr = 0;
static ::jack_port_t   * volatile MAIN_mfx_port_arr [2] [2]; // [in/out] [chn]
static ::jack_status_t            MAIN_status = ::JackServerFailed;



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

static int MAIN_audio_process_jack (::jack_nframes_t nbr_spl, void *arg)
{
	Context &      ctx = *reinterpret_cast <Context *> (arg);
	const jack_default_audio_sample_t * src_arr [2];
	jack_default_audio_sample_t *       dst_arr [2];
	for (int chn = 0; chn < 2; ++chn)
	{
		src_arr [chn] = reinterpret_cast <const jack_default_audio_sample_t *> (
			::jack_port_get_buffer (MAIN_mfx_port_arr [0] [chn], nbr_spl)
		);
		dst_arr [chn] = reinterpret_cast <jack_default_audio_sample_t *> (
			::jack_port_get_buffer (MAIN_mfx_port_arr [1] [chn], nbr_spl)
		);
	}

	return MAIN_audio_process (ctx, dst_arr, src_arr, nbr_spl);
}



static int MAIN_audio_init (double &sample_freq, int &max_block_size)
{
	int            ret_val = 0;

	sample_freq    = 44100;
	max_block_size = 4096;

	MAIN_client_ptr = jack_client_open (
		"MultiFX",
		::JackNullOption,
		&MAIN_status,
		0
	);
	if (MAIN_client_ptr == 0)
	{
		fprintf (
			stderr,
			"jack_client_open() failed, status = 0x%2.0x\n",
			MAIN_status
		);
		if ((MAIN_status & ::JackServerFailed) != 0)
		{
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		sample_freq    = jack_get_sample_rate (MAIN_client_ptr);
		max_block_size = 4096;
	}

	return ret_val;
}



static int MAIN_audio_start (Context &ctx)
{
	int            ret_val = 0;

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
		if ((MAIN_status & ::JackServerStarted) != 0)
		{
			fprintf (stderr, "JACK server started\n");
		}
		if ((MAIN_status & ::JackNameNotUnique) != 0)
		{
			const char *   name_0 = ::jack_get_client_name (MAIN_client_ptr);
			fprintf (stderr, "Unique name \"%s\" assigned\n", name_0);
		}

		::jack_set_process_callback (MAIN_client_ptr, MAIN_audio_process_jack, &ctx);
		::jack_on_shutdown (MAIN_client_ptr, MAIN_jack_shutdown, &ctx);

		static const char *  port_name_0_arr [2] [2] =
		{
			{ "Input L", "Input R" }, { "Output L", "Output R" }
		};
		for (int dir = 0; dir < 2 && ret_val == 0; ++dir)
		{
			for (int chn = 0; chn < 2 && ret_val == 0; ++chn)
			{
				MAIN_mfx_port_arr [dir] [chn] = ::jack_port_register (
					MAIN_client_ptr,
					port_name_0_arr [dir] [chn],
					JACK_DEFAULT_AUDIO_TYPE,
					port_dir [dir],
					0
				);
				if (MAIN_mfx_port_arr [dir] [chn] == 0)
				{
					fprintf (stderr, "No more JACK ports available.\n");
					ret_val = -1;
				}
			}
		}
	}

	if (ret_val == 0)
	{
		ret_val = ::jack_activate (MAIN_client_ptr);
		if (ret_val != 0)
		{
			fprintf (stderr, "cannot activate client, returned %d.\n", ret_val);
		}
	}

	for (int dir = 0; dir < 2 && ret_val == 0; ++dir)
	{
		const char **  port_0_arr = ::jack_get_ports (
			MAIN_client_ptr,
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
					inout_0 [1 - dir] = ::jack_port_name (MAIN_mfx_port_arr [dir] [chn]);
					inout_0 [    dir] = port_0_arr [chn];
					ret_val = ::jack_connect (MAIN_client_ptr, inout_0 [0], inout_0 [1]);
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

	return ret_val;
}



static int MAIN_audio_stop ()
{
	if (MAIN_client_ptr != 0)
	{
		::jack_client_close (MAIN_client_ptr);
		MAIN_client_ptr = 0;
	}

	return 0;
}



#elif (MAIN_API == MAIN_API_ALSA)



// Ref:
// http://www.saunalahti.fi/~s7l/blog/2005/08/21/Full%20Duplex%20ALSA
// http://jzu.blog.free.fr/public/SLAB/slab.c

static snd_pcm_t * MAIN_handle_in;
static snd_pcm_t * MAIN_handle_out;

static int MAIN_audio_init (double &sample_freq, int &max_block_size)
{
	sample_freq    = 44100;
	max_block_size = 4096;

	if (ret_val == 0)
	{
		ret_val = ::snd_pcm_open (
			&MAIN__handle_in,
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
			&MAIN__handle_out,
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


	return 0;
}

static int MAIN_audio_start (Context &ctx)
{

	/*** To do ***/

	return 0;
}

static int MAIN_audio_stop ()
{

	/*** To do ***/

	return 0;
}



#else // MAIN_API



static int MAIN_audio_init (double &sample_freq, int &max_block_size)
{
	sample_freq    = 44100;
	max_block_size = 4096;


	/*** To do ***/

	return 0;
}

static int MAIN_audio_start (Context &ctx)
{

	/*** To do ***/

	return 0;
}

static int MAIN_audio_stop ()
{

	/*** To do ***/

	return 0;
}



#endif



static void MAIN_print_text (int x, int y, const char *txt_0, uint8_t *screenbuf_ptr, int stride, mfx::ui::Font &fnt, int mag_x, int mag_y)
{
	int             pos    = 0;
	const int       x_org  = x;
	const int       char_w = fnt.get_char_w ();
	const int       char_h = fnt.get_char_h ();
	while (txt_0 [pos] != '\0')
	{
		const char      c = txt_0 [pos];
		++ pos;
		if (c == '\n')
		{
			x = x_org;
			y += char_h * mag_y;
		}
		else
		{
			fnt.render_char (
				screenbuf_ptr + x + y * stride,
				c,
				stride,
				mag_x, mag_y
			);
			x += char_w * mag_x;
		}
	}
}



static int MAIN_main_loop (Context &ctx)
{
	fprintf (stderr, "Entering main loop...\n");

	int            ret_val = 0;

	uint8_t *      p_ptr  = ctx._display.use_screen_buf ();
	const int      scr_w  = ctx._display.get_width ();
	const int      scr_h  = ctx._display.get_height ();
	const int      scr_s  = ctx._display.get_stride ();
	bool           scr_clean_flag = false;
	bool           scr_rfrsh_flag  = true;

	while (ret_val == 0 && ! ctx._quit_flag)
	{
#if 0 // When doing manual time sharing
		while (! ctx._thread_spi.process_single_task ())
		{
			continue;
		}
#endif

		ctx._model.process_messages ();
		
		mfx::ModelObserverInterface::PluginInfoSPtr pi_efx_sptr =
			ctx._slot_info_list [0] [mfx::PiType_MAIN];
		mfx::ModelObserverInterface::PluginInfoSPtr pi_mix_sptr =
			ctx._slot_info_list [0] [mfx::PiType_MIX ];

		const bool   disto_flag =
			(pi_mix_sptr->_param_arr [mfx::pi::DryWet::Param_BYPASS] < 0.5f);
		const mfx::piapi::ParamDescInterface & desc =
			pi_efx_sptr->_pi.get_param_info (
				mfx::piapi::ParamCateg_GLOBAL,
				mfx::pi::DistoSimple::Param_GAIN
			);
		const float  disto_gain_nrm =
			pi_efx_sptr->_param_arr [mfx::pi::DistoSimple::Param_GAIN];
		const float  disto_gain = float (desc.conv_nrm_to_nat (disto_gain_nrm));
		const bool   tuner_flag = ctx._tuner_flag;
		const float  usage_max  = ctx._usage_max.exchange (-1);
		const float  usage_min  = ctx._usage_min.exchange (-1);
		const float  freq = (tuner_flag) ? ctx._detected_freq : 0;
		char         cpu_0 [127+1] = "Time usage: ------ % / ------ %";
		if (usage_max >= 0 && usage_min >= 0)
		{
			fstb::snprintf4all (
				cpu_0, sizeof (cpu_0),
				"Time usage: %6.2f %% / %6.2f %%",
				usage_min * 100, usage_max * 100
			);
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
				fstb::snprintf4all (
					note3_0, sizeof (note3_0),
					"%s%1d",
					note_0_arr [note], octave
				);
			}
			fstb::snprintf4all (
				note4_0, sizeof (note4_0),
				"%2s%-2d",
				note_0_arr [note], octave
			);
			fstb::snprintf4all (
				freq_0, sizeof (freq_0),
				"Note: %4s %+4d %7.3lf Hz",
				note4_0, cents, freq
			);

			const int      mid_index = (nbr_led - 1) / 2;
			const float    cents_abs = fabs (cents_dbl);
			lum_arr [mid_index    ] = std::max (5 - cents_abs, 0.0f) * (1.0f / 5);

			const float    lum      = fstb::limit (cents_abs * (1.0f / 25), 0.0f, 1.0f);
			lum_arr [mid_index - 1] = (cents_dbl < 0) ? lum : 0;
			lum_arr [mid_index + 1] = (cents_dbl > 0) ? lum : 0;
		}

		for (int led_index = 0; led_index < nbr_led; ++led_index)
		{
			float           val = lum_arr [led_index];
			val = val * val;  // Gamma 2.0
			ctx._leds.set_led (led_index, val);
		}



		char param_0 [255+1];
		fstb::snprintf4all (
			param_0, sizeof (param_0),
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
			MAIN_print_text (pos_x, pos_y, note3_0, p_ptr, scr_s, ctx._fnt_8x12, mag_x, mag_y);
			scr_clean_flag = false;
			scr_rfrsh_flag = true;
		}
		else
		{
			const char   txt_0 [] =
//				"012345678901234567890\n"
				"Ceci est un texte\n"
				"ecrit en tout petit.\n";
			MAIN_print_text (0,  0, txt_0  , p_ptr, scr_s, ctx._fnt_6x6 , 1, 1);
			MAIN_print_text (0, 16, param_0, p_ptr, scr_s, ctx._fnt_6x8 , 1, 1);
			MAIN_print_text (0, 32, param_0, p_ptr, scr_s, ctx._fnt_8x12, 1, 1);
			scr_rfrsh_flag = true;
		}
		if (scr_rfrsh_flag)
		{
			ctx._display.refresh (0, 0, scr_w, scr_h);
			scr_rfrsh_flag = false;
		}

		bool wait_flag = true;

#if 1

		if (wait_flag)
		{
			const int    wait_ms = 100;
		#if fstb_IS (ARCHI, ARM)
			::delay (wait_ms);
		#else
			::Sleep (wait_ms);
		#endif
		}

#else

/********************************************* TEMP *********************************/

		const int      nbr_spl = 64;
		float          buf [4] [nbr_spl];
		memset (&buf [0] [0], 0, sizeof (buf [0]));
		memset (&buf [1] [0], 0, sizeof (buf [1]));
		const float *  src_arr [2] = { buf [0], buf [1] };
		float *        dst_arr [2] = { buf [2], buf [3] };

		MAIN_audio_process (ctx, dst_arr, src_arr, nbr_spl);

/********************************************* TEMP *********************************/

#endif
	}

	fprintf (stderr, "Exiting main loop.\n");

	return ret_val;
}



#if fstb_IS (ARCHI, ARM)
int main (int argc, char *argv [])
#else
int CALLBACK WinMain (::HINSTANCE instance, ::HINSTANCE prev_instance, ::LPSTR cmdline_0, int cmd_show)
#endif
{
	int            ret_val = 0;

#if fstb_IS (ARCHI, ARM)
	::wiringPiSetupPhys ();

	::pinMode (MAIN_pin_reset, OUTPUT);

	::digitalWrite (MAIN_pin_reset, LOW);
	::delay (100);
	::digitalWrite (MAIN_pin_reset, HIGH);
	::delay (100);
#endif

	mfx::dsp::mix::Align::setup ();

	double         sample_freq;
	int            max_block_size;
	ret_val = MAIN_audio_init (sample_freq, max_block_size);

	MAIN_context_ptr = std::unique_ptr <Context> (
		new Context (sample_freq, max_block_size)
	);
	Context &      ctx = *MAIN_context_ptr;

	if (ret_val == 0)
	{
		ret_val = MAIN_audio_start (ctx);
	}

	if (ret_val == 0)
	{
		ret_val = MAIN_main_loop (ctx);
	}

	MAIN_audio_stop ();

	MAIN_context_ptr.reset ();

	fprintf (stderr, "Exiting with code %d.\n", ret_val);

	return ret_val;
}
