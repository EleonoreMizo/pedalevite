
// Running jack: sudo jackd -P70 -p16 -t2000 -dalsa -p64 -n3 -r44100 -s &
//
// -march=armv8-a doesn't work with std::thread on this GCC version,
// see the Jonathan Wakely's comment (2015-10-05) for bug #42734 on gcc.gnu.org 


#if defined (WIN32) || defined (_WIN32) || defined (__CYGWIN__)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#endif

#define MAIN_USE_ST7920

// No I/O, audio only. Useful to debug soundcard-related problems.
#undef MAIN_USE_VOID

#include "fstb/def.h"

#define MAIN_API_JACK   1
#define MAIN_API_ALSA   2
#define MAIN_API_ASIO   3
#define MAIN_API_MANUAL 4
#define MAIN_API_PVAB   5
#if 0 // For debugging complex audio things
	#define MAIN_API MAIN_API_MANUAL
#elif fstb_IS (SYS, LINUX)
//	#define MAIN_API MAIN_API_JACK
	#define MAIN_API MAIN_API_ALSA
//	#define MAIN_API MAIN_API_PVAB
#else
	#define MAIN_API MAIN_API_ASIO
#endif

#include "fstb/AllocAlign.h"
#include "fstb/fnc.h"
#include "mfx/adrv/CbInterface.h"
#include "mfx/adrv/DriverInterface.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/doc/SerRText.h"
#include "mfx/doc/SerWText.h"
#include "mfx/pi/dist1/Param.h"
#include "mfx/pi/dtone1/Param.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/freqsh/FreqShiftDesc.h"
#include "mfx/pi/freqsh/Param.h"
#include "mfx/pi/iifix/Param.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/pi/trem1/Param.h"
#include "mfx/pi/trem1/Waveform.h"
#include "mfx/pi/wah1/Param.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/ui/FontDataDefault.h"
#include "mfx/ui/TimeShareThread.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/ParentInterface.h"
#include "mfx/CmdLine.h"
#include "mfx/Model.h"
#include "mfx/ModelObserverDefault.h"
#include "mfx/PageSet.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"
#include "mfx/View.h"
#include "mfx/WaMsgQueue.h"
#include "mfx/WorldAudio.h"

#if fstb_IS (SYS, LINUX)
	#if (MAIN_API == MAIN_API_JACK)
		#include "mfx/adrv/DJack.h"
	#elif (MAIN_API == MAIN_API_ALSA)
		#include "mfx/adrv/DAlsa.h"
	#elif (MAIN_API == MAIN_API_MANUAL)
		#include "mfx/adrv/DManual.h"
	#elif (MAIN_API == MAIN_API_PVAB)
		#include "mfx/adrv/DPvabI2s.h"
	#else
		#error Wrong MAIN_API value
	#endif // MAIN_API
	#include "mfx/hw/FileIOPi3.h"

 #if ! defined (MAIN_USE_VOID)
  #if defined (MAIN_USE_ST7920)
	#include "mfx/hw/DisplayPi3St7920.h"
  #else
	#include "mfx/hw/DisplayPi3Pcd8544.h"
  #endif
	#include "mfx/hw/LedPi3.h"
	#include "mfx/hw/UserInputPi3.h"

	#include <wiringPi.h>
	#include <wiringPiI2C.h>
	#include <wiringPiSPI.h>
 #endif // MAIN_USE_VOID

	#include <arpa/inet.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <signal.h>

#elif fstb_IS (SYS, WIN)
	#include "mfx/hw/FileIOWindows.h"
	#include "mfx/hw/IoWindows.h"

	#if (MAIN_API == MAIN_API_ASIO)
		#include "mfx/adrv/DAsio.h"
	#elif (MAIN_API == MAIN_API_MANUAL)
		#include "mfx/adrv/DManual.h"
	#else
		#error Wrong MAIN_API value
	#endif

	#include <Windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>

	#if defined (_MSC_VER)
		#include	<crtdbg.h>
		#include	<new.h>
		#include	<new>
	#endif	// _MSC_VER

