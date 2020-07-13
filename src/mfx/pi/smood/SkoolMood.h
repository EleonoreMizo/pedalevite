/*****************************************************************************

        SkoolMood.h
        Author: Laurent de Soras, 2020

Adapted from the Vibe effect by Josep Andreu

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_smood_SkoolMood_HEADER_INCLUDED)
#define mfx_pi_smood_SkoolMood_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/lfo/LfoModule.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/pi/smood/SkoolMoodDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace smood
{



class SkoolMood final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SkoolMood ();
	               ~SkoolMood () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _max_blk_size = 64; // Samples
	static const int  _mod_rate     = 4;  // Samples, power of 2
	static const int  _nbr_stages   = 4;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Stage
	{
	public:
		void           setup_all_z_eq ();
		static void    setup_z_eq (dsp::iir::OnePole &flt, float n0, float n1, float d0, float d1);
		dsp::iir::OnePole
		               _vc;
		dsp::iir::OnePole
		               _vcvo;
		dsp::iir::OnePole
		               _ecvc;
		dsp::iir::OnePole
		               _vevo;
		float          _c1   = 0;
		float          _kc2oc2pc1 = 0;
		float          _en1  = 0;
		float          _en0  = 0;
		float          _ed1  = 0;
		float          _ed0  = 0;
		float          _cn1  = 0;
		float          _cn0  = 0;
		float          _cd1  = 0;
		float          _cd0  = 0;
		float          _ecn1 = 0;
		float          _ecn0 = 0;
		float          _ecd1 = 0;
		float          _ecd0 = 0;
		float          _on1  = 0;
		float          _on0  = 0;
		float          _od1  = 0;
		float          _od0  = 0;

		float          _cvolt_old = 0;
	};
	typedef std::array <Stage, _nbr_stages> StageArray;

	class Channel
	{
	public:
		dsp::ctrl::lfo::LfoModule
		               _lfo;
		dsp::ctrl::Ramp
		               _lfo_val;
		dsp::ctrl::Ramp
		               _pan_lvl;

		float          _drc    = 0;   // Dynamic time constant
		float          _alpha  = 0;
		float          _dalpha = 0;

		float          _step   = 0;
		float          _fb     = 0;
		float          _g      = 0;

		StageArray     _stage_arr;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           resync_lfo_phases ();
	float          get_lfo (const dsp::ctrl::lfo::LfoModule &lfo) const;
	void           modulate (Channel &chn, float ldr);

	static inline constexpr float
	               shape_bjt (float x);
	static inline constexpr float
	               compute_pan_lvl (float x);

	State          _state;

	SkoolMoodDesc _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_stdif;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_base;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_phase;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_phset;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_shape;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_wf;

	ChannelArray   _chn_arr;
	float          _st_dif;    // Channel phase difference in rounds
	float          _depth;
	float          _width;
	dsp::ctrl::Ramp
	               _mix;
	dsp::ctrl::Ramp
	               _chncross;
	dsp::ctrl::Ramp
	               _feedback;

	// Circuit parameters depending on the sampling rate
	float          _lamptc;
	float          _k;
	float          _kc2;
	float          _kgain;

	// Static circuit parameters
	static const float
	               _ra;        // Cds cell dark resistance
	static const float
	               _ra_log;
	static const float
	               _rb;        // Cds cell full illumination
	static const float
	               _b;
	static const float
	               _r1;
	static const float
	               _rv;
	static const float
	               _c2;
	static const float         // Transistor forward gain.
	               _beta;
	static const float
	               _gain;
	static const float
	               _dtc;
	static const float
	               _mintc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SkoolMood (const SkoolMood &other)         = delete;
	               SkoolMood (SkoolMood &&other)              = delete;
	SkoolMood &    operator = (const SkoolMood &other)        = delete;
	SkoolMood &    operator = (SkoolMood &&other)             = delete;
	bool           operator == (const SkoolMood &other) const = delete;
	bool           operator != (const SkoolMood &other) const = delete;

}; // class SkoolMood



}  // namespace smood
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/smood/SkoolMood.hpp"



#endif   // mfx_pi_smood_SkoolMood_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
