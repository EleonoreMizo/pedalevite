
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
#undef MAIN_USE_VOID

#include "fstb/def.h"

#define MAIN_API_JACK 1
#define MAIN_API_ALSA 2
#define MAIN_API_ASIO 3
#if fstb_IS (ARCHI, ARM)
	#define MAIN_API MAIN_API_JACK
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
#include "mfx/pi/DistoSimple.h"
#include "mfx/pi/DryWet.h"
#include "mfx/pi/FrequencyShifter.h"
#include "mfx/pi/Tremolo.h"
#include "mfx/pi/Tuner.h"
#include "mfx/pi/Wha.h"
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
		#include "mfx/adrv/DJack.h"
	#elif (MAIN_API == MAIN_API_ALSA)
		#include "mfx/adrv/DAlsa.h"
	#else
		#error
	#endif
	#include <wiringPi.h>
	#include <wiringPiI2C.h>
	#include <wiringPiSPI.h>
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



static void MAIN_print_text (int x, int y, const char *txt_0, uint8_t *screenbuf_ptr, int stride, const mfx::ui::Font &fnt, int mag_x, int mag_y)
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



class Context
:	public mfx::ModelObserverInterface
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

	// View
	mfx::ui::Font  _fnt_8x12;
	mfx::ui::Font  _fnt_6x8;
	mfx::ui::Font  _fnt_6x6;
	mfx::doc::Setup
	               _setup;
	int            _preset_index  = 0;
	int            _bank_index    = 0;
	mfx::ModelObserverInterface::SlotInfoList
	               _slot_info_list;
	volatile bool  _tuner_flag    = false;
	volatile float _detected_freq = 0;
	int            _disp_cur_slot = -1; // Negative: displays the preset page

	Context ();
	~Context ();
	void           set_proc_info (double sample_freq, int max_block_size);
	void           display_page_preset ();
	void           display_page_efx (int slot_index);
	void           display_page_tuner (const char *note_0);
	static void    video_invert (int x, int y, int w, int h, uint8_t *buf_ptr, int stride);
