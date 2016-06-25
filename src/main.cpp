
// g++ --std=c++11 -I. -Wall -mfpu=neon `pkg-config --cflags --libs jack` -l asound -l wiringPi -l pthread main.cpp mailbox.c fstb/fnc.cpp mfx/*.cpp mfx/tuner/*.cpp mfx/ui/*.cpp
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
#undef MAIN_USE_VOID

#include "fstb/def.h"

#define MAIN_API_JACK 1
#define MAIN_API_ALSA 2
#define MAIN_API_ASIO 3
#if fstb_IS (ARCHI, ARM)
//	#define MAIN_API MAIN_API_JACK
	#define MAIN_API MAIN_API_ALSA
#else
	#define MAIN_API MAIN_API_ASIO
#endif

#include "fstb/AllocAlign.h"
#include "fstb/fnc.h"
#include "mfx/adrv/CbInterface.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/doc/FxId.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dwm/DryWet.h"
#include "mfx/pi/freqsh/FrequencyShifter.h"
#include "mfx/pi/trem1/Tremolo.h"
#include "mfx/pi/wha1/Wha.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/ui/Font.h"
#include "mfx/ui/FontDataDefault.h"
#include "mfx/ui/TimeShareThread.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/Page.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/uitk/ParentInterface.h"
#include "mfx/uitk/pg/CurProg.h"
#include "mfx/uitk/pg/CtrlEdit.h"
#include "mfx/uitk/pg/EditProg.h"
#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/pg/MenuMain.h"
#include "mfx/uitk/pg/MenuSlot.h"
#include "mfx/uitk/pg/NotYet.h"
#include "mfx/uitk/pg/PageType.h"
#include "mfx/uitk/pg/ParamControllers.h"
#include "mfx/uitk/pg/ParamEdit.h"
#include "mfx/uitk/pg/ParamList.h"
#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/pg/Tuner.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/ModelObserverDefault.h"
#include "mfx/MsgQueue.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"
#include "mfx/View.h"
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
		#include "mfx/adrv/DJack.h"
	#elif (MAIN_API == MAIN_API_ALSA)
		#include "mfx/adrv/DAlsa.h"
	#else
		#error
	#endif
	#include <wiringPi.h>
	#include <wiringPiI2C.h>
	#include <wiringPiSPI.h>
	#include <arpa/inet.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <signal.h>

#elif fstb_IS (ARCHI, X86)
	#include "mfx/ui/IoWindows.h"

	#if (MAIN_API == MAIN_API_ASIO)
		#include "mfx/adrv/DAsio.h"
	#else
		#error Wrong MAIN_API value
	#endif

	#include <Windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>

#else
	#error Unsupported architecture

#endif

#if defined (MAIN_USE_VOID)
	#include "mfx/ui/DisplayVoid.h"
	#include "mfx/ui/LedVoid.h"
	#include "mfx/ui/UserInputVoid.h"
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



