
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
#include "mfx/pi/DistoSimple.h"
#include "mfx/pi/DryWet.h"
#include "mfx/pi/Tuner.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/ui/Font.h"
#include "mfx/ui/FontDataDefault.h"
#include "mfx/MsgQueue.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"
#include "mfx/WorldAudio.h"

#if fstb_IS (ARCHI, ARM)
	#include "mfx/ui/DisplayPi3Pcd8544.h"
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
{
public:
#if fstb_IS (ARCHI, ARM)
	std::mutex     _mutex_spi;
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
	volatile bool  _disto_flag    = false;
	volatile float _disto_gain_nat = 1;
	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	// New audio engine
	mfx::ProcessingContext
	               _proc_ctx;
	mfx::ProcessingContext
	               _tune_ctx;
	mfx::MsgQueue  _queue_cmd_to_audio;
	mfx::MsgQueue  _queue_audio_to_cmd;
	mfx::ui::UserInputInterface::MsgQueue
	               _queue_from_input;
	mfx::PluginPool
	               _plugin_pool;
	conc::CellPool <mfx::Msg>
	               _msg_pool_cmd;
	mfx::WorldAudio
	               _audio_world;
	int            _pi_id_disto_main = -1;
	int            _pi_id_disto_mix  = -1;
	int            _pi_id_tuner_main = -1;

	// Not for the audio thread
	volatile bool	_quit_flag       = false;
	volatile bool  _input_quit_flag = false;
#if fstb_IS (ARCHI, ARM)
	mfx::ui::DisplayPi3Pcd8544
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
	mfx::ui::UserInputInterface::MsgQueue
	               _user_input_queue;
	mfx::ui::Font  _fnt_8x12;

	Context (double sample_freq, int max_block_size);
	~Context ();
	void send_param (int pi_id, int index, float val);
	void send_context (mfx::ProcessingContext &ctx);
};

