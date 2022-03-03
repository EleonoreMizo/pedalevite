/*****************************************************************************

        WorldAudio.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_WorldAudio_HEADER_INCLUDED)
#define mfx_WorldAudio_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



// For debugging: records all the processing steps
#undef mfx_WorldAudio_BUF_REC

// Define this symbol to use the highest resolution timer, based on clock
// cycles. However it seems worsen the performances on ARM.
#undef mfx_Worldaudio_USE_UNSAFE_CLOCK


/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/CellPool.h"
#include "fstb/AllocAlign.h"
#if defined (mfx_Worldaudio_USE_UNSAFE_CLOCK)
# include "fstb/ClockUnsafe.h"
#endif
#include "fstb/SingleObj.h"
#include "mfx/ui/UserInputInterface.h"
#include "mfx/dsp/dyn/MeterRmsPeakHold.h"
#include "mfx/dsp/dyn/MeterRmsPeakHold4Simd.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/BufPack.h"
#include "mfx/Cst.h"
#include "mfx/MeterResultSet.h"
#include "mfx/ProgSwitcher.h"
#include "mfx/WaMsgQueue.h"

#include <atomic>
#include <vector>

#include <cstdint>



namespace mfx
{

namespace piapi
{
	class ProcInfo;
}

class PluginPool;
class ProcessingContext;

class WorldAudio
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       WorldAudio (PluginPool &plugin_pool, WaMsgQueue &queue_from_cmd, WaMsgQueue &queue_to_cmd, ui::UserInputInterface::MsgQueue &queue_from_input, ui::UserInputInterface &input_device, conc::CellPool <WaMsg> &msg_pool_cmd);
	               ~WorldAudio ();

	void           set_process_info (double sample_freq, int max_block_size);

	void           process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl) noexcept;

	MeterResultSet &
	               use_meters () noexcept;
	float          get_audio_period_ratio () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

// Temporary test code
#if defined (mfx_Worldaudio_USE_UNSAFE_CLOCK)
	typedef fstb::ClockUnsafe::Counter ClockCount;
	fstb_FORCEINLINE ClockCount   read_clock () const noexcept { return fstb::ClockUnsafe::read (); }
	static fstb_FORCEINLINE auto  get_clock_val (ClockCount c) noexcept { return c; }
#else
	typedef std::chrono::microseconds ClockCount;
	fstb_FORCEINLINE ClockCount   read_clock () const noexcept { return _input_device.get_cur_date (); }
	static fstb_FORCEINLINE auto  get_clock_val (ClockCount c) noexcept { return c.count (); }
#endif

	enum class CellRet
	{
		NONE = 0,
		POOL,
		CMD,

		NBR_ELT
	};

	static const int  _max_nbr_evt = 1024; // Per plug-ins. This is a soft limit.
	static const int  _msg_limit   = 1024; // Maximum number of events processed per block, for each queue.

	static_assert (Cst::_max_nbr_input <= Cst::_max_nbr_output * 2, "");
	typedef std::array <
		const float *,
		Cst::_max_nbr_output * 2 * piapi::PluginInterface::_max_nbr_chn
	> BufSrcArr;
	typedef std::array <
		float *,
		Cst::_max_nbr_output * piapi::PluginInterface::_max_nbr_chn
	> BufDstArr;
	typedef std::array <float *, Cst::_max_nbr_sig> BufSigArr;

	typedef std::vector <piapi::EventTs> EventArray;

	typedef std::vector <const piapi::EventTs *> EventPtrList;
	typedef std::array <EventPtrList, Cst::_max_nbr_plugins> EventPtrListArray;

	typedef fstb::SingleObj <
		dsp::dyn::MeterRmsPeakHold4Simd,
		fstb::AllocAlign <dsp::dyn::MeterRmsPeakHold4Simd, 16>
	> LevelMeter;

	typedef std::array <float, Cst::_max_nbr_sig_ports> SigResultArray;

	void           reset_everything ();
	void           reset_plugin (int pi_id);
	void           collect_msg_cmd (bool proc_flag, bool ctx_update_flag);
	void           collect_msg_ui (bool proc_flag);
	void           setup_new_context ();
	void           update_aux_param_pi (const ProcessingContextNode &node);
	void           handle_controller (const ControlSource &controller, float val_raw);
	void           copy_input (const float * const * src_arr, int nbr_spl);
	void           check_signal_level (float * const * dst_arr, const float * const * src_arr, int nbr_spl);
	bool           check_silent_buffer () const;
	void           store_send (int nbr_spl);
	void           copy_output (float * const * dst_arr, int nbr_spl);
	void           process_plugin_bundle (const ProcessingContext::PluginContext &pi_ctx, int nbr_spl);
	void           process_single_plugin (int plugin_id, piapi::ProcInfo &proc_info);
	void           mix_source_channels (const ProcessingContextNode::Side &side, const ProcessingContext::PluginContext::MixInputArray &mix_in_arr, int nbr_spl);
	void           prepare_buffers (piapi::ProcInfo &proc_info, const ProcessingContext::PluginContext &pi_ctx, PiType type, bool use_byp_as_src_flag);
	void           handle_signals (const piapi::ProcInfo &proc_info, const ProcessingContextNode &node);

	void           handle_msg_param (const WaMsg::Param &msg);
	void           handle_msg_tempo (const WaMsg::Tempo &msg);
	void           handle_msg_reset (const WaMsg::Reset &msg);

#if defined (mfx_WorldAudio_BUF_REC)
	void           store_data (const float src_ptr [], int nbr_spl);
#endif
	PluginPool &   _pi_pool;
	WaMsgQueue &   _queue_from_cmd;
	WaMsgQueue &   _queue_to_cmd;
	ui::UserInputInterface::MsgQueue &
	               _queue_from_input;
	ui::UserInputInterface &
	               _input_device;
	conc::CellPool <WaMsg> &
	               _msg_pool_cmd;

	int            _max_block_size;
	float          _sample_freq;

	BufPack        _buf_pack;

	const ProcessingContext *
	               _ctx_ptr;

	LevelMeter     _lvl_meter;
	MeterResultSet _meter_result;       // This structure can be accessed from any thread
	std::atomic <float>                 // Ratio of the actual time / expected time between two calls. Useful to dectect a driver lag or sync errors
	               _period_now;
	float          _rate_expected;      // 1e-6 * Fs / block size

	EventArray     _evt_arr;            // Preallocated
	EventPtrList   _evt_ptr_arr;        // Preallocated

	float          _tempo_new;          // BPM. 0 when nothing new
	float          _tempo_cur;          // BPM, > 0

	bool           _denorm_conf_flag;   // Indicates we have configured the denormal stuff for the processing thread
	ClockCount     _proc_date_beg;
	ClockCount     _proc_date_end;
	dsp::dyn::MeterRmsPeakHold
	               _proc_analyser;

	SigResultArray _sig_res_arr;

	bool           _reset_flag;         // Indicates that we should reset all the chain, because something wrong occured.

	ProgSwitcher   _prog_switcher;

#if defined (mfx_WorldAudio_BUF_REC)
	static const int  _max_rec_duration = 30;   // Seconds. Records to disk once time elapsed.
	bool           _data_rec_flag;
	std::vector <AlignedZone>           // One buffer per recorded channel (in/intermediate/out)
	               _data_rec_arr;
	std::atomic <size_t>                // Current buffer being recorded. Reset at the beginning of each audio block.
	               _data_rec_cur_buf;
	std::atomic <size_t>                // Recording position in samples. Reset to 0 when the recording is saved to disk.
	               _data_rec_pos;
	size_t         _data_rec_len;
#endif



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               WorldAudio ()                               = delete;
	               WorldAudio (const WorldAudio &other)        = delete;
	               WorldAudio (WorldAudio &&other)             = delete;
	WorldAudio &   operator = (const WorldAudio &other)        = delete;
	WorldAudio &   operator = (WorldAudio &&other)             = delete;
	bool           operator == (const WorldAudio &other) const = delete;
	bool           operator != (const WorldAudio &other) const = delete;

}; // class WorldAudio



}  // namespace mfx



//#include "mfx/WorldAudio.hpp"



#endif   // mfx_WorldAudio_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