protected:
	// mfx::ModelObserverInterface
	virtual void   do_set_edit_mode (bool edit_flag);
	virtual void   do_set_pedalboard_layout (const mfx::doc::PedalboardLayout &layout);
	virtual void   do_set_bank (int index, const mfx::doc::Bank &bank);
	virtual void   do_select_bank (int index);
	virtual void   do_activate_preset (int index);
	virtual void   do_store_preset (int index);
	virtual void   do_set_tuner (bool active_flag);
	virtual void   do_set_tuner_freq (float freq);
	virtual void	do_set_slot_info_for_current_preset (const mfx::ModelObserverInterface::SlotInfoList &info_list);
	virtual void   do_set_param (int pi_id, int index, float val, int slot_index, mfx::PiType type);
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
,	_fnt_8x12 ()
,	_fnt_6x8 ()
,	_fnt_6x6 ()
,	_slot_info_list ()
{
	_dropout_flag.store (false);
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
				  (   type == mfx::ui::UserInputType_POT
				   || type == mfx::ui::UserInputType_ROTENC)
				? &_queue_input_to_audio
				: &_queue_input_to_cmd
			);
		}
	}

	_model.set_observer (this);

	mfx::doc::Bank bank;
	{
		mfx::doc::Preset& preset   = bank._preset_arr [0];
		preset._name = "Basic disto";
		mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
		preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
		slot_ptr->_label    = "Disto 1";
		slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
		slot_ptr->_settings_mixer._param_list =
			std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
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
		pi_settings._map_param_ctrl [mfx::pi::DistoSimple::Param_GAIN] = cls;

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
					new mfx::doc::ActionParam (fx_id, mfx::pi::DryWet::Param_BYPASS, val)
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
				std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
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
						new mfx::doc::ActionParam (fx_id, mfx::pi::DryWet::Param_BYPASS, val)
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
				std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
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
			pi_settings._map_param_ctrl [mfx::pi::Tremolo::Param_AMT] = cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 5;
			cls._bind_sptr->_step          = 0.02f;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::Tremolo::Param_FREQ] = cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 6;
			cls._bind_sptr->_step          = 1.0f / (mfx::pi::Tremolo::Waveform_NBR_ELT - 1);
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::Tremolo::Param_WF] = cls;
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
				std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
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
			pi_settings._map_param_ctrl [mfx::pi::Wha::Param_FREQ] = cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 5;
			cls._bind_sptr->_step          = 0.05f;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::Wha::Param_Q] = cls;
		}
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Disto 1";
			slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			mfx::doc::CtrlLinkSet cls;
			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 6;
			cls._bind_sptr->_step          = 0.05f;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::DistoSimple::Param_GAIN] = cls;

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
						new mfx::doc::ActionParam (fx_id, mfx::pi::DryWet::Param_BYPASS, val)
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
				std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });

			mfx::doc::CtrlLinkSet cls;

			cls._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls._bind_sptr->_source._index = 5;
			cls._bind_sptr->_step          = 0.05f;
			cls._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls._bind_sptr->_u2b_flag      = false;
			cls._bind_sptr->_base          = 0;
			cls._bind_sptr->_amp           = 1;
			slot_ptr->_settings_mixer._map_param_ctrl [mfx::pi::DryWet::Param_WET] = cls;

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
			pi_settings._map_param_ctrl [mfx::pi::FrequencyShifter::Param_FREQ] = cls;
		}
		{
			mfx::doc::Slot *  slot_ptr = new mfx::doc::Slot;
			preset._slot_list.push_back (mfx::doc::Preset::SlotSPtr (slot_ptr));
			slot_ptr->_label    = "Disto 1";
			slot_ptr->_pi_model = mfx::pi::PluginModel_DISTO_SIMPLE;
			slot_ptr->_settings_mixer._param_list =
				std::vector <float> ({ 0, 1, mfx::pi::DryWet::_gain_neutral });
			mfx::doc::PluginSettings & pi_settings =
				slot_ptr->_settings_all [slot_ptr->_pi_model];

			mfx::doc::CtrlLinkSet cls_main;
			cls_main._bind_sptr = mfx::doc::CtrlLinkSet::LinkSPtr (new mfx::doc::CtrlLink);
			cls_main._bind_sptr->_source._type  = mfx::ControllerType (mfx::ui::UserInputType_ROTENC);
			cls_main._bind_sptr->_source._index = 6;
			cls_main._bind_sptr->_step          = 0.05f;
			cls_main._bind_sptr->_curve         = mfx::ControlCurve_LINEAR;
			cls_main._bind_sptr->_u2b_flag      = false;
			cls_main._bind_sptr->_base          = 0;
			cls_main._bind_sptr->_amp           = 1;
			pi_settings._map_param_ctrl [mfx::pi::DistoSimple::Param_GAIN] = cls_main;

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
						new mfx::doc::ActionParam (fx_id, mfx::pi::DryWet::Param_BYPASS, val)
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

void	Context::display_page_preset ()
{
	uint8_t *      p_ptr  = _display.use_screen_buf ();
	const int      scr_w  = _display.get_width ();
	const int      scr_h  = _display.get_height ();
	const int      scr_s  = _display.get_stride ();

	memset (p_ptr, 0, scr_s * scr_h);

	const mfx::ui::Font &   fnt_big = _fnt_8x12;
	const mfx::ui::Font &   fnt_sml = _fnt_6x6;
	const int      nbr_chr_big = scr_w / fnt_big.get_char_w ();
	const int      nbr_chr_sml = scr_w / fnt_sml.get_char_w ();
	const int      fx_list_y   = fnt_big.get_char_h () + 2;
	const int      chr_h_sml   = fnt_sml.get_char_h ();
	const int      nbr_lines   = (scr_h - fx_list_y) / chr_h_sml;

	const int      max_textlen = 1023;
	char           txt_0 [max_textlen+1];
	assert (nbr_chr_big <= max_textlen);
	assert (nbr_chr_sml <= max_textlen);

	const mfx::doc::Preset &   preset =
		_setup._bank_arr [_bank_index]._preset_arr [_preset_index];

	// Preset title
	fstb::snprintf4all (
		txt_0, nbr_chr_big + 1, "%02d %s",
		_preset_index + 1,
		preset._name.c_str ()
	);
	MAIN_print_text (0, 0, txt_0, p_ptr, scr_s, fnt_big, 1, 1);

	// Effect list
	int            line_pos = 0;
	for (const auto &slot_sptr : preset._slot_list)
	{
		if (line_pos >= nbr_lines)
		{
			break;
		}
		if (slot_sptr.get () != 0)
		{
			const mfx::doc::Slot &  slot = *slot_sptr;
			std::string    pi_type_name =
				mfx::pi::PluginModel_get_name (slot._pi_model);
			pi_type_name = mfx::pi::param::Tools::print_name_bestfit (
				nbr_chr_sml, pi_type_name.c_str ()
			);
			memcpy (txt_0, pi_type_name.c_str (), pi_type_name.length () + 1);
			const int      y = fx_list_y + line_pos * chr_h_sml;
			MAIN_print_text (0, y, txt_0, p_ptr, scr_s, fnt_sml, 1, 1);
			bool           mod_flag = (! slot._settings_mixer._map_param_ctrl.empty ());
			if (! mod_flag)
			{
				const auto     it = slot._settings_all.find (slot._pi_model);
				if (it != slot._settings_all.end ())
				{
					mod_flag = (! it->second._map_param_ctrl.empty ());
				}
			}
			if (mod_flag)
			{
				video_invert (0, y, scr_w, chr_h_sml, p_ptr, scr_s);
			}
			++ line_pos;
		}
	}

	_display.refresh (0, 0, scr_w, scr_h);
}

void	Context::display_page_efx (int slot_index)
{
	uint8_t *      p_ptr  = _display.use_screen_buf ();
	const int      scr_w  = _display.get_width ();
	const int      scr_h  = _display.get_height ();
	const int      scr_s  = _display.get_stride ();

	memset (p_ptr, 0, scr_s * scr_h);

	const mfx::ui::Font &   fnt_big = _fnt_8x12;
	const mfx::ui::Font &   fnt_mid = _fnt_6x8;
	const mfx::ui::Font &   fnt_sml = _fnt_6x6;
	const int      nbr_chr_big = scr_w / fnt_big.get_char_w ();
	const int      nbr_chr_mid = scr_w / fnt_mid.get_char_w ();
	const int      nbr_chr_sml = scr_w / fnt_sml.get_char_w ();
	const int      par_list_y  = fnt_big.get_char_h () + 2 + fnt_mid.get_char_h () + 2;
	const int      chr_h_sml   = fnt_sml.get_char_h ();
	const int      nbr_lines   = (scr_h - par_list_y) / chr_h_sml;

	const int      max_textlen = 1023;
	char           txt_0 [max_textlen+1];
	assert (nbr_chr_big <= max_textlen);
	assert (nbr_chr_mid <= max_textlen);
	assert (nbr_chr_sml <= max_textlen);

	const mfx::doc::Preset &   preset =
		_setup._bank_arr [_bank_index]._preset_arr [_preset_index];

	// Preset title
	fstb::snprintf4all (
		txt_0, nbr_chr_big + 1, "%02d %s",
		_preset_index + 1,
		preset._name.c_str ()
	);
	MAIN_print_text (0, 0, txt_0, p_ptr, scr_s, fnt_big, 1, 1);

	// Effect name
	const mfx::doc::Slot &  slot = *(preset._slot_list [slot_index]);
	std::string    pi_type_name =
		mfx::pi::PluginModel_get_name (slot._pi_model);
	pi_type_name = mfx::pi::param::Tools::print_name_bestfit (
		nbr_chr_mid, pi_type_name.c_str ()
	);
	memcpy (txt_0, pi_type_name.c_str (), pi_type_name.length () + 1);
	MAIN_print_text (
		0, fnt_big.get_char_h () + 2, txt_0, p_ptr, scr_s, fnt_mid, 1, 1
	);

	// Parameter list
	int            line_pos = 0;
	for (int type = 0; type < mfx::PiType_NBR_ELT && line_pos < nbr_lines; ++type)
	{
		const mfx::doc::PluginSettings * settings_ptr =
			&slot._settings_mixer;
		if (type == mfx::PiType_MAIN)
		{
			const auto     it = slot._settings_all.find (slot._pi_model);
			if (it == slot._settings_all.end ())
			{
				assert (false);
			}
			else
			{
				settings_ptr = &it->second;
			}
		}
		const int      nbr_param = int (settings_ptr->_param_list.size ());
		for (int p = 0; p < nbr_param && line_pos < nbr_lines; ++p)
		{
			if (   _slot_info_list.empty ()
			    || _slot_info_list [slot_index] [type].get () == 0)
			{
				// When plug-in is not available
				if (type == mfx::PiType_MIX)
				{
					break;
				}
				fstb::snprintf4all (
					txt_0, nbr_chr_sml + 1, "%-4d%*.4f",
					p,
					nbr_chr_sml - 4,
					settings_ptr->_param_list [p]
				);
			}

			else
			{
				const mfx::ModelObserverInterface::PluginInfo & pi_info =
					(*_slot_info_list [slot_index] [type]);
				const mfx::piapi::ParamDescInterface & desc =
					pi_info._pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, p);

				const double   nat = desc.conv_nrm_to_nat (pi_info._param_arr [p]);
				const std::string val_s = desc.conv_nat_to_str (nat, nbr_chr_sml);

				const int      max_name_len = int (nbr_chr_sml - val_s.length ());
				const std::string name = desc.get_name (max_name_len);

				fstb::snprintf4all (
					txt_0, nbr_chr_sml + 1, "%*s%s",
					-max_name_len,
					name.c_str (),
					val_s.c_str ()
				);
			}

			const int      y = par_list_y + line_pos * chr_h_sml;
			MAIN_print_text (0, y, txt_0, p_ptr, scr_s, fnt_sml, 1, 1);

			++ line_pos;
		}
	}

	_display.refresh (0, 0, scr_w, scr_h);
}