#else
	#error Unsupported operating system

#endif

#if defined (MAIN_USE_VOID)
	#include "mfx/ui/DisplayVoid.h"
	#include "mfx/ui/LedVoid.h"
	#include "mfx/ui/UserInputVoid.h"
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
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



class Context
:	public mfx::ModelObserverDefault
,	public mfx::adrv::CbInterface
{
public:
	mfx::CmdLine   _cmd_line;
	double         _sample_freq;
	int            _max_block_size;
#if fstb_IS (SYS, LINUX)
	mfx::ui::TimeShareThread
	               _thread_spi;
#endif
	const int      _tuner_subspl  = 4;
	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	// Audio engine
	mfx::ProcessingContext
	               _proc_ctx;
	mfx::ProcessingContext
	               _tune_ctx;
	mfx::ui::UserInputInterface::MsgQueue
	               _queue_input_to_gui;
	mfx::ui::UserInputInterface::MsgQueue
	               _queue_input_to_cmd;
	mfx::ui::UserInputInterface::MsgQueue
	               _queue_input_to_audio;

	// Not for the audio thread
	volatile bool	_quit_flag = false;

	// Controller
#if defined (MAIN_USE_VOID)
	mfx::ui::DisplayVoid
	               _display;
	mfx::ui::UserInputVoid
	               _user_input;
	mfx::ui::LedVoid
	               _leds;
#elif fstb_IS (SYS, LINUX)
 #if defined (MAIN_USE_ST7920)
	mfx::hw::DisplayPi3St7920
 #else
	mfx::hw::DisplayPi3Pcd8544
 #endif
	               _display;
	mfx::hw::UserInputPi3
	               _user_input;
	mfx::hw::LedPi3
	               _leds;
#else
	mfx::hw::IoWindows
	               _all_io;
	mfx::ui::DisplayInterface &
	               _display;
	mfx::ui::UserInputInterface &
	               _user_input;
	mfx::ui::LedInterface &
	               _leds;
#endif
#if fstb_IS (SYS, LINUX)
	mfx::hw::FileIOPi3
	               _file_io;
#else
	mfx::hw::FileIOWindows
	               _file_io;
#endif

	mfx::Model     _model;

	// View
	mfx::View      _view;
	mfx::PageSet   _page_set;

	explicit       Context (mfx::adrv::DriverInterface &snd_drv);
	               ~Context ();
	void           set_proc_info (double sample_freq, int max_block_size);
protected:
	// mfx::ModelObserverDefault
	virtual void   do_set_tuner (bool active_flag);
	// mfx::adrv:CbInterface
	virtual void   do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl);
	virtual void   do_notify_dropout ();
	virtual void   do_request_exit ();
private:
	static void    init_empty_bank (mfx::doc::Bank &bank);
};

