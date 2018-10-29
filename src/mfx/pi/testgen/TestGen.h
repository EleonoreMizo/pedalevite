/*****************************************************************************

        TestGen.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_testgen_TestGen_HEADER_INCLUDED)
#define mfx_pi_testgen_TestGen_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/dsp/nz/PinkShade.h"
#include "mfx/dsp/nz/WhiteFast.h"
#include "mfx/pi/testgen/LoopPause.h"
#include "mfx/pi/testgen/SweepDur.h"
#include "mfx/pi/testgen/TestGenDesc.h"
#include "mfx/pi/testgen/ToneFreq.h"
#include "mfx/pi/testgen/Type.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>

#include <cstdint>



namespace mfx
{
namespace pi
{
namespace testgen
{



class TestGen
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               TestGen ();
	virtual        ~TestGen () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Channel
	{
	public:
		dsp::nz::PinkShade
		               _nz_pink;
		dsp::nz::WhiteFast
		               _nz_white;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag);

	void           restart_gen ();
	void           gen_noise (ProcInfo &proc);
	void           gen_tone (ProcInfo &proc);
	void           gen_sweep (ProcInfo &proc);
	void           gen_sweep_running (ProcInfo &proc);
	void           gen_pulse (ProcInfo &proc);
	void           handle_pause (ProcInfo &proc);
	void           dup_mono_out (ProcInfo &proc);
	uint32_t       compute_phase_step (int pos);

	static inline float
	               compute_cos (uint32_t phase);

	State          _state;

	TestGenDesc    _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq, <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	ChannelArray   _chn_arr;

	bool           _work_flag;
	bool           _multichn_flag;
	Type           _type;
	float          _level;
	uint32_t       _tone_phase;         // 0:32
	uint32_t       _tone_phase_step;    // Idem

	uint32_t       _sweep_phase;        // 0:32
	uint32_t       _sweep_phase_step;
	int            _sweep_len;          // Samples
	int            _sweep_pos;          // Samples, [0 ; _sweep_len-1]
	int            _sweep_fade_in;      // Fade length, samples, >= 0
	int            _sweep_fade_out;     // Fade length, samples, >= 0

	int            _pause_len;          // Samples, 0 = no loop
	int            _pause_pos;          // Samples, [0 ; _pause_pos-1].
	bool           _pause_flag;         // true: pause, false: running

	static const std::array <float, ToneFreq_NBR_ELT>
	               _tone_freq_table;
	static const std::array <float, SweepDur_NBR_ELT>
	               _duration_table;
	static const std::array <float, LoopPause_NBR_ELT>
	               _pause_table;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestGen (const TestGen &other)           = delete;
	TestGen &      operator = (const TestGen &other)        = delete;
	bool           operator == (const TestGen &other) const = delete;
	bool           operator != (const TestGen &other) const = delete;

}; // class TestGen



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/testgen/TestGen.hpp"



#endif   // mfx_pi_testgen_TestGen_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