static std::string MAIN_get_ip_address ()
{
	std::string    ip_addr;

#if fstb_IS (ARCHI, ARM)

	// Source:
	// http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
	int            fd = socket (AF_INET, SOCK_DGRAM, 0);
	struct ifreq   ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	fstb::snprintf4all (ifr.ifr_name, IFNAMSIZ, "%s", "eth0");
	int            ret_val = ioctl (fd, SIOCGIFADDR, &ifr);
	if (ret_val == 0)
	{
		ip_addr = inet_ntoa (((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr);
	}
	close (fd);

#else

	::WSADATA      wsa_data;
	::WSAStartup (2, &wsa_data);
	char           name_0 [255+1];
	int            ret_val = gethostname (name_0, sizeof (name_0));
	if (ret_val == 0)
	{
		::PHOSTENT     hostinfo = gethostbyname (name_0);
		if (hostinfo != 0)
		{
			ip_addr = inet_ntoa (*(struct in_addr *)(*hostinfo->h_addr_list));
		}
	}
	::WSACleanup ();

#endif

	return ip_addr;
}



class Context
:	public mfx::ModelObserverDefault
,	public mfx::adrv::CbInterface
{
public:
	double         _sample_freq;
	int            _max_block_size;
	std::atomic <bool>
	               _dropout_flag;
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
	const int      _tuner_subspl  = 4;
	std::vector <float, fstb::AllocAlign <float, 16 > >
	               _buf_alig;

	// New audio engine
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
#elif fstb_IS (ARCHI, ARM)
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
	mfx::ui::IoWindows
	               _all_io;
	mfx::ui::DisplayInterface &
	               _display;
	mfx::ui::UserInputInterface &
	               _user_input;
	mfx::ui::LedInterface &
	               _leds;
#endif
	mfx::Model     _model;
	std::vector <mfx::pi::PluginModel>
	               _pi_type_list;
	std::vector <mfx::uitk::pg::CtrlSrcNamed>
	               _csn_list;

	// View
	mfx::View      _view;
	mfx::ui::Font  _fnt_8x12;
	mfx::ui::Font  _fnt_6x8;
	mfx::ui::Font  _fnt_6x6;

	mfx::uitk::Rect
	               _inval_rect;
	mfx::LocEdit   _loc_edit;
	mfx::uitk::Page
	               _page_mgr;
	mfx::uitk::PageSwitcher
	               _page_switcher;
	mfx::uitk::pg::CurProg
	               _page_cur_prog;
	mfx::uitk::pg::Tuner
	               _page_tuner;
	mfx::uitk::pg::MenuMain
	               _page_menu_main;
	mfx::uitk::pg::EditProg
	               _page_edit_prog;
	mfx::uitk::pg::ParamList
	               _page_param_list;
	mfx::uitk::pg::ParamEdit
	               _page_param_edit;
	mfx::uitk::pg::NotYet
	               _page_not_yet;
	mfx::uitk::pg::Question
	               _page_question;
	mfx::uitk::pg::ParamControllers
	               _page_param_controllers;
	mfx::uitk::pg::CtrlEdit
	               _page_ctrl_edit;
	mfx::uitk::pg::MenuSlot
	               _page_menu_slot;
	mfx::uitk::pg::EditText
	               _page_edit_text;

	Context ();
	~Context ();
	void           set_proc_info (double sample_freq, int max_block_size);
protected:
	// mfx::ModelObserverDefault
	virtual void   do_set_tuner (bool active_flag);
	// mfx::adrv:CbInterface
	virtual void   do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl);
	virtual void   do_notify_dropout ();
	virtual void   do_request_exit ();
};

Context::Context ()
:	_sample_freq (0)
,	_max_block_size (0)
,	_dropout_flag ()
#if fstb_IS (ARCHI, ARM)
,	_thread_spi (10 * 1000)

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
#elif fstb_IS (ARCHI, ARM)
,	_display (_thread_spi)
,	_user_input (_thread_spi)
,	_leds ()
#else
,	_all_io (_quit_flag)
,	_display (_all_io)
,	_user_input (_all_io)
,	_leds (_all_io)
#endif
,	_model (_queue_input_to_cmd, _queue_input_to_audio, _user_input)
,	_pi_type_list ({
		mfx::pi::PluginModel_DISTO_SIMPLE,
		mfx::pi::PluginModel_TREMOLO,
		mfx::pi::PluginModel_WHA,
		mfx::pi::PluginModel_FREQ_SHIFT
	})
,	_csn_list ({
		{ mfx::ControllerType_POT   ,  0, "Expression 0" },
		{ mfx::ControllerType_POT   ,  1, "Expression 1" },
		{ mfx::ControllerType_POT   ,  2, "Expression 2" },
		{ mfx::ControllerType_ROTENC,  0, "Knob 0"       },
		{ mfx::ControllerType_ROTENC,  1, "Knob 1"       },
		{ mfx::ControllerType_ROTENC,  2, "Knob 2"       },
		{ mfx::ControllerType_ROTENC,  3, "Knob 3"       },
		{ mfx::ControllerType_ROTENC,  4, "Knob 4"       },
		{ mfx::ControllerType_SW    ,  2, "Footsw 0"     },
		{ mfx::ControllerType_SW    ,  3, "Footsw 1"     },
		{ mfx::ControllerType_SW    ,  4, "Footsw 2"     },
		{ mfx::ControllerType_SW    ,  5, "Footsw 3"     },
		{ mfx::ControllerType_SW    ,  6, "Footsw 4"     },
		{ mfx::ControllerType_SW    ,  7, "Footsw 5"     },
		{ mfx::ControllerType_SW    ,  8, "Footsw 6"     },
		{ mfx::ControllerType_SW    ,  9, "Footsw 7"     },
		{ mfx::ControllerType_SW    , 14, "Footsw 8"     },
		{ mfx::ControllerType_SW    , 15, "Footsw 9"     },
		{ mfx::ControllerType_SW    , 16, "Footsw 10"    },
		{ mfx::ControllerType_SW    , 17, "Footsw 11"    }
	})
,	_view ()
,	_fnt_8x12 ()
,	_fnt_6x8 ()
,	_fnt_6x6 ()
,	_inval_rect ()
,	_loc_edit ()
,	_page_mgr (_model, _view, _display, _queue_input_to_gui, _user_input, _fnt_6x6, _fnt_6x8, _fnt_8x12)
,	_page_switcher (_page_mgr)
,	_page_cur_prog (_page_switcher, MAIN_get_ip_address ())
,	_page_tuner (_page_switcher, _leds)
,	_page_menu_main (_page_switcher)
,	_page_edit_prog (_page_switcher, _loc_edit, _pi_type_list)
,	_page_param_list (_page_switcher, _loc_edit)
,	_page_param_edit (_page_switcher, _loc_edit)
,	_page_not_yet (_page_switcher)
,	_page_question (_page_switcher)
,	_page_param_controllers (_page_switcher, _loc_edit, _csn_list)
,	_page_ctrl_edit (_page_switcher, _loc_edit, _csn_list)
,	_page_menu_slot (_page_switcher, _loc_edit, _pi_type_list)
,	_page_edit_text (_page_switcher)
{
	_dropout_flag.store (false);
	_usage_min.store (-1);
	_usage_max.store (-1);
	mfx::ui::FontDataDefault::make_08x12 (_fnt_8x12);
	mfx::ui::FontDataDefault::make_06x08 (_fnt_6x8);
	mfx::ui::FontDataDefault::make_06x06 (_fnt_6x6);

	// Assigns input devices
	/*** To do: build a common table in mfx::Cst for all assignments ***/
	for (int type = 0; type < mfx::ui::UserInputType_NBR_ELT; ++type)
	{
		const int      nbr_param = _user_input.get_nbr_param (
			static_cast <mfx::ui::UserInputType> (type)
		);
		for (int index = 0; index < nbr_param; ++index)
		{
			mfx::ui::UserInputInterface::MsgQueue * queue_ptr =
				&_queue_input_to_cmd;
			if (   type == mfx::ui::UserInputType_POT
			    || type == mfx::ui::UserInputType_ROTENC)
			{
				queue_ptr = &_queue_input_to_audio;
			}
			if (   type == mfx::ui::UserInputType_SW
			    && (index == 0 || index == 1 || (index >= 10 && index < 14)))
			{
				/*** To do: rotenc 5 and 6 ***/
				queue_ptr = &_queue_input_to_gui;
			}
			_user_input.set_msg_recipient (
				static_cast <mfx::ui::UserInputType> (type),
				index, queue_ptr
			);
		}
	}

	_model.set_observer (&_view);
	_view.add_observer (*this);

	mfx::doc::Bank bank;
	bank._name = "Cr\xC3\xA9" "dit Usurier";
	for (auto &preset : bank._preset_arr)
	{
		preset._name = "<Empty preset>";
	}
	{
		mfx::doc::Preset& preset   = bank._preset_arr [0];
		preset._name = "Basic disto";
		mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
		preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
		slot_ptr->_label    = "Disto 1";
		slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
		slot_ptr->_settings_mixer._param_list =
			std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });
		mfx::doc::PluginSettings & pi_settings =
			slot_ptr->_settings_all [slot_ptr->_pi_model];

		pi_settings._param_list = std::vector <float> (1, 0);

		mfx::doc::CtrlLinkSet cls;

		cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
		cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_POT);
		cls._bind_sptr->_source._index = 0;
		cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
		cls._bind_sptr->_u2b_flag      = false;
		cls._bind_sptr->_base          = 0;
		cls._bind_sptr->_amp           = 1;
		pi_settings._map_param_ctrl [mfx::pi::dist1::DistoSimple::Param_GAIN] = cls;

		{
			mfx::doc::PedalActionCycle &  cycle =
				preset._layout._pedal_arr [11]._action_arr [mfx::doc::ActionTrigger_PRESS];
			const mfx::doc::FxId    fx_id (slot_ptr->_label, mfx::PiType_MIX);
			mfx::doc::PedalActionCycle::ActionArray   action_arr (1);
			for (int i = 0; i < 2; ++i)
			{
				static const float val_arr [2] = { 1, 0 };
				const float        val = val_arr [i];
				action_arr [0] = mfx::doc::PedalActionCycle::ActionSPtr (
					new mfx::doc::ActionParam (fx_id, mfx::pi::dwm::DryWet::Param_BYPASS, val)
				);
				cycle._cycle.push_back (action_arr);
			}
		}
	}
	{
		mfx::doc::Preset& preset   = bank._preset_arr [1];
		preset._name = "Tremolisto";
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Disto 1";
			slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			pi_settings._param_list = std::vector <float> (1, 0.125f);

			{
				mfx::doc::PedalActionCycle &  cycle =
					preset._layout._pedal_arr [11]._action_arr [mfx::doc::ActionTrigger_PRESS];
				const mfx::doc::FxId    fx_id (slot_ptr->_label, mfx::PiType_MIX);
				mfx::doc::PedalActionCycle::ActionArray   action_arr (1);
				for (int i = 0; i < 2; ++i)
				{
					static const float val_arr [2] = { 1, 0 };
					const float        val = val_arr [i];
					action_arr [0] = mfx::doc::PedalActionCycle::ActionSPtr (
						new mfx::doc::ActionParam (fx_id, mfx::pi::dwm::DryWet::Param_BYPASS, val)
					);
					cycle._cycle.push_back (action_arr);
				}
			}
		}
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Tremolo";
			slot_ptr->_pi_model = mfx::pi::PluginModel_TREMOLO;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			pi_settings._param_list = std::vector <float> ({
				0.45f, 0.31f, 0, 0.75f, 0.5f
			});

			mfx::doc::CtrlLinkSet cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_POT);
			cls._bind_sptr->_source._index = 0;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::trem1::Tremolo::Param_AMT] = cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 5;
			cls._bind_sptr->_step          = 0.02f;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::trem1::Tremolo::Param_FREQ] = cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 6;
			cls._bind_sptr->_step          = 1.0f / (mfx::pi::trem1::Tremolo::Waveform_NBR_ELT - 1);
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::trem1::Tremolo::Param_WF] = cls;
		}
	}
	{
		mfx::doc::Preset& preset   = bank._preset_arr [2];
		preset._name = "Ouah-ouah";
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Wha";
			slot_ptr->_pi_model = mfx::pi::PluginModel_WHA;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			pi_settings._param_list = std::vector <float> ({
				0.5f, 1.0f/3
			});

			mfx::doc::CtrlLinkSet cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_POT);
			cls._bind_sptr->_source._index = 0;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0.35f;	// Limits the range to the CryBaby's
			cls._bind_sptr->_amp           = 0.75f - cls._bind_sptr->_base;
			pi_settings._map_param_ctrl [mfx::pi::wha1::Wha::Param_FREQ] = cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 5;
			cls._bind_sptr->_step          = float (mfx::Cst::_step_param);
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::wha1::Wha::Param_Q] = cls;