Context::Context (mfx::adrv::DriverInterface &snd_drv)
:	_cmd_line ()
,	_sample_freq (0)
,	_max_block_size (0)
#if fstb_IS (SYS, LINUX)
,	_thread_spi (std::chrono::milliseconds (10))
#endif
,	_buf_alig (4096 * 4)
,	_proc_ctx ()
,	_queue_input_to_gui ()
,	_queue_input_to_cmd ()
,	_queue_input_to_audio ()
#if defined (MAIN_USE_VOID)
,	_display ()
,	_user_input ()
,	_leds ()
#elif fstb_IS (SYS, LINUX)
,	_display (_thread_spi)
,	_user_input (_thread_spi)
,	_leds ()
#else
,	_all_io (_quit_flag)
,	_display (_all_io)
,	_user_input (_all_io)
,	_leds (_all_io)
#endif
,	_file_io (_leds)
,	_model (_queue_input_to_cmd, _queue_input_to_audio, _user_input, _file_io)
,	_view ()
,	_page_set (
		_model, _view, _display, _queue_input_to_gui, _user_input, _leds, _cmd_line,
		snd_drv
	)
{
#if ! fstb_IS (SYS, LINUX)
	_all_io.set_model (_model);
#endif

	// First, scans the input queue to check if the ESC button
	// is pressed. If it is the case, we request exiting the program.
	_user_input.set_msg_recipient (
		mfx::ui::UserInputType_SW,
		1 /*** To do: constant ***/,
		&_queue_input_to_gui
	);
	mfx::ui::UserInputInterface::MsgCell * cell_ptr = 0;
	bool           scan_flag = true;
	do
	{
		cell_ptr = _queue_input_to_gui.dequeue ();
		if (cell_ptr != 0)
		{
			const mfx::ui::UserInputType  type  = cell_ptr->_val.get_type ();
			const int                     index = cell_ptr->_val.get_index ();
			const float                   val   = cell_ptr->_val.get_val ();
			if (type == mfx::ui::UserInputType_SW && index == 1 /*** To do: constant ***/)
			{
fprintf (stderr, "Reading ESC button...\n");
				if (val > 0.5f)
				{
					_quit_flag = true;
					fprintf (stderr, "Exit requested.\n");
				}
				scan_flag = false;
			}
			_user_input.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0 && scan_flag);

	// Assigns message queues to input devices
	_user_input.assign_queues_to_input_dev (
		_queue_input_to_cmd,
		_queue_input_to_gui,
		_queue_input_to_audio
	);

	// Lists public plug-ins
	_page_set.list_plugins ();

	_model.set_observer (&_view);
	_view.add_observer (*this);

	const int      ret_val = _model.load_from_disk ();
	if (ret_val != 0)
	{

		/*** To do: error message? ***/

		mfx::doc::PedalboardLayout layout;
		layout.set_default_conf ();
		_model.set_pedalboard_layout (layout);

		mfx::doc::Bank bank;
		init_empty_bank (bank);
		for (int b = 0; b < mfx::Cst::_nbr_banks; ++b)
		{
			_model.set_bank (b, bank);
		}
		_model.select_bank (0);
		_model.activate_preset (0);
	}

#if 0
	{
		// Serialization consistency test
		mfx::doc::SerWText ser_w;
		ser_w.clear ();
		_view.use_setup ().ser_write (ser_w);
		ser_w.terminate ();
		const std::string result = ser_w.use_content ();

		mfx::doc::SerRText ser_r;
		ser_r.start (result);
		std::unique_ptr <mfx::doc::Setup> sss_uptr (new mfx::doc::Setup);
		sss_uptr->ser_read (ser_r);
		ser_r.terminate ();

		ser_w.clear ();
		_view.use_setup ().ser_write (ser_w);
		ser_w.terminate ();
		const std::string result2 = ser_w.use_content ();

		assert (result == result2);
	}
#endif

//	_model.set_chn_mode (mfx::ChnMode_1M_1S);

	mfx::uitk::PageSwitcher &  page_switcher = _page_set.use_page_switcher ();
	page_switcher.switch_to (mfx::uitk::pg::PageType_CUR_PROG, 0);

/**********************************************************************************************************************************************************************************************************************************/
// Debugging code
//	_model.select_bank (2);
//	_model.activate_preset (3);
/**********************************************************************************************************************************************************************************************************************************/
}



Context::~Context ()
{
	// Nothing
}



void	Context::set_proc_info (double sample_freq, int max_block_size)
{
	_sample_freq    = sample_freq;
	_max_block_size = max_block_size;
	_model.set_process_info (_sample_freq, _max_block_size);
}



void	Context::do_set_tuner (bool active_flag)
{
	if (active_flag)
	{
		mfx::uitk::PageSwitcher &  page_switcher =
			_page_set.use_page_switcher ();
		mfx::uitk::Page & page_mgr = _page_set.use_page_mgr ();
		page_switcher.call_page (
			mfx::uitk::pg::PageType_TUNER,
			0,
			page_mgr.get_cursor_node ()
		);
	}
}



void	Context::do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	_model.process_block (dst_arr, src_arr, nbr_spl);
}