void	Context::display_page_tuner (const char *note_0)
{
	uint8_t *      p_ptr  = _display.use_screen_buf ();
	const int      scr_w  = _display.get_width ();
	const int      scr_h  = _display.get_height ();
	const int      scr_s  = _display.get_stride ();

	memset (p_ptr, 0, scr_s * scr_h);

	const int      char_w  = _fnt_8x12.get_char_w ();
	const int      char_h  = _fnt_8x12.get_char_h ();
	const int      txt_len = int (strlen (note_0));
	const int      mag_x   = scr_w / (txt_len * char_w);
	const int      mag_y   = scr_h / char_h;
	const int      pos_x   = (scr_w - txt_len * char_w * mag_x) >> 1;
	const int      pos_y   = (scr_h -           char_h * mag_y) >> 1;
	MAIN_print_text (pos_x, pos_y, note_0, p_ptr, scr_s, _fnt_8x12, mag_x, mag_y);
	_display.refresh (0, 0, scr_w, scr_h);
}

void	Context::video_invert (int x, int y, int w, int h, uint8_t *buf_ptr, int stride)
{
	buf_ptr += y * stride + x;
	for (y = 0; y < h; ++y)
	{
		for (x = 0; x < w; ++x)
		{
			buf_ptr [x] = ~buf_ptr [x];
		}
		buf_ptr += stride;
	}
}