#if 0
			mfx::doc::ParamPresentation pp;
			pp._disp_mode = mfx::doc::ParamPresentation::DispMode_NOTE;
			pi_settings._map_param_pres [mfx::pi::wha1::Wha::Param_FREQ] = pp;
#endif
		}
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Disto 1";
			slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			mfx::doc::CtrlLinkSet cls;
			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 6;
			cls._bind_sptr->_step          = float (mfx::Cst::_step_param);
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::dist1::DistoSimple::Param_GAIN] = cls;

			pi_settings._param_list = std::vector <float> (1, 0.25f);

			{
				mfx::doc::PedalActionCycle &  cycle =
					preset._layout._pedal_arr [11]._action_arr [mfx::doc::ActionTrigger_PRESS];
				const mfx::doc::FxId    fx_id (slot_ptr->_label, mfx::PiType_MIX);
				mfx::doc::PedalActionCycle::ActionArray   action_arr (1);
				for (int i = 0; i < 2; ++i)
				{
					static const float val_arr [2] = { 1, 0 };
					const float        val = val_arr [i];
					action_arr [0] = mfx::doc::PedalActionCycle::ActionSPtr (
						new mfx::doc::ActionParam (fx_id, mfx::pi::dwm::DryWet::Param_BYPASS, val)
					);
					cycle._cycle.push_back (action_arr);
				}
			}
		}
	}
	{
		mfx::doc::Preset& preset   = bank._preset_arr [3];
		preset._name = "Inharmonic";
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "FreqShift";
			slot_ptr->_pi_model = mfx::pi::PluginModel_FREQ_SHIFT;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });

			mfx::doc::CtrlLinkSet cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 5;
			cls._bind_sptr->_step          = float (mfx::Cst::_step_param);
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			slot_ptr->_settings_mixer._map_param_ctrl [mfx::pi::dwm::DryWet::Param_WET] = cls;

			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];
			pi_settings._param_list = std::vector <float> ({
				0.5f
			});

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_POT);
			cls._bind_sptr->_source._index = 0;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0.5f;   // More accuracy on the useful range
			cls._bind_sptr->_amp           = 1.0f - cls._bind_sptr->_base;
			{
				mfx::pi::freqsh::FrequencyShifter dummy;
				const mfx::piapi::ParamDescInterface & dd = dummy.get_param_info (
					mfx::piapi::ParamCateg_GLOBAL,
					mfx::pi::freqsh::FrequencyShifter::Param_FREQ
				);
				for (int oct = 2; oct < 7; ++oct)
				{
					std::array <int, 2> note_arr = {{ 0, 7 }};
					for (size_t n = 0; n < note_arr.size (); ++n)
					{
						const int      note = oct * 12 + note_arr [n];
						const double   freq = 440 * pow (2, (note - 69) / 12.0);
						const double   nrm  = dd.conv_nat_to_nrm (freq);
						cls._bind_sptr->_notch_list.insert (float (nrm));
					}
				}
			}
			pi_settings._map_param_ctrl [mfx::pi::freqsh::FrequencyShifter::Param_FREQ] = cls;
		}
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Disto 1";
			slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::dwm::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			mfx::doc::CtrlLinkSet cls_main;
			cls_main._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls_main._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls_main._bind_sptr->_source._index = 6;
			cls_main._bind_sptr->_step          = float (mfx::Cst::_step_param);
			cls_main._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls_main._bind_sptr->_u2b_flag      = false;
			cls_main._bind_sptr->_base          = 0;
			cls_main._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::dist1::DistoSimple::Param_GAIN] = cls_main;

			pi_settings._param_list = std::vector <float> (1, 0.25f);

			{
				mfx::doc::PedalActionCycle &  cycle =
					preset._layout._pedal_arr [11]._action_arr [mfx::doc::ActionTrigger_PRESS];
				const mfx::doc::FxId    fx_id (slot_ptr->_label, mfx::PiType_MIX);
				mfx::doc::PedalActionCycle::ActionArray   action_arr (1);
				for (int i = 0; i < 2; ++i)
				{
					static const float val_arr [2] = { 1, 0 };
					const float        val = val_arr [i];
					action_arr [0] = mfx::doc::PedalActionCycle::ActionSPtr (
						new mfx::doc::ActionParam (fx_id, mfx::pi::dwm::DryWet::Param_BYPASS, val)
					);
					cycle._cycle.push_back (action_arr);
				}
			}
		}
	}

	for (int p = 0; p < 5; ++p)
	{
		mfx::doc::PedalActionCycle &  cycle =
			bank._layout._pedal_arr [p]._action_arr [mfx::doc::ActionTrigger_PRESS];
		mfx::doc::PedalActionCycle::ActionArray   action_arr;
		action_arr.push_back (mfx::doc::PedalActionCycle::ActionSPtr (
			new mfx::doc::ActionPreset (false, p)
		));
		cycle._cycle.push_back (action_arr);
	}
	{
		mfx::doc::PedalActionCycle &  cycle =
			bank._layout._pedal_arr [5]._action_arr [mfx::doc::ActionTrigger_PRESS];
		mfx::doc::PedalActionCycle::ActionArray   action_arr;
		action_arr.push_back (mfx::doc::PedalActionCycle::ActionSPtr (
			new mfx::doc::ActionToggleTuner
		));
		cycle._cycle.push_back (action_arr);
	}

	_model.set_bank (0, bank);
	_model.select_bank (0);
	_model.activate_preset (3);

	_page_switcher.add_page (mfx::uitk::pg::PageType_CUR_PROG         , _page_cur_prog         );
	_page_switcher.add_page (mfx::uitk::pg::PageType_TUNER            , _page_tuner            );
	_page_switcher.add_page (mfx::uitk::pg::PageType_MENU_MAIN        , _page_menu_main        );
	_page_switcher.add_page (mfx::uitk::pg::PageType_EDIT_PROG        , _page_edit_prog        );
	_page_switcher.add_page (mfx::uitk::pg::PageType_PARAM_LIST       , _page_param_list       );
	_page_switcher.add_page (mfx::uitk::pg::PageType_PARAM_EDIT       , _page_param_edit       );
	_page_switcher.add_page (mfx::uitk::pg::PageType_NOT_YET          , _page_not_yet          );
	_page_switcher.add_page (mfx::uitk::pg::PageType_QUESTION         , _page_question         );
	_page_switcher.add_page (mfx::uitk::pg::PageType_PARAM_CONTROLLERS, _page_param_controllers);
	_page_switcher.add_page (mfx::uitk::pg::PageType_CTRL_EDIT        , _page_ctrl_edit        );
	_page_switcher.add_page (mfx::uitk::pg::PageType_MENU_SLOT        , _page_menu_slot        );
	_page_switcher.add_page (mfx::uitk::pg::PageType_EDIT_TEXT        , _page_edit_text        );

	_page_switcher.switch_to (mfx::uitk::pg::PageType_CUR_PROG, 0);
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
		_page_switcher.call_page (
			mfx::uitk::pg::PageType_TUNER,
			0,
			_page_mgr.get_cursor_node ()
		);
	}
}