void	Context::do_notify_dropout ()
{
	mfx::MeterResultSet &   meters = _model.use_meters ();
	meters._dsp_overload_flag.exchange (true);
}



void	Context::do_request_exit ()
{
	_quit_flag = true;
}



void	Context::init_empty_bank (mfx::doc::Bank &bank)
{
	for (auto &preset : bank._preset_arr)
	{
		preset._name = mfx::Cst::_empty_preset_name;
	}
}



static int MAIN_main_loop (Context &ctx, mfx::adrv::DriverInterface &snd_drv)
{
	fprintf (stderr, "Entering main loop...\n");

	int            ret_val         =  0;
	int            loop_count      =  0;
	int            overload_count  =  0;
	const int      overload_limit  = 10;
	int            recovery_count  =  0;
	const int      recovery_limit  = 10;
	const int      recovery_limit2 = 50; // For the restart
	int            restart_count   =  0;
	int            restart_limit   = 32;

	while (ret_val == 0 && ! ctx._quit_flag)
	{
		int            wait_ms = 100; // Milliseconds

#if 0 // When doing manual time sharing
		while (! ctx._thread_spi.process_single_task ())
		{
			continue;
		}
#endif

		ctx._model.process_messages ();

		const bool     tuner_flag = ctx._view.is_tuner_active ();

		mfx::MeterResultSet &   meters = ctx._model.use_meters ();

		// Watchdog for audio overloads
		const bool     overload_flag = meters._dsp_overload_flag.exchange (false);
		if (overload_flag)
		{
			wait_ms        = 10;
			recovery_count = 0;
			++ overload_count;
			if (   overload_count > overload_limit
			    && restart_count  < restart_limit)
			{
				snd_drv.restart ();
				overload_count = 0;
				++ restart_count;
			}
		}
		else
		{
			++ recovery_count;
			if (recovery_count > recovery_limit)
			{
				if (recovery_count > recovery_limit2)
				{
					restart_count = 0;
				}
				overload_count = 0;
			}
		}

		// LEDs
		if (! tuner_flag)
		{
			const int      nbr_led           = 3;
			float          lum_arr [nbr_led] = { 0, 0, 0 };
			if (meters.check_signal_clipping ())
			{
				lum_arr [0] = 1;
			}
			if (overload_flag)
			{
				if (restart_count >= restart_limit)
				{
					lum_arr [0] = sqrt (0.375f);
					lum_arr [1] = sqrt (0.125f);
					lum_arr [2] = sqrt (0.5f  );
				}
				else
				{
					lum_arr [2] = 1;
				}
			}
			for (int led_index = 0; led_index < nbr_led; ++led_index)
			{
				float           val = lum_arr [led_index];
				val = val * val;  // Gamma 2.0
				ctx._leds.set_led (led_index, val);
			}
		}

// Pollutes the logs when run in init.d
#if defined (MAIN_USE_VOID) || ! fstb_IS (SYS, LINUX)
		const float  usage_max  = meters._dsp_use._peak;
		const float  usage_avg  = meters._dsp_use._rms;
		const float  period_now = ctx._model.get_audio_period_ratio ();
		char         cpu_0 [127+1] = "Time usage: ------ % / ------ %";
		if (usage_max >= 0 && usage_avg >= 0)
		{
			fstb::snprintf4all (
				cpu_0, sizeof (cpu_0),
				"Time usage: %6.2f %% / %6.2f %%, Speed: %6.2f %%",
				usage_avg * 100, usage_max * 100, 100 / period_now
			);
		}

		fprintf (stderr, "%s\r", cpu_0);
		fflush (stderr);
#endif

		ctx._page_set.use_page_mgr ().process_messages ();

		bool wait_flag = true;

#if 1

		if (wait_flag)
		{
			static const std::chrono::milliseconds wait_duration (wait_ms);
			std::this_thread::sleep_for (wait_duration);
		}

	#if (MAIN_API == MAIN_API_MANUAL)

		{
			mfx::adrv::DManual & snd_drv_man =
				dynamic_cast <mfx::adrv::DManual &> (snd_drv);
			size_t         sample_index = snd_drv_man.get_sample_index ();
			const int      nbr_spl      = ctx._max_block_size;
			const int      nbr_blocks   = fstb::ceil_int (ctx._sample_freq * wait_ms / (1000.0 * nbr_spl));

#if 0
			const int      sec          = int (sample_index / size_t (ctx._sample_freq));
			const size_t   sec_spl     = sec * size_t (ctx._sample_freq);
			if (sec_spl <= sample_index && sample_index < sec_spl + nbr_spl * nbr_blocks)
			{
				printf ("%d ", sec);
				fflush (stdout);
			}
#endif

			float *        in_ptr_arr [mfx::adrv::DriverInterface::_nbr_chn];
			const float *  out_ptr_arr [mfx::adrv::DriverInterface::_nbr_chn];
			snd_drv_man.get_buffers (in_ptr_arr, out_ptr_arr);
			const double   freq = 82.40689; // E
			const double   mul  = 2 * fstb::PI * freq / ctx._sample_freq;
			const float    amp  = 0.1f;
			for (int b = 0; b < nbr_blocks; ++b)
			{
				for (int i = 0; i < nbr_spl; ++i)
				{
					const float     val = float (sin ((sample_index + i) * mul)) * amp; 
					for (int chn = 0; chn < mfx::adrv::DriverInterface::_nbr_chn; ++ chn)
					{
						in_ptr_arr [chn] [i] = val;
					}
				}

				snd_drv_man.process_block ();

				sample_index += nbr_spl;
			}
		}

	#endif

#else

/********************************************* TEMP *********************************/

		const int      nbr_spl = 64;
		float          buf [4] [nbr_spl];
	#if 0
		memset (&buf [0] [0], 0, sizeof (buf [0]));
		memset (&buf [1] [0], 0, sizeof (buf [1]));
	#else
		static double  angle_pos  = 0;
		const double   angle_step = 2 * fstb::PI * 200 / ctx._sample_freq;
		for (int i = 0; i < nbr_spl; ++i)
		{
			const float       val = float (sin (angle_pos));
			buf [0] [i] = val;
			buf [1] [i] = val;
			angle_pos += angle_step;
		}
		angle_pos = fmod (angle_pos, 2 * fstb::PI);
	#endif
		const float *  src_arr [2] = { buf [0], buf [1] };
		float *        dst_arr [2] = { buf [2], buf [3] };

		ctx._model.process_block (dst_arr, src_arr, nbr_spl);

/********************************************* TEMP *********************************/

#endif


/********************************************* TEMP *********************************/
#if 0
		if (loop_count == 10)
		{
			ctx._user_input.send_message (0, mfx::ui::UserInputType_POT, 0, 0.5f);
//			ctx._user_input.send_message (0, mfx::ui::UserInputType_SW, 2, 1);
		}
#endif
/********************************************* TEMP *********************************/

		++ loop_count;
	}

	fprintf (stderr, "Exiting main loop.\n");

	return ret_val;
}