Context::Context (double sample_freq, int max_block_size)
:	_buf_alig (4096)
,	_proc_ctx ()
,	_queue_cmd_to_audio ()
,	_queue_audio_to_cmd ()
,	_queue_from_input ()
,	_plugin_pool ()
,	_msg_pool_cmd ()
,	_audio_world (_plugin_pool, _queue_cmd_to_audio, _queue_audio_to_cmd, _queue_from_input, _user_input, _msg_pool_cmd)
#if fstb_IS (ARCHI, ARM)
,	_display (_mutex_spi)
,	_user_input (_mutex_spi)
,	_leds ()
#else
,	_display ()
,	_user_input ()
,	_leds ()
#endif
,	_user_input_queue ()
,	_fnt_8x12 ()
{
	_usage_min.store (-1);
	_usage_max.store (-1);
	mfx::ui::FontDataDefault::make_08x12 (_fnt_8x12);

	// Default: everything to the main queue
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
				&_user_input_queue
			);
		}
	}

	// Pot 0 to the audio engine
	_user_input.set_msg_recipient (mfx::ui::UserInputType_POT, 0, &_queue_from_input);

	// Setup: disto + drywet / tuner
	{
		mfx::PluginPool::PluginUPtr disto_main_uptr (new mfx::pi::DistoSimple);
		mfx::PluginPool::PluginUPtr disto_mix_uptr (new mfx::pi::DryWet);
		mfx::PluginPool::PluginUPtr tuner_main_uptr (new mfx::pi::Tuner);
		disto_main_uptr->init ();
		disto_mix_uptr->init ();
		tuner_main_uptr->init ();
		_pi_id_disto_main = _plugin_pool.add (disto_main_uptr);
		_pi_id_disto_mix  = _plugin_pool.add (disto_mix_uptr );
		_pi_id_tuner_main = _plugin_pool.add (tuner_main_uptr);
	}

	// Processing steps and buffers
	// 0: interface input L, disto input
	// 1: interface input/output R (bypass)
	// 2: disto output, mixer input
	// 3: disto bypass
	// 4: mixer output, interface output L
	{
		mfx::ProcessingContextNode::Side &si =
			_proc_ctx._interface_ctx._side_arr [mfx::piapi::PluginInterface::Dir_IN ];
		mfx::ProcessingContextNode::Side &so =
			_proc_ctx._interface_ctx._side_arr [mfx::piapi::PluginInterface::Dir_OUT];
		si._buf_arr [0] = 0;
		si._buf_arr [1] = 1;
		si._nbr_chn     = 2;
		si._nbr_chn_tot = 2;
		so._buf_arr [0] = 4;
		so._buf_arr [1] = 1;
		so._nbr_chn     = 2;
		so._nbr_chn_tot = 2;
	}
	{
		mfx::ProcessingContext::PluginContext disto_bundle;
		disto_bundle._mixer_flag = true;
		{
			mfx::ProcessingContextNode & node = disto_bundle._main;
			node._pi_id = _pi_id_disto_main;
			mfx::ProcessingContextNode::Side &si =
				node._side_arr [mfx::piapi::PluginInterface::Dir_IN ];
			mfx::ProcessingContextNode::Side &so =
				node._side_arr [mfx::piapi::PluginInterface::Dir_OUT];
			si._buf_arr [0] = 0;
			si._nbr_chn     = 1;
			si._nbr_chn_tot = 1;
			so._buf_arr [0] = 2;
			so._nbr_chn     = 1;
			so._nbr_chn_tot = 1;
			node._bypass_buf_arr [0] = 3;
		}
		{
			mfx::ProcessingContextNode & node = disto_bundle._mixer;
			node._pi_id = _pi_id_disto_mix;
			mfx::ProcessingContextNode::Side &si =
				node._side_arr [mfx::piapi::PluginInterface::Dir_IN ];
			mfx::ProcessingContextNode::Side &so =
				node._side_arr [mfx::piapi::PluginInterface::Dir_OUT];
			si._buf_arr [0] = 2;
			si._buf_arr [1] = si._buf_arr [0];
			si._nbr_chn     = 1;
			si._nbr_chn_tot = 2;
			so._buf_arr [0] = 4;
			so._nbr_chn     = 1;
			so._nbr_chn_tot = 1;
		}
		_proc_ctx._context_arr.push_back (disto_bundle);
	}

	// Alternate processing: tuner
	// 0: interface input L, tuner input
	// 1: interface input R (bypass)
	// 2: tuner output, interface output L & R
	{
		mfx::ProcessingContextNode::Side &si =
			_tune_ctx._interface_ctx._side_arr [mfx::piapi::PluginInterface::Dir_IN ];
		mfx::ProcessingContextNode::Side &so =
			_tune_ctx._interface_ctx._side_arr [mfx::piapi::PluginInterface::Dir_OUT];
		si._buf_arr [0] = 0;
		si._buf_arr [1] = 1;
		si._nbr_chn     = 2;
		si._nbr_chn_tot = 2;
		so._buf_arr [0] = 2;
		so._buf_arr [1] = 2;
		so._nbr_chn     = 2;
		so._nbr_chn_tot = 2;
	}
	{
		mfx::ProcessingContext::PluginContext tuner_bundle;
		tuner_bundle._mixer_flag = false;
		{
			mfx::ProcessingContextNode & node = tuner_bundle._main;
			node._pi_id = _pi_id_tuner_main;
			mfx::ProcessingContextNode::Side &si =
				node._side_arr [mfx::piapi::PluginInterface::Dir_IN ];
			mfx::ProcessingContextNode::Side &so =
				node._side_arr [mfx::piapi::PluginInterface::Dir_OUT];
			si._buf_arr [0] = 0;
			si._nbr_chn     = 1;
			si._nbr_chn_tot = 1;
			so._buf_arr [0] = 2;
			so._nbr_chn     = 1;
			so._nbr_chn_tot = 1;
		}
		_tune_ctx._context_arr.push_back (tuner_bundle);
	}

	// Automate the distortion gain with pot 0
	mfx::ParamCoord   disto_gain_coord (
		_pi_id_disto_main, mfx::pi::DistoSimple::Param_GAIN
	);
	std::shared_ptr <mfx::CtrlUnit> disto_gain_unit_sptr (
		new mfx::CtrlUnit
	);
	disto_gain_unit_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_POT);
	disto_gain_unit_sptr->_source._index = 0;
	disto_gain_unit_sptr->_curve         = mfx::ControlCurve_LINEAR;
	disto_gain_unit_sptr->_u2b_flag      = false;
	disto_gain_unit_sptr->_abs_flag      = true;
	disto_gain_unit_sptr->_base          = 0;
	disto_gain_unit_sptr->_amp           = 1;
	std::shared_ptr <mfx::ControlledParam> disto_gain_ctrl (
		new mfx::ControlledParam (disto_gain_coord)
	);
	disto_gain_ctrl->use_unit_list ().push_back (disto_gain_unit_sptr);
	_proc_ctx._map_param_ctrl.insert (std::make_pair (
		disto_gain_coord,
		disto_gain_ctrl
	));
	_proc_ctx._map_src_param.insert (std::make_pair (
		disto_gain_unit_sptr->_source,
		disto_gain_ctrl
	));
	_proc_ctx._map_src_unit.insert (std::make_pair (
		disto_gain_unit_sptr->_source,
		disto_gain_unit_sptr
	));

	_msg_pool_cmd.expand_to (256);

	_audio_world.set_context (_proc_ctx);

	// Get ready
	_audio_world.set_process_info (sample_freq, max_block_size);
	int             latency;
	_plugin_pool.use_plugin (_pi_id_disto_main)._pi_uptr->reset (
		sample_freq, max_block_size, latency
	);
	_plugin_pool.use_plugin (_pi_id_disto_mix )._pi_uptr->reset (
		sample_freq, max_block_size, latency
	);
	_plugin_pool.use_plugin (_pi_id_tuner_main)._pi_uptr->reset (
		sample_freq, max_block_size, latency
	);

	// Initial parameters values
	send_param (
		_pi_id_disto_mix,
		mfx::pi::DryWet::Param_BYPASS,
		1.f
	);
	send_param (
		_pi_id_disto_main,
		mfx::pi::DistoSimple::Param_GAIN,
		0.0f
	);
}