void	Context::do_set_edit_mode (bool edit_flag)
{
	// Nothing
}

void	Context::do_set_pedalboard_layout (const mfx::doc::PedalboardLayout &layout)
{
	// Nothing
}

void	Context::do_set_bank (int index, const mfx::doc::Bank &bank)
{
	_setup._bank_arr [index] = bank;
}

void	Context::do_select_bank (int index)
{
	_bank_index = index;
}

void	Context::do_activate_preset (int index)
{
	_preset_index = index;
	_slot_info_list.clear ();
	_disp_cur_slot = -1;
}

void	Context::do_store_preset (int index)
{
	// Nothing
}

void	Context::do_set_tuner (bool active_flag)
{
	_tuner_flag = active_flag;
	_disp_cur_slot = -1;
}

void	Context::do_set_tuner_freq (float freq)
{
	_detected_freq = freq;
}

void	Context::do_set_slot_info_for_current_preset (const mfx::ModelObserverInterface::SlotInfoList &info_list)
{
	_slot_info_list = info_list;
}

void	Context::do_set_param (int pi_id, int index, float val, int slot_index, mfx::PiType type)
{
	// Nothing
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
		
		const bool     tuner_flag = ctx._tuner_flag;
		bool           disto_flag = false;
		float          disto_gain = 1;

		if (! ctx._slot_info_list.empty () && ctx._preset_index < 2)
		{
			mfx::ModelObserverInterface::PluginInfoSPtr pi_efx_sptr =
				ctx._slot_info_list [0] [mfx::PiType_MAIN];
			mfx::ModelObserverInterface::PluginInfoSPtr pi_mix_sptr =
				ctx._slot_info_list [0] [mfx::PiType_MIX ];

			if (! tuner_flag)
			{
				disto_flag = (pi_mix_sptr->_param_arr [mfx::pi::DryWet::Param_BYPASS] < 0.5f);
				const mfx::piapi::ParamDescInterface & desc =
					pi_efx_sptr->_pi.get_param_info (
						mfx::piapi::ParamCateg_GLOBAL,
						mfx::pi::DistoSimple::Param_GAIN
					);
				const float  disto_gain_nrm =
					pi_efx_sptr->_param_arr [mfx::pi::DistoSimple::Param_GAIN];
				disto_gain = float (desc.conv_nrm_to_nat (disto_gain_nrm));
			}
		}
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

		if (ctx._model.check_signal_clipping ())
		{
			lum_arr [0] = 1;
		}
		if (ctx._dropout_flag.exchange (false))
		{
			lum_arr [2] = 1;
		}

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

		char disto_gain_0 [255+1] = "------";
		if (! tuner_flag)
		{
			fstb::snprintf4all (
				disto_gain_0, sizeof (disto_gain_0),
				"%6.1f",
				disto_gain
			);
		}
		char param_0 [255+1];
		fstb::snprintf4all (
			param_0, sizeof (param_0),
			"[%s] [%s] [%s]",
			tuner_flag ? "T" : " ",
			tuner_flag ? "-" : disto_flag ? "D" : " ",
			disto_gain_0
		);

		fprintf (stderr, "%s %s %s\r", cpu_0, freq_0, param_0);
		fflush (stderr);

		// Display test
		if (tuner_flag)
		{
			ctx.display_page_tuner (note3_0);
		}
		else
		{
			const mfx::doc::Preset &   preset =
				ctx._setup._bank_arr [ctx._bank_index]._preset_arr [ctx._preset_index];
			const int    nbr_slots = int (preset._slot_list.size ());
			if (ctx._disp_cur_slot < 0 || ctx._disp_cur_slot >= nbr_slots)
			{
				ctx.display_page_preset ();
			}
			else
			{
				ctx.display_page_efx (ctx._disp_cur_slot);
			}
			if ((loop_count & 31) == 31)
			{
				do
				{
					++ ctx._disp_cur_slot;
				}
				while (ctx._disp_cur_slot < nbr_slots && preset._slot_list [ctx._disp_cur_slot].get () == 0);
				if (ctx._disp_cur_slot >= nbr_slots)
				{
					ctx._disp_cur_slot = -1;
				}
			}
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