#if defined (_MSC_VER)

static int __cdecl	MAIN_new_handler_cb (size_t dummy)
{
	throw std::bad_alloc ();
	return (0);
}
void MAIN_prog_init ()
{
	::_set_new_handler (::MAIN_new_handler_cb);

#if ! defined (NDEBUG)
	const int	mode =   (1 * _CRTDBG_MODE_DEBUG)
					       | (1 * _CRTDBG_MODE_WNDW);
	::_CrtSetReportMode (_CRT_WARN, mode);
	::_CrtSetReportMode (_CRT_ERROR, mode);
	::_CrtSetReportMode (_CRT_ASSERT, mode);

	const int	old_flags = ::_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
	::_CrtSetDbgFlag (  old_flags
	                  | (1 * _CRTDBG_LEAK_CHECK_DF)
	                  | (0 * _CRTDBG_CHECK_ALWAYS_DF));
	::_CrtSetBreakAlloc (-1);	// Specify here a memory bloc number
#endif	// NDEBUG
}

void MAIN_prog_end ()
{
#if defined (_MSC_VER) && ! defined (NDEBUG)
	const int	mode =   (1 * _CRTDBG_MODE_DEBUG)
					       | (0 * _CRTDBG_MODE_WNDW);
	::_CrtSetReportMode (_CRT_WARN, mode);
	::_CrtSetReportMode (_CRT_ERROR, mode);
	::_CrtSetReportMode (_CRT_ASSERT, mode);

	::_CrtMemState	mem_state;
	::_CrtMemCheckpoint (&mem_state);
	::_CrtMemDumpStatistics (&mem_state);
#endif	// _MSC_VER, NDEBUG
}