Context::~Context ()
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

void Context::send_param (int pi_id, int index, float val)
{
	conc::LockFreeCell <mfx::Msg> * cell_ptr =
		_msg_pool_cmd.take_cell (true);
	cell_ptr->_val._sender = mfx::Msg::Sender_CMD;
	cell_ptr->_val._type   = mfx::Msg::Type_PARAM;
	cell_ptr->_val._content._param._plugin_id = pi_id;
	cell_ptr->_val._content._param._index     = index;
	cell_ptr->_val._content._param._val       = val;
	_queue_cmd_to_audio.enqueue (*cell_ptr);
}

void Context::send_context (mfx::ProcessingContext &ctx)
{
	conc::LockFreeCell <mfx::Msg> * cell_ptr =
		_msg_pool_cmd.take_cell (true);
	cell_ptr->_val._sender = mfx::Msg::Sender_CMD;
	cell_ptr->_val._type   = mfx::Msg::Type_CTX;
	cell_ptr->_val._content._ctx._ctx_ptr = &ctx;
	_queue_cmd_to_audio.enqueue (*cell_ptr);
}

static std::unique_ptr <Context>	MAIN_context_ptr;



static void MAIN_physical_input_thread (Context &ctx)
{
	while (! ctx._input_quit_flag)
	{
		// From the input thread
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
						case  2: // Tuner toggle
							if (val >= 0.5f)
							{
								ctx._tuner_flag = (! ctx._tuner_flag);
								ctx.send_context (
									ctx._tuner_flag ? ctx._tune_ctx : ctx._proc_ctx
								);
							}
							break;
						case  3: // Distortion toggle
							if (val >= 0.5f)
							{
								ctx._disto_flag = (! ctx._disto_flag);
								ctx.send_param (
									ctx._pi_id_disto_mix,
									mfx::pi::DryWet::Param_BYPASS,
									(ctx._disto_flag) ? 0.f : 1.f
								);
							}
							break;
						}
					}

					ctx._user_input.return_cell (*cell_ptr);
				}
			}
			while (cell_ptr != 0 && ! ctx._input_quit_flag);
		}

		// From the audio thread
		{
			conc::LockFreeCell <mfx::Msg> * cell_ptr = 0;
			do
			{
				cell_ptr = ctx._queue_audio_to_cmd.dequeue ();
				if (cell_ptr != 0)
				{
					if (cell_ptr->_val._type == mfx::Msg::Type_PARAM)
					{
						const int      pi_id = cell_ptr->_val._content._param._plugin_id;
						const int      index = cell_ptr->_val._content._param._index;
						if (   pi_id == ctx._pi_id_disto_main
						    && index == mfx::pi::DistoSimple::Param_GAIN)
						{
							const mfx::piapi::ParamDescInterface & desc =
								ctx._plugin_pool.use_plugin (pi_id)._pi_uptr->get_param_info (
									mfx::piapi::ParamCateg_GLOBAL,
									index
								);
							const float    val = cell_ptr->_val._content._param._val;
							ctx._disto_gain_nat = float (desc.conv_nrm_to_nat (val));
						}
					}

					ctx._msg_pool_cmd.return_cell (*cell_ptr);
				}
			}
			while (cell_ptr != 0 && ! ctx._input_quit_flag);
		}

		// 10 ms between updates
#if fstb_IS (ARCHI, ARM)
		::delay (10);
#else
		::Sleep (10);
#endif
	}
}



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
	ctx._audio_world.process_block (dst_arr, src_arr, nbr_spl);

	// Tuner
	if (ctx._tuner_flag)
	{
		const mfx::pi::Tuner &  tuner = dynamic_cast <const mfx::pi::Tuner &> (
			*ctx._plugin_pool.use_plugin (ctx._pi_id_tuner_main)._pi_uptr
		);
		ctx._detected_freq = tuner.get_freq ();
	}

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



int MAIN_main_loop (Context &ctx)
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
		const bool   tuner_flag = ctx._tuner_flag;
		const bool   disto_flag = ctx._disto_flag;
		const float  disto_gain = ctx._disto_gain_nat;
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
			ctx._leds.set_led (led_index, lum_arr [led_index]);
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

#if 1

	#if fstb_IS (ARCHI, ARM)
		::delay (100);
	#else
		::Sleep (100);
	#endif

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

	MAIN_audio_stop ();

	MAIN_context_ptr.reset ();

	fprintf (stderr, "Exiting with code %d.\n", ret_val);

	return ret_val;
}