void	Context::do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	const int64_t  time_beg    = MAIN_get_time ();
	const int64_t  dur_tot     =  time_beg - _time_beg;
	const int64_t  dur_act     = _time_end - _time_beg;
	const float    usage_frame = float (dur_act) / float (dur_tot);
	float          usage_max   = _usage_max.load ();
	float          usage_min   = _usage_min.load ();
	usage_max = std::max (usage_max, usage_frame);
	usage_min = (usage_min < 0) ? usage_frame : std::min (usage_min, usage_frame);
	_usage_max.store (usage_max);
	_usage_min.store (usage_min);
	_time_beg = time_beg;

	// Audio graph
	_model.process_block (dst_arr, src_arr, nbr_spl);

	_time_end = MAIN_get_time ();
}

void	Context::do_notify_dropout ()
{
	_dropout_flag.exchange (true);
}

void	Context::do_request_exit ()
{
	_quit_flag = true;
}



static int MAIN_main_loop (Context &ctx)
{
	fprintf (stderr, "Entering main loop...\n");

	int            ret_val = 0;
	int            loop_count = 0;

	while (ret_val == 0 && ! ctx._quit_flag)
	{
#if 0 // When doing manual time sharing
		while (! ctx._thread_spi.process_single_task ())
		{
			continue;
		}
#endif

		ctx._model.process_messages ();

		const bool     tuner_flag = ctx._view.is_tuner_active ();

		const mfx::ModelObserverInterface::SlotInfoList & slot_info_list =
			ctx._view.use_slot_info_list ();

		const float  usage_max  = ctx._usage_max.exchange (-1);
		const float  usage_min  = ctx._usage_min.exchange (-1);
		char         cpu_0 [127+1] = "Time usage: ------ % / ------ %";
		if (usage_max >= 0 && usage_min >= 0)
		{
			fstb::snprintf4all (
				cpu_0, sizeof (cpu_0),
				"Time usage: %6.2f %% / %6.2f %%",
				usage_min * 100, usage_max * 100
			);
		}

		if (! tuner_flag)
		{
			const int      nbr_led           = 3;
			float          lum_arr [nbr_led] = { 0, 0, 0 };
			if (ctx._model.check_signal_clipping ())
			{
				lum_arr [0] = 1;
			}
			if (ctx._dropout_flag.exchange (false))
			{
				lum_arr [2] = 1;
			}
			for (int led_index = 0; led_index < nbr_led; ++led_index)
			{
				float           val = lum_arr [led_index];
				val = val * val;  // Gamma 2.0
				ctx._leds.set_led (led_index, val);
			}
		}

		fprintf (stderr, "%s\r", cpu_0);
		fflush (stderr);

		ctx._page_mgr.process_messages ();

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

		MAIN_audio_process (ctx, dst_arr, src_arr, nbr_spl);

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



#if fstb_IS (ARCHI, ARM)
int main (int argc, char *argv [])
#else
int CALLBACK WinMain (::HINSTANCE instance, ::HINSTANCE prev_instance, ::LPSTR cmdline_0, int cmd_show)
#endif
{
#if fstb_IS (ARCHI, ARM)
	::wiringPiSetupPhys ();

	::pinMode (MAIN_pin_reset, OUTPUT);

	::digitalWrite (MAIN_pin_reset, LOW);
	::delay (100);
	::digitalWrite (MAIN_pin_reset, HIGH);
	::delay (100);
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
#else
	#error
#endif


	std::unique_ptr <Context>  ctx_uptr (new Context);
	Context &      ctx = *ctx_uptr;

	double         sample_freq;
	int            max_block_size;
	int            ret_val = snd_drv.init (
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
		ret_val = MAIN_main_loop (ctx);
	}

	snd_drv.stop ();

	ctx_uptr.reset ();

	fprintf (stderr, "Exiting with code %d.\n", ret_val);

	return ret_val;
}