#endif // _MSC_VER



#if fstb_IS (SYS, LINUX)
int main (int argc, char *argv [], char *envp [])
#else
int CALLBACK WinMain (::HINSTANCE instance, ::HINSTANCE prev_instance, ::LPSTR cmdline_0, int cmd_show)
#endif
{
#if defined (_MSC_VER)
	MAIN_prog_init ();
#endif

#if fstb_IS (SYS, LINUX) && ! defined (MAIN_USE_VOID)
	::wiringPiSetupPhys ();

	::pinMode (22, INPUT);
	if (::digitalRead (22) == LOW)
	{
		fprintf (stderr, "Emergency exit\n");
		exit (0);
	}

	::pinMode (MAIN_pin_reset, OUTPUT);

	::digitalWrite (MAIN_pin_reset, LOW);
	std::this_thread::sleep_for (std::chrono::milliseconds (100));
	::digitalWrite (MAIN_pin_reset, HIGH);
	std::this_thread::sleep_for (std::chrono::milliseconds (100));
#endif

	mfx::dsp::mix::Align::setup ();

	int            chn_idx_in  = 0;
	int            chn_idx_out = 0;

#if (MAIN_API == MAIN_API_JACK)
	mfx::adrv::DJack  snd_drv;
#elif (MAIN_API == MAIN_API_ALSA)
	mfx::adrv::DAlsa  snd_drv;
#elif (MAIN_API == MAIN_API_ASIO)
	mfx::adrv::DAsio  snd_drv;
	chn_idx_in = 2;
#elif (MAIN_API == MAIN_API_MANUAL)
	mfx::adrv::DManual   snd_drv;
#elif (MAIN_API == MAIN_API_PVAB)
	mfx::adrv::DPvabI2s  snd_drv;
#else
	#error
#endif


	std::unique_ptr <Context>  ctx_uptr (new Context (snd_drv));
	Context &      ctx = *ctx_uptr;
#if fstb_IS (SYS, LINUX)
	ctx._cmd_line.set (argc, argv, envp);
#endif

	int            ret_val = 0;

	if (! ctx._quit_flag)
	{
		double         sample_freq;
		int            max_block_size;
		ret_val = snd_drv.init (
			sample_freq,
			max_block_size,
			ctx,
			0,
			chn_idx_in,
			chn_idx_out
		);

		if (ret_val == 0)
		{
			ctx.set_proc_info (sample_freq, max_block_size);
			ret_val = snd_drv.start ();
		}

		if (ret_val == 0)
		{
			ret_val = MAIN_main_loop (ctx, snd_drv);
		}

		snd_drv.stop ();
	}

	ctx_uptr.reset ();

	fprintf (stderr, "Exiting with code %d.\n", ret_val);

#if defined (_MSC_VER)
	MAIN_prog_end ();
#endif

	return ret_val;
}
